

#ifndef ESNPL_STOR_NODE_TMP_H
#define ESNPL_STOR_NODE_TMP_H

#include <cstdint>
#include <iostream>
#include <vector>
#include <rapidjson/document.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>
#include <stor/document/iterator.h>
#include <stor/document/const_iterator.h>
#include <stor/exceptions/document_exception.h>

namespace esft{
    namespace stor{

        class iterator;
        class const_iterator;

        /** @brief node represents an interface to a specific element in the underlying JSON data tree.
         *
         *  node is a thin wrapper of an underlying JSON data node, only made of weak pointers
         *  to such data and exposes the functions used to create or alter JSON data. For this reason,
         *  nodes can cheaply be returned by value.
         *  ATTENTION:
         *  Because the owner of the underlying JSON tree is esft::stor::document,
         *  a single node existence depends on the existence of document. For this
         *  reason, a node instance must not be used after its document owner has ceased to exist.
         *
         *  Some functions are only usable if the underlying data is either of type
         *  value, object or array, or a combination of them.
         *  If a certain functionality is invoked on the wrong node, a document_exception is thrown.
         *
         */
        class node {
        public:
            friend class iterator;
            friend class const_iterator;

            /**
             * @brief Destructor
             */
            virtual ~node();

            /**
             * @brief Shallow Copy constructor.
             *
             * For deep copy, use json()\json(string)
             * serialization/deserialziation mechanism
             * or copy(node&)
             */
            node(const node &o);

            /**
             * @brief Move constructor
             */
            node(node &&o);

            /**
             * @brief Shallow Copy assignment
             *
             * For deep copy, use json()\json(string)
             * serialization/deserialziation mechanism
             * or copy(node&)
             */
            node &operator=(const node &o);

            /**
             * @brief Move assignment
             */
            node &operator=(node &&o);

            /** @brief Check whether underlying data is a Number.
             *
             * Underlying data is a Number if it's either an:
             * + int
             * + long
             * + double
             * @return True if Number, else False.
             *
             */
            virtual bool is_num() const;

            /** @brief Check whether underlying data is a Integer.
             *
             * @return True if Integer, else False.
             *
             */
            virtual bool is_int() const;

            /** @brief Check whether underlying data is a Boolean.
             *
             * @return True if Boolean, else False.
             *
             */
            virtual bool is_bool() const;

            /** @brief Check whether underlying data is a Long.
             *
             * @return True if Long, else False.
             *
             */
            virtual bool is_long() const;

            /** @brief Check whether underlying data is a Double.
             *
             * @return True if Double, else False.
             *
             */
            virtual bool is_double() const;

            /** @brief Check whether underlying data is a String.
             *
             * @return True if String, else False.
             *
             */
            virtual bool is_string() const;


            /** @brief Check whether underlying data is an Object.
             *
             * An Object is intended in the JSON sense of Object.
             *
             * @return True if Object, else False.
             *
             */
            virtual bool is_object() const;

            /** @brief Check whether underlying data is an Array.
             *
             * An Array is intended in the JSON sense of Array.
             *
             * @return True if Array, else False.
             *
             */
            virtual bool is_array() const;

            /** @brief Check whether underlying data is Null.
             *
             * An Null is intended in the JSON sense of Null,
             * as in Empty. ie: {}, []
             *
             * @return True if Object, else False.
             *
             */
            virtual bool is_null() const;

            /** @brief Attempts to return the underlying data as Integer.
             *
             * This function may only be called on a value node, which is
             * a node that is not an object node nor an array node.
             * In JSON terms, it can only be used for leaf elements.
             * ie: node = "1"       -> ok
             *     node = {"a":1}   -> not ok
             *
             * @return data as Integer
             * @throws @ref document_exception if node is not of type value.
             */
            virtual int as_int() const;

            /** @brief Attempts to return the underlying data as Long.
             *
             * This function may only be called on a value node, which is
             * a node that is not an object node nor an array node.
             * In JSON terms, it can only be used for leaf elements.
             * ie: node = "1"       -> ok
             *     node = {"a":1}   -> not ok
             *
             * @return data as Long
             * @throws document_exception if node is not of type value.
             */
            virtual int64_t as_long() const;

