//
// Created by devhack on 23/05/16.
//

#ifndef ESNPL_STOR_COLLECTION_H
#define ESNPL_STOR_COLLECTION_H

#include <stor/document/document.h>
#include <stor/store/index_path.h>
#include <stor/store/query.h>
#include <leveldb/db.h>
#include <leveldb/options.h>
#include <unordered_set>
#include <memory>
#include <mutex>


namespace esft {
    namespace stor {

        class store;
        
        class eq_instruction;

        class gt_instruction;

        class gte_instruction;

        class neq_instruction;

        class lt_instruction;

        class lte_instruction;

        /**
         * @brief collection is a logical partition of a store,
         * where documents representing similar or correlated entities
         * can be added.
         */
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
             * @brief Returns a set of all the indices currently in use
             *
             * @return indices in use
             */
            const std::unordered_set<index_path> &indices() const;

            /**
             * @brief Add index to 'path' to allow lookup
             * during query operations
             *
             * @param path path to a datapoint to store index for
             */
            void add_index(const index_path &path);

            /**
             * @brief Add all the index_paths provided by @p ic
             *
             * @param ic iterable container of index_paths
             */
            template<typename indices_container>
            void add_indices(indices_container &&ic);

            /**
             * @brief Add all the index_paths in the range [beg, end)
             *
             * @param beg beginning of the range
             * @param end one after last element to add in the range
             */
            template<typename index_path_iter>
            void add_indices(index_path_iter beg, index_path_iter end);

            /**
             * @brief Return name of collection
             *
             * @returns name of collection
             */
            const std::string &name() const;

            /**
             * @brief Insert/Update document @p doc in the collection
             *
             * @param doc document to insert into collection
             *
             * @returns true if document @p doc was successfully added, false if not
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

        protected:

            /**
             * @brief Constructor.
             *
             * @param s pointer to parent store
             * @param info document containing initializing values for collection
             */
            collection(store *s, document &info);

            /**
             * @brief Move Constructor.
             */
            collection(collection &&o);

            /**
             * @brief Move Assignment.
             */
            collection &operator=(collection &&o);

            /**
             * @brief Stores any structural changes made to this collection
             * in the store it belongs to
             */
            void persist();

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

        private:

            /**
             * name of collection
             */
            std::string _name;

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

        template<typename index_path_iter>
        inline void collection::add_indices(index_path_iter beg, index_path_iter end) {
            bool modified = false;
            //targetting begin/end let's us take in intializer_lists too
            for (; beg != end; ++beg){
                auto path = *beg;
                if (_indices.count(path) == 0) {
                    _indices.insert(path);
                    modified = true;
                }
            }
            if (modified){
                persist();
            }
        }

        template <typename indices_container>
        inline void collection::add_indices(indices_container &&ic) {
            add_indices(ic.begin(), ic.end());
        }


    }
}

#endif //ESNPL_STOR_COLLECTION_H
