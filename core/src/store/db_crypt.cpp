//
// Created by efelici on 6/15/2016.
//

#include <thread>
#include <future>
#include <iostream>
#include <stor/store/db_crypt.h>
#include <stor/store/collection_visitor.h>
#include <leveldb/options.h>
#include <leveldb/env.h>


namespace esft{
    namespace stor{

        db_crypt::db_crypt(mode m, const std::shared_ptr<access_manager> &am):
        _am{am}
	//,_thread_pool_size{10}
        {
            switch (m){
                case mode::DECRYPT:{
                    _crypted_file_name_ext = "_decrypted";
                    _crypto_funct = [this](const std::string &s1, const std::string &s2){
                        return _am->decrypt(s1,s2);
                    };
                    break;
                }
                case mode::ENCRYPT:{
                    _crypted_file_name_ext = "_encrypted";
                    _crypto_funct = [this](const std::string &s1, const std::string &s2){
                        return _am->encrypt(s1,s2);
                    };
                    break;
                }
            }
        }


        bool db_crypt::operator()(const std::string &stor_root) const
        {
            leveldb::Options opt;
            leveldb::Env &env = *opt.env;

            collection_visitor v{stor_root};
            std::vector<std::string> attempted;

            /**
             * take all attempted files ending in _tmp
             * and rename them to original file name.
             * Delete _encrypted files if existing.
             */
            auto rollback = [this,&attempted,&env](){
                for(const auto &path: attempted){
                    env.RenameFile(path+"_tmp",path);
                    env.DeleteFile(path+_crypted_file_name_ext);
                }
            };

            /**
             * take all attempted files ending in
             * _crypted_file_name_ext files.
             * Rename them to original file name.
             * if Rename succeeds on all files,
             * delete _tmp files. Else, rollback.
             */
            auto commit = [this,&attempted,&env,&rollback](){
                for(const auto &path: attempted){
                    auto s2 = env.RenameFile(path+_crypted_file_name_ext,path);
                    if (!s2.ok()){
                        rollback();
                        return false;
                    }
                }
                //delete all _tmp
                for (const auto &path: attempted){
                    env.DeleteFile(path + "_tmp");
                }
                return true;
            };

            //attempt encrypting all collections files
            //revert to original files if encryption fails
            //for at least one file.

//            //ASYNC CODE
//
//            //thread pool
//            std::vector< std::future<int> >                futures;
//            std::vector< std::promise<int> >               promises;
//            std::vector< std::unique_ptr<std::thread> >    threads;
//            std::mutex m;
//
//            size_t queued = 0;
//
//            END ASYNC CODE



            for(size_t i = 0; i < v.size(); ++i){

                std::string path = v[i];


                //SYNC CODE (behaves better. keep for now)

                attempted.push_back(path);
                auto status = _crypto_funct(path,path+_crypted_file_name_ext);
                auto r = env.RenameFile(path,path+"_tmp");
                if (status != access_manager::ok){
                    rollback();
                    return false;
                }

                //END SYNC CODE


                //ASYNC CODE

//                if (queued < _thread_pool_size){
//                    size_t indx = i % _thread_pool_size;
//
//                    promises.emplace_back();
//                    futures.push_back(
//                            promises[indx].get_future()
//                    );
//                    threads.push_back(
//                            std::unique_ptr<std::thread>(
//                                    new std::thread(
//                                            [this,&m,&env,&promises,&attempted,path,indx](){
//                                                auto ok = _crypto_funct(path,path+_crypted_file_name_ext);
//                                                std::unique_lock<std::mutex> l(m);
//                                                promises[indx].set_value(ok);
//                                                attempted.push_back(path);
//                                                auto r = env.RenameFile(path,path+"_tmp");
//                                            }
//                                    )
//                            )
//                    );
//                    queued++;
//                }
//
//                //if last thread (out of total or for given batch) wait for termination of all outstanding threads
//                if ((queued == _thread_pool_size) | (i == v.size() - 1)){
//                    for(size_t i = 0; i < queued;++i){
//                        threads[i]->join();
//                        futures[i].wait();
//                    }
//                    for(size_t i = 0; i < queued;++i){
//                        auto ok = futures[i].get();
//                        if (ok != access_manager::ok){
//                            rollback();
//                            return false;
//                        }
//                    }
//                    queued = 0;
//                    futures.clear();
//                    promises.clear();
//                    threads.clear();
//                }
//
                //END ASYNC CODE

            }



            return commit();
        }


    }
}
