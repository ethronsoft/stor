//
// Created by efelici on 10/31/2017.
//

#include <leveldb/env.h>
#include <ext/sqlite3.h>
#include <utils.h>
#include <leveldb/options.h>
#include <bits/unordered_set.h>
#include <stor/document/document.h>
#include "sqlite_benchmark.h"

namespace esft {
    namespace stor {
        namespace bmark {

#pragma GCC push_options
#pragma GCC optimize ("O0")
            
            namespace detail {

                sqlite3 *make_db(const std::string &db_path, std::vector<std::string> stmnts) {
                    sqlite3 *db;
                    int rc = sqlite3_open(db_path.c_str(), &db);
                    if (rc != SQLITE_OK) {
                        throw std::runtime_error{"Could not open db: " + db_path};
                    }

                    for (const auto &stmnt: stmnts) {
                        int rc = sqlite3_exec(db, stmnt.c_str(), nullptr, nullptr, nullptr);
                        if (rc != SQLITE_OK) {
                            sqlite3_close(db);
                            throw std::runtime_error{"failed executing statement: " + stmnt};
                        }
                    }
                    return db;
                }

                class prepared_write_stmnt {
                public:
                    prepared_write_stmnt(sqlite3 *db, const std::string &stmt, bool tx = true) : db{db},
                                                                                                 _transaction{tx} {
                        int rc = sqlite3_prepare_v2(db, stmt.c_str(), -1, &_stmnt, nullptr);
                        if (rc != SQLITE_OK) {
                            throw std::runtime_error{"Could not create prepared statement: " + stmt};
                        }
                        if (_transaction) {
                            sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);
                        }
                    }

                    void exec(std::vector<std::string> values) {
                        for (std::size_t i = 0; i < values.size(); ++i) {
                            sqlite3_bind_text(_stmnt, i + 1, values[i].c_str(), -1, nullptr);
                        }
                        sqlite3_step(_stmnt);
                        sqlite3_reset(_stmnt);
                    }

                    void flush() {
                        if (_transaction) {
                            sqlite3_exec(db, "END TRANSACTION", nullptr, nullptr, nullptr);
                        }
                        sqlite3_finalize(_stmnt);
                        _stmnt = nullptr;
                    }

                    ~prepared_write_stmnt() {
                        if (_stmnt) {
                            sqlite3_finalize(_stmnt);
                        }
                    }

                private:
                    sqlite3 *db;
                    sqlite3_stmt *_stmnt;
                    bool _transaction;
                };

                class prepared_read_stmnt {
                public:
                    prepared_read_stmnt(sqlite3 *db, const std::string &stmt) : db{db} {
                        int rc = sqlite3_prepare_v2(db, stmt.c_str(), -1, &_stmnt, nullptr);
                        if (rc != SQLITE_OK) {
                            throw std::runtime_error{"Could not create prepared statement: " + stmt};
                        }
                    }

                    std::unordered_set<document> exec(std::vector<std::string> values) {
                        for (std::size_t i = 0; i < values.size(); ++i) {
                            sqlite3_bind_text(_stmnt, i + 1, values[i].c_str(), -1, nullptr);
                        }
                        auto step = sqlite3_step(_stmnt);
                        std::unordered_set<document> docs;
                        if (step == SQLITE_ROW) {
                            std::string id(((const char*)sqlite3_column_text(_stmnt, 0)));
                            std::string json(((const char*)sqlite3_column_text(_stmnt, 1)));
                            docs.insert(document(json, id));
                        }
                        sqlite3_reset(_stmnt);
                        return docs;
                    }

                    void flush() {
                        sqlite3_finalize(_stmnt);
                        _stmnt = nullptr;
                    }

                    ~prepared_read_stmnt() {
                        if (_stmnt) {
                            sqlite3_finalize(_stmnt);
                        }
                    }

                private:
                    sqlite3 *db;
                    sqlite3_stmt *_stmnt;
                };

