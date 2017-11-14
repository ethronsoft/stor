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
                            std::size_t some_value_to_prevent_optimizaton = 0;
                            for (std::size_t i = 0; i < _ops; ++i) {
                                auto doc = document(json_generator(_value_gen1));
                                some_value_to_prevent_optimizaton+=doc.id().size();
                            }
                            return some_value_to_prevent_optimizaton;
                        });

                std::atomic<std::uint64_t> _l;
                auto incr_ids = timer<std::chrono::microseconds>{}(
                        [this, &_l, &_value_gen2]() {
                            std::size_t some_value_to_prevent_optimizaton = 0;
                            for (std::size_t i = 0; i < _ops; ++i) {
                                auto doc = document(json_generator(_value_gen2), std::to_string(_l++));
                                some_value_to_prevent_optimizaton+=doc.id().size();
                            }
                            return some_value_to_prevent_optimizaton;
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
                    std::size_t some_value_to_prevent_optimizaton = 0;
                    for (std::size_t i = 0; i < _ops; ++i) {
                        int x = (int) coll.put(document(json_generator(_value_gen1), kg()));
                        some_value_to_prevent_optimizaton += x;
                    }
                    return some_value_to_prevent_optimizaton;
                };

                auto elapsed = timer<std::chrono::microseconds>{}(runner);

                return result(elapsed, _ops, _key_size + _value_size);
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
                    std::size_t some_value_to_prevent_optimizaton = 0;
                    for (std::size_t i = 0; i < _ops; ++i) {
                        auto key = read_kg();
                        auto d = coll[key];
                        some_value_to_prevent_optimizaton+= d.id().size();
                    }
                    return some_value_to_prevent_optimizaton;
                };

                auto elapsed = timer<std::chrono::microseconds>{}(runner);

                return result(elapsed, _ops, _key_size + _value_size);

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
                    auto q = document("{}","fake_id");
                    q.with("$eq").put("a", _value_gen2());
                    return q;
                };

                auto runner = [this, &coll, &query_gen]() {
                    std::size_t some_value_to_prevent_optimizaton = 0;
                    for (std::size_t i = 0; i < _ops; ++i) {
                        auto q = query_gen();
                        auto res = coll.find(q);
                        some_value_to_prevent_optimizaton += res.size();
                    }
                    return some_value_to_prevent_optimizaton;
                };

                auto elapsed = timer<std::chrono::microseconds>{}(runner);

                return result(elapsed, _ops, _key_size + _value_size);
            }


#pragma GCC pop_options

        }
    }
}