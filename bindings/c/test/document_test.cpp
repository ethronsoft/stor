//
// Created by efelici on 9/1/2017.
//

#include <catch/catch.hpp>

#include <stdio.h>
#include <string.h>
#include <cstor/document/cdocument.h>

TEST_CASE("construction from json string", "[document]") {
    esft_stor_error_t err = esft_stor_error_init();

    SECTION("correct object") {
        esft_stor_document_t *doc = esft_stor_document_create("{\"""a\""":1,\"""b\""": 2}", &err);
        REQUIRE_FALSE(err);
        esft_stor_node_t *root = esft_stor_document_root(doc, &err);
        REQUIRE_FALSE(err);
        CHECK(esft_stor_node_is_object(root));
        esft_stor_node_delete(root);
        esft_stor_document_delete(doc);
    }

    SECTION("correct array") {
        esft_stor_document_t *doc = esft_stor_document_create("[1,2,3]", &err);
        REQUIRE_FALSE(err);
        esft_stor_node_t *root = esft_stor_document_root(doc, &err);
        REQUIRE_FALSE(err);
        CHECK(esft_stor_node_is_array(root));
        esft_stor_node_delete(root);
        esft_stor_document_delete(doc);
    }

    SECTION("correct value") {
        esft_stor_document_t *doc = esft_stor_document_create("5", &err);
        REQUIRE_FALSE(err);
        esft_stor_node_t *root = esft_stor_document_root(doc, &err);
        REQUIRE_FALSE(err);
        CHECK(esft_stor_node_is_num(root));
        esft_stor_node_delete(root);
        esft_stor_document_delete(doc);
    }

    SECTION("wrong") {
        esft_stor_document_t *doc = esft_stor_document_create("1,", &err);
        CHECK(err);
        REQUIRE_FALSE(doc);
        err = no_error;
        doc = esft_stor_document_create("{\"""a\""":1,\"""b\""": 2", &err);
        CHECK(err);
        REQUIRE_FALSE(doc);
        err = no_error;
        doc = esft_stor_document_create("[1,2,3", &err);
        CHECK(err);
        REQUIRE_FALSE(doc);
    }

    SECTION("repurposed node from json"){
        esft_stor_document_t *doc = esft_stor_document_create("[1,2,3]", &err);
        esft_stor_node_t *root = esft_stor_document_root(doc, &err);
        esft_stor_node_from_json(root,"5",&err);
        REQUIRE_FALSE(err);
        CHECK(esft_stor_node_is_num(root));
        esft_stor_node_delete(root);
        esft_stor_document_delete(doc);
    }

    SECTION("repurposed node from node"){
        esft_stor_document_t *doc = esft_stor_document_create("[1,2,3]", &err);
        esft_stor_document_t *doc2 = esft_stor_document_create("{}", &err);

        esft_stor_node_t *root = esft_stor_document_root(doc, &err);
        esft_stor_node_t *root2 = esft_stor_document_root(doc2, &err);

        esft_stor_node_from_node(root,root2,&err);
        REQUIRE_FALSE(err);
        CHECK(esft_stor_node_is_object(root));

        esft_stor_node_delete(root);
        esft_stor_document_delete(doc);
        esft_stor_node_delete(root2);
        esft_stor_document_delete(doc2);
    }


}

TEST_CASE("construction from empty node", "[document]") {
    esft_stor_error_t err = esft_stor_error_init();
    SECTION("object node") {
        esft_stor_document_t *doc = esft_stor_document_create_as_object(&err);
        REQUIRE_FALSE(err);
        esft_stor_node_t *root = esft_stor_document_root(doc, &err);
        REQUIRE_FALSE(err);
        CHECK(esft_stor_node_is_object(root));
        esft_stor_node_delete(root);
        esft_stor_document_delete(doc);
    }

    SECTION("object array") {
        esft_stor_document_t *doc = esft_stor_document_create_as_array(&err);
        REQUIRE_FALSE(err);
        esft_stor_node_t *root = esft_stor_document_root(doc, &err);
        REQUIRE_FALSE(err);
        CHECK(esft_stor_node_is_array(root));
        esft_stor_node_delete(root);
        esft_stor_document_delete(doc);
    }
}

