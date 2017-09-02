#ifndef ESNPL_STOR_ITERATOR_H
#define ESNPL_STOR_ITERATOR_H

#include <stor/document/node.h>
#include <rapidjson/document.h>

namespace esft{

namespace stor{

    class node;
    class const_iterator;

    class iterator {
        friend class node;
        /**
         * to allow conversion from iterator to const_iterator
         */
        friend class const_iterator;
    public:
        /**
         * @brief Constructor 1
         *
         * Constructs an iterator for an underlying array node
         *
         * @param it  underlying data array iterator
         * @param doc  pointer to rapidjson Document.
         */
        iterator(rapidjson::Value::ValueIterator it, rapidjson::Document *doc);

        /**
          * @brief Constructor 2
          *
          * Constructs an iterator for an underlying object node
          *
          * @param it  underlying data object iterator
          * @param doc  pointer to rapidjson Document.
          */
        iterator(rapidjson::Value::MemberIterator it, rapidjson::Document *doc);

        /**
         * @brief Move constructor
         */
        iterator(iterator &&o);

        /**
         * @brief Copy constructor
         */
        iterator(const iterator &o);

        /**
         * @brief member-wise swap
         */
        friend void swap(iterator &lh, iterator &rh);

        /**
         * @brief assignment operator
         */
        iterator &operator=(iterator o);

        /**
         * @brief Destructor
         */
        ~iterator() {}

        /**
         * @brief Advances iterator forward by one
         */
        iterator operator++();

        /**
         * @brief Returns node (array element or object value) this iterator points to
         *
         * @return node iterator points to
         */
        node operator*();

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
        bool operator==(const iterator &rh) const;

        /**
         * @brief iterator inequality comparator
         */
        bool operator!=(const iterator &rh) const;

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
                rapidjson::Value::ValueIterator vit;
                rapidjson::Value::MemberIterator mit;
            } _it;
        } _ith;

        rapidjson::Document *_doc;
    };

}

}

#endif // ESNPL_STOR_ITERATOR_H
