//
// Created by efelici on 8/30/2017.
//

#include <stdlib.h>
#include <stor/document/document.h>
#include <cstor/document/cdocument.h>
#include <internal/opaque_types.h>
#include <cstor/document/cnode.h>


extern "C" {




esft_stor_document_t *esft_stor_document_create(const char *json,
                                                esft_stor_error_t *e) {
    assert(*e == 0);
    esft_stor_document_t *res = nullptr;
    esft::stor::document *rep = nullptr;
    try{
        res = new esft_stor_document_t{};
        rep = new esft::stor::document{json};
        res->rep = rep;
        return res;
    }catch (const esft::stor::document_exception &ex){
        *e = esft_stor_error_t::document_error;
    }
    catch (const std::bad_alloc &ex){
        *e = esft_stor_error_t::out_of_memory;
    }
    catch (...){
        *e = esft_stor_error_t::generic_error;
    }
    if (rep) delete rep;
    if (res) delete res;
    return NULL;
}

esft_stor_document_t *esft_stor_document_create_as_object(esft_stor_error_t *e) {
    return esft_stor_document_create("{}",e);
}

esft_stor_document_t *esft_stor_document_create_as_array(esft_stor_error_t *e) {
    return esft_stor_document_create("[]",e);
}

esft_stor_node_t *esft_stor_document_root(esft_stor_document_t *doc,
                                          esft_stor_error_t *e) {
    assert(*e == 0);
    esft_stor_node_t *res = nullptr;
    try{
        res = new esft_stor_node_t{};
        //there is no need to dynamically allocated a node, as
        //esft::stor::document is a node and our wrapper
        //can just take that
        res->rep = dynamic_cast<esft::stor::node *>(doc->rep);
        return res;
    }catch (const std::bad_alloc &ex){
       *e = esft_stor_error_t::out_of_memory;
    }catch (...){
        *e = esft_stor_error_t::generic_error;
    }
    if (res) delete res;
    return  NULL;

}

const char *esft_stor_document_id(esft_stor_document_t *doc){
    try{
        return doc->rep->id().c_str();
    }catch (...){
        //set error code

        //set error message
    }
    return  NULL;
}

void esft_stor_document_delete(esft_stor_document_t *doc) {
    delete doc;
}



}