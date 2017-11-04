//
// Created by devhack on 04/06/16.
//

#include <string>
#include <limits>
#include <stor/exceptions/store_exception.h>
#include <stor/store/store.h>
#include <stor/store/db_crypt.h>
#include <leveldb/write_batch.h>


namespace esft{
    namespace stor{

        namespace impl{
            void check_path(std::string & _path, std::string &_name){
                if (_name.empty()){
                    throw store_exception{"invalid db name."};
                }

                if (!_path.empty() && (_path[_path.size()-1] != '/'))
                {
                    _path.append("/");
                }

                if (_name[0] == '/'){
                    _name = _name.substr(1,_name.size());
                }
                if (_name[_name.size()] == '/'){
                    _name = _name.substr(0,_name.size()-1);
                }
                _path = _path + _name + "/";
            }
        }




        store::store(const std::string &path_to_db, const std::string &db_name, bool temp):
        _onclose{nullptr},_path{path_to_db},_name{db_name}, _opt{},
        _temp{false}, _am{nullptr}, _encryption_failure_handler{default_encryption_failure_handler},
        _async{true}
        {
            impl::check_path(_path,_name);

            _opt.env->CreateDir(_path);
            _opt.create_if_missing = true;

            leveldb::Status conn = leveldb::DB::Open(_opt,_path + "catalog",&_db);

            if (!conn.ok()){
                delete _db;
                throw store_exception{"failed connecting to " + _path};
            }


            std::string json;
            _db->Get(leveldb::ReadOptions{},"db_info",&json);
            if (!json.empty()){
                document stored_info{json};
                if (stored_info.has("temporary") &&
                        stored_info["temporary"].is_bool()){
                    _temp = stored_info["temporary"].as_bool();
                }else{
                    _temp = temp;
                }
            }else{
                _temp = temp;
                //add database info if not already there
                document dbinfo = document::as_object();
                dbinfo.put("temporary",temp);

                leveldb::WriteOptions wo;
                wo.sync = !_async;
                _db->Put(wo,"db_info",dbinfo.json());
            }

            if (_temp){

                auto onclose_operation = [](const std::string &path) {
                    leveldb::Options opt;
                    if ((opt.env != nullptr) & !path.empty()) {
                        std::vector<std::string> db_dirs;
                        leveldb::Status st = opt.env->GetChildren(path, &db_dirs);
                        for (const auto &dir: db_dirs) {
                            if ((dir != ".") & (dir != "..")) {
                                leveldb::DestroyDB(path + dir, opt);
                                opt.env->DeleteDir(path + dir);
                            }
                        }
                        opt.env->DeleteDir(path);
                    }
                };
                _onclose = std::unique_ptr<onclose>(new onclose{_path,onclose_operation});
            }


        }

        store::store(const std::string &path_to_db, const std::string &db_name,
                     std::unique_ptr<access_manager> am, encryption_failure_handler handler):
                _onclose{nullptr},_path{path_to_db},_name{db_name},
                _opt{}, _temp{false}, _am{am.release()},_encryption_failure_handler{handler},
                _async{true}
        {
            impl::check_path(_path,_name);

            _opt.env->CreateDir(_path);
            _opt.create_if_missing = true;

            leveldb::Status conn = leveldb::DB::Open(_opt,_path + "catalog",&_db);

            if (!conn.ok()){
                delete _db;
                throw store_exception{"failed connecting to " + _path};
            }

            bool encrypted = false;
            bool to_encrypt = true;
            //check if database is marked as encrypted
            //if not, mark it as such for the future,
            //as this constructor call represents a desire for encryption
            leveldb::WriteOptions wo;
            wo.sync = !_async;
            std::string json;
            _db->Get(leveldb::ReadOptions{},"db_info",&json);
            if (!json.empty()){
                stor::document d(json);
                if (d.has("encrypted")) encrypted = d["encrypted"].as_bool();
                d.put("encrypted",true);
                _db->Put(wo,"db_info",d.json());
            }else{
                stor::document d = stor::document::as_object();
                d.put("encrypted",true);
                _db->Put(wo,"db_info",d.json());
            }

            if (encrypted && !_am){
                throw stor::store_exception{"database is encrypted. Authentication parameters required"};
            }

            //if encrypted == true, the db was in an encrypted state.
            //decrypt
            if (_am && encrypted) {
                db_crypt decrypt{db_crypt::mode::DECRYPT, _am};
                if (!decrypt(_path)) {
                    delete _db;
                    throw store_exception{"failed decryption"};
                }
            }

            //set onclose operation to encrypt db
            if(_am && to_encrypt){

                //set encrypter operation
                //make sure stor_test independent copies are passed on to onclose operation
                //as it will be triggered after stor_test has been deleted
                auto _amcopy = _am;
                auto _handler_copy = _encryption_failure_handler;
                auto onclose_operation = [_amcopy, _handler_copy](const std::string &path){
                    db_crypt encrypt{db_crypt::mode::ENCRYPT,_amcopy};
                    if (!encrypt(path)){
                        _handler_copy();
                    }
                };
                _onclose = std::unique_ptr<onclose>(new onclose{_path,onclose_operation});
            }
        }


