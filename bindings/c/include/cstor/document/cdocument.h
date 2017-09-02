//
// Created by efelici on 8/30/2017.
//

#ifndef PROJECT_CSTOR_DOCUMENT_H
#define PROJECT_CSTOR_DOCUMENT_H


#include <cstor/errors/errors.h>
#include <cstor/document/cnode.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef struct esft_stor_node_t esft_stor_node_t;

typedef struct esft_stor_document_t esft_stor_document_t;

/**
 * @brief creates a document backed by a JSON object
 * and returns ownership of it.
 * If operation fails, returns NULL and sets the error code
 *
 * @return JSON object document or NULL.

 *
 */
esft_stor_document_t *esft_stor_document_create_as_object(esft_stor_error_t *e);

/**
 * @brief creates a document backed by a JSON array
 * and returns ownership of it.
 * If operation fails, returns NULL and sets the error
 * code
 *
 * @return JSON array document or NULL.
 */
esft_stor_document_t *esft_stor_document_create_as_array(esft_stor_error_t *e);

/**
 * @brief creates a document constructed from JSON @p json
 * and returns ownership of it.
 * If operation fails, returns NULL and sets the error code
 *
 * @return a document constructed from JSON @p json or NULL
 */
esft_stor_document_t *esft_stor_document_create(const char *json,
                                                       esft_stor_error_t *e);

/**
 * @brief returns the identifier of the document @p doc
 * If operation fails, returns NULL.
 *
 * @return document id or NULL.
 */
const char *esft_stor_document_id(esft_stor_document_t *doc);

/**
 * @brief returns the root of the document @p doc as a node
 * If the operation fails, returns NULL
 *
 * @return node root or NULL.
 *
 */
esft_stor_node_t* esft_stor_document_root(esft_stor_document_t *doc,
                                                 esft_stor_error_t *e);

/**
 * @brief deletes the object esft_stor_document_t.
 */
extern void esft_stor_document_delete(esft_stor_document_t *doc);

#ifdef __cplusplus
}
#endif

#endif //PROJECT_CSTOR_DOCUMENT_H
