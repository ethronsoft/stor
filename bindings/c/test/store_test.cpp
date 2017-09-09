//
// Created by efelici on 9/4/2017.
//

#include <catch/catch.hpp>
#include <cstor/store/cstore.h>
#include <cstring>
#include <cstor/store/ccollection.h>
#include <cstor/document/cdocument.h>

TEST_CASE("db init") {
    esft_stor_error_t err = esft_stor_error_init();
    const char *home = TEMP_DIR;

    esft_stor_store_t *db = esft_stor_store_open(home,"test",true,&err);
    REQUIRE(db);

    char h[strlen(home) + strlen("test/") + 1];
    strcpy(h, home);
    strcat(h, "test/");

    const char *db_home = esft_stor_store_home(db);
    CHECK(strcmp(h,db_home) == 0);

    esft_stor_store_delete(db);
}

TEST_CASE("db async flagging"){
    esft_stor_error_t err = esft_stor_error_init();
    const char *home = TEMP_DIR;

    esft_stor_store_t *db = esft_stor_store_open(home,"test",true,&err);
    REQUIRE(db);

    esft_stor_store_async(db,false);
    CHECK_FALSE(esft_stor_store_is_async(db));
    esft_stor_store_async(db,true);
    CHECK(esft_stor_store_is_async(db));

    esft_stor_store_delete(db);
}

TEST_CASE("collection creation/removal"){
    esft_stor_error_t err = esft_stor_error_init();
    const char *home = TEMP_DIR;

    esft_stor_store_t *db = esft_stor_store_open(home,"test",true,&err);
    REQUIRE(db);
    const char *collection_name = "my_collection";
    CHECK_FALSE(esft_stor_store_collection_exists(db,collection_name));
    esft_stor_collection_t *c = esft_stor_store_collection(db,collection_name,&err);
    if (err){
        esft_stor_store_delete(db);
        FAIL();
    }
    CHECK(esft_stor_store_collection_exists(db, collection_name));
    CHECK(strcmp(esft_stor_collection_name(c), collection_name) == 0);

    esft_stor_store_collection_remove(db,collection_name);
    CHECK_FALSE(esft_stor_store_collection_exists(db,collection_name));

    esft_stor_collection_delete(c);
    esft_stor_store_delete(db);
}

TEST_CASE("collection add/remove document"){
    esft_stor_error_t err = esft_stor_error_init();
    const char *home = TEMP_DIR;

    esft_stor_store_t *db = esft_stor_store_open(home,"test",true,&err);
    REQUIRE(db);
    const char *collection_name = "my_collection";
    esft_stor_collection_t *c = esft_stor_store_collection(db,collection_name,&err);
    if (err){
        esft_stor_store_delete(db);
        FAIL();
    }

    esft_stor_document_t *doc = esft_stor_document_create("{\"""a\""":1,\"""b\""": 2}", &err);
    if(err){
        esft_stor_collection_delete(c);
        esft_stor_store_delete(db);
        FAIL();
    }
    esft_stor_collection_document_put(c,doc,&err);
    if (err){
        esft_stor_collection_delete(c);
        esft_stor_store_delete(db);
        esft_stor_document_delete(doc);
    }
    CHECK(esft_stor_collection_document_exists(c,esft_stor_document_id(doc)));

    esft_stor_document_t *x = esft_stor_collection_document_get(c,esft_stor_document_id(doc),&err);
    CHECK(x);
    esft_stor_document_delete(x);

    esft_stor_collection_document_remove(c,esft_stor_document_id(doc));
    x = esft_stor_collection_document_get(c,esft_stor_document_id(doc),&err);
    CHECK_FALSE(x);
    esft_stor_document_delete(x);

    esft_stor_collection_delete(c);
    esft_stor_store_delete(db);
    esft_stor_document_delete(doc);
}

TEST_CASE("collection query document"){
    esft_stor_error_t err = esft_stor_error_init();
    const char *home = TEMP_DIR;

    esft_stor_store_t *db = esft_stor_store_open(home,"test",true,&err);
    REQUIRE(db);
    const char *collection_name = "my_collection";
    esft_stor_collection_t *c = esft_stor_store_collection(db,collection_name,&err);
    if (err){
        esft_stor_store_delete(db);
        FAIL();
    }
    if (!esft_stor_collection_index_add(c, "a")){
        esft_stor_collection_delete(c);
        esft_stor_store_delete(db);
        FAIL();
    }

    esft_stor_document_t *doc = esft_stor_document_create("{\"""a\""":1,\"""b\""": 2}", &err);
    if(err){
        esft_stor_collection_delete(c);
        esft_stor_store_delete(db);
        FAIL();
    }
    esft_stor_collection_document_put(c,doc,&err);
    if (err){
        esft_stor_collection_delete(c);
        esft_stor_store_delete(db);
        esft_stor_document_delete(doc);
    }
    const char *query = "{\"""$eq\""": {\"""a\""": 1}}";
    size_t len = 0;
    esft_stor_document_t ** query_res = esft_stor_collection_query(c,query,&len,&err);
    if (err){
        esft_stor_collection_delete(c);
        esft_stor_store_delete(db);
        FAIL();
    }
    CHECK(len == 1);
    CHECK(strcmp(esft_stor_document_id(query_res[0]),esft_stor_document_id(doc)) == 0);

    esft_stor_collection_delete(c);
    esft_stor_store_delete(db);
    esft_stor_document_delete(doc);
    esft_stor_collection_query_result_delete(query_res, len);
}