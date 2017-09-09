#include <stor/store/collection.h>
#include <stor/exceptions/store_exception.h>
#include <leveldb/write_batch.h>
#include <leveldb/filter_policy.h>
#include <sstream>
#include <set>
#include <stor/store/store.h>
#include <stor/store/query.h>
#include <stor/store/eq_instruction.h>
#include <stor/store/gt_instruction.h>
#include <stor/store/gte_instruction.h>
#include <stor/store/neq_instruction.h>
#include <stor/store/lt_instruction.h>
#include <stor/store/lte_instruction.h>



namespace esft {
    namespace stor {


        namespace impl {
            std::unordered_map<std::string, std::string> get_indices(leveldb::DB &db,
                                                                     const stor::document &d,
                                                                     const leveldb::ReadOptions &ro);

            std::string next_smallest(const std::string &s);
        }

        collection::collection(store *s, document &info) :
                _store{s} {
            if (!info.has("name")) throw store_exception{"missing collection name"};
            _name = info["name"].as_string();

            if (info.has("indices")) {
                for (auto n: info["indices"]) {
                    _indices.emplace(n.as_string());
                }
            }

            std::string fn = _store->home() + _name;

            _opt.create_if_missing = true;
            _opt.filter_policy = leveldb::NewBloomFilterPolicy(10);

            leveldb::Status conn = leveldb::DB::Open(_opt, fn, &_db);
            if (!conn.ok()) {
                delete _db;
                delete _opt.filter_policy;
                throw store_exception{"failed connecting to " + fn};
            }
        }

        collection::collection(collection &&o) :
                _name{std::move(o._name)},
                _store{o._store}, _db{o._db}, _opt{o._opt},
                _indices{std::move(o._indices)} {
            o._opt.filter_policy = nullptr;
            o._db = nullptr;
            o._store = nullptr;
        }

        collection &collection::operator=(collection &&o) {
            _name = std::move(o._name);
            _store = o._store;
            _db = o._db;
            _opt = o._opt;
            _indices = std::move(o._indices);

            o._opt.filter_policy = nullptr;
            o._db = nullptr;
            o._store = nullptr;
            return *this;
        }

        collection::~collection() {
            delete _db;
            delete _opt.filter_policy;
        }

        void collection::persist() {
            if (_store != nullptr) {
                _store->put(*this);
            }
        }

        const std::string &collection::name() const {
            return _name;
        }


        bool collection::clear_indices() {
            if (_indices.empty()) return false;

            leveldb::WriteOptions wo;
            wo.sync = !_store->is_async();

            leveldb::WriteBatch batch;

            std::string stop = impl::next_smallest("#/");

            leveldb::Iterator *it = _db->NewIterator(leveldb::ReadOptions{});
            for (it->Seek("#"); it->Valid() && it->key().ToString() < stop; it->Next()) {
                batch.Delete(it->key());
            }

            auto st = _db->Write(wo, &batch);

            delete it;

            if (st.ok()) {
                _indices.clear();
            }

            persist();
            return st.ok();
        }

        void collection::add_index(const index_path &path) {
            if (_indices.count(path) == 0) {
                _indices.insert(path);
                persist();
            }
        }

        const std::unordered_set<index_path>& collection::indices() const {
            return _indices;
        }

        bool collection::put(const document &doc) {

            leveldb::WriteOptions wo;
            wo.sync = !_store->is_async();

            leveldb::WriteBatch batch;

            //document may have been modified
            //so drop indices and reinsert
            leveldb::ReadOptions ro;
            ro.snapshot = _db->GetSnapshot();
            auto curr_indices = impl::get_indices(*_db, doc, ro);
            for (const auto &im: curr_indices) {
                batch.Delete(im.first);
                batch.Delete(im.second);
            }

            for (const auto &path: _indices) {
                put(batch, path, doc);
            }
            _db->ReleaseSnapshot(ro.snapshot);

            //document insertion
            put(batch, doc);

            leveldb::Status update = _db->Write(wo, &batch);

            return update.ok();
        }

        bool collection::remove(const document::identifier &id) {
            if (!has(id)) return false;
            return remove((*this)[id]);
        }

