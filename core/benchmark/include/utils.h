

#ifndef STOR_BENCHMARK_H
#define STOR_BENCHMARK_H

#include <cstddef>
#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <leveldb/env.h>
#include <leveldb/options.h>
#include <set>

namespace esft {
    namespace stor {
        namespace bmark {

            inline void rmdir(const std::string &db) {
                leveldb::Options opt;
                leveldb::Env &env = *opt.env;
                env.DeleteDir(db);
            }

            inline void rmfile(const std::string &file){
                leveldb::Options opt;
                leveldb::Env &env = *opt.env;
                env.DeleteFile(file);
            }

            inline void mkdir(const std::string &db) {
                leveldb::Options opt;
                leveldb::Env &env = *opt.env;
                auto s = env.CreateDir(db);
                if (!s.ok()) {
                    throw std::runtime_error{"Failed creating SQLite DB folder"};
                }
            }

            inline std::unordered_map<std::string, double> result(double net, std::size_t ops, std::size_t data_size) {
                const std::size_t MB = 1024 * 1024;
                const long micros = 1000000;
                const double sec_ratio = double(1) / micros;


                return std::unordered_map<std::string, double>{
                        {"micros/ops", net / ops},
                        {"IOPS",       long(micros / (net / ops))},
                        {"MB/sec",     (double(data_size) * ops / MB) / (sec_ratio * net)},
                };
            };

#pragma GCC push_options
#pragma GCC optimize ("O0")


            /**
             * As presented and explained here:
             * https://stackoverflow.com/questions/40122141/preventing-compiler-optimizations-while-benchmarking
             */
            static void escape(void *p) {
                asm volatile("" : : "g"(p) : "memory");
            }

            static void clobber() {
                asm volatile("" : : : "memory");
            }
            /**
             * //////////////////////////////////////////////////////////////////////////////////////////////////
             */

            template<typename duration>
            class timer {
            public:

                template<typename Func, typename... Args>
                long long int operator()(Func &&f, Args &&... args) const {
                    auto start = std::chrono::high_resolution_clock::now();
                    int rc;
                    escape(&rc);
                    rc = f(std::forward<Args>(args)...);
                    clobber();
                    return std::chrono::duration_cast<duration>(std::chrono::high_resolution_clock::now() - start).count();
                }

            };

            class value_generator {
            public:
                value_generator(size_t total_size) {

                    std::ostringstream oss;

                    std::size_t max_chars = 25 + 25 + 10; //25 lower char, 25 upper, 10 digits
                    if (total_size > max_chars) {
                        std::logic_error{"Current implementation does not support the given value size"};
                    }

                    for (std::size_t i = 0; i < 10; ++i) {
                        oss << i;
                    }

                    for (std::size_t i = 0; i < 25; ++i) {
                        oss << char('A' + i);
                    }

                    for (std::size_t i = 0; i < 25; ++i) {
                        oss << char('a' + i);
                    }

                    auto tmp = oss.str();

                    base = std::string(tmp.cbegin(), tmp.cbegin() + (total_size));
                }

                const std::string &operator()() {
                    std::next_permutation(base.begin(), base.end());
                    return base;
                }

            private:
                std::string base;

            };

            template<typename _value_gen>
            std::string json_generator(_value_gen &&kg) {
                std::ostringstream oss;
                oss << "{\"a\":\""
                    << kg()
                    << "\"}";
                return oss.str();
            }

            inline void print_map(std::string map_name,
                           const std::unordered_map<std::string, double> &map,
                           std::vector<std::string> columns = {},
                           std::size_t line_length = 80) {
                //trim name
                map_name = std::string(map_name.cbegin(),map_name.cbegin() + (std::min(line_length, map_name.size())));
                if (columns.empty()) {
                    std::transform(map.cbegin(), map.cend(), std::back_inserter(columns),
                                   [](const std::pair<std::string, double> &p) { return p.first; });
                }

                std::cout << std::string(line_length, '-') << "\n";
                std::cout << std::string((line_length / 2) - map_name.length() / 2, ' ')
                << map_name
                << std::string(line_length - (line_length / 2) - map_name.length(), ' ') << "\n";

                if (!map.empty()){
                    for (const auto &column: columns) {
                        std::cout << column << ": " << map.at(column) << "\n";
                    }
                }

                std::cout << std::string(line_length, '-') << "\n";
            }

            inline std::unordered_map<std::string, double> compare(const std::unordered_map<std::string, double> &map1,
                                                            const std::unordered_map<std::string, double> &map2) {
                std::set<std::string> col1;
                std::set<std::string> col2;
                std::transform(map1.cbegin(),map1.cend(),std::inserter(col1,col1.begin()),
                               [](const std::pair<std::string, double> &p){ return p.first;});
                std::transform(map2.cbegin(),map2.cend(),std::inserter(col2,col2.begin()),
                               [](const std::pair<std::string, double> &p){ return p.first;});

                if (col1 != col2){
                    throw std::runtime_error{"Cannot compare maps with different keys"};
                }

                std::unordered_map<std::string, double> res;
                for (const auto &c: col1){
                    res.emplace(c, map1.at(c)/map2.at(c));
                }

                return res;
            }

#pragma GCC pop_options

        }
    }
}

#endif //STOR_BENCHMARK_H
