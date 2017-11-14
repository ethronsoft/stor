//
// Created by Edd on 25/06/2017.
//
#include <catch/catch.hpp>
#include <stor/store/access_manager.h>
#include <stor/exceptions/access_exception.h>

#ifdef STOR_CRYPTO

#include <iostream>
#include <fstream>
#include <stor_test/multiplatform_test_util.h>

TEST_CASE("set up","[setup]"){

    using namespace esft::stor;

    std::string short_key = "";

    CHECK_THROWS_AS(std::unique_ptr<access_manager>(new access_manager(short_key)),access_exception);

    std::string valid_key = "0123456789012345";

    CHECK_NOTHROW(std::unique_ptr<access_manager>(new access_manager(valid_key)));
}

TEST_CASE("correct key encryption-decryption","[correct_key]"){
    std::string input1_file_name = esft::stor_test::home() + "/input.txt";
    std::string input2_file_name = esft::stor_test::home() + "/output.txt";
    std::string output1_file_name = esft::stor_test::home() + "/output.txt";
    std::string output2_file_name = esft::stor_test::home() + "/inputz.txt";

    using namespace esft::stor;

    SECTION("small file"){
        std::ofstream ofs{input1_file_name};
        for (int i = 0; i < 100; ++i){
            ofs << char(i);
        }

        std::string key = "0123456789012345";

        auto am = std::unique_ptr<access_manager>(new access_manager(key));

        access_manager::result eok;
        CHECK_NOTHROW(eok = am->encrypt(input1_file_name,output1_file_name));
        CHECK(eok == 1);

        access_manager::result dok;
        CHECK_NOTHROW(dok = am->decrypt(input2_file_name,output2_file_name));
        CHECK(dok == 1);
    }

    SECTION("big file"){
        std::ofstream ofs{input1_file_name};
        for (int i = 0; i < 1000000; ++i){
            ofs << char(i % 255);
        }

        std::string key = "0123456789012345";

        auto am = std::unique_ptr<access_manager>(new access_manager(key));

        access_manager::result eok;
        CHECK_NOTHROW(eok = am->encrypt(input1_file_name,output1_file_name));
        CHECK(eok == 1);

        access_manager::result dok;
        CHECK_NOTHROW(dok = am->decrypt(input2_file_name,output2_file_name));
        CHECK(dok == 1);
    }

}

TEST_CASE("wrong key encryption-decryption","[wrong_key]"){
    std::string input1_file_name = esft::stor_test::home() + "/input.txt";
    std::string input2_file_name = esft::stor_test::home() + "/output.txt";
    std::string output1_file_name = esft::stor_test::home() + "/output.txt";
    std::string output2_file_name = esft::stor_test::home() + "/inputz.txt";

    using namespace esft::stor;

    SECTION("small file"){
        std::ofstream ofs{input1_file_name};
        for (int i = 0; i < 100; ++i){
            ofs << char(i);
        }

        {
            std::string key = "0123456789012345";

            auto am = std::unique_ptr<access_manager>(new access_manager(key));

            access_manager::result eok;
            CHECK_NOTHROW(eok = am->encrypt(input1_file_name,output1_file_name));
            CHECK(eok == 1);
        }
        {
            std::string key = "0123456789012342";

            auto am = std::unique_ptr<access_manager>(new access_manager(key));

            access_manager::result dok;
            CHECK_NOTHROW(dok = am->decrypt(input2_file_name,output2_file_name));
            CHECK(dok != 1);
        }
    }

    SECTION("big file"){
        std::ofstream ofs{input1_file_name};
        for (int i = 0; i < 1000000; ++i){
            ofs << char(i % 255);
        }


        {
            std::string key = "0123456789012345";

            auto am = std::unique_ptr<access_manager>(new access_manager(key));

            access_manager::result eok;
            CHECK_NOTHROW(eok = am->encrypt(input1_file_name,output1_file_name));
            CHECK(eok == 1);
        }
        {
            std::string key = "0123456789012342";

            auto am = std::unique_ptr<access_manager>(new access_manager(key));

            access_manager::result dok;
            CHECK_NOTHROW(dok = am->decrypt(input2_file_name,output2_file_name));
            CHECK(dok != 1);
        }
    }

}

TEST_CASE("tampered file failure","[tampering]"){
    std::string input1_file_name = esft::stor_test::home() + "/input.txt";
    std::string input2_file_name = esft::stor_test::home() + "/output.txt";
    std::string output1_file_name = esft::stor_test::home() + "/output.txt";
    std::string output2_file_name = esft::stor_test::home() + "/inputz.txt";

    using namespace esft::stor;


    std::ofstream ofs{input1_file_name};
    for (int i = 0; i < 1000000; ++i){
        ofs << char(i % 255);
    }

    std::string key = "0123456789012345";

    auto am = std::unique_ptr<access_manager>(new access_manager(key));

    access_manager::result eok;
    CHECK_NOTHROW(eok = am->encrypt(input1_file_name,output1_file_name));
    CHECK(eok == 1);

    //TAMPERING
    {
        std::fstream fs{input2_file_name, std::ios_base::out | std::ios_base::binary | std::ios_base::ate};
        fs << 'a';
    }

    access_manager::result dok;
    CHECK_NOTHROW(dok = am->decrypt(input2_file_name,output2_file_name));
    CHECK(dok != 1);
}

#else

TEST_CASE("no access_manager", "[store_test]"){
    using namespace esft::stor;

    CHECK_THROWS_AS(access_manager{"0123456789012345"}.encrypt("a","b"),access_exception);
    CHECK_THROWS_AS(access_manager{"0123456789012345"}.decrypt("a","b"),access_exception);
}

#endif