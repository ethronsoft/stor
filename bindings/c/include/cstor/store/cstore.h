//
// Created by efelici on 9/1/2017.
//

#ifndef CSTOR_CSTORE_H
#define CSTOR_CSTORE_H

#include <cstor/errors/errors.h>

/**
 * @file cstore.h
 * @brief C Bindings for store.h
 **/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct esft_stor_collection_t esft_stor_collection_t;

typedef struct esft_stor_store_t esft_stor_store_t;

/**
 * @brief callback function type, to be notified of a failure
 * during encryption of cstore
 */
typedef void(*encryption_failure_handler)();

/**
 * @brief Opens/Creates a database at @p store_home with name @p store_name.
 * If remove_on_delete == true, the database and all of its files will be deleted
 * upon destruction of the store instance via @code esft_stor_store_delete.
 * If the operation fails, the @p err is set accordingly.
 *
 * TRANSFER OWNERSHIP ON SUCCESS: YES
 * RESOURCE RELEASE FUNCTION: esft_stor_store_delete
 *
 * @return cstore
 */
CSTOR_API esft_stor_store_t *esft_stor_store_open(const char *store_home,
                                        const char *store_name,
                                        bool remove_on_delete,
                                        esft_stor_error_t *err);

/**
 * @brief Attempts to Open/Create a database at @p store_home with name @p store_name
 * with password @p key. Before the store instance is destroyed via @p code esft_stor_store_delete
 * the store will be encrypted with password @p key. If this operation fails, the database
 * is left unencrypted but in a valid state and the function @p f is called.
 *
 * TRANSFER OWNERSHIP ON SUCCESS: YES
 * RESOURCE RELEASE FUNCTION: esft_stor_store_delete
 *
 * @return encryption-enabled cstor
 */
CSTOR_API esft_stor_store_t *esft_stor_store_open_encrypted(const char *store_home,
                                                  const char *store_name,
                                                  const char *key,
                                                  encryption_failure_handler f,
                                                  esft_stor_error_t *err);

/**
 * @brief Returns the home path of the store, where all of its files are going to be
 * written to.
 *
 * TRANSFER OWNERSHIP ON SUCCESS: NO
 *
 * @return store home
 */
CSTOR_API const char *esft_stor_store_home(esft_stor_store_t *store);

/**
 * @brief Sets the store write operations as async (async == true) or sync (async = false).
 * Async operations are not guaranteed to be written to file on completion but they complete
 * more quickly in normal circumstances. Particularly useful if client is only writing to the
 * store and no synchronization is therefore needed when reading.
 *
 */
CSTOR_API void esft_stor_store_async(esft_stor_store_t *store, bool async);

/**
 * @brief checks whether the store in async mode.
 *
 * @return true if in async mode, false if in sync mode.
 * @see esft_stor_store_async
 */
CSTOR_API bool esft_stor_store_is_async(esft_stor_store_t *store);

/**
 * @brief Gets(creates if non existing) a collection with name @p collection_name from the store.
 * Although the collection ownership is returned to the caller, the collection itself is dependant
 * on the store @p store. Therefore, the colleciton should not be used after @p store has been
 * destroyed via @code esft_stor_store_delete.
 *
 * TRANSFER OWNERSHIP ON SUCCESS: YES
 * RESOURCE RELEASE FUNCTION: esft_stor_collection_delete
 *
 * @return collection with name @p collection_name
 */
CSTOR_API esft_stor_collection_t *esft_stor_store_collection(esft_stor_store_t *store,
                                                   const char *collection_name,
                                                   esft_stor_error_t *err);

/**
 * @brief Checks whether collection with name @p collection_name exists in store @p store
 */
CSTOR_API bool esft_stor_store_collection_exists(esft_stor_store_t *store,
                                       const char *collection_name);

/**
 * @brief Removes collection with name @p collection_name from store @p store and returns
 * true if operation succeded. If the operation succeded, all of the collections filed are
 * erased but the collection that @p collection_name refers to still has to be destroyed
 * via @code esft_stor_collection_delete
 */
CSTOR_API bool esft_stor_store_collection_remove(esft_stor_store_t *store,
                                       const char *collection_name);

/**
 * @brief releases the resources of @p store
 */
CSTOR_API void esft_stor_store_delete(esft_stor_store_t *store);


#ifdef __cplusplus
}
#endif

#endif //CSTOR_CSTORE_H
