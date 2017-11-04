//
// Created by efelici on 10/31/2017.
//

#ifndef STOR_STOR_BENCHMARK_H
#define STOR_STOR_BENCHMARK_H


#include <unordered_map>
#include <string>


namespace esft {
    namespace stor {
        namespace bmark {

            class stor_driver {
            public:

                stor_driver(std::size_t value_size, std::size_t key_size,  std::size_t ops);

                ~stor_driver();

                std::unordered_map<std::string, double> documents() const;

                std::unordered_map<std::string, double> writes() const;

                std::unordered_map<std::string, double> reads_by_key() const;

                std::unordered_map<std::string, double> reads_by_value() const;

            private:

                std::string _db_name;

                std::size_t _value_size;

                std::size_t _key_size;

                std::size_t _ops;


            };

        }


    }
}

#endif //STOR_STOR_BENCHMARK_H
