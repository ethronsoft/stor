//
// Created by efelici on 9/7/2017.
//

#ifndef PYSTOR_UTILITY_H
#define PYSTOR_UTILITY_H

#include "export.h"

/**
 * @file utility.h
 * @brief General utility functions
 **/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief deletes an arbitrary block of memory @p m
 */
CSTOR_API void esft_stor_utils_delete(void *m);

#ifdef __cplusplus
}
#endif

#endif //PYSTOR_UTILITY_H
