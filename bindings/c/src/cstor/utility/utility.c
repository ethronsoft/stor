

#include <stdlib.h>



/**
 * @brief deletes an arbitrary block of memory @p m
 */
void esft_stor_utils_delete(void *m){
    free(m);
}