        bool collection::remove(const document &doc) {
            if (!has(doc.id())) return false;

            leveldb::WriteOptions wo;
            wo.sync = !_store->is_async();

            leveldb::WriteBatch batch;

            for (const auto &path: _indices) {
                remove(batch, path, doc);
            }
            remove(batch, doc);

            leveldb::Status remove = _db->Write(wo, &batch);
            return remove.ok();
        }

        document collection::operator[](const stor::document::identifier &id) const {
            leveldb::ReadOptions ro;
            ro.snapshot = _db->GetSnapshot();

            std::string json;
            _db->Get(leveldb::ReadOptions{}, id, &json);

            _db->ReleaseSnapshot(ro.snapshot);
            if (!json.empty()) {
                return document{json, id};
            } else {
                throw store_exception{"id does not refer to existing document"};
            }
        }

        bool collection::has(const stor::document::identifier &id) const {
            leveldb::ReadOptions ro;
            ro.snapshot = _db->GetSnapshot();

            std::string json;
            _db->Get(leveldb::ReadOptions{}, id, &json);
            _db->ReleaseSnapshot(ro.snapshot);

            return !json.empty();
        }

        std::unordered_set<document> collection::find(const query &q) {
            std::unordered_set<document> res;

            leveldb::ReadOptions ro;
            ro.snapshot = _db->GetSnapshot();

            std::unordered_set<document::identifier> keys = q.answer(*this, ro);

            for (const auto &key: keys) {
                std::string json;
                if (_db->Get(ro, key, &json).ok()) {
                    res.emplace(json, key);
                }
            }

            _db->ReleaseSnapshot(ro.snapshot);

            return res;
        }

        void collection::put(leveldb::WriteBatch &b, const document &d) const {
            b.Put(d.id(), d.json());
        }

        void collection::put(leveldb::WriteBatch &batch, const index_path &index, const document &d) const {

            std::pair<std::string, std::string> data = index.extract(d);
            std::string key = data.first;
            std::string type = data.second;

            if (key.empty()) return;
            //key starting in # represent indices to data
            std::string kindx = "#/" + index.str() + "/" + key + "/" + type + "/" + d.id();
            batch.Put(kindx, d.id());

            //key starting in @ represent indices to #keys
            std::string iindx = "@/" + d.id() + "/" + index.str();
            batch.Put(iindx, kindx);

        }

        void collection::remove(leveldb::WriteBatch &batch, const document &d) const {
            batch.Delete(d.id());
        }

        void collection::remove(leveldb::WriteBatch &batch, const index_path &index, const document &d) const {
            std::pair<std::string, std::string> data = index.extract(d);
            std::string key = data.first;
            std::string type = data.second;

            if (key.empty()) return;
            std::string kindx = "#/" + index.str() + "/" + key + "/" + type + "/" + d.id();
            batch.Delete(kindx);

            //key starting in @ represent indices to #keys
            std::string iindx = "@/" + d.id() + "/" + index.str();
            batch.Delete(iindx);
        }

        std::unordered_set<std::string> collection::process(const eq_instruction &i,
                                                            const leveldb::ReadOptions &ro) const {

            std::unordered_set<std::string> res;


            leveldb::Iterator *it = _db->NewIterator(ro);

            std::string start = "#/" + i.target().str() + "/" + i.value() + "/";
            std::string stop = "#/" + i.target().str() + "/" + impl::next_smallest(i.value()) + "/";

            for (it->Seek(start); it->Valid() && it->key().ToString() < stop; it->Next()) {
                res.insert(it->value().ToString());
            }

            delete it;

            return res;
        }

        void collection::deflate(std::ostream &o, bool keep_indices) const {
            leveldb::ReadOptions ro;
            ro.snapshot = _db->GetSnapshot();

            leveldb::Iterator *it = _db->NewIterator(ro);
            for (it->SeekToFirst(); it->Valid(); it->Next()) {
                if (keep_indices) {
                    o << it->key().ToString() + ":" + it->value().ToString() << std::endl;
                } else {
                    std::string key = it->key().ToString();
                    if (!key.empty() && key[0] != '#' && key[0] != '@') {
                        o << it->key().ToString() + ":" + it->value().ToString() << std::endl;
                    }
                }
            };

            _db->ReleaseSnapshot(ro.snapshot);
            delete it;
        }

