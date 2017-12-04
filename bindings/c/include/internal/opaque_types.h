

#ifndef CSTOR_TYPES_H
#define CSTOR_TYPES_H

#include <stor/document/node.h>
#include <stor/document/document.h>
#include <stor/store/collection.h>

#ifdef __cplusplus
extern "C" {
#endif

struct esft_stor_document_t     { esft::stor::document *rep; };

struct esft_stor_node_t         { esft::stor::node     *rep; };

struct esft_stor_collection_t   { esft::stor::store *rep_p; /** no ownership **/
                                  std::string name           ;};

struct esft_stor_store_t        { esft::stor::store *rep; };

#ifdef __cplusplus
}
#endif

#endif //CSTOR_TYPES_H
