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

typedef struct esft_stor_collection_name_t{
    const char *value;
} esft_stor_collection_name_t;

typedef struct esft_stor_collection_query_result{
    size_t len;
    esft_stor_document_t *values;
};


esft_stor_collection_name_t esft_stor_collection_name(esft_stor_collection_t *c,
                                                      esft_stor_error_t *err);

void esft_stor_collection_name_dispose(esft_stor_collection_name_t *);

esft_stor_document_t *esft_stor_collection_document(const char * doc_id,
                                                    esft_stor_error_t *err);

void esft_stor_collection_document_remove(const char * doc_id);

esft_stor_collection_query_result esft_stor_collection_query(const char *query,
                                                             esft_stor_error_t *err);

void esft_stor_collection_delete(esft_stor_collection_t *c);


#ifdef __cplusplus
}
#endif


#endif //CSTOR_CCOLLECTION_H
