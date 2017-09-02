//
// Created by devhack on 04/06/16.
//
#include <fstream>
#include <memory>
#include <iostream>
#include <catch/catch.hpp>
#include <leveldb/options.h>
#include <leveldb/env.h>
#include <stor/exceptions/store_exception.h>
#include <stor/store/store.h>
#include <stor/store/collection_visitor.h>
#include <stor/store/access_manager.h>
#include <stor/exceptions/access_exception.h>
#include <stor/store/db_crypt.h>
#include <stor_test/query_test.h>
#include <stor_test/multiplatform_test_util.h>

using namespace esft;

TEST_CASE("stor_test catalog test","[catalog_test]"){

    std::string path = stor_test::home();

    stor::store db{path,"test",true};

    CHECK(db.home() ==  path + "test/");

    stor::collection c = db["test_collection"];

    CHECK(db.has(c.name()));

    db.remove(c.name());

    CHECK(!db.has(c.name()));

    db.put(c);

    CHECK(db.has(c.name()));

}


TEST_CASE("directory visitor test","[directory_visitor_test]"){
    leveldb::Options opt;
    leveldb::Env &env = *opt.env;

    env.CreateDir(stor_test::home() + "visitor_test");
    env.CreateDir(stor_test::home() + "visitor_test/dir1");
    env.CreateDir(stor_test::home() + "visitor_test/dir2");

    {
        std::ofstream ofs{stor_test::home() + "visitor_test/dir1/t1"};
        ofs << "hello";
    }
    {
        std::ofstream ofs{stor_test::home() + "visitor_test/dir2/t2"};
        ofs<< "hello";
    }

    std::set<std::string> paths;
    paths.insert(stor_test::home() + "visitor_test/dir1/t1");
    paths.insert(stor_test::home() + "visitor_test/dir2/t2");


    SECTION("level 1"){
        stor::collection_visitor visitor{stor_test::home() + "visitor_test"};
        std::set<std::string> tmp;
        for (auto it = visitor.cbegin(); it != visitor.cend(); ++it){
            CHECK(paths.find(*it) != paths.end());
            tmp.insert(*it);
        }
        CHECK(paths == tmp);
    }

    env.DeleteFile(stor_test::home() + "visitor_test/dir2/t2");
    env.DeleteFile(stor_test::home() + "visitor_test/dir1/t1");
    env.DeleteDir(stor_test::home() + "visitor_test/dir2");
    env.DeleteDir(stor_test::home() + "visitor_test/dir1");
    env.DeleteDir(stor_test::home() + "visitor_test");

}