        void collection::inflate(std::istream &i) {
            leveldb::WriteOptions wo;
            wo.sync = !_store->is_async();

            leveldb::WriteBatch batch;

            std::ostringstream key_buffer;
            std::ostringstream value_buffer;
            std::ostringstream *current = &key_buffer;

            size_t buffer_size = 1024;
            char buffer[buffer_size];
            while (true) {
                i.get(buffer, buffer_size);
                size_t read = i.gcount();
                for (size_t i = 0; i < read; ++i) {
                    char x = buffer[i];
                    switch (x) {
                        case ':':
                            current = &value_buffer;
                            break;
                        case '\n':
                            batch.Put(key_buffer.str(), value_buffer.str());
                            key_buffer.str(std::string{});
                            value_buffer.str(std::string{});
                            current = &key_buffer;
                            break;
                        default:
                            (*current) << x;
                    }

                }
                if (!i.good()) break;
            }

        }


        std::unordered_set<std::string> collection::process(const gt_instruction &i,
                                                            const leveldb::ReadOptions &ro) const {
            std::unordered_set<std::string> res;

            leveldb::Iterator *it = _db->NewIterator(ro);

            std::string start = "#/" + i.target().str() + "/";
            std::string stop = "#/" + impl::next_smallest(i.target().str()) + "/";

            for (it->Seek(start); it->Valid() && it->key().ToString() < stop; it->Next()) {
                std::string k = it->key().ToString();

                //extract type, to determine type of datum
                //values: b(bool), n(numeric), s(string)
                std::string type = k.substr(
                        (k.size() - (it->value().size() + 1)) - 1,
                        1
                );

                //extract value
                std::string x = k.substr(
                        start.size(),
                        (k.size() - start.size() - (it->value().size() + 1) - (type.length() + 1))
                );

                //convert to type
                std::istringstream target_iss{x};
                std::istringstream value_iss{i.value()};
                if (type == "n") {
                    double t, v;
                    target_iss >> t;
                    value_iss >> v;
                    if (t > v) {
                        res.insert(it->value().ToString());
                    }
                } else if (type == "s") {
                    if (x > i.value()) {
                        res.insert(it->value().ToString());
                    }
                } else if (type == "b") {
                    bool t, v;
                    target_iss >> t;
                    value_iss >> v;
                    if (t > v) {
                        res.insert(it->value().ToString());
                    }
                }

            }

            delete it;

            return res;
        }

        std::unordered_set<std::string> collection::process(const gte_instruction &i,
                                                            const leveldb::ReadOptions &ro) const {
            std::unordered_set<std::string> res;

            leveldb::Iterator *it = _db->NewIterator(ro);

            std::string start = "#/" + i.target().str() + "/";
            std::string stop = "#/" + impl::next_smallest(i.target().str()) + "/";

            for (it->Seek(start); it->Valid() && it->key().ToString() < stop; it->Next()) {
                std::string k = it->key().ToString();

                //extract type, to determine type of datum
                //values: b(bool), n(numeric), s(string)
                std::string type = k.substr(
                        (k.size() - (it->value().size() + 1)) - 1,
                        1
                );

                //extract value
                std::string x = k.substr(
                        start.size(),
                        (k.size() - start.size() - (it->value().size() + 1) - (type.length() + 1))
                );

                //convert to type
                std::istringstream target_iss{x};
                std::istringstream value_iss{i.value()};
                if (type == "n") {
                    double t, v;
                    target_iss >> t;
                    value_iss >> v;
                    if (t >= v) {
                        res.insert(it->value().ToString());
                    }
                } else if (type == "s") {
                    if (x >= i.value()) {
                        res.insert(it->value().ToString());
                    }
                } else if (type == "b") {
                    bool t, v;
                    target_iss >> t;
                    value_iss >> v;
                    if (t >= v) {
                        res.insert(it->value().ToString());
                    }
                }

            }

            delete it;

            return res;
        }

        std::unordered_set<std::string> collection::process(const neq_instruction &i,
                                                            const leveldb::ReadOptions &ro) const {
            std::unordered_set<std::string> res;

            leveldb::Iterator *it = _db->NewIterator(ro);

            std::string start = "#/" + i.target().str() + "/";
            std::string stop = "#/" + impl::next_smallest(i.target().str()) + "/";
            for (it->Seek(start); it->Valid() && it->key().ToString() < stop; it->Next()) {
                std::string k = it->key().ToString();

                //extract value
                std::string x = k.substr(
                        start.size(),
                        (k.size() - start.size() - (it->value().size() + 1) - (2))
                );

                if (x != i.value()) {
                    res.insert(it->value().ToString());
                }
            }

            delete it;

            return res;
        }

