
#ifndef ESNPL_STOR_CONST_ITERATOR_H
#define ESNPL_STOR_CONST_ITERATOR_H

#include <rapidjson/document.h>
#include <stor/document/node.h>

namespace esft{

    namespace stor{

        class node;
        class iterator;

        /**
         * @brief const_iterator allows to iterate over read-only nodes
         */
        class const_iterator {
            friend class node;
        public:

            /**
             * @brief Constructor 1
             *
             * Constructs a const iterator for an underlying array node
             *
             * @param it  underlying data array const iterator
             * @param doc  pointer to rapidjson Document.
             */
            const_iterator(rapidjson::Value::ConstValueIterator it, rapidjson::Document *doc);

            /**
              * @brief Constructor 2
              *
              * Constructs a const iterator for an underlying object node
              *
              * @param it  underlying data object const iterator
              * @param doc  pointer to rapidjson Document.
              */
            const_iterator(rapidjson::Value::ConstMemberIterator it, rapidjson::Document *doc);

            /**
             * @brief Conversion from iterator constructor
             */
            const_iterator(const iterator &o);

            /**
             * @brief Conversion from iterator assignment
             */
            const_iterator &operator=(const iterator &o);

            /**
             * @brief Move constructor
             */
            const_iterator(const_iterator &&o);

            /**
             * @brief Copy constructor
             */
            const_iterator(const const_iterator &o);

            /**
             * @brief member-wise swap 
             */
            friend void swap(const_iterator &lh, const_iterator &rh);

            /**
             * @brief assignment operator
             */
            const_iterator &operator=(const_iterator o);

            /**
             * @brief Destructor
             */
            ~const_iterator() {}

            /**
             * @brief Advances iterator forward by one
             */
            const_iterator operator++();

            /**
             * @brief Returns const node (array element or object value) this iterator points to
             *
             * @return const node iterator points to
             */
            const node operator*() const;

            /**
             * @brief If underlying iterator is for object node, returns the key of the object
             * this iterator points to
             *
             * @return pointed to object's key
             *
             * @throws  @ref document_exception if the underlying iterator is not for an object node
             */
            std::string key() const;

            /**
             * @brief iterator equality comparator
             */
            bool operator==(const const_iterator &rh) const;

            /**
             * @brief iterator inequality comparator
             */
            bool operator!=(const const_iterator &rh) const;

        private:

            /**
             * internal helper variable to distinguish between
             * an underlying iterator that points to an array (VALUE) or object(MEMBER)
             */
            enum class type {MEMBER,VALUE};

            /**
             * tagged union struct to underlying iterator
             */
            struct it_holder{
                type _tag;
                union it{
                    rapidjson::Value::ConstValueIterator vit;
                    rapidjson::Value::ConstMemberIterator mit;
                } _it;
            };

            it_holder _ith;

            rapidjson::Document *_doc;
        };

    }

}
#endif //ESNPL_STOR_CONST_ITERATOR_H
