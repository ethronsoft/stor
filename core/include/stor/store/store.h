//
// Created by devhack on 23/05/16.
//

#ifndef ESNPL_STOR_STORE_H
#define ESNPL_STOR_STORE_H

#include <string>
#include <unordered_map>
#include <memory>
#include <leveldb/env.h>
#include <leveldb/db.h>
#include <leveldb/options.h>
#include <stor/store/onclose.h>
#include <stor/store/access_manager.h>

namespace esft{
    namespace stor{


        /**
         * default callback function called if encryption fails
         */
        void default_encryption_failure_handler();

        class collection;

        /**
         * @brief store represents a database as a set of collections of documents
         */
        class store final{
        public:


            typedef std::function<void()> encryption_failure_handler;

            /**
             * @brief Constructor 1
             *
             * @param path_to_db path to root of store
             * @param db_name store name
             * @param temp optional temporary store flag. Temporary store gets deleted
             * when the store object or the last collection object goes out of scope.
             * Default value is false.
             */
            store(const std::string &path_to_db, const std::string &db_name, bool temp = false);

            /**
             * @brief Constructor 2
             *
             * @param path_to_db path to root of store
             * @param db_name store name
             * @param am shared ptr to access_manager to be used for ecnryption/decryption of store
             * @param encryption_failure_handler optional handler for encryption handler.
             * Default handler prints a message to std::cerr
             */
            store(const std::string &path_to_db,
                  const std::string &db_name,
                  std::unique_ptr<access_manager> am,
                  encryption_failure_handler = default_encryption_failure_handler);

            /**
             * @brief Copy constructor. Not allowed.
             */
            store(const store &o) = delete;

            /**
             * @brief Copy assignment. Not allowed.
             */
            store &operator=(const store &o) = delete;

            /**
             * @brief Move constructor.
             */
            store(store && o);

            /**
             * @brief Destructor.
             */
            ~store();

            /**
             * @brief Return an existing collection or creates a new one with the given name.
             *
             * @returns new or existing collection with given name.
             */
            collection &operator[](const std::string &collection_name);

            /**
             * @brief Check if a collection exists with the given name
             *
             * @returns true if the collection exists, false if not
             */
            bool has(const std::string &collection_name) const;

            /**
             * @brief Upsert a collection in the store
             */
            void put(const collection &c);

            /**
             * @brief Remove a collection in the store.
             * Any reference of the collection deleted
             * is invalidated.
             */
            bool remove(const std::string &collection_name);

            /**
             * @brief Return root of store
             *
             * @returns root of store
             */
            const std::string &home() const;

            /**
             *@brief Mark store write operations as async or not.
             *
             * Async operations will return although the underlying operation won't be
             * completed necessarily. Sync operations won't.
             */
            void set_async(bool async);

            /**
             * @brief Check whether store is in async mode or not
             *
             * @returns true if store is async, false if not
             */
            bool is_async() const;

        private:

            /**
             * unique_ptr to onclose (object that performs an action
             * when destructed)
             *
             */
            std::unique_ptr<onclose> _onclose;

            /**
             * store root
             */
            std::string _path;

            /**
             * store name
             */
            std::string _name;

            /**
             *
             */
            leveldb::DB *_db;

            /**
             * leveldb options
             */
            leveldb::Options _opt;

            /**
             * flag to determine if database is to be deleted or not
             * after being used.
             */
            bool _temp;

            /**
             * optional pointer to access_manager. If available,
             * store will attempt decrypting/encrypting its files.
             */
            std::shared_ptr<access_manager> _am;

            /**
             * optional handler of any error occurring while encrpytion.
             * Encryption happens through the onclose operation. As such,
             * store and any other collection object has already been distroyed.
             * One way of dealing with this is to inject handler from the client.
             */
            encryption_failure_handler _encryption_failure_handler;

            /**
             * flag to determine whether store is in asynchronous or synchronous mode
             */
            bool _async;

            /**
             * map of collection that have been created/requested since this object instantiation.
             * It does not necessarily represent the entire set of collections, as some may
             * not have been queried since this object instantiation.
             */
            std::unordered_map<std::string, std::unique_ptr<collection>> _collections;
        };

    }
}

#endif //ESNPL_STOR_STORE_H
