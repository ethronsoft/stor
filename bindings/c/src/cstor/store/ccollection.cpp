//
// Created by efelici on 9/4/2017.
//
#include <cstor/store/ccollection.h>
#include <internal/opaque_types.h>
#include <cstor/document/cdocument.h>
#include <stor/exceptions/store_exception.h>
#include <stor/store/store.h>
#include <stor/store/query.h>



extern "C" {

const char * esft_stor_collection_name(esft_stor_collection_t *c){
    try{
        return (*c->rep_p)[c->name].name().c_str();
    }catch (...){}
    return nullptr;
}

bool esft_stor_collection_index_add(esft_stor_collection_t *c,
                                    const char * index_path){
    try{
        (*c->rep_p)[c->name].add_index(index_path);
        return true;
    }catch (...) {}
    return false;
}

void esft_stor_collection_indices_add(esft_stor_collection_t *c,
                                      const char **index_paths,
                                      size_t len){
    try{
        (*c->rep_p)[c->name].add_indices(index_paths,index_paths+len);
    }catch (...) {}
}

char ** esft_stor_collection_index_list(esft_stor_collection_t *c,
                                        size_t *result_len,
                                        esft_stor_error_t *e){
    char ** res = nullptr;
    size_t sz = 0;
    try{
        auto indices = (*c->rep_p)[c->name].indices();
        sz = indices.size();
        *result_len = sz;
        res = new char*[sz];
        size_t i = 0;
        for (const auto &s: indices){
            auto str_path = s.str();
            res[i] = new char[str_path.size()+1];
            strcpy(res[i],str_path.c_str());
            ++i;
        }
        return res;
    }catch(std::bad_alloc &ex){
        *e = out_of_memory;
    }catch(...){
        *e = generic_error;
    }
    *result_len = 0;
    esft_stor_collection_index_list_delete(res,sz);
    return res;
}


void esft_stor_collection_index_list_delete(char ** index_paths,
                                            size_t len){
    if (index_paths){
        for (size_t i = 0; i < len; ++i){
            delete[] index_paths[i];
        }
        delete[] index_paths;
    }
}



bool esft_stor_collection_index_remove_all(esft_stor_collection_t *c){
    try{
        return (*c->rep_p)[c->name].clear_indices();
    }catch (...) {}
    return false;
}

void esft_stor_collection_document_put(esft_stor_collection_t *c,
                                       esft_stor_document_t *doc,
                                       esft_stor_error_t *e){
    try{
        (*c->rep_p)[c->name].put(*doc->rep);
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
        auto &coll = (*c->rep_p)[c->name];
        if (coll.has(doc_id)){
            res = new esft_stor_document_t{};
            res->rep = new esft::stor::document{coll[doc_id]};
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
        return (*c->rep_p)[c->name].has(doc_id);
    }
    catch (...){
    }
    return false;
}

bool esft_stor_collection_document_remove(esft_stor_collection_t *c,
                                          const char * doc_id){
    try{
        return (*c->rep_p)[c->name].remove(std::string(doc_id));
    }
    catch (...){
    }
    return false;
}

esft_stor_document_t** esft_stor_collection_query(esft_stor_collection_t *c,
                                                             const char *query,
                                                            size_t *result_len,
                                                             esft_stor_error_t *e){
    esft_stor_document_t**res = nullptr;
    size_t sz = 0;
    try{
        auto &coll = (*c->rep_p)[c->name];
        auto docs = coll.find(query);
        sz = docs.size();
        *result_len = sz;
        if (sz > 0){
            res = new esft_stor_document_t*[sz];
            std::size_t i = 0;
            for (auto &doc : docs){
                res[i] = new esft_stor_document_t{};
                //set elements are unmodifiable but because we are not going to use the set
                //afterward, we can force a move-out
                res[i]->rep = new esft::stor::document(doc.json(),doc.id());
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
    esft_stor_collection_query_result_delete(res,sz);
    *result_len = 0;
    return res;
}

void esft_stor_collection_query_result_delete(esft_stor_document_t**res, size_t len){
    if (res){
        for (size_t i = 0; i < len; ++i){
            delete[] res[i];
        }
        delete[] res;
    }
}


void esft_stor_collection_delete(esft_stor_collection_t *c){
    if (c){
        delete c;
    }
}

}