TEST_CASE("db encryption test","[db_encryption_test]"){
    leveldb::Options opt;
    leveldb::Env &env = *opt.env;

    std::string fn1 = stor_test::home() + "encryption_test/dir1/t1";
    std::string fn2 = stor_test::home() + "encryption_test/dir2/t2";

    env.CreateDir(stor_test::home() + "encryption_test");
    env.CreateDir(stor_test::home() + "encryption_test/dir1");
    env.CreateDir(stor_test::home() + "encryption_test/dir2");

    {
        std::ofstream ofs{fn1};
        ofs << "hello";
    }
    {
        std::ofstream ofs{fn2};
        ofs<< "hello";
    }

    SECTION("successful commit"){

        class access_manager_mock: public stor::access_manager{
        public:
            access_manager_mock(const std::string &pwd):stor::access_manager{pwd} {}

            stor::access_manager::result encrypt(const std::string &i,
                                                 const std::string &o) const {
                std::ofstream ofs{o};
                ofs << "hello_encrypted";
                return stor::access_manager::ok;
            }

            stor::access_manager::result decrypt(const std::string &i,
                                                 const std::string &o) const {return stor::access_manager::ok;}
        };

        std::shared_ptr<stor::access_manager> ptr = std::make_shared<access_manager_mock>("fake_56789012345");

        stor::db_crypt enc{stor::db_crypt::mode::ENCRYPT, ptr};

        //if all good, enc returns true
        CHECK(enc(stor_test::home() + "encryption_test"));

        //check that temp files have been deleted
        CHECK_FALSE(std::ifstream{fn1 + "_encrypted"}.is_open());
        CHECK_FALSE(std::ifstream{fn2 + "_encrypted"}.is_open());
        CHECK_FALSE(std::ifstream{fn1 + "_tmp"}.is_open());
        CHECK_FALSE(std::ifstream{fn2 + "_tmp"}.is_open());

        //check that original files are intact
        std::string line;
        {
            std::ifstream ifs{fn1};
            REQUIRE(ifs.is_open());
            ifs>>line;
            CHECK(line == "hello_encrypted");
        }
        {
            std::ifstream ifs{fn2};
            REQUIRE(ifs.is_open());
            ifs>>line;
            CHECK(line == "hello_encrypted");
        }
    }

    SECTION("rollback"){
        class access_manager_mock: public stor::access_manager{
        public:
            access_manager_mock(const std::string &pwd):stor::access_manager{pwd} {}

            stor::access_manager::result encrypt(const std::string &i,
                                                 const std::string &o) const {
                std::ofstream ofs{o};
                ofs << "hello_encrypted_failed";
                return stor::access_manager::cipher_fail;
            }

            stor::access_manager::result decrypt(const std::string &i,
                                                 const std::string &o) const {return stor::access_manager::ok;}
        };

        std::shared_ptr<stor::access_manager> ptr = std::make_shared<access_manager_mock>(access_manager_mock("fake_56789012345"));

        stor::db_crypt enc{stor::db_crypt::mode::ENCRYPT, ptr};

        //if something goes wrong, enc returns false
        CHECK_FALSE(enc(stor_test::home() + "encryption_test"));

        //check that temp files have been deleted
        CHECK_FALSE(std::ifstream{fn1 + "_encrypted"}.is_open());
        CHECK_FALSE(std::ifstream{fn2 + "_encrypted"}.is_open());
        CHECK_FALSE(std::ifstream{fn1 + "_tmp"}.is_open());
        CHECK_FALSE(std::ifstream{fn2 + "_tmp"}.is_open());

        //check that original files are intact
        std::string line;
        {
            std::ifstream ifs{fn1};
            CHECK(ifs.is_open());
            ifs>>line;
            CHECK(line == "hello");
        }
        {
            std::ifstream ifs{fn2};
            CHECK(ifs.is_open());
            ifs>>line;
            CHECK(line == "hello");
        }

    }

    env.DeleteFile(fn1);
    env.DeleteFile(fn2);
    env.DeleteDir(stor_test::home() + "encryption_test/dir2");
    env.DeleteDir(stor_test::home() + "encryption_test/dir1");
    env.DeleteDir(stor_test::home() + "encryption_test");

}

