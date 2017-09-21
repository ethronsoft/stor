//
// Created by efelici on 8/30/2017.
//

#ifndef CSTOR_ERRORS_H_H
#define CSTOR_ERRORS_H_H

#include <cstor/utility/export.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum esft_stor_error_t{
    no_error = 0,
    generic_error,
    out_of_memory,
    document_error,
    io_error,
    store_error,
    query_error,
    access_error
} esft_stor_error_t;

/**
 * @brief returns an instance of esft_stor_error_t
 * with value no_error
 * @return no_error esft_stor_error_t
 */
CSTOR_API esft_stor_error_t esft_stor_error_init();

/**
 * @brief returns the string representation of @p t
 */
CSTOR_API const char * esft_stor_error_string(esft_stor_error_t t);

#ifdef __cplusplus
}
#endif

#endif //CSTOR_ERRORS_H_H
