

#ifndef CSTOR_CCOLLECTION_H
#define CSTOR_CCOLLECTION_H

#include <stddef.h>
#include <cstor/errors/errors.h>


/**
 * @file ccollection.h
 * @brief C Bindings for collection.h
 **/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct esft_stor_collection_t  esft_stor_collection_t;

typedef struct esft_stor_document_t  esft_stor_document_t;

/**
 * @brief Returns the name of the collection @p c.
 *
 * TRANSFER OWNERSHIP ON SUCCESS: NO
 *
 *
 * @return collection name
 */
CSTOR_API const char * esft_stor_collection_name(esft_stor_collection_t *c);

/**
 * @brief Adds the index @p index_path to collection @p c.
 *
 * index_path syntax:
 *
 * i.e. Given the following JSON: {"a":{"b":1}} ,
 * the path to value 1 is "a.b"
 *
 * @return true if the path was added, false if not
 */
CSTOR_API bool esft_stor_collection_index_add(esft_stor_collection_t *c,
                                    const char * index_path);

/**
 * @brief Adds @p len index_paths provided by array @p index_paths
 */
CSTOR_API void esft_stor_collection_indices_add(esft_stor_collection_t *c,
                                      const char **index_paths,
                                      size_t len);

/**
 * @brief Returns an array of indices that the collection holds.
 * The length of the array is written to @p result_len
 * If the operation fails, NULL is returned, result_len is set to 0
 * and err code @p err is set accordingly.
 *
 * TRANSFER OWNERSHIP ON SUCCESS: YES
 * RESOURCE RELEASE FUNCTION: esft_stor_collection_index_list_delete
 *
 */
CSTOR_API char ** esft_stor_collection_index_list(esft_stor_collection_t *c,
                                        size_t *result_len,
                                        esft_stor_error_t *err);

/**
 * @brief deletes teh array of index_paths @p index_paths with len @p len
 */
CSTOR_API void esft_stor_collection_index_list_delete(char ** index_paths,
                                       size_t len);

/**
 * @brief Removes all index paths set on collection @p.
 *
 * @return true if at least one path was removed, false if not.
 */
CSTOR_API bool esft_stor_collection_index_remove_all(esft_stor_collection_t *c);

/**
 * @brief Puts (insert/replace) document @p doc in the collection @p c.
 * If the operation fails, the error code @p err is set accordingly.
 *
 */
CSTOR_API void esft_stor_collection_document_put(esft_stor_collection_t *c,
                                       esft_stor_document_t *doc,
                                       esft_stor_error_t *err);

/**
 * @brief Checks whether document with id @p doc_id is in the collection.
 *
 * @return true if document with id @p doc_id exists, false if not
 */
CSTOR_API bool esft_stor_collection_document_exists(esft_stor_collection_t *c,
                                          const char *doc_id);

/**
 * @brief Returns the document with id @p doc_id in the collection @p c or NULL
 * if such document does not exist.
 * If the operation fails, the error code @p err is set accordingly.
 *
 * TRANSFER OWNERSHIP ON SUCCESS: YES
 * RESOURCE RELEASE FUNCTION: esft_stor_document_delete
 *
 * @return document with id @p doc_id if it exists, NULL if it doesn't.
 */
CSTOR_API esft_stor_document_t *esft_stor_collection_document_get(esft_stor_collection_t *c,
                                                        const char *doc_id,
                                                        esft_stor_error_t *err);

/**
 * @brief Removes a document with id @p doc_id from the collection @p if it exists.
 *
 * The document's memory the id @p doc_id refers to is not freed by this function.
 *
 * @return true if document removed, false if not
 */
CSTOR_API bool esft_stor_collection_document_remove(esft_stor_collection_t *c,
                                          const char * doc_id);

/**
 * @brief Returns an array of documents as result of the query @p query executed
 * against the collection @p c. The array size is written in @p result_len. If
 * something goes wrong, NULL is returned and @p err is set accordingly.
 *
 * TRANSFER OWNERSHIP ON SUCCESS: YES
 * RESOURCE RELEASE FUNCTION: esft_stor_collection_query_result_delete
 *
 * @return query result as an array of documents
 *
 */
CSTOR_API esft_stor_document_t** esft_stor_collection_query(esft_stor_collection_t *c,
                                                             const char *query,
                                                             size_t *result_len,
                                                             esft_stor_error_t *err);

/**
 * @brief releases the resources of array of query result @p qr which has size @p qr_len
 */
CSTOR_API void esft_stor_collection_query_result_delete(esft_stor_document_t**qr, size_t qr_len);

/**
 * @brief releases the resources of collection @p c
 */
CSTOR_API void esft_stor_collection_delete(esft_stor_collection_t *c);


#ifdef __cplusplus
}
#endif


#endif //CSTOR_CCOLLECTION_H