            /** @brief Attempts to return the underlying data as Double.
             *
             * This function may only be called on a value node, which is
             * a node that is not an object node nor an array node.
             * In JSON terms, it can only be used for leaf elements.
             * ie: node = "1.0"       -> ok
             *     node = {"a":1.0}   -> not ok
             *
             * @return data as Double
             * @throws document_exception if node is not of type value.
             */
            virtual double as_double() const;

            /** @brief Attempts to return the underlying data as a string.
             *
             * This function may only be called on a value node, which is
             * a node that is not an object node nor an array node.
             * In JSON terms, it can only be used for leaf elements.
             * ie: node = "hello"       -> ok
             *     node = {"a":"hello"} -> not ok
             *
             * @return data as C++ String
             * @throws document_exception if node is not of type value.
             */
            virtual std::string as_string() const;

            /** @brief Attempts to return the underlying data as a C string
             * paired with its actual length.
             *
             * The actual length refers to the entire string, which may be exceed the first
             * \0 character encountered. Having this length returned is necessary,
             * as JSON can contain \0 (escaped as \u0000, different from \\0 required by C),
             * according to RFC 4627.
             * If you know that the returned string will be a normal C string, then
             * just invoke esft_stor_node_as_string
             *
             * This function may only be called on a value node, which is
             * a node that is not an object node nor an array node.
             * In JSON terms, it can only be used for leaf elements.
             * ie: node = "hello"       -> ok
             *     node = {"a":"hello"} -> not ok
             *
             * @return data as C String with its length
             * @throws document_exception if node is not of type value.
             */
            virtual std::pair<const char *, std::size_t> as_cstring() const;

            /** @brief Attempts to return the underlying data as Boolean.
             *
             * This function may only be called on a value node, which is
             * a node that is not an object node nor an array node.
             * In JSON terms, it can only be used for leaf elements.
             * ie: node = "true"       -> ok
             *     node = {"a":"true"} -> not ok
             *
             * @return data as Boolean
             * @throws document_exception if node is not of type value.
             */
            virtual bool as_bool() const;

            /** @brief Get or create in place object node under key @param key
             *
             * This function can only be called on an object node.
             * If object node exists with @param key, a reference to it is returned.
             * Otherwise, such object is added as a member to the underyling data, with
             * key @param key
             * ie: node = [1,2,3]         -> not ok
             *     node = {"a":{"b": 1}}  -> ok. with_key("a") returns {"b": 1}
             *     node = {"a":1}         -> ok. with_key("b") makes {"a":1,"b":{}} and returns {}
             *
             * @param key parent of object
             * @return object node for key
             * @throws document_exception if node is not of type object.
             */
            virtual node with(const std::string &key);

            /** @brief Get or create in place array node under key @param key
             *
             * This function can only be called on an object node.
             * If array node exists with @param key, a reference to it is returned.
             * Otherwise, such object is added as a member to the underlying data, with
             * key @param key
             * ie: node = [1,2,3]        -> not ok
             *     node = {"a":[1,2,3]}  -> ok. with_key("a") returns [1,2,3]
             *     node = {"a":1}        -> ok. with_key("b") makes {"a":1,"b":[]} and returns []
             *
             * @param key parent of array
             * @return array node for key
             * @throws document_exception if node is not of type object.
             */
            virtual node with_array(const std::string &key);

            /** @brief Set current value node underlying data to @param v
             *
             * @param v Integer to assign as new value to underlying data
             * @throws document_exception if node is not of type value
             */
            virtual void operator=(int v);

            /** @brief Set current value node underlying data to @param v
             *
             * @param v Long to assign as new value to underlying data
             * @throws document_exception if node is not of type value
             */
           void operator=(int64_t v);

            /** @brief Set current value node underlying data to @param v
             *
             * @param v Double to assign as new value to underlying data
             * @throws document_exception if node is not of type value
             */
            virtual void operator=(double v);

            /** @brief Set current value node underlying data to @param v
             *
             * @param v Bool to assign as new value to underlying data
             * @throws document_exception if node is not of type value
             */
            virtual void operator=(bool v);

