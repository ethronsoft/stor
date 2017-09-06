//
// Created by efelici on 9/4/2017.
//
#include <cstor/store/ccollection.h>
#include <internal/opaque_types.h>
#include <cstor/document/cdocument.h>
#include <stor/exceptions/store_exception.h>

extern "C" {

const char * esft_stor_collection_name(esft_stor_collection_t *c){
    try{
        return c->rep->name().c_str();
    }catch (...){}
    return nullptr;
}

bool esft_stor_collection_index_add(esft_stor_collection_t *c,
                                    const char * index_path){
    try{
        c->rep->add_index(index_path);
        return true;
    }catch (...) {}
    return false;
}

bool esft_stor_collection_index_remove_all(esft_stor_collection_t *c){
    try{
        return c->rep->clear_indices();
    }catch (...) {}
    return false;
}

void esft_stor_collection_document_put(esft_stor_collection_t *c,
                                       esft_stor_document_t *doc,
                                       esft_stor_error_t *e){
    try{
        c->rep->put(*doc->rep);
    }
    catch (...){
        *e = generic_error;
    }
}

esft_stor_document_t *esft_stor_collection_document_get(esft_stor_collection_t *c,
                                                        const char *doc_id,
                                                        esft_stor_error_t *e){
    esft_stor_document_t *res = nullptr;
    try{
        if (c->rep->has(doc_id)){
            res = new esft_stor_document_t{};
            res->rep = new esft::stor::document{(*c->rep)[doc_id]};
            return res;
        }
    }catch(const std::bad_alloc &ex){
        *e = out_of_memory;
    }catch(const esft::stor::store_exception &ex){
        *e = store_error;
    }catch(...){
        *e = generic_error;
    }
    esft_stor_document_delete(res);
    return nullptr;
}


bool esft_stor_collection_document_exists(esft_stor_collection_t *c,
                                          const char *doc_id){
    try{
        return c->rep->has(doc_id);
    }
    catch (...){
    }
    return false;
}

bool esft_stor_collection_document_remove(esft_stor_collection_t *c,
                                          const char * doc_id){
    try{
        return c->rep->remove(std::string(doc_id));
    }
    catch (...){
    }
    return false;
}

esft_stor_collection_query_result_t esft_stor_collection_query(esft_stor_collection_t *c,
                                                             const char *query,
                                                             esft_stor_error_t *e){
    esft_stor_collection_query_result_t res;

    try{
        auto &coll = *c->rep;
        auto docs = coll.find(query);
        res.len = docs.size();
        if (res.len > 0){
            res.values = new esft_stor_document_t*[res.len];
            std::size_t i = 0;
            for (auto &doc : docs){
                res.values[i] = new esft_stor_document_t{};
                //set elements are unmodifiable but because we are not going to use the set
                //afterward, we can force a move-out
                res.values[i]->rep = new esft::stor::document(std::move(*const_cast<esft::stor::document *>(&doc)));
                i++;
            }
        }
        return res;
    }catch (const std::bad_alloc &ex){
        *e = out_of_memory;
    }
    catch (const esft::stor::store_exception &ex){
        *e = query_error;
    }
    catch (...){
        *e = generic_error;
    }
    esft_stor_collection_query_result_dispose(&res);
    return res;
}

void esft_stor_collection_query_result_dispose(esft_stor_collection_query_result_t *res){
    if (res){
        if (res->values){
            for (std::size_t i = 0; i < res->len; ++i){
                esft_stor_document_delete(res->values[i]);
            }
        }
        delete[] res->values;
        res->len = 0;
    }
}


void esft_stor_collection_delete(esft_stor_collection_t *c){
    if (c){
        delete c->rep;
        delete c;
    }
}

}