        std::unordered_set<std::string> collection::process(const lt_instruction &i,
                                                            const leveldb::ReadOptions &ro) const {
            std::unordered_set<std::string> res;

            leveldb::Iterator *it = _db->NewIterator(ro);

            std::string start = "#/" + i.target().str() + "/";
            std::string stop = "#/" + impl::next_smallest(i.target().str()) + "/";

            for (it->Seek(start); it->Valid() && it->key().ToString() < stop; it->Next()) {
                std::string k = it->key().ToString();

                //extract type, to determine type of datum
                //values: b(bool), n(numeric), s(string)
                std::string type = k.substr(
                        (k.size() - (it->value().size() + 1)) - 1,
                        1
                );

                //extract value
                std::string x = k.substr(
                        start.size(),
                        (k.size() - start.size() - (it->value().size() + 1) - (type.length() + 1))
                );

                //convert to type
                std::istringstream target_iss{x};
                std::istringstream value_iss{i.value()};
                if (type == "n") {
                    double t, v;
                    target_iss >> t;
                    value_iss >> v;
                    if (t < v) {
                        res.insert(it->value().ToString());
                    }
                } else if (type == "s") {
                    if (x < i.value()) {
                        res.insert(it->value().ToString());
                    }
                } else if (type == "b") {
                    bool t, v;
                    target_iss >> t;
                    value_iss >> v;
                    if (t < v) {
                        res.insert(it->value().ToString());
                    }
                }

            }

            delete it;

            return res;
        }

        std::unordered_set<std::string> collection::process(const lte_instruction &i,
                                                            const leveldb::ReadOptions &ro) const {
            std::unordered_set<std::string> res;

            leveldb::Iterator *it = _db->NewIterator(ro);

            std::string start = "#/" + i.target().str() + "/";
            std::string stop = "#/" + impl::next_smallest(i.target().str()) + "/";

            for (it->Seek(start); it->Valid() && it->key().ToString() < stop; it->Next()) {
                std::string k = it->key().ToString();

                //extract type, to determine type of datum
                //values: b(bool), n(numeric), s(string)
                std::string type = k.substr(
                        (k.size() - (it->value().size() + 1)) - 1,
                        1
                );

                //extract value
                std::string x = k.substr(
                        start.size(),
                        (k.size() - start.size() - (it->value().size() + 1) - (type.length() + 1))
                );

                //convert to type
                std::istringstream target_iss{x};
                std::istringstream value_iss{i.value()};
                if (type == "n") {
                    double t, v;
                    target_iss >> t;
                    value_iss >> v;
                    if (t <= v) {
                        res.insert(it->value().ToString());
                    }
                } else if (type == "s") {
                    if (x <= i.value()) {
                        res.insert(it->value().ToString());
                    }
                } else if (type == "b") {
                    bool t, v;
                    target_iss >> t;
                    value_iss >> v;
                    if (t <= v) {
                        res.insert(it->value().ToString());
                    }
                }

            }

            delete it;

            return res;
        }


        //impl
        std::unordered_map<std::string, std::string> impl::get_indices(leveldb::DB &db,
                                                                       const stor::document &d,
                                                                       const leveldb::ReadOptions &ro) {
            std::unordered_map<std::string, std::string> res;

            std::string start = "@/" + d.id() + "/";
            std::string stop = "@/" + impl::next_smallest(d.id()) + "/";

            leveldb::Iterator *it = db.NewIterator(ro);

            for (it->Seek(start); it->Valid() && it->key().ToString() < stop; it->Next()) {
                res[it->key().ToString()] = it->value().ToString();
            }

            delete it;

            return res;
        }


        std::string impl::next_smallest(const std::string &s) {
            //given a strign s, return the next string in lexicographical order X.
            //The following must hold: there is no string Z such that s < Z < X
            std::string res = s;
            if (s.empty()) {
                //return the smallest available char
                res += char(1);
            } else {
                //advance last character of string
                int c = res[s.size() - 1];
                c++;//next char
                res[res.size() - 1] = char(c);
            }
            return res;
        }
    }
}
