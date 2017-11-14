#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>
#include <catch/catch.hpp>
#include <stor/document/document.h>
#include <stor/document/const_iterator.h>

using namespace esft::stor;


TEST_CASE("document parsing json","[document_test]"){


    SECTION("object parsing"){
        const std::string s = R"( {"a": 1, "b": 2.0,
                                   "c": 80000000, "d": true,
                                    "e": "hello" } )";

        CHECK_NOTHROW(document{s});

        document d{s};
        CHECK(d.is_object());
        CHECK(d.has("a"));
        CHECK(d["a"].is_int());
        CHECK(d["a"].as_int() == 1);

        CHECK(d.has("b"));
        CHECK(d["b"].is_double());
        CHECK(d["b"].as_double() == 2.0);

        CHECK(d.has("c"));
        CHECK(d["c"].is_long());
        CHECK(d["c"].as_long() == 80000000);

        CHECK(d.has("d"));
        CHECK(d["d"].is_bool());
        CHECK(d["d"].as_bool() == true);


        CHECK(d.has("e"));
        CHECK(d["e"].is_string());
        CHECK(std::string(d["e"].as_string()) == "hello");
        auto p = d["e"].as_cstring();
        CHECK(strcmp("hello", p.first) == 0);
    }
    
    SECTION("array parsing"){
        std::string s = R"( [1,"hello",2.0,false,80000000] )";
        
        CHECK_NOTHROW(document{s});
        
        document d{s};
        CHECK(d.is_array());

        CHECK(d.size() == 5);
        CHECK(d[0].is_int());
        CHECK(d[0].as_int() == 1);
        CHECK(d[1].is_string());
        CHECK(std::string(d[1].as_string()) == "hello");
        CHECK(d[2].is_double());
        CHECK(d[2].as_double() == 2.0);
        CHECK(d[3].is_bool());
        CHECK(d[3].as_bool() == false);
        CHECK(d[4].is_long());
        CHECK(d[4].as_long() == 80000000);
    }

    SECTION("value parsing"){
        CHECK_NOTHROW(document{"1"});
        document d1{"1"};
        CHECK(d1.is_int());
        CHECK(d1.as_int() == 1);

        CHECK_NOTHROW(document{"2.0"});
        document d2{"2.0"};
        CHECK(d2.is_double());
        CHECK(d2.as_double() == 2.0);

        CHECK_NOTHROW(document{"true"});
        document d3{"true"};
        CHECK(d3.is_bool());
        CHECK(d3.as_bool());

        CHECK_NOTHROW(document{"8000000"});
        document d4{"8000000"};
        CHECK(d4.is_long());
        CHECK(d4.as_long() == 8000000);

        CHECK_NOTHROW(document{"\"true\""});
        document d5{"\"true\""};
        CHECK(d5.is_string());
        CHECK(std::string(d5.as_string()) == "true");
    }
}

TEST_CASE("document creation","[document_test]"){

    SECTION("object creation"){
        document d = document::as_object();
        d.put("a",1).put("b",2.0).put("c",80000000)
                .put("d",true).put("e","hello").with("z").put("zz",1);
        d.with_array("az").add(1);

        CHECK(d.is_object());

        CHECK(d.has("a"));
        CHECK(d["a"].is_int());
        CHECK(d["a"].as_int() == 1);

        CHECK(d.has("b"));
        CHECK(d["b"].is_double());
        CHECK(d["b"].as_double() == 2.0);

        CHECK(d.has("c"));
        CHECK(d["c"].is_long());
        CHECK(d["c"].as_long() == 80000000);

        CHECK(d.has("d"));
        CHECK(d["d"].is_bool());
        CHECK(d["d"].as_bool() == true);

        CHECK(d.has("e"));
        CHECK(d["e"].is_string());
        CHECK(std::string(d["e"].as_string()) == "hello");

        CHECK_FALSE(d.has("zz"));

        CHECK(d.has("z"));
        CHECK(d["z"].is_object());
        CHECK(d["z"]["zz"].is_int());
        CHECK(d["z"]["zz"].as_int() == 1);

        CHECK(d.has("az"));
        CHECK(d["az"].is_array());
        CHECK(d["az"].size() == 1);
        CHECK(d["az"][0].as_int() == 1);
    }

    SECTION("array creation"){
        document d = document::as_array();
        d.add(1).add("hello").add(2.0).add(false).add(80000000);
        d.add_array().add(1);
        d.add_object().put("a",1);
        //test iterator version of add
        std::vector<int> v{1,2,3,4};
        d.add_array().add(v.begin(),v.end());

        CHECK(d.is_array());

        CHECK(d.size() == 8);
        CHECK(d[0].is_int());
        CHECK(d[0].as_int() == 1);
        CHECK(d[1].is_string());
        CHECK(std::string(d[1].as_string()) == "hello");
        CHECK(d[2].is_double());
        CHECK(d[2].as_double() == 2.0);
        CHECK(d[3].is_bool());
        CHECK(d[3].as_bool() == false);
        CHECK(d[4].is_long());
        CHECK(d[4].as_long() == 80000000);
        CHECK(d[5].is_array());
        CHECK(d[5][0].is_int());
        CHECK(d[5][0].as_int() == 1);
        CHECK(d[6].is_object());
        CHECK(d[6].has("a"));
        CHECK(d[6]["a"].as_int() == 1);
        CHECK(d[7].is_array());
        for (size_t i = 0; i < d[7].size();++i){
            CHECK(d[7][i].is_int());
            CHECK(d[7][i].as_int() == (i+1));
        }
    }

    SECTION("deep copy object"){
        document d = document{R"( {"a":1, "b":2} )"};
        document d2 = document{R"( {"c": 2, "c":3} )"};

        REQUIRE(d.json() != d2.json());
        d.copy(d2);
        REQUIRE(d.json() == d2.json());
        //deep copy
        d.put("z",5);
        CHECK(d.has("z"));
        CHECK_FALSE(d2.has("z"));

    }

    SECTION("deep copy array"){
        document d = document{R"( [1,2,3] )"};
        document d2 = document{R"( [4,5,6] )"};

        auto d_sz = d.size();
        auto d2_sz = d2.size();
        REQUIRE(d.json() != d2.json());
        d.copy(d2);
        REQUIRE(d.json() == d2.json());
        //deep copy
        d.add(10);
        CHECK(d.size() == d_sz+1);
        CHECK(d2.size()== d2_sz);
    }

    SECTION("deep copy change type"){
        document d = document::as_object();
        document d2 = document::as_array();

        d.json("[]");
        CHECK(d.is_array());
        d2.json("{}");
        CHECK(d2.is_object());
        d.json("5");
        CHECK(d.is_num());
    }


    SECTION("object move"){
        auto d = document{R"( {"a":1, "b":2.0} )"};
        CHECK(d.is_object());
        CHECK(d.size() == 2);
        auto d2 = std::move(d);
        CHECK(d2.is_object());
        CHECK(d2.size() == 2);
        CHECK(d2.has("a"));
        CHECK(d2["a"].is_int());
        CHECK(d2.has("b"));
        CHECK(d2["b"].is_double());

    }


}

