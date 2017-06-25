//
// Created by devhack on 23/05/16.
//

#ifndef ESNPL_STOR_COLLECTION_H
#define ESNPL_STOR_COLLECTION_H

#include <stor/document/document.h>
#include <stor/store/store.h>
#include <stor/store/index_path.h>
#include <stor/store/onclose.h>
#include <stor/store/query.h>
#include <stor/store/eq_instruction.h>
#include <stor/store/gt_instruction.h>
#include <stor/store/gte_instruction.h>
#include <stor/store/neq_instruction.h>
#include <stor/store/lt_instruction.h>
#include <stor/store/lte_instruction.h>
#include <leveldb/db.h>
#include <leveldb/options.h>
#include <unordered_set>
#include <memory>
#include <mutex>


namespace esft {
    namespace stor {

        class store;

        class query;

        class eq_instruction;

        class gt_instruction;

        class gte_instruction;

        class neq_instruction;

        class lt_instruction;

        class lte_instruction;

        class collection final {
        public:
            friend class store;

            friend class eq_instruction;

            friend class gt_instruction;

            friend class gte_instruction;

            friend class neq_instruction;

            friend class lt_instruction;

            friend class lte_instruction;


            /**
             * @brief Copy Constructor. Not available.
             */
            collection(const collection &o) = delete;

            /**
             * @brief Copy Assignment. Not available.
             */
            collection &operator=(const collection &o) = delete;

            /**
             * @brief Move constructor.
             */
            collection(collection &&o);

            /**
             * @brief Move Assignment.
             */
            collection &operator=(collection &&o);

            /**
             * @brief Destructor
             */
            ~collection();

            /**
             * @brief Remove all stored indices for this collection
             *
             * @returns true if successful. false if not or if there weren't any indices to clear
             */
            bool clear_indices();

            /**
             * @brief Add index to 'path' to allow lookup
             * during query operations
             *
             * @param path path to a datapoint to store index for
             */
            void add_index(const index_path &path);

            /**
             * @brief Return name of collection
             *
             * @returns name of collection
             */
            const std::string &name() const;

            /**
             * @brief Stores any structural changes made to this collection
             * in the store it belongs to
             */
            void persist();

            /**
             * @brief Insert/Update document 'doc' in the collection
             *
             * @param doc document to insert into collection
             */
            bool put(const document &doc);

            /**
             * @brief Remove document 'doc' from the collection.
             *
             * @param doc document to remove from collection
             *
             * @returns true if document existed and was removed. false if not
             */
            bool remove(const document &doc);

            /**
             * @brief Remove document from the collection that has id = 'id'
             *
             * @param id identifier for document to remove from collection
             *
             * @returns true if document existed and was removed. false if not
             */
            bool remove(const document::identifier &id);

            /**
             * @brief Retrieve document with id = 'id' from the collection
             *
             * @throws @ref store_exception if no document with id = 'id' exists
             * in the collection
             *
             * @param id identifer from document to retrieve from collection
             *
             * @returns document with id = 'id' from the collection
             */
            document operator[](const stor::document::identifier &id) const;

            /**
             * @brief Checks whether document with id = 'id' exists in the collection
             *
             * @param id identifier of document to check for existence
             */
            bool has(const stor::document::identifier &id) const;

            /**
             * @brief Execute query against the collection and return all matching documents
             *
             * @param q query that provides instructions to retrieve a set of documents from collection
             *
             * @returns documents matching result for query 'q'
             */
            std::unordered_set<document> find(const query &q);

            /**
             * @brief Write data and indices of store to std::ostream
             *
             * @param o output stream to write collection to
             */
            void deflate(std::ostream &o, bool keep_indices = false) const;

            /**
             * @brief Read data and indices from std::istream and build collection from them
             * @param i input stream to read collection from
             */
            void inflate(std::istream &i);

        private:

            /**
             * @brief Constructor.
             *
             * @param s pointer to parent store
             * @param info document containing initializing values for collection
             * @parm oc shared_ptr to onclose object
             */
            collection(store *s, document &info, std::shared_ptr<onclose> oc);

