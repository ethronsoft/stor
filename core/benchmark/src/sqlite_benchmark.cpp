

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

                    std::size_t exec(std::vector<std::string> values) {
                        for (std::size_t i = 0; i < values.size(); ++i) {
                            sqlite3_bind_text(_stmnt, i + 1, values[i].c_str(), -1, nullptr);
                        }
                        sqlite3_step(_stmnt);
                        sqlite3_reset(_stmnt);
                        return values.size();
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
                    value_generator vg1(_value_size - 7 - 1);


                    auto runner = [this, &writes, &kg1, &vg1]() {
                        std::size_t some_value_to_prevent_optimizaton = 0;
                        for (std::size_t i = 0; i < _ops; ++i) {
                            std::string jsn = json_generator(vg1);
                            std::string id = kg1();
                            document doc(jsn, id);
                            some_value_to_prevent_optimizaton += doc.id().size();
                            some_value_to_prevent_optimizaton += writes.exec({id, jsn});
                        }
                        writes.flush();
                        return some_value_to_prevent_optimizaton;
                    };

                    auto elapsed = timer<std::chrono::microseconds>{}(runner);


                    sqlite3_close(db);
                    rmfile(db_name);

                    return result(elapsed, _ops, _key_size + _value_size);
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
                        std::size_t some_value_to_prevent_optimizaton = 0;
                        for (std::size_t i = 0; i < _ops; ++i) {
                            auto res = reads.exec({kg1()});
                            some_value_to_prevent_optimizaton += res.size();
                        }
                        reads.flush();
                        return some_value_to_prevent_optimizaton;
                    };

                    auto elapsed = timer<std::chrono::microseconds>{}(runner);

                    sqlite3_close(db);
                    rmfile(db_name);

                    return result(elapsed, _ops, _key_size + _value_size);

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
                    auto _value_gen2 = value_generator(_value_size - 7 - 1);
                    //to simulate the same query generation made by STOR
                    auto query_gen = [&_value_gen2]() {
                        auto q = document("{}","fake_id");
                        q.with("$eq").put("a", _value_gen2());
                        return q;
                    };

                    auto runner = [this, &reads, &vg1, &query_gen]() {
                        std::size_t some_value_to_prevent_optimizaton = 0;
                        for (std::size_t i = 0; i < _ops; ++i) {
                            auto q = query_gen();
                            some_value_to_prevent_optimizaton+= q.size();
                            auto res = reads.exec({json_generator(vg1)});
                            some_value_to_prevent_optimizaton += res.size();
                        }
                        reads.flush();
                        return some_value_to_prevent_optimizaton;
                    };

                    auto elapsed = timer<std::chrono::microseconds>{}(runner);

                    sqlite3_close(db);
                    rmfile(db_name);

                    return result(elapsed, _ops, _key_size + _value_size);

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