TEST_CASE("document modification", "[document_test]"){


    SECTION("modifying by replacement"){
        std::string s = R"( {"a":1,"b":[true, { "z":2 }, 3]} )";
        document d{s};

        REQUIRE(d["a"].as_int() == 1);
        d["a"] = 2;
        CHECK(d["a"].as_int() == 2);

        REQUIRE(d["b"][0].as_bool() == true);
        d["b"][0] = false;
        CHECK(d["b"][0].as_bool() == false);

        REQUIRE(d["b"][1]["z"].as_int() == 2);
        d["b"][1].put("z",5);
        int a = d["b"][1]["z"].as_int();
        REQUIRE(d["b"][1]["z"].as_int() == 5);
    }

    SECTION("modifying by removal"){
        document d1 = document::as_object();
        REQUIRE(d1.empty());
        d1.put("a", 1).put("b", 2)
                .with_array("c").add(std::vector<int>{1, 2, 3});
        REQUIRE(d1.size() == 3);
        d1.remove("a");
        CHECK_FALSE(d1.has("a"));
        CHECK(d1.size() == 2);
        d1.remove_all();
        CHECK(d1.empty());

        document d2 = document::as_array();
        REQUIRE(d2.empty());
        d2.add(std::vector<int>{1, 2, 3});
        REQUIRE(d2.size() == 3);
        d2.remove_all();
        CHECK(d2.empty());
        
        //iterator removal
        //object
        d1.put("a", 1).put("b", 2);
        
        auto it = d1.cbegin();
        for (std::size_t len = d1.size(); it != d1.cend(); --len){
            it = d1.remove(it);
            CHECK(d1.size() == len-1);
        }
        CHECK(it == d1.cend());

        d1.put("a", 1).put("b", 2);
        auto last = d1.remove(d1.cbegin(),d1.cend());
        CHECK(last == d1.cend());
        CHECK(d1.empty());
        //array
        d2.add(std::vector<int>{1, 2, 3});
        auto it2 = d2.cbegin();
        for (std::size_t len = d2.size(); it2 != d2.cend(); --len){
            it2 = d2.remove(it2);
            CHECK(d2.size() == len-1);
        }
        CHECK(it2 == d2.cbegin());

        d2.add(std::vector<int>{1, 2, 3});
        auto last2 = d2.remove(d2.begin(),d2.end());
        CHECK(last2 == d2.cend());
        CHECK(d2.empty());
        
        //value
        d1.put("a", 1);
        CHECK_THROWS_AS(d1["a"].remove(d1["a"].cbegin()),document_exception);
    }
}

TEST_CASE("document streams","[document_test]"){
    SECTION("create with stream"){
        std::string s = R"( {"a":1,"b":[true, { "z":2 }]} )";
        std::istringstream iss{s};

        document d;
        CHECK_NOTHROW(iss >> d);

        CHECK(d.is_object());
        CHECK(d.has("a"));
        CHECK(d["b"].is_array());
        CHECK(d["b"][1].is_object());
        CHECK(d.has("b"));
    }

    SECTION("write to stream"){
        std::string s = R"({"a":1,"b":true})";
        document d{s};

        std::ostringstream oss;
        oss << d;

        CHECK(s == oss.str());
    }
}

TEST_CASE("document uniqueness","[document_test]")
{
    document prev;
    bool ok = true;
    for (int i = 0; i < 1000; ++i){
        document n;
        ok &= (n.id() != prev.id());
        prev = std::move(n);
    }
    CHECK(ok);
}