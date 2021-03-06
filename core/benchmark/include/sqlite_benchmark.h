//
// Created by efelici on 10/31/2017.
//

#ifndef STOR_SQLITE_BENCHMARK_H
#define STOR_SQLITE_BENCHMARK_H

#include <unordered_map>
#include <string>

namespace esft {
    namespace stor {
        namespace bmark {

            class sqlite_driver {
            public:

                sqlite_driver(std::size_t key_size, std::size_t value_size, std::size_t ops);

                ~sqlite_driver();

                std::unordered_map<std::string, double> writes() const;

                std::unordered_map<std::string, double> reads_by_key() const;

                std::unordered_map<std::string, double> reads_by_value() const;

            private:

                std::string _home;

                std::size_t _key_size;

                std::size_t _value_size;

                std::size_t _ops;

            };

        }


    }
}


#endif //STOR_SQLITE_BENCHMARK_H
