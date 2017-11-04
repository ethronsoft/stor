//
// Created by efelici on 10/31/2017.
//

#include <stor/store/store.h>
#include <stor_benchmark.h>
#include <utils.h>
#include <atomic>
#include <sstream>

namespace esft {
    namespace stor {
        namespace bmark {

#pragma GCC push_options
#pragma GCC optimize ("O0")

            stor_driver::stor_driver(std::size_t value_size, std::size_t key_size, std::size_t ops) :
                    _db_name{"_stor_benchmark"}, _value_size{value_size}, _key_size{key_size}, _ops{ops} { }

            stor_driver::~stor_driver() {

            }

            std::unordered_map<std::string, double> stor_driver::documents() const {


                //value to use for writes (implicitly counting '\0')
                //sample_value = {"a":"x"} -> 7 characters + '\0' + len(x)
                auto _value_gen1 = value_generator(_value_size - 7 - 1);
                auto _value_gen2 = value_generator(_value_size - 7 - 1);


                auto uuid_ids = timer<std::chrono::microseconds>{}(
                        [this, &_value_gen1]() {
                            for (std::size_t i = 0; i < _ops; ++i) {
                                auto doc = document(json_generator(_value_gen1));
                            }
                        });

                std::atomic<std::uint64_t> _l;
                auto incr_ids = timer<std::chrono::microseconds>{}(
                        [this, &_l, &_value_gen2]() {
                            for (std::size_t i = 0; i < _ops; ++i) {
                                auto doc = document(json_generator(_value_gen2), std::to_string(_l++));
                            }
                        });

                return std::unordered_map<std::string, double>{
                        {"micros/uuid id", double(uuid_ids) / _ops},
                        {"micros/incr id", double(incr_ids) / _ops}
                };
            }

            std::unordered_map<std::string, double> stor_driver::writes() const {
                store db{"", _db_name, true};
                db.set_async(true);
                auto &coll = db["bmark"];

                auto _value_gen1 = value_generator(_value_size - 7 - 1);
                auto _value_gen2 = value_generator(_value_size - 7 - 1);

                //more predictable performance than document::make_id()
                //which is benchmarked separately.
                value_generator kg{_key_size};

                auto runner = [this, &coll, &kg, &_value_gen1]() {
                    for (std::size_t i = 0; i < _ops; ++i) {
                        coll.put(document(json_generator(_value_gen1), kg()));
                    }
                };

                auto elapsed = timer<std::chrono::microseconds>{}(runner);


                auto discount_docs_init = timer<std::chrono::microseconds>{}(
                        [this, &kg, &_value_gen2]() {
                            for (std::size_t i = 0; i < _ops; ++i) {
                                auto doc = document(json_generator(_value_gen2), kg());
                            }
                        });


                if (discount_docs_init >= elapsed) {
                    throw std::runtime_error{"Timer error. discount_docs_init: " +
                                             std::to_string(discount_docs_init) +
                                             ", elapsed: " + std::to_string(elapsed)};
                }

                //net amount of `duration`-seconds that it took to execute the runner
                auto net = elapsed - discount_docs_init;
                return result(net, _ops, _key_size + _value_size);
            }

            std::unordered_map<std::string, double> stor_driver::reads_by_key() const {
                store db{"", _db_name, true};
                auto &coll = db["bmark"];

                //filling
                value_generator write_kg{_key_size};

                value_generator read_kg(_key_size);
                value_generator read_kg2(_key_size);

                //value to use for writes (implicitly counting '\0')
                //sample_value = {"a":"x"} -> 7 characters + '\0' + len(x)
                auto _value = json_generator(value_generator(_value_size - 7 - 1));

                for (std::size_t i = 0; i < _ops; ++i) {
                    coll.put(document(_value, write_kg()));
                }

                //testing
                auto runner = [this, &coll, &read_kg]() {
                    for (std::size_t i = 0; i < _ops; ++i) {
                        auto key = read_kg();
                        auto d = coll[key];
                    }
                };

                auto elapsed = timer<std::chrono::microseconds>{}(runner);


                auto discount_key_gen = timer<std::chrono::microseconds>{}(
                        [this, &read_kg2]() {
                            for (std::size_t i = 0; i < _ops; ++i) {
                                auto s = read_kg2();
                            }
                        });


                if (discount_key_gen >= elapsed) {
                    throw std::runtime_error{"Timer error. discount_key_gen: " +
                                             std::to_string(discount_key_gen) +
                                             ", elapsed: " + std::to_string(elapsed)};
                }


                //net amount of `duration`-seconds that it took to execute the runner
                auto net = elapsed - discount_key_gen;
                return result(net, _ops, _key_size + _value_size);

            }

            std::unordered_map<std::string, double> stor_driver::reads_by_value() const {
                store db{"", _db_name, true};
                auto &coll = db["bmark"];
                coll.add_index("a");

                //filling
                value_generator write_kg{_key_size};

                value_generator read_kg(write_kg);

                //value to use for writes (implicitly counting '\0')
                //sample_value = {"a":"x"} -> 7 characters + '\0' + len(x)
                auto _value_gen1 = value_generator(_value_size - 7 - 1);
                auto _value_gen2 = value_generator(_value_size - 7 - 1);

                for (std::size_t i = 0; i < _ops; ++i) {
                    coll.put(document(json_generator(_value_gen1), write_kg()));
                }

                //testing
                auto query_gen = [&_value_gen2]() {
                    auto q = document::as_object();
                    q.with("$eq").put("a", _value_gen2());
                    return q;
                };

                auto runner = [this, &coll, &query_gen]() {
                    for (std::size_t i = 0; i < _ops; ++i) {
                        auto q = query_gen();
                        auto res = coll.find(q);
                    }
                };

                auto elapsed = timer<std::chrono::microseconds>{}(runner);


                auto discount_query_gen = timer<std::chrono::microseconds>{}(
                        [this, &query_gen]() {
                            for (std::size_t i = 0; i < _ops; ++i) {
                                auto s = query_gen();
                            }
                        });


                if (discount_query_gen >= elapsed) {
                    throw std::runtime_error{"Timer error. discount_query_gen: " +
                                             std::to_string(discount_query_gen) +
                                             ", elapsed: " + std::to_string(elapsed)};
                }

                //net amount of `duration`-seconds that it took to execute the runner
                auto net = elapsed - discount_query_gen;
                return result(net, _ops, _key_size + _value_size);
            }


#pragma GCC pop_options

        }
    }
}