

#include <iostream>
#include <chrono>
#include <thread>
#include <utils.h>
#include <sqlite_benchmark.h>
#include <stor_benchmark.h>
#include <stor/document/document.h>
#include <leveldb_driver.h>

int main(int argc, char **argv) {

    /*****************
     * Configuration *
     *****************/

    //number of bytes in a key
    std::size_t key_len = esft::stor::document::make_id().length() + 1;

    //number of bytes in a value (including '\0')
    std::size_t value_len = 50;

    //number of performed operations
    std::size_t ops = 10000;

    //order list of columns for write
    //test. -- must match those
    //returned by the benchmark drivers
    std::vector<std::string> write_columns{
            "micros/ops",
            "MB/sec",
            "IOPS"
    };

    //order list of columns for read
    //test. -- must match those
    //returned by the benchmark drivers
    std::vector<std::string> read_columns{
            "micros/ops",
            "MB/sec",
            "IOPS"
    };


    /*****************
     *    Results    *
     *****************/

    esft::stor::bmark::sqlite_driver _sqlite_driver{value_len, key_len, ops};
    esft::stor::bmark::stor_driver _stor_driver{value_len, key_len, ops};
    esft::stor::bmark::leveldb_driver _leveldb_driver{value_len, key_len, ops};


    //reads by key
    auto stor_reads_by_key = _stor_driver.reads_by_key();
    auto sql_reads_by_key = _sqlite_driver.reads_by_key();
    auto leveldb_reads_by_key = _leveldb_driver.reads_by_key();

    //reads by value query
    auto stor_reads_by_value = _stor_driver.reads_by_value();
    auto sqlite_reads_by_value = _sqlite_driver.reads_by_value();

    //writes
    auto stor_writes = _stor_driver.writes();
    auto sqlite_writes = _sqlite_driver.writes();
    auto leveldb_writes = _leveldb_driver.writes();

    //document generation
    auto docs_gen = _stor_driver.documents();


    /*****************
     * Presentation  *
     *****************/

    //general info
    std::cout << std::boolalpha;
    std::cout.precision(10);

    std::cout << "STOR VERSION: " << ESFT_STOR_VERSION << "\n";
    std::cout << "Document Key Bytes: " << key_len << "\n";
    std::cout << "Document Value Bytes: " << value_len << "\n";
    std::cout << "Caching: " << false << "\n";
    std::cout << "Operations: " << ops << "\n";
    std::cout << "LevelDB Block Size: " << 4096 << "\n"; //as per: https://github.com/google/leveldb/blob/master/doc/index.md
    std::cout << "SQLITE page size: " << 4096 << "\n"; //as per: https://sqlite.org/pragma.html#pragma_page_size
    std::cout << "\n";

    //docs gen
    esft::stor::bmark::print_map("STOR: Documents", docs_gen);

    //reads by key results
    esft::stor::bmark::print_map("LevelDB: Doc Reads by Key", leveldb_reads_by_key, read_columns);
    esft::stor::bmark::print_map("STOR: Doc Reads by Key", stor_reads_by_key, read_columns);
    esft::stor::bmark::print_map("SQLITE: Doc Reads by Key", sql_reads_by_key, read_columns);

    //reads by query result
    esft::stor::bmark::print_map("STOR: Doc Reads by Indexed Value", stor_reads_by_value, read_columns);
    esft::stor::bmark::print_map("SQLITE: Doc Reads by Indexed Value", sqlite_reads_by_value, read_columns);

    //writes results
    esft::stor::bmark::print_map("LevelDB: Doc Writes", leveldb_writes, write_columns);
    esft::stor::bmark::print_map("STOR: Doc Writes", stor_writes, write_columns);
    esft::stor::bmark::print_map("SQLITE: Doc Writes", sqlite_writes, write_columns);


    return 0;
}