TEST_CASE("value read/write", "[document]") {
    esft_stor_error_t err = esft_stor_error_init();

    SECTION("object int") {
        esft_stor_document_t *doc = esft_stor_document_create_as_object(&err);
        esft_stor_node_t *root = esft_stor_document_root(doc, &err);

        int value = 1;
        esft_stor_node_object_put_int(root, "a", value);

        esft_stor_node_t *node_a = esft_stor_node_object_get(root, "a", &err);
        REQUIRE_FALSE(err);
        REQUIRE(node_a);
        REQUIRE(esft_stor_node_is_int(node_a));
        CHECK(esft_stor_node_as_int(node_a) == value);

        esft_stor_node_delete(node_a);
        esft_stor_node_delete(root);
        esft_stor_document_delete(doc);
    }

    SECTION("object long") {
        esft_stor_document_t *doc = esft_stor_document_create_as_object(&err);
        esft_stor_node_t *root = esft_stor_document_root(doc, &err);

        long long value = 1;
        esft_stor_node_object_put_long(root, "a", value);

        esft_stor_node_t *node_a = esft_stor_node_object_get(root, "a", &err);
        REQUIRE_FALSE(err);
        REQUIRE(node_a);
        REQUIRE(esft_stor_node_is_long(node_a));
        CHECK(esft_stor_node_as_long(node_a) == value);

        esft_stor_node_delete(node_a);
        esft_stor_node_delete(root);
        esft_stor_document_delete(doc);
    }

    SECTION("object double") {
        esft_stor_document_t *doc = esft_stor_document_create_as_object(&err);
        esft_stor_node_t *root = esft_stor_document_root(doc, &err);

        double value = 1.0;
        esft_stor_node_object_put_double(root, "a", value);

        esft_stor_node_t *node_a = esft_stor_node_object_get(root, "a", &err);
        REQUIRE_FALSE(err);
        REQUIRE(node_a);
        REQUIRE(esft_stor_node_is_double(node_a));
        CHECK(esft_stor_node_as_double(node_a) == value);

        esft_stor_node_delete(node_a);
        esft_stor_node_delete(root);
        esft_stor_document_delete(doc);
    }

    SECTION("object bool") {
        esft_stor_document_t *doc = esft_stor_document_create_as_object(&err);
        esft_stor_node_t *root = esft_stor_document_root(doc, &err);

        bool value = false;
        esft_stor_node_object_put_bool(root, "a", value);

        esft_stor_node_t *node_a = esft_stor_node_object_get(root, "a", &err);
        REQUIRE_FALSE(err);
        REQUIRE(node_a);
        REQUIRE(esft_stor_node_is_bool(node_a));
        CHECK(esft_stor_node_as_bool(node_a) == value);

        esft_stor_node_delete(node_a);
        esft_stor_node_delete(root);
        esft_stor_document_delete(doc);
    }

    SECTION("object string") {
        esft_stor_document_t *doc = esft_stor_document_create_as_object(&err);
        esft_stor_node_t *root = esft_stor_document_root(doc, &err);

        const char *value = "hello";
        esft_stor_node_object_put_string(root, "a", value);

        esft_stor_node_t *node_a = esft_stor_node_object_get(root, "a", &err);
        REQUIRE_FALSE(err);
        REQUIRE(node_a);
        REQUIRE(esft_stor_node_is_string(node_a));
        CHECK(strcmp(esft_stor_node_as_string(node_a), value) == 0);

        esft_stor_node_delete(node_a);
        esft_stor_node_delete(root);
        esft_stor_document_delete(doc);
    }

    SECTION("object object") {
        esft_stor_document_t *doc = esft_stor_document_create_as_object(&err);
        esft_stor_node_t *root = esft_stor_document_root(doc, &err);

        esft_stor_node_object_put_obj(root, "a");

        esft_stor_node_t *node_a = esft_stor_node_object_get(root, "a", &err);
        REQUIRE_FALSE(err);
        REQUIRE(node_a);
        REQUIRE(esft_stor_node_is_object(node_a));

        esft_stor_node_delete(node_a);
        esft_stor_node_delete(root);
        esft_stor_document_delete(doc);
    }

    SECTION("object array") {
        esft_stor_document_t *doc = esft_stor_document_create_as_object(&err);
        esft_stor_node_t *root = esft_stor_document_root(doc, &err);

        esft_stor_node_object_put_array(root, "a");

        esft_stor_node_t *node_a = esft_stor_node_object_get(root, "a", &err);
        REQUIRE_FALSE(err);
        REQUIRE(node_a);
        REQUIRE(esft_stor_node_is_array(node_a));

        esft_stor_node_delete(node_a);
        esft_stor_node_delete(root);
        esft_stor_document_delete(doc);
    }


    SECTION("array int") {
        esft_stor_document_t *doc = esft_stor_document_create_as_array(&err);
        esft_stor_node_t *root = esft_stor_document_root(doc, &err);

        int value = 1;
        esft_stor_node_array_add_int(root, value);

        esft_stor_node_t *node_a = esft_stor_node_array_get(root, 0, &err);
        REQUIRE_FALSE(err);
        REQUIRE(node_a);
        REQUIRE(esft_stor_node_is_int(node_a));
        CHECK(esft_stor_node_as_int(node_a) == value);

        esft_stor_node_delete(node_a);
        esft_stor_node_delete(root);
        esft_stor_document_delete(doc);
    }

    SECTION("array long") {
        esft_stor_document_t *doc = esft_stor_document_create_as_array(&err);
        esft_stor_node_t *root = esft_stor_document_root(doc, &err);

        long long value = 1;
        esft_stor_node_array_add_long(root, value);

        esft_stor_node_t *node_a = esft_stor_node_array_get(root, 0, &err);
        REQUIRE_FALSE(err);
        REQUIRE(node_a);
        REQUIRE(esft_stor_node_is_long(node_a));
        CHECK(esft_stor_node_as_long(node_a) == value);

        esft_stor_node_delete(node_a);
        esft_stor_node_delete(root);
        esft_stor_document_delete(doc);
    }

    SECTION("array double") {
        esft_stor_document_t *doc = esft_stor_document_create_as_array(&err);
        esft_stor_node_t *root = esft_stor_document_root(doc, &err);

        double value = 1.0;
        esft_stor_node_array_add_double(root, value);

        esft_stor_node_t *node_a = esft_stor_node_array_get(root, 0, &err);
        REQUIRE_FALSE(err);
        REQUIRE(node_a);
        REQUIRE(esft_stor_node_is_double(node_a));
        CHECK(esft_stor_node_as_double(node_a) == value);

        esft_stor_node_delete(node_a);
        esft_stor_node_delete(root);
        esft_stor_document_delete(doc);
    }

    SECTION("array bool") {
        esft_stor_document_t *doc = esft_stor_document_create_as_array(&err);
        esft_stor_node_t *root = esft_stor_document_root(doc, &err);

        bool value = false;
        esft_stor_node_array_add_bool(root, value);

        esft_stor_node_t *node_a = esft_stor_node_array_get(root, 0, &err);
        REQUIRE_FALSE(err);
        REQUIRE(node_a);
        REQUIRE(esft_stor_node_is_bool(node_a));
        CHECK(esft_stor_node_as_bool(node_a) == value);

        esft_stor_node_delete(node_a);
        esft_stor_node_delete(root);
        esft_stor_document_delete(doc);
    }

    SECTION("array string") {
        esft_stor_document_t *doc = esft_stor_document_create_as_array(&err);
        esft_stor_node_t *root = esft_stor_document_root(doc, &err);

        const char *value = "hello";
        esft_stor_node_array_add_string(root, value);

        esft_stor_node_t *node_a = esft_stor_node_array_get(root, 0, &err);
        REQUIRE_FALSE(err);
        REQUIRE(node_a);
        REQUIRE(esft_stor_node_is_string(node_a));
        CHECK(strcmp(esft_stor_node_as_string(node_a), value) == 0);

        esft_stor_node_delete(node_a);
        esft_stor_node_delete(root);
        esft_stor_document_delete(doc);
    }

    SECTION("array object") {
        esft_stor_document_t *doc = esft_stor_document_create_as_array(&err);
        esft_stor_node_t *root = esft_stor_document_root(doc, &err);

        esft_stor_node_array_add_obj(root);

        esft_stor_node_t *node_a = esft_stor_node_array_get(root, 0, &err);
        REQUIRE_FALSE(err);
        REQUIRE(node_a);
        REQUIRE(esft_stor_node_is_object(node_a));

        esft_stor_node_delete(node_a);
        esft_stor_node_delete(root);
        esft_stor_document_delete(doc);
    }

    SECTION("array array") {
        esft_stor_document_t *doc = esft_stor_document_create_as_array(&err);
        esft_stor_node_t *root = esft_stor_document_root(doc, &err);

        esft_stor_node_array_add_array(root);

        esft_stor_node_t *node_a = esft_stor_node_array_get(root, 0, &err);
        REQUIRE_FALSE(err);
        REQUIRE(node_a);
        REQUIRE(esft_stor_node_is_array(node_a));

        esft_stor_node_delete(node_a);
        esft_stor_node_delete(root);
        esft_stor_document_delete(doc);
    }


}