TEST_CASE("db decryption test","[db_decryption_test]"){
    leveldb::Options opt;
    leveldb::Env &env = *opt.env;

    std::string fn1 = stor_test::home() + "decryption_test/dir1/t1";
    std::string fn2 = stor_test::home() + "decryption_test/dir2/t2";

    env.CreateDir(stor_test::home() + "decryption_test");
    env.CreateDir(stor_test::home() + "decryption_test/dir1");
    env.CreateDir(stor_test::home() + "decryption_test/dir2");

    {
        std::ofstream ofs{fn1};
        ofs << "hello_encrypted";
    }
    {
        std::ofstream ofs{fn2};
        ofs<< "hello_encrypted";
    }

    SECTION("successful commit"){

        class access_manager_mock: public stor::access_manager{
        public:
            access_manager_mock(const std::string &pwd):stor::access_manager{pwd} {}

            stor::access_manager::result decrypt(const std::string &i,
                                                 const std::string &o) const {
                std::ofstream ofs{o};
                ofs << "hello_decrypted";
                return stor::access_manager::ok;
            }

            stor::access_manager::result encrypt(const std::string &i,
                                                 const std::string &o) const {return stor::access_manager::ok;}
        };

        std::shared_ptr<stor::access_manager> ptr = std::make_shared<access_manager_mock>(access_manager_mock("fake_56789012345"));

        stor::db_crypt enc{stor::db_crypt::mode::DECRYPT, ptr};

        //if all good, enc returns true
        CHECK(enc(stor_test::home() + "decryption_test"));

        //check that temp files have been deleted
        CHECK_FALSE(std::ifstream{fn1 + "_decrypted"}.is_open());
        CHECK_FALSE(std::ifstream{fn2 + "_decrypted"}.is_open());
        CHECK_FALSE(std::ifstream{fn1 + "_tmp"}.is_open());
        CHECK_FALSE(std::ifstream{fn2 + "_tmp"}.is_open());

        //check that original files are intact
        std::string line;
        {
            std::ifstream ifs{fn1};
            CHECK(ifs.is_open());
            ifs>>line;
            CHECK(line == "hello_decrypted");
        }
        {
            std::ifstream ifs{fn2};
            CHECK(ifs.is_open());
            ifs>>line;
            CHECK(line == "hello_decrypted");
        }
    }

    SECTION("rollback"){
        class access_manager_mock: public stor::access_manager{
        public:
            access_manager_mock(const std::string &pwd):stor::access_manager{pwd} {}

            stor::access_manager::result decrypt(const std::string &i,
                                                 const std::string &o) const {
                std::ofstream ofs{o};
                ofs << "hello_decrypted_failed";
                return stor::access_manager::cipher_fail;
            }

            stor::access_manager::result encrypt(const std::string &i,
                                                 const std::string &o) const {return stor::access_manager::ok;}
        };

        std::shared_ptr<stor::access_manager> ptr = std::make_shared<access_manager_mock>(access_manager_mock("fake_56789012345"));

        stor::db_crypt dec{stor::db_crypt::mode::DECRYPT, ptr};

        //if something goes wrong, enc returns false
        CHECK_FALSE(dec(stor_test::home() + "decryption_test"));

        //check that temp files have been deleted
        CHECK_FALSE(std::ifstream{fn1 + "_decrypted"}.is_open());
        CHECK_FALSE(std::ifstream{fn2 + "_decrypted"}.is_open());
        CHECK_FALSE(std::ifstream{fn1 + "_tmp"}.is_open());
        CHECK_FALSE(std::ifstream{fn2 + "_tmp"}.is_open());

        //check that original files are intact
        std::string line;
        {
            std::ifstream ifs{fn1};
            CHECK(ifs.is_open());
            ifs>>line;
            CHECK(line == "hello_encrypted");
        }
        {
            std::ifstream ifs{fn2};
            CHECK(ifs.is_open());
            ifs>>line;
            CHECK(line == "hello_encrypted");
        }

    }

    env.DeleteFile(fn1);
    env.DeleteFile(fn2);
    env.DeleteDir(stor_test::home() + "decryption_test/dir2");
    env.DeleteDir(stor_test::home() + "decryption_test/dir1");
    env.DeleteDir(stor_test::home() + "decryption_test");
}

TEST_CASE("document addition-removal test","[mod_doc_test]"){
    stor::store db{stor_test::home(), "test_db",true};
    stor::collection c = db["test_collection"];
    c.add_index("a");
    c.persist();

    stor::document d{R"( {"a":1} )"};
    auto id = d.id();
    //insertion
    REQUIRE(c.put(d));
    CHECK(c.has(id));
    //modification
    d["a"] = 5;
    REQUIRE(c.put(d));
    CHECK(c.has(id));
    //removal
    REQUIRE(c.remove(id));
    CHECK_FALSE(c.has(id));
}

TEST_CASE("get document by id test","[get_by_id_test]"){
    stor::store db{stor_test::home(), "query_test",true};

    stor::collection c = db["test_collection"];

    stor::document d1 = stor::document::as_object();
    d1.put("a",1);
    c.put(d1);
    auto id = d1.id();

    REQUIRE(c.has(id));
    REQUIRE_FALSE(c.has("fake_id"));

    stor::document dz = c[id];
    REQUIRE(dz.id() == d1.id());
}

TEST_CASE("query not equality test","[neq_query_test]"){

    stor::store db{stor_test::home(), "query_test",true};

    stor::collection c = db["eq_test_collection"];

    //fill documents with some
    stor::document d1 = stor::document::as_object();
    d1.put("a",1);
    d1.put("z",true);
    d1.with("sub").put("b","aaa");

    stor::document d2 = stor::document::as_object();
    d2.put("a",2);
    d2.put("z",true);
    d2.with("sub").put("b","baa");

    c.add_index("a");
    c.add_index("z");
    c.add_index("sub.b");
    c.persist();

    c.put(d1);
    c.put(d2);

    SECTION("single successful hit"){
        auto res = c.find(R"( { "$neq":{"a": 1} } )");

        REQUIRE(res.size() == 1);
        CHECK((*res.begin()).id() == d2.id());

        res = c.find(R"( { "$neq":{"sub.b": "aaa"} } )");

        REQUIRE(res.size() == 1);
        CHECK((*res.begin()).id() == d2.id());
    }

    SECTION("multi successful hits"){
        auto res = c.find(R"( { "$neq":{"z": true} } )");
        CHECK(res.empty());
    }
}