        store::store(store &&o):
                _onclose{std::move(o._onclose)},
        _path{std::move(o._path)}, _name{std::move(o._name)},
        _db{o._db}, _opt{o._opt},_temp{o._temp}, _am{std::move(o._am)},
                _async{o._async},
                _collections{std::move(o._collections)}
        {
            o._db = nullptr;
            o._async = true;
        }

        store::~store()
        {
            delete _db;
        }

        void store::set_async(bool async) { _async = async; }

        bool store::is_async() const {return _async; }

        bool store::has(const std::string &collection_name) const
        {
            std::string json;
            _db->Get(leveldb::ReadOptions{},collection_name,&json);
            bool collection_dir_exists = _opt.env->FileExists(home()+collection_name);
            return !json.empty() && collection_dir_exists;
        }

        collection &store::operator[](const std::string &collection_name){

            //do not overwrite system info
            if (collection_name == "db_info") throw store_exception{"db_info name is reserved."};

            if (_collections.count(collection_name) == 0)
            {
                std::string json;
                leveldb::ReadOptions ro;
                leveldb::Status read = _db->Get(ro,collection_name,&json);
                //if (!read.ok()) throw store_exception{"failure reading collection info."};
                //if collection doesn't exist in catalog, create default
                if (json.empty()){
                    document new_info = document::as_object();
                    new_info.put("name",collection_name)
                            .with_array("indices");
                    std::unique_ptr<collection> c(new collection(this,new_info));
                    put(*c);
                    _collections[collection_name] = std::move(c);
                }else{
                    document stored_info{json};
                    std::unique_ptr<collection> c(new collection(this,stored_info));
                    _collections[collection_name] = std::move(c);
                }
            }
            return *_collections.at(collection_name);
        }

        void store::put(const collection &c)
        {
           // std::unique_lock{_mutex};

            leveldb::WriteOptions wo;
            wo.sync = !_async;
            document info = document::as_object();
            info.put("name",c.name());
            node indices = info.with_array("indices");
            for (const auto &indx: c._indices){
                indices.add(indx.str());
            }
            leveldb::Status write = _db->Put(wo,c.name(),info.json());
            if (!write.ok()) throw store_exception{"failure writing collection info."};
        }

        bool store::remove(const std::string &collection_name)
        {
            leveldb::WriteOptions wo;
            leveldb::ReadOptions ro;
            wo.sync = true; //force sync
            //we are about to delete collection. Which means that everything related
            //to it will be destroyed, included the reference returned by collection.name()
            //We have no guarantee that @p collection_name is provided by collection.name()
            //so if we are going to delete the collection, we will first take make a copy of
            //the name
            auto clean_up = [this, &wo, &ro](const std::string &coll_name){
                _db->Delete(wo,coll_name);
                leveldb::DestroyDB(home() + coll_name, _opt);
                _opt.env->DeleteDir(home() + coll_name);
                return true;
            };
            if (_collections.count(collection_name) > 0){
                std::string copied_name = collection_name;
                _collections.erase(copied_name);
                return clean_up(copied_name);
            }else{
                //it could exist even if it wasn't loaded
                //in _collections
                std::string tmp;
                _db->Get(ro,collection_name,&tmp);
                if (!tmp.empty()){
                    return clean_up(collection_name);
                }
            }
            return false;
        }


        const std::string &store::home() const{
            return _path;
        }

        void default_encryption_failure_handler()
        {
            std::cerr << "failed encryption" << std::endl;
        }


    }
}