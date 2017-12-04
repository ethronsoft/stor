

#include <cstor/errors/errors.h>


#ifdef __cplusplus
extern "C" {
#endif

esft_stor_error_t esft_stor_error_init(){
    return no_error;
}

const char *esft_stor_error_string(esft_stor_error_t t) {
    switch (t) {
        case generic_error:
            return "Unknown error";
        case out_of_memory:
            return "Out of memory";
        case document_error:
            return "Document error";
        case io_error:
            return "IO error";
        case store_error:
            return "Store error";
        case query_error:
            return "Query error";
        case access_error:
            return "Access error";
        default:
            return "";
    }
}

#ifdef __cplusplus
}
#endif