TEST_CASE("query equality test","[eq_query_test]"){

    stor::store db{stor_test::home(), "query_test",true};

    stor::collection c = db["eq_test_collection"];

    //fill documents with some
    stor::document d1 = stor::document::as_object();
    d1.put("a",1);
    d1.put("z",true);
    d1.with("sub").put("b","aaa");

    stor::document d2 = stor::document::as_object();
    d2.put("a",2);
    d2.put("z",true);
    d2.with("sub").put("b","baa");

    c.add_index("a");
    c.add_index("z");
    c.add_index("sub.b");
    c.persist();

    c.put(d1);
    c.put(d2);


    SECTION("wrong query"){
        CHECK_THROWS(stor::query{R"( { "a":1 } )"});
        CHECK_THROWS(stor::query{""});
    }

    SECTION("single successful hit"){
        auto res = c.find(R"( { "$eq":{"a": 1} } )");

        REQUIRE(res.size() == 1);
        CHECK((*res.begin()).id() == d1.id());

        res = c.find(R"( { "$eq":{"sub.b": "aaa"} } )");

        REQUIRE(res.size() == 1);
        CHECK((*res.begin()).id() == d1.id());
    }

    SECTION("multi successful hits"){
        auto res = c.find(R"( { "$eq":{"z": true} } )");
        CHECK(res.size() == 2);

        std::unordered_set<stor::document::identifier> ids;
        ids.insert(d1.id());
        ids.insert(d2.id());
        for(auto it = res.begin(); it != res.end(); ++it){
            CHECK(ids.count((*it).id()) == 1);
            ids.erase((*it).id());
        }
        CHECK(ids.empty());

    }

    SECTION("unsuccessful hit"){
        auto res = c.find(R"( { "$eq":{"a": 3} } )");

        CHECK(res.empty());

        res = c.find(R"( { "$eq":{"sub.b": "lol"} } )");

        CHECK(res.empty());
    }
}

TEST_CASE("query greater than test","[gt_query_test]"){
    stor::store db{stor_test::home(), "query_test",true};

    stor::collection c = db["gt_test_collection"];

    stor::document d = stor::document::as_object();
    d.put("a",-10);
    d.put("b","a");

    stor::document d1 = stor::document::as_object();
    d1.put("a",500);
    d1.put("b","b");

    stor::document d2 = stor::document::as_object();
    d2.put("a",700);
    d2.put("b","c");

    stor::document d3 = stor::document::as_object();
    d3.put("a",900);
    d3.put("b","d");

    c.add_index("a");
    c.add_index("b");
    c.persist();

    c.put(d);
    c.put(d1);
    c.put(d2);
    c.put(d3);

    SECTION("no match"){
        auto res = c.find(R"( {"$gt": {"a": 900}} )");
        CHECK(res.size() == 0);

        //greater-equal
        res = c.find(R"( {"$gte": {"a": 901}} )");
        CHECK(res.size() == 0);
    }

    SECTION("1 match"){
        auto res = c.find(R"( {"$gt": {"a": 700}} )");
        REQUIRE(res.size() == 1);
        CHECK((*res.begin()).id() == d3.id());

        //greater-equal
        res = c.find(R"( {"$gte": {"a": 900}} )");
        REQUIRE(res.size() == 1);
        CHECK((*res.begin()).id() == d3.id());
    }

    SECTION("full match"){
        auto res = c.find(R"( {"$gt": {"a": -11}} )");
        REQUIRE(res.size() == 4);

        std::unordered_set<stor::document::identifier> ids;
        ids.insert(d.id());
        ids.insert(d1.id());
        ids.insert(d2.id());
        ids.insert(d3.id());

        for(auto it = res.begin(); it != res.end(); ++it){
            CHECK(ids.count((*it).id()) == 1);
            ids.erase((*it).id());
        }
        CHECK(ids.empty());

        ids.insert(d.id());
        ids.insert(d1.id());
        ids.insert(d2.id());
        ids.insert(d3.id());

        //greater-equal
        res = c.find(R"( {"$gte": {"a": -10}} )");
        REQUIRE(res.size() == 4);

        for(auto it = res.begin(); it != res.end(); ++it){
            CHECK(ids.count((*it).id()) == 1);
            ids.erase((*it).id());
        }
        CHECK(ids.empty());
    }

}

