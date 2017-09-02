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

esft_stor_store_t *esft_stor_store_open(const char *store_home,
                                        const char *store_name,
                                        esft_stor_error_t *err);

esft_stor_store_t *esft_stor_store_open_encrypted(const char *store_home,
                                                  const char *store_name,
                                                  const char *key,
                                                  esft_stor_error_t *err);

const char *esft_stor_store_home(esft_stor_store_t *store);

void esft_stor_store_async(bool async_mode);

bool esft_stor_store_is_async();

esft_stor_collection_t *esft_stor_store_collection(const char *collection_name,
                                                   esft_stor_error_t *err);

void esft_stor_store_collection_remove(const char *collection_name);

void esft_stor_store_close(esft_stor_store_t *store,
                           esft_stor_error_t *err);

void esft_stor_store_delete(esft_stor_store_t *store);


#ifdef __cplusplus
}
#endif

#endif //CSTOR_CSTORE_H
