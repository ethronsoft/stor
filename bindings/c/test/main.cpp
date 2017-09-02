//
// Created by efelici on 8/30/2017.
//

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"




//int main(int argc, char **argv){
//
//    esft_stor_error_t err = esft_stor_error_init();
//
//    esft_stor_document_t *doc = esft_stor_document_create("{\"""a\""":1,\"""b\""": 2}",&err);
//    if (!doc){
//        printf("%s\n", esft_stor_error_string(err));
//        return 1;
//    }
//
//    esft_stor_node_t *root = esft_stor_document_root(doc,&err);
//
//
//    if(esft_stor_node_is_object(root)){
//        esft_stor_node_object_put_int(root,"c",3);
//        esft_stor_node_object_put_array(root, "d");
//        esft_stor_node_t *arr = esft_stor_node_object_get(root,"d",&err);
//        if (err){
//            printf("%s\n", esft_stor_error_string(err));
//            esft_stor_node_delete(root);
//            esft_stor_document_delete(doc);
//        }
//        esft_stor_node_array_add_int(arr,1);
//        esft_stor_node_array_add_int(arr,2);
//        esft_stor_node_array_add_int(arr,3);
//    }
//
//
//    esft_stor_node_keys_t keys = esft_stor_node_object_keys(root, &err);
//    for (size_t i = 0; i < keys.len; ++i){
//        esft_stor_node_t *node_a = esft_stor_node_object_get(root, keys.values[i], &err);
//        if (!node_a){
//            printf("%s\n", esft_stor_error_string(err));
//            esft_stor_document_delete(doc);
//            esft_stor_node_delete(root);
//            return 1;
//        }
//        if (esft_stor_node_is_int(node_a)){
//            int b = esft_stor_node_as_int(node_a);
//            printf("%s: %d\n", keys.values[i], b);
//        }else if (esft_stor_node_is_array(node_a)){
//            size_t len = esft_stor_node_size(node_a);
//            printf("[");
//            for (size_t i = 0; i < len; ++i){
//                esft_stor_node_t *x = esft_stor_node_array_get(node_a,i,&err);
//                printf("%d,", esft_stor_node_as_int(x));
//                esft_stor_node_delete(x);
//            }
//            printf("]\n");
//        }
//        esft_stor_node_delete(node_a);
//    }
//    esft_stor_node_object_keys_dispose(&keys);
//
//
//
//    printf("%s\n", esft_stor_document_id(doc));
//
//    esft_stor_json_t json = esft_stor_node_json(root);
//    printf("%s\n", json.value);
//    esft_stor_json_dispose(&json);
//
//    esft_stor_document_delete(doc);
//    esft_stor_node_delete(root);
//
//    return 0;
//}