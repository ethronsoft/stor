//
// Created by efelici on 9/1/2017.
//

#ifndef CSTOR_CSTORE_H
#define CSTOR_CSTORE_H

#include <cstor/errors/errors.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct esft_stor_collection_t esft_stor_collection_t;

typedef struct esft_stor_store_t esft_stor_store_t;

typedef void(*encryption_failure_handler)();

esft_stor_store_t *esft_stor_store_open(const char *store_home,
                                        const char *store_name,
                                        bool remove_on_delete,
                                        esft_stor_error_t *err);

esft_stor_store_t *esft_stor_store_open_encrypted(const char *store_home,
                                                  const char *store_name,
                                                  const char *key,
                                                  encryption_failure_handler f,
                                                  esft_stor_error_t *err);

const char *esft_stor_store_home(esft_stor_store_t *store);

void esft_stor_store_async(esft_stor_store_t *store, bool async_mode);

bool esft_stor_store_is_async(esft_stor_store_t *store);

esft_stor_collection_t *esft_stor_store_collection(esft_stor_store_t *store,
                                                   const char *collection_name,
                                                   esft_stor_error_t *err);

bool esft_stor_store_collection_exists(esft_stor_store_t *store,
                                       const char *collection_name);

bool esft_stor_store_collection_remove(esft_stor_store_t *store,
                                       const char *collection_name);

void esft_stor_store_delete(esft_stor_store_t *store);


#ifdef __cplusplus
}
#endif

#endif //CSTOR_CSTORE_H