            /**
             * @brief Register insertion of document key:value id:json in batch
             * @param batch operation holder used for atomic writes
             * @param d document to persist as json string
             */
            void put(leveldb::WriteBatch &batch, const document &d) const;

            /**
             * @brief Register insertion of document key:value index_path:value_of_path in batch
             *
             * @param batch operation holder used for atomic writes
             * @param index_path to value of document to store and key to such value
             * @param d document whose value at index_path 'index' is to be stored
             */
            void put(leveldb::WriteBatch &batch, const index_path &index, const document &d) const;

            /**
             * @brief Register removal of value at index_path 'index' of document 'd' in batch
             *
             * @param batch operation holder used for atomic writes
             * @param index index_path to value of document to remove
             * @param d document whose value at index_path 'index' is to be removed
             *
             */
            void remove(leveldb::WriteBatch &batch, const index_path &index, const document &d) const;

            /**
             * @brief Register removal of of document 'd' in batch
             *
             * @param  batch operation holder used for atomic writes
             * @document d document to remove
             */
            void remove(leveldb::WriteBatch &batch, const document &d) const;

            /**
             * @brief Execute equality instruction 'i' on this collection using ReadOptions 'ro'
             *
             * @param i instruction to execute against collection
             * @param ro ReadOptions to be used for any needed reads to collection
             */
            std::unordered_set<std::string> process(const eq_instruction &i, const leveldb::ReadOptions &ro) const;

            /**
             * @brief Execute greater-than instruction 'i' on this collection using ReadOptions 'ro'
             *
             * @param i instruction to execute against collection
             * @param ro ReadOptions to be used for any needed reads to collection
             */
            std::unordered_set<std::string> process(const gt_instruction &i, const leveldb::ReadOptions &ro) const;

            /**
             * @brief Execute greater-equal-than instruction 'i' on this collection using ReadOptions 'ro'
             *
             * @param i instruction to execute against collection
             * @param ro ReadOptions to be used for any needed reads to collection
             */
            std::unordered_set<std::string> process(const gte_instruction &i, const leveldb::ReadOptions &ro) const;

            /**
             * @brief Execute non-equality instruction 'i' on this collection using ReadOptions 'ro'
             *
             * @param i instruction to execute against collection
             * @param ro ReadOptions to be used for any needed reads to collection
             */
            std::unordered_set<std::string> process(const neq_instruction &i, const leveldb::ReadOptions &ro) const;

            /**
             * @brief Execute less-than instruction 'i' on this collection using ReadOptions 'ro'
             *
             * @param i instruction to execute against collection
             * @param ro ReadOptions to be used for any needed reads to collection
             */
            std::unordered_set<std::string> process(const lt_instruction &i, const leveldb::ReadOptions &ro) const;

            /**
             * @brief Execute less-equal-than instruction 'i' on this collection using ReadOptions 'ro'
             *
             * @param i instruction to execute against collection
             * @param ro ReadOptions to be used for any needed reads to collection
             */
            std::unordered_set<std::string> process(const lte_instruction &i, const leveldb::ReadOptions &ro) const;


            /**
             * shared_ptr to onclose object.
             * When last shared_ptr gets destroyed, onclose trigger its operation
             */
            std::shared_ptr<onclose> _onclose;

            /**
             * name of collection
             */
            std::string _name;

            /**
             * version of collection.
             * Increased once for every 'persist' operation to store
             */
            int _version;

            /**
             * flag to check if version has already been increased.
             * prevent more increases to version while true
             */
            bool _version_increased;

            /**
             * pointer to parent store.
             */
            store *_store;

            /**
             * pointer to underlying database
             */
            leveldb::DB *_db;

            /**
             * shared database options
             */
            leveldb::Options _opt;

            /**
             * set of currently stored indices to documents' datapoints
             */
            std::unordered_set<index_path> _indices;

        };
    }
}

#endif //ESNPL_STOR_COLLECTION_H