                std::unordered_map<std::string, double> result(double net, std::size_t ops, std::size_t data_size) {
                    const std::size_t MB = 1024 * 1024;
                    const long micros = 1000000;
                    const double sec_ratio = double(1) / micros;


                    return std::unordered_map<std::string, double>{
                            {"micros/ops", net / ops},
                            {"IOPS",       long(micros / (net / ops))},
                            {"MB/sec",     (double(data_size) * ops / MB) / (sec_ratio * net)},
                    };
                };

            }

            sqlite_driver::sqlite_driver(std::size_t key_size, std::size_t value_size, std::size_t ops) :
                    _home{"_sqlite_benchmark"}, _key_size{key_size}, _value_size{value_size}, _ops{ops} {
                mkdir(_home);
            }

            sqlite_driver::~sqlite_driver() {
                rmdir(_home);
            }

            std::unordered_map<std::string, double> sqlite_driver::writes() const {

                std::string db_name = _home + "/" + "bmark_db";
                std::string create = "CREATE TABLE TBL (ID TEXT PRIMARY KEY, VALUE TEXT);";
                std::string async = "PRAGMA synchronous = OFF";
                std::string page_sz = "PRAGMA page_size = 4096";
                std::string cache = "PRAGMA cache_size = 0";
                sqlite3 *db;

                //SQLite performance optimization tips taken from:
                //https://stackoverflow.com/questions/1711631/improve-insert-per-second-performance-of-sqlite

                try {

                    db = detail::make_db(db_name, {create, async, page_sz, cache});
                    detail::prepared_write_stmnt writes(db, "INSERT INTO TBL(ID, VALUE) VALUES(?,?);");

                    value_generator kg1(_key_size);
                    value_generator vg1(_value_size);


                    auto runner = [this, &writes, &kg1, &vg1]() {
                        for (std::size_t i = 0; i < _ops; ++i) {
                            writes.exec({kg1(), vg1()});
                        }
                        writes.flush();
                    };

                    auto elapsed = timer<std::chrono::microseconds>{}(runner);

                    value_generator kg2(_key_size);
                    value_generator vg2(_value_size);
                    auto discount_key_gen = timer<std::chrono::microseconds>{}(
                            [this, &kg2]() {
                                for (std::size_t i = 0; i < _ops; ++i) {
                                    auto s = kg2();
                                }
                            });
                    auto discount_value_gen = timer<std::chrono::microseconds>{}(
                            [this, &vg2]() {
                                for (std::size_t i = 0; i < _ops; ++i) {
                                    auto s = vg2();
                                }
                            });

                    if ((discount_key_gen + discount_value_gen) >= elapsed) {
                        throw std::runtime_error{"Timer error. discount_key_gen: " + std::to_string(discount_key_gen) +
                                                 "discount_value_gen: " + std::to_string(discount_value_gen) +
                                                 ", elapsed: " + std::to_string(elapsed)};
                    }

                    sqlite3_close(db);
                    rmfile(db_name);

                    auto net = elapsed - discount_key_gen - discount_value_gen;
                    return result(net, _ops, _key_size + _value_size);
                } catch (...) {
                    if (db) {
                        sqlite3_close(db);
                    }
                    rmfile(db_name);
                    throw;
                }

            };

