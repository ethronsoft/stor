//
// Created by efelici on 9/1/2017.
//

#ifndef CSTOR_CCOLLECTION_H
#define CSTOR_CCOLLECTION_H

#include <stddef.h>
#include <cstor/errors/errors.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef struct esft_stor_collection_t  esft_stor_collection_t;

typedef struct esft_stor_document_t  esft_stor_document_t;

typedef struct esft_stor_collection_query_result_t{
    size_t len;
    esft_stor_document_t * *values;
} esft_stor_collection_query_result_t;


const char * esft_stor_collection_name(esft_stor_collection_t *c);

bool esft_stor_collection_index_add(esft_stor_collection_t *c,
                                    const char * index_path);

bool esft_stor_collection_index_remove_all(esft_stor_collection_t *c);

void esft_stor_collection_document_put(esft_stor_collection_t *c,
                                       esft_stor_document_t *doc,
                                       esft_stor_error_t *err);

bool esft_stor_collection_document_exists(esft_stor_collection_t *c,
                                          const char *doc_id);

esft_stor_document_t *esft_stor_collection_document_get(esft_stor_collection_t *c,
                                                        const char *doc_id,
                                                        esft_stor_error_t *err);

bool esft_stor_collection_document_remove(esft_stor_collection_t *c,
                                          const char * doc_id);

esft_stor_collection_query_result_t esft_stor_collection_query(esft_stor_collection_t *c,
                                                             const char *query,
                                                             esft_stor_error_t *err);

void esft_stor_collection_query_result_dispose(esft_stor_collection_query_result_t *res);

void esft_stor_collection_delete(esft_stor_collection_t *c);


#ifdef __cplusplus
}
#endif


#endif //CSTOR_CCOLLECTION_H
