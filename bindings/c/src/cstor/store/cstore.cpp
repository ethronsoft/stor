//
// Created by efelici on 9/4/2017.
//

#include <internal/opaque_types.h>
#include <cstor/store/cstore.h>
#include <cstor/store/ccollection.h>
#include <stor/store/store.h>
#include <stor/exceptions/store_exception.h>
#include <stor/exceptions/access_exception.h>


extern "C" {

esft_stor_store_t *esft_stor_store_open(const char *store_home,
                                        const char *store_name,
                                        bool remove_on_delete,
                                        esft_stor_error_t *e){
    esft_stor_store_t *res = nullptr;
    try{
        res = new esft_stor_store_t{};
        res->rep = new esft::stor::store(store_home,store_name, remove_on_delete);
        return res;
    }catch(const esft::stor::store_exception &ex){
        *e = io_error;
    }catch(const esft::stor::access_exception &ex){
        *e = access_error;
    }catch (...){
        *e = generic_error;
    }
    esft_stor_store_delete(res);
    return nullptr;
}

esft_stor_store_t *esft_stor_store_open_encrypted(const char *store_home,
                                                  const char *store_name,
                                                  const char *key,
                                                  encryption_failure_handler f,
                                                  esft_stor_error_t *e){
    esft_stor_store_t *res = nullptr;
    try{
        res = new esft_stor_store_t{};
        std::unique_ptr<esft::stor::access_manager> am{new esft::stor::access_manager(key)};
        res->rep = new esft::stor::store(store_home,store_name, std::move(am), f);
        return res;
    }catch(const esft::stor::store_exception &){
        *e = io_error;
    }catch(const std::bad_alloc &){
        *e = out_of_memory;
    }catch(const esft::stor::access_exception &){
        *e = access_error;
    }catch (...){
        *e = generic_error;
    }
    esft_stor_store_delete(res);
    return nullptr;
}

const char *esft_stor_store_home(esft_stor_store_t *store){
    try{
        return store->rep->home().c_str();
    }catch (...){}
    return NULL;
}

void esft_stor_store_async(esft_stor_store_t *store,
                           bool async_mode){
    try{
        store->rep->set_async(async_mode);
    }catch (...){}
}

bool esft_stor_store_is_async(esft_stor_store_t *store){
    try{
        return store->rep->is_async();
    }catch (...){}
    return false;
}

esft_stor_collection_t *esft_stor_store_collection(esft_stor_store_t *store,
                                                   const char *collection_name,
                                                   esft_stor_error_t *e){
    esft_stor_collection_t *res = nullptr;
    try{
        res = new esft_stor_collection_t{};
        //create collection if non existing
        auto &coll = (*store->rep)[collection_name];
        res->name = coll.name();
        res->rep_p = store->rep;
        return res;
    }catch(const esft::stor::store_exception &){
        *e = io_error;
    }catch(const std::bad_alloc &){
        *e = out_of_memory;
    }catch(const esft::stor::document_exception &){
        *e = document_error;
    }catch (...){
        *e = generic_error;
    }
    esft_stor_collection_delete(res);
    return nullptr;
}

bool esft_stor_store_collection_exists(esft_stor_store_t *store,
                                       const char *collection_name){
    try{
        return store->rep->has(collection_name);
    }catch (...){}
    return false;
}

bool esft_stor_store_collection_remove(esft_stor_store_t *store,
                                       const char *collection_name){
    try{
        return store->rep->remove(collection_name);
    }catch (...){}
    return false;
}

void esft_stor_store_delete(esft_stor_store_t *store){
    if (store){
        delete store->rep;
        delete store;
    }
}

}