            std::unordered_map<std::string, double> sqlite_driver::reads_by_key() const {
                std::string db_name = _home + "/" + "bmark_db";
                std::string create = "CREATE TABLE TBL (ID TEXT PRIMARY KEY, VALUE TEXT);";
                std::string page_sz = "PRAGMA page_size = 4096";
                std::string async = "PRAGMA synchronous = OFF";
                std::string cache = "PRAGMA cache_size = 0";
                sqlite3 *db;

                try {

                    db = detail::make_db(db_name, {create, async, page_sz, cache});

                    //fill db
                    {
                        detail::prepared_read_stmnt writes(db, "INSERT INTO TBL(ID, VALUE) VALUES(?,?);");

                        value_generator kg1(_key_size);
                        value_generator vg1(_value_size - 7 - 1);


                        for (std::size_t i = 0; i < _ops; ++i) {
                            writes.exec({kg1(), json_generator(vg1)});
                        }
                        writes.flush();
                    }


                    detail::prepared_read_stmnt reads(db, "SELECT ID, VALUE FROM TBL WHERE ID = ?;");

                    value_generator kg1(_key_size);
                    auto runner = [this, &reads, &kg1]() {
                        for (std::size_t i = 0; i < _ops; ++i) {
                            auto res = reads.exec({kg1()});
                        }
                        reads.flush();
                    };

                    auto elapsed = timer<std::chrono::microseconds>{}(runner);

                    value_generator kg2(_key_size);
                    auto discount_key_gen = timer<std::chrono::microseconds>{}(
                            [this, &kg2]() {
                                for (std::size_t i = 0; i < _ops; ++i) {
                                    auto s = kg2();
                                }
                            });

                    if (discount_key_gen >= elapsed) {
                        throw std::runtime_error{"Timer error. discount_key_gen: " + std::to_string(discount_key_gen) +
                                                 ", elapsed: " + std::to_string(elapsed)};
                    }


                    sqlite3_close(db);
                    rmfile(db_name);

                    auto net = elapsed - discount_key_gen;
                    return result(net, _ops, _key_size + _value_size);

                } catch (...) {
                    if (db) {
                        sqlite3_close(db);
                    }
                    rmfile(db_name);
                    throw;
                }
            };

            std::unordered_map<std::string, double> sqlite_driver::reads_by_value() const {
                std::string db_name = _home + "/" + "bmark_db";
                std::string create = "CREATE TABLE TBL (ID TEXT PRIMARY KEY, VALUE TEXT);";
                std::string make_index = "CREATE INDEX value_index ON TBL(VALUE);";
                std::string async = "PRAGMA synchronous = OFF";
                std::string page_sz = "PRAGMA page_size = 4096";
                std::string cache = "PRAGMA cache_size = 0";
                sqlite3 *db;

                try {

                    db = detail::make_db(db_name, {create, async, make_index, page_sz, cache});

                    //fill db
                    {
                        detail::prepared_write_stmnt writes(db, "INSERT INTO TBL(ID, VALUE) VALUES(?,?);");

                        value_generator kg1(_key_size);
                        value_generator vg1(_value_size - 7 - 1);


                        for (std::size_t i = 0; i < _ops; ++i) {
                            writes.exec({kg1(), json_generator(vg1)});
                        }
                        writes.flush();
                    }

                    detail::prepared_read_stmnt reads(db, "SELECT ID, VALUE FROM TBL WHERE VALUE = ?;");

                    value_generator vg1(_value_size - 7 - 1);

                    auto runner = [this, &reads, &vg1]() {
                        for (std::size_t i = 0; i < _ops; ++i) {
                            auto res = reads.exec({json_generator(vg1)});
                        }
                        reads.flush();
                    };

                    auto elapsed = timer<std::chrono::microseconds>{}(runner);

                    value_generator vg2(_value_size);
                    auto discount_value_gen = timer<std::chrono::microseconds>{}(
                            [this, &vg2]() {
                                for (std::size_t i = 0; i < _ops; ++i) {
                                    auto s = vg2();
                                }
                            });

                    if (discount_value_gen >= elapsed) {
                        throw std::runtime_error{
                                "Timer error. discount_value_gen: " + std::to_string(discount_value_gen) +
                                ", elapsed: " + std::to_string(elapsed)};
                    }

                    sqlite3_close(db);
                    rmfile(db_name);

                    auto net = elapsed - discount_value_gen;
                    return result(net, _ops, _key_size + _value_size);

                } catch (...) {
                    if (db) {
                        sqlite3_close(db);
                    }
                    rmfile(db_name);
                    throw;
                }
            };

#pragma GCC pop_options


        }
    }
}