TEST_CASE("query less than test","[lt_query_test]"){
    stor::store db{stor_test::home(), "query_test",true};

    stor::collection c = db["gt_test_collection"];

    stor::document d = stor::document::as_object();
    d.put("a",-10);
    d.put("b","a");

    stor::document d1 = stor::document::as_object();
    d1.put("a",500);
    d1.put("b","b");

    stor::document d2 = stor::document::as_object();
    d2.put("a",700);
    d2.put("b","c");

    stor::document d3 = stor::document::as_object();
    d3.put("a",900);
    d3.put("b","d");

    c.add_index("a");
    c.add_index("b");
    c.persist();

    c.put(d);
    c.put(d1);
    c.put(d2);
    c.put(d3);

    SECTION("no match"){
        auto res = c.find(R"( {"$lt": {"a": -10}} )");
        CHECK(res.size() == 0);

        //smaller-equal
        res = c.find(R"( {"$lte": {"a": -11}} )");
        CHECK(res.size() == 0);
    }

    SECTION("1 match"){
        auto res = c.find(R"( {"$lt": {"a": 500}} )");
        REQUIRE(res.size() == 1);
        CHECK((*res.begin()).id() == d.id());

        //smaller-equal
        res = c.find(R"( {"$lte": {"a": -10}} )");
        REQUIRE(res.size() == 1);
        CHECK((*res.begin()).id() == d.id());
    }

    SECTION("full match"){
        auto res = c.find(R"( {"$lt": {"a": 901}} )");
        REQUIRE(res.size() == 4);

        std::unordered_set<stor::document::identifier> ids;
        ids.insert(d.id());
        ids.insert(d1.id());
        ids.insert(d2.id());
        ids.insert(d3.id());

        for(auto it = res.begin(); it != res.end(); ++it){
            CHECK(ids.count((*it).id()) == 1);
            ids.erase((*it).id());
        }
        CHECK(ids.empty());

        ids.insert(d.id());
        ids.insert(d1.id());
        ids.insert(d2.id());
        ids.insert(d3.id());

        //smaller-equal
        res = c.find(R"( {"$lte": {"a": 900}} )");
        REQUIRE(res.size() == 4);

        for(auto it = res.begin(); it != res.end(); ++it){
            CHECK(ids.count((*it).id()) == 1);
            ids.erase((*it).id());
        }
        CHECK(ids.empty());
    }

}

TEST_CASE("query or test","[or_query_test]"){
    stor::store db{stor_test::home(), "query_test",true};

    stor::collection c = db["or_test_collection"];

    //fill documents with some
    stor::document d1 = stor::document::as_object();
    d1.put("a",1);
    d1.put("z",true);
    d1.with("sub").put("b","aaa");

    stor::document d2 = stor::document::as_object();
    d2.put("a",2);
    d2.put("z",true);
    d2.with("sub").put("b","baa");

    c.add_index("a");
    c.add_index("z");
    c.add_index("sub.b");
    c.persist();

    c.put(d1);
    c.put(d2);

    SECTION("wrong query"){
      CHECK_THROWS(stor::query{R"( { "$or" : [{ "a": 1 }] } )"});
      CHECK_THROWS(stor::query{R"( { "$or" : [] } )"});
      CHECK_THROWS(stor::query{R"( { "$or" : [{ "$eq": { "a": 1 }}] } )"});
    }

    SECTION("or + equality"){


        //testing 0 keys match
        {
            stor::document q = stor::document::as_object();
            q.with_array("$or").add_object().with("$eq").put("a","UNKNWN");
            q.with_array("$or").add_object().with("$eq").put("a","UNKNWN2");
            auto res = c.find(q);
            REQUIRE(res.size() == 0);
        }

        //testing 1 key match
        {
        stor::document q = stor::document::as_object();
        q.with_array("$or").add_object().with("$eq").put("a",1);
        q.with_array("$or").add_object().with("$eq").put("a","UNKNWN");
        auto res = c.find(q);
        REQUIRE(res.size() == 1);
        CHECK((*res.begin()).id() == d1.id());
        }

        //testing 2 keys match
        {
            stor::document q = stor::document::as_object();
            q.with_array("$or").add_object().with("$eq").put("a",1);
            q.with_array("$or").add_object().with("$eq").put("a",2);
            auto res = c.find(q);
            REQUIRE(res.size() == 2);

            std::unordered_set<stor::document::identifier> ids;
            ids.insert(d1.id());
            ids.insert(d2.id());
            for(auto it = res.begin(); it != res.end(); ++it){
                CHECK(ids.count((*it).id()) == 1);
                ids.erase((*it).id());
            }
            CHECK(ids.empty());
        }



    }

}

