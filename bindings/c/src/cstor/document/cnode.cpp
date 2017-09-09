//
// Created by efelici on 8/31/2017.
//

#include <cstor/document/cnode.h>
#include <internal/opaque_types.h>
#include <cstring>
#include <cassert>

extern "C" {

esft_stor_json_t esft_stor_node_json(esft_stor_node_t *node){
    esft_stor_json_t res;
    try{
        auto json = node->rep->json();
        res.len = json.size();
        res.value = new char[res.len + 1];
        strcpy(res.value,json.c_str());
    }catch(...){
        esft_stor_json_dispose(&res);
    }
    return res;
}

void esft_stor_node_from_json(esft_stor_node_t *node,
                              const char *json,
                              esft_stor_error_t *e){
    try{
        node->rep->json(json);
    }catch (const esft::stor::document_exception){
        *e = document_error;
    }catch (...){
        *e = generic_error;
    }
}

void esft_stor_node_from_node(esft_stor_node_t *node,
                              esft_stor_node_t *copy_from,
                              esft_stor_error_t *e){
    try{
        node->rep->copy(*(copy_from->rep));
    }catch (const esft::stor::document_exception){
        *e = document_error;
    }catch (...){
        *e = generic_error;
    }
}


void esft_stor_json_dispose(esft_stor_json_t *json){
    if (json){
        delete json->value;
        json->len = 0;
    }
}


size_t esft_stor_node_size(esft_stor_node_t *node){
    assert(node->rep->is_array() || node->rep->is_object());
    try{
        return node->rep->size();
    }catch(...){}
    return 0;
}

bool esft_stor_node_is_num(esft_stor_node_t *node){
    try{
        return node->rep->is_num();
    }catch(...){}
    return false;
}


bool esft_stor_node_is_int(esft_stor_node_t *node){
    try{
        return node->rep->is_int();
    }catch(...){}
    return false;
}

int esft_stor_node_as_int(esft_stor_node_t *node){
    assert(node->rep->is_int());
    try{
        return node->rep->as_int();
    }catch(...){}
    return 0;
}


bool esft_stor_node_is_long(esft_stor_node_t *node){
    try{
        return node->rep->is_long();
    }catch(...){}
    return false;
}

long long esft_stor_node_as_long(esft_stor_node_t *node){
    assert(node->rep->is_long());
    try{
        return node->rep->as_long();
    }catch(...){}
    return 0;
}


bool esft_stor_node_is_double(esft_stor_node_t *node){
    try{
        return node->rep->is_double();
    }catch(...){}
    return false;
}

double esft_stor_node_as_double(esft_stor_node_t *node){
    assert(node->rep->is_double());
    try{
        return node->rep->as_double();
    }catch(...){}
    return 0;
}


bool esft_stor_node_is_bool(esft_stor_node_t *node){
    try{
        return node->rep->is_bool();
    }catch(...){}
    return false;
}

bool esft_stor_node_as_bool(esft_stor_node_t *node){
    assert(node->rep->is_bool());
    try{
        return node->rep->as_bool();
    }catch(...){}
    return 0;
}


bool esft_stor_node_is_string(esft_stor_node_t *node){
    try{
        return node->rep->is_string();
    }catch(...){}
    return false;
}

const char * esft_stor_node_as_string(esft_stor_node_t *node){
    assert(node->rep->is_string());
    try{
        return node->rep->as_cstring().first;
    }catch(...){}
    return NULL;
}

const char * esft_stor_node_as_stringl(esft_stor_node_t *node,size_t *str_len){
    assert(node->rep->is_string());
    try{
        auto p = node->rep->as_cstring();
        *str_len = p.second;
        return p.first;
    }catch(...){}
    return NULL;
}


bool esft_stor_node_is_object(esft_stor_node_t *node){
    try{
        return node->rep->is_object();
    }catch(...){}
    return false;
}

esft_stor_node_t *esft_stor_node_object_get(esft_stor_node_t *node,
                                            const char * key,
                                            esft_stor_error_t *e){
    assert(node->rep->is_object());
    esft_stor_node_t *res = nullptr;
    try{
        res = new esft_stor_node_t{};
        res->rep = new esft::stor::node{(*node->rep)[key]};
        return res;
    }catch (const esft::stor::document_exception &) {
        *e = document_error;
    }catch (const std::bad_alloc &){
        *e = out_of_memory;
    }catch (...){
        *e = generic_error;
    }
    esft_stor_node_delete(res);
    return NULL;
}

bool esft_stor_node_object_has(esft_stor_node_t *node,
                               const char * key){
    try{
        return node->rep->has(key);
    }catch (...){}
    return false;
}


char ** esft_stor_node_object_keys(esft_stor_node_t *node,
                                   size_t *out_len){
    assert(node->rep->is_object());
    char ** res = nullptr;
    size_t sz = 0;
    try{
        sz = node->rep->size();
        *out_len = sz;
        if (sz > 0){
            res = new char*[sz];
            std::vector<std::string> keys(sz);
            size_t i = 0;
            for (auto it = node->rep->cbegin(); it != node->rep->cend(); ++it){
                keys[i] = it.key();
                ++i;
            }
            //no errors, we can allocate and copy over
            for (std::size_t i = 0; i < sz; ++i){
                res[i] = new char[keys[i].size() + 1];
                strcpy(res[i],keys[i].c_str());
            }
            return res;
        }
    }catch (...){
        //delete any string allocated
        esft_stor_node_object_keys_delete(res,sz);
        *out_len = 0;
    }
    return nullptr;
}

void esft_stor_node_object_keys_delete(char **keys, size_t len){
    if (keys){
        for (size_t i = 0; i < len; ++i){
            delete[] keys[i];
        }
        delete[] keys;
    }

}

bool esft_stor_node_is_null(esft_stor_node_t *node){
    try{
        return node->rep->is_null();
    }catch(...){}
    return false;
}

bool esft_stor_node_is_array(esft_stor_node_t *node){
    try{
        return node->rep->is_array();
    }catch(...){}
    return false;
}

size_t esft_stor_node_array_size(esft_stor_node_t *node){
    assert(node->rep->is_array());
    try{
        return node->rep->size();
    }catch(...){}
    return 0;
}

esft_stor_node_t *esft_stor_node_array_get(esft_stor_node_t *node,
                                           unsigned int index,
                                           esft_stor_error_t *e){
    if (index < esft_stor_node_array_size(node)){
        esft_stor_node_t *res = nullptr;
        try{
            res = new esft_stor_node_t{};
            res->rep = new esft::stor::node{(*node->rep)[index]};
            return res;
        }catch (const esft::stor::document_exception &) {
            *e = document_error;
        }catch (const std::bad_alloc &){
            *e = out_of_memory;
        }catch (...){
            *e = generic_error;
        }
    }
    return NULL;
}




void esft_stor_node_object_put_obj(esft_stor_node_t *node,
                                   const char *key){
    assert(node->rep->is_object());
    try{
        node->rep->with(key);
    }catch (...){}
}


void esft_stor_node_object_put_array(esft_stor_node_t *node,
                                     const char *key){
    assert(node->rep->is_object());
    try{
        node->rep->with_array(key);
    }catch (...){}
}


void esft_stor_node_object_put_int(esft_stor_node_t *node,
                                   const char *key,
                                   int value){
    assert(node->rep->is_object());
    try{
           node->rep->put(key,value);
    }catch (...) {}
}


void esft_stor_node_object_put_long(esft_stor_node_t *node,
                                    const char *key,
                                    long long value){
    assert(node->rep->is_object());
    try{
        node->rep->put(key,value);
    }catch (...) {}
}


void esft_stor_node_object_put_double(esft_stor_node_t *node,
                                      const char *key,
                                      double value){
    assert(node->rep->is_object());
    try{
        node->rep->put(key,value);
    }catch (...) {}
}


void esft_stor_node_object_put_bool(esft_stor_node_t *node,
                                    const char *key,
                                    bool value){
    assert(node->rep->is_object());
    try{
        node->rep->put(key,value);
    }catch (...) {}
}


void esft_stor_node_object_put_string(esft_stor_node_t *node,
                                      const char *key,
                                      const char * value){
    assert(node->rep->is_object());
    try{
        node->rep->put(key,value);
    }catch (...) {}
}


void esft_stor_node_array_add_obj(esft_stor_node_t *node){
    assert(node->rep->is_array());
    try{
        node->rep->add_object();
    }catch (...){}
}



void esft_stor_node_array_add_array(esft_stor_node_t *node){
    assert(node->rep->is_array());
    try{
        node->rep->add_array();
    }catch (...){}
}



void esft_stor_node_array_add_int(esft_stor_node_t *node,
                                  int value){
    assert(node->rep->is_array());
    try{
        node->rep->add(value);
    }catch (...) {}
}


void esft_stor_node_array_add_long(esft_stor_node_t *node,
                                   long long value){
    assert(node->rep->is_array());
    try{
        node->rep->add(value);
    }catch (...) {}
}


void esft_stor_node_array_add_double(esft_stor_node_t *node,
                                     double value){
    assert(node->rep->is_array());
    try{
        node->rep->add(value);
    }catch (...) {}
}


void esft_stor_node_array_add_bool(esft_stor_node_t *node,
                                   bool value){
    assert(node->rep->is_array());
    try{
        node->rep->add(value);
    }catch (...) {}
}


void esft_stor_node_array_add_string(esft_stor_node_t *node,
                                     const char * value){
    assert(node->rep->is_array());
    try{
        node->rep->add(value);
    }catch (...) {}
}


void esft_stor_node_array_add_stringl(esft_stor_node_t *node,
                                      const char * value,
                                      size_t str_len){
    assert(node->rep->is_array());
    try{
        node->rep->add(value);
    }catch (...) {}
}


void esft_stor_node_delete(esft_stor_node_t *node){
    if (node) delete node->rep;
    delete node;
}

}