            /** @brief Set current value node underlying data to @param v
             *
             * @param v String to assign as new value to underlying data
             * @throws document_exception if node is not of type value
             */
            virtual void operator=(const std::string &v);

            /** @brief Set current value node underlying data to @param v
             *
             * @param v C string to assign as new value to underlying data
             * @throws document_exception if node is not of type value
             */
            virtual void operator=(const char * v);

            /** @brief Add key-value to object node
             *
             * @param k key
             * @param v int to assign to key
             * @return this node
             * @throws document_exception if node is not of type object
             */
            virtual node &put(const std::string &key, int v);

            /** @brief Add key-value to object node
             *
             * @param k key
             * @param v long to assign to key
             * @return this node
             * @throws document_exception if node is not of type object
             */
            virtual node &put(const std::string &key, int64_t v);

            /** @brief Add key-value to object node
             *
             * @param k key
             * @param v double to assign to key
             * @return this node
             * @throws document_exception if node is not of type object
             */
            virtual node &put(const std::string &key, double v);

            /** @brief Add key-value to object node
             *
             * @param k key
             * @param v bool to assign to key
             * @return this node
             * @throws document_exception if node is not of type object
             */
            virtual node &put(const std::string &key, bool v);

            /** @brief Add key-value to object node
             *
             * @param k key
             * @param v string to assign to key
             * @return this node
             * @throws document_exception if node is not of type object
             */
            virtual node &put(const std::string &key, const std::string &v);

            /** @brief Add key-value to object node
             *
             * @param k key
             * @param v C string to assign to key
             * @return this node
             * @throws document_exception if node is not of type object
             */
            virtual node &put(const std::string &key, const char *v);

            /** @brief Add int to array node
             *
             * @param v int value
             * @return this node
             * @throws document_exception if node is not of type array
             */
            virtual node &add(int v);

            /** @brief Add long to array node
             *
             * @param v long value
             * @return this node
             * @throws document_exception if node is not of type array
             */
            virtual node &add(int64_t v);

            /** @brief Add double to array node
             *
             * @param v double value
             * @return this node
             * @throws document_exception if node is not of type array
             */
            virtual node &add(double v);

            /** @brief Add bool to array node
             *
             * @param v bool value
             * @return this node
             * @throws document_exception if node is not of type array
             */
            virtual node &add(bool v);

            /** @brief Add string to array node
             *
             * @param v string value
             * @return this node
             * @throws document_exception if node is not of type array
             */
            virtual node &add(const std::string &v);

            /** @brief Add C string to array node
             *
             * @param v C string value
             * @return this node
             * @throws document_exception if node is not of type array
             */
            virtual node &add(const char * v);

            /** @brief Add all items dereferenced from forward_iterator forw_it
             *  to array node.
             *
             * @tparam forw_it forward iterator to items of an array node
             * @param begin iterator pointing to beginning of range
             * @param end   iterator pointing to one after end of range.
             *
             * @return this node
             * @throws document_exception if node is not of type array
             */
            template<typename forw_it>
            node &add(forw_it begin, forw_it end);

            /** @brief Add all items from the vector @p v to array node.
             *
             * @tparam item item of an array node
             * @param v vector of items
             *
             * @return this node
             * @throws document_exception if node is not of type array
             */
            template<typename item>
            node &add(const std::vector<item> &v);

            /** @brief Add array to array node
             *
             * @return created array
             * @throws document_exception if node is not of type array
             */
            virtual node add_array();

            /** @brief Add object to array node
             *
             * @return created object
             * @throws document_exception if node is not of type array
             */
            virtual node add_object();

            /**
             * @brief Check whether a member exists under @param key
             * 
             * This function can only be used from an object node
             * 
             * @param key 
             * @return True if member exists, False otherwise
             * @throws document_exception if called on a non object node
             */
            virtual bool has(const std::string &key) const;

            /**@brief Attempts to return node with key 'key'
             *
             * This function can only be used on an object node
             *
             * @param key
             * @return node with key 'key'
             * @throws @ref document_exception if called on a non object node or
             * if key does not exist
             */
            virtual node operator[](const std::string &key) const;

