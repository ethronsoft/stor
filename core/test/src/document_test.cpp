#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>
#include <catch/catch.hpp>
#include <stor/document/document.h>
#include <stor/document/const_iterator.h>

using namespace esft::stor;


TEST_CASE("document parsing json","[json_parse]"){


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

TEST_CASE("document creation","[creation]"){

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

}

TEST_CASE("document streams","[streams]"){
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

TEST_CASE("document uniqueness","[doc_uniqueness]")
{
    document prev;
    for (int i = 0; i < 10; ++i){
        document n;
        CHECK(n.id() != prev.id());
        prev = std::move(n);
    }
}