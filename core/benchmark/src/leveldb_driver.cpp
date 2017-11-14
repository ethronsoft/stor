//
// Created by Edd on 04/11/2017.
//

#include <leveldb/db.h>
#include <leveldb/filter_policy.h>
#include <unordered_set>
#include <stor/document/document.h>
#include "leveldb_driver.h"
#include "utils.h"


namespace esft {
    namespace stor {
        namespace bmark {

#pragma GCC push_options
#pragma GCC optimize ("O0")


            leveldb_driver::leveldb_driver(std::size_t key_size, std::size_t value_size, std::size_t ops):
            _home{"_leveldb_benchmark"}, _key_size{key_size}, _value_size{value_size}, _ops{ops}
            {
                mkdir(_home);
            }

            leveldb_driver::~leveldb_driver() {
                rmdir(_home);
            }

            std::unordered_map<std::string, double> leveldb_driver::writes() const {
                std::string db_name = _home + "/bmark_db";

                leveldb::Options opt;
                opt.create_if_missing = true;
                opt.filter_policy = leveldb::NewBloomFilterPolicy(10);

                leveldb::DB *db;

                try{
                    leveldb::Status conn = leveldb::DB::Open(opt, db_name,&db);

                    if (!conn.ok()){
                        throw std::runtime_error{"Failed opening LevelDB"};
                    }

                    value_generator kg1(_key_size);
                    value_generator vg1(_value_size - 7 - 1);

                    leveldb::WriteOptions wo;
                    wo.sync = false;
                    auto runner = [this, db, &wo, &kg1, &vg1]() {
                        std::size_t some_value_to_prevent_optimizaton = 0;
                        for (std::size_t i = 0; i < _ops; ++i){
                            std::string id = kg1();
                            std::string jsn = json_generator(vg1);
                            document doc(jsn, id);
                            some_value_to_prevent_optimizaton += doc.id().size();
                            auto s = db->Put(wo,id, jsn);
                            some_value_to_prevent_optimizaton += (std::size_t) &s;
                        }
                        return some_value_to_prevent_optimizaton;
                    };

                    auto elapsed = timer<std::chrono::microseconds>{}(runner);

                    delete db;
                    delete opt.filter_policy;
                    leveldb::DestroyDB(db_name, opt);
                    rmdir(db_name);

                    return result(elapsed,_ops,_key_size+_value_size);
                }catch (...){
                    delete db;
                    delete opt.filter_policy;
                    leveldb::DestroyDB(db_name, opt);
                    rmdir(db_name);
                    throw;
                }
            }

            std::unordered_map<std::string, double> leveldb_driver::reads_by_key() const {
                std::string db_name = _home + "/bmark_db";

                leveldb::Options opt;
                opt.create_if_missing = true;
                opt.filter_policy = leveldb::NewBloomFilterPolicy(10);

                leveldb::DB *db;

                try{
                    leveldb::Status conn = leveldb::DB::Open(opt, db_name,&db);

                    if (!conn.ok()){
                        throw std::runtime_error{"Failed opening LevelDB"};
                    }

                    //fill
                    {
                        value_generator kg1(_key_size);
                        value_generator vg1(_value_size);

                        leveldb::WriteOptions wo;
                        wo.sync = false;
                        for (std::size_t i = 0; i < _ops; ++i){
                            auto s = db->Put(wo,kg1(),json_generator(vg1));
                        }
                    }

                    value_generator kg1(_key_size);
                    value_generator vg1(_value_size);

                    leveldb::ReadOptions ro;
                    ro.snapshot = db->GetSnapshot();
                    auto runner = [this, db, &ro, &kg1, &vg1]() {
                        std::size_t some_value_to_prevent_optimizaton = 0;
                        for (std::size_t i = 0; i < _ops; ++i){
                            std::string json;
                            std::string id = kg1();
                            db->Get(ro,id, &json);
                            std::unordered_set<document> docs;
                            auto p = docs.emplace(json,id);
                            some_value_to_prevent_optimizaton += (std::size_t) &p.first;
                        }
                        db->ReleaseSnapshot(ro.snapshot);
                        return some_value_to_prevent_optimizaton;
                    };

                    auto elapsed = timer<std::chrono::microseconds>{}(runner);

                    delete db;
                    delete opt.filter_policy;
                    leveldb::DestroyDB(db_name, opt);
                    rmdir(db_name);

                    return result(elapsed,_ops,_key_size+_value_size);
                }catch (...){
                    delete db;
                    delete opt.filter_policy;
                    leveldb::DestroyDB(db_name, opt);
                    rmdir(db_name);
                    throw;
                }
            }

#pragma GCC pop_options
        }
    }
}