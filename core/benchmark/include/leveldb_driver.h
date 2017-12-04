

#ifndef STOR_LEVELDB_BENCHMARK_H
#define STOR_LEVELDB_BENCHMARK_H

#include <string>
#include <unordered_map>

namespace esft {
    namespace stor {
        namespace bmark {

            class leveldb_driver {
            public:

                leveldb_driver(std::size_t key_size, std::size_t value_size, std::size_t ops);

                ~leveldb_driver();

                std::unordered_map<std::string, double> writes() const;

                std::unordered_map<std::string, double> reads_by_key() const;

            private:

                std::string _home;

                std::size_t _key_size;

                std::size_t _value_size;

                std::size_t _ops;
            };

        }
    }
}



#endif //STOR_LEVELDB_BENCHMARK_H