TEST_CASE("query and test","[and_query_test]"){

    stor::store db{stor_test::home(), "query_test",true};

    stor::collection c = db["and_test_collection"];

    //fill documents with some
    stor::document d1 = stor::document::as_object();
    d1.put("a",1);
    d1.put("z",true);
    d1.with("sub").put("b","aaa");

    stor::document d2 = stor::document::as_object();
    d2.put("a",2);
    d2.put("z",true);
    d2.with("sub").put("b","baa");

    c.add_index("a");
    c.add_index("z");
    c.add_index("sub.b");
    c.persist();

    c.put(d1);
    c.put(d2);


    SECTION("and + equality"){

        //testing no match 1
        {
            stor::document q = stor::document::as_object();
            q.with_array("$and").add_object().with("$eq").put("a","UNKNWN");
            q.with_array("$and").add_object().with("$eq").put("z",false);
            auto res = c.find(q);
            REQUIRE(res.size() == 0);
        }

        //testing no match 2
        {
            stor::document q = stor::document::as_object();
            q.with_array("$and").add_object().with("$eq").put("a",1);
            q.with_array("$and").add_object().with("$eq").put("z",false);
            auto res = c.find(q);
            REQUIRE(res.size() == 0);
        }

        //testing no match 3
        {
            stor::document q = stor::document::as_object();
            q.with_array("$and").add_object().with("$eq").put("a",1);
            q.with_array("$and").add_object().with("$eq").put("sub.b","baa");
            auto res = c.find(q);
            REQUIRE(res.size() == 0);
        }

        //testing match
        {
            stor::document q = stor::document::as_object();
            q.with_array("$and").add_object().with("$eq").put("a",1);
            q.with_array("$and").add_object().with("$eq").put("sub.b","aaa");
            auto res = c.find(q);
            REQUIRE(res.size() == 1);
            CHECK((*res.begin()).id() == d1.id());
        }

    }

}

TEST_CASE("inflate deflate test","[inflate_deflate_test]") {

    stor::store db{stor_test::home(), "fldb", true};

    stor::collection c = db["coll"];
    c.add_index("a");
    c.persist();

    {
        std::ofstream ofs{stor_test::home() + "xflate.txt"};
        stor::document d{R"( { "a":1 } )"};
        c.put(d);
        c.deflate(ofs, false);
    }

    {
        std::ofstream ofs{stor_test::home() + "xflate2.txt"};
        std::ifstream ifs{stor_test::home() + "xflate.txt"};
        stor::collection c1 = db["coll2"];
        c1.add_index("a");
        c1.persist();
        c.inflate(ifs);
        c.deflate(ofs,false);
    }

    {
        std::ifstream ifs1{stor_test::home() + "xflate.txt"};
        std::ifstream ifs2{stor_test::home() + "xflate2.txt"};
        std::string line1;
        std::string line2;
        REQUIRE(ifs1.tellg() == ifs2.tellg());
        while(std::getline(ifs1,line1) && std::getline(ifs2,line2)){
            CHECK(line1 == line2);
        }
    }

    leveldb::Options opt;
    leveldb::Env &e = *opt.env;
    e.DeleteFile(stor_test::home() + "xflate.txt");
    e.DeleteFile(stor_test::home() + "xflate2.txt");
}