            /**
             * @brief Attempts to return node at index
             *
             * This function can only be used on an object node
             *
             * @param key
             * @return node with key 'key'
             * @throws @ref document_exception if called on a non array node
             * @throws @ref std::out_of_range if index is out of range
             */
            virtual node operator[](std::size_t indx) const;

            /**
             * @brief Return number of children for this node
             *
             * @throws @ref document_exception if called on a value node
             */
            virtual std::size_t size() const;

            /**
             * @brief Return true if size() == 0
             *
             * @throws @ref document_exception if called on a value node
             */
            virtual bool empty() const;

            /**
             * @brief Remove the child node at key @p key.
             *
             * @return If operation succeded, returns true, else false
             *
             * @throws @ref document_exception if called on a value node
             */
            virtual bool remove(const std::string &key);

            /**
             * @brief removes the member iterator @p it points to and returns the iterator
             * to item coming after the one deleted. If @p it is equal to cend(),
             * the behaviour is undefined.
             *
             * @return next iterator
             */
            const_iterator remove(const_iterator it);

            /**
             * @brief removes the members in the range [first, last) and returns
             * the item coming after the one deleted.
             *
             * @return next iterator
             */
            const_iterator remove(const_iterator first, const_iterator last);

            /**
             * @brief Removes all the children of this node
             *
             * @throws @ref document_exception if called on a value node
             */
            virtual void remove_all() const;

            /**
             * @brief Return string JSON represeantation of the this node and descendants.
             */
            virtual std::string json() const;

            /**
             * @brief replaces this node with the node resulting from parsing @p json
             *
             * @throws @ref document_exception if provided JSON is invalid
             */
            virtual void json(const std::string &jsn);

            /**
             * @brief deep copies the node @p v onto this node.
             */
            virtual void copy(const node &v);

            /**
             * @brief Write this node and descendants into std::ostream.
             *
             * @param os reference to std::ostream to write into
             */
            virtual std::ostream &write_to_stream(std::ostream&os) const;

            /**
             * @brief return iterator to first child of this node
             *
             * @throws @ref document_exception if this node is a value node
             */
            virtual iterator begin();

            /**
             * @brief return iterator to one after the last child of this node
             *
             * @throws @ref document_exception if this node is a value node
             */
            virtual iterator end();

            /**
             * @brief return const_iterator to first child of this node
             *
             * @throws @ref document_exception if this node is a value node
             */
            virtual const_iterator cbegin() const;

            /**
             * @brief return const_iterator to one after the last child of this node
             *
             * @throws @ref document_exception if this node is a value node
             */
            virtual const_iterator cend() const;

        protected:

            /**
             * @brief Constructor
             *
             * @param value pointer to underlying data
             * @param doc pointer to document (node tree owner)
             */
            node(rapidjson::Value *value, rapidjson::Document *doc);

            /**
             * @brief Default constructor
             */
            node();

            /**
             * @brief Inject underlying data pointer into node
             */
            void set_underlying(rapidjson::Value *underlying);

            /**
             * @brief Inject rapidjson::Document pointer into node
             */
            void set_document(rapidjson::Document *doc);

        private:

            /**
             * @brief rapidjson::Value* is a pointer to the underlying data
             */
            rapidjson::Value *_underlying;

            /**
             * @brief rapidjson::Document* is a pointer to the root of the
             * underlying nodes tree (and owner of all underlying nodes)
             * as well as the source to allocate memory for new nodes
             */
            rapidjson::Document *_doc;
        };

        template<typename forw_it>
        inline node &node::add(forw_it begin, forw_it end)
        {
            if (_underlying->GetType() != rapidjson::kArrayType){
                throw document_exception{"invalid request"};
            }
            for (; begin != end; ++begin){
                add(*begin);
            }
            return *this;
        }

        template<typename item>
        inline node & node::add(const std::vector<item> &v) {
            return add<typename std::vector<item>::const_iterator>(v.cbegin(),v.cend());
        }

        inline std::ostream &operator<<(std::ostream&os, const node &n)
        {
            return n.write_to_stream(os);
        }
    }
}
#endif //ESNPL_STOR_NODE_TMP_H
