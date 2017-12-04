

#ifndef ESNPL_STOR_DOCUMENT_H
#define ESNPL_STOR_DOCUMENT_H

#include <unordered_map>
#include <memory>
#include <stor/document/node.h>
#include <rapidjson/document.h>
#include <cstdint>

namespace esft {
    namespace stor {


        /**
         * @brief Document is a Node that represnts the root of the node's tree.
         */
        class document: public node{

        public:

            /**
             * @brief unique identifier for this document.
             */
            typedef std::string identifier;

            /**
             * @brief Constructor 1
             *
             * @param json  underlying data json representation
             * @param id    unique identifier for this document. It must be unique.
             */
            document(const std::string &json, document::identifier id = make_id());

            /**
             * @brief Constructor 2
             *
             * @param json  underlying data json representation
             */
            document(const char *json);

            /**
             * @brief Copy constructor. Not allowed
             */
            document(const document &o) = delete;

            /**
             * @brief Copy assignment. Not allowed
             */
            document &operator=(const document &o) = delete;

            /**
             * @brief Move constructor
             */
            document(document &&o);

            /**
             * @brief Move constructor
             */
            document &operator=(document &&o);

            /**
             * @brief Defaukt constructor
             */
            document();

            /**
             * @brief Factory function that returns document with root node as object
             */
            static document as_object();

            /**
             * @brief Factory function that returns document with root node as array
             */
            static document as_array();

            /**
             * @brief Generates a unique id
             *
             * @return unique id
             */
            static document::identifier make_id();

            /**
             * @brief Return document's id
             *
             * @return id
             */
            const document::identifier &id() const;

            /**
             * @brief Builds document with JSON data read from std::istream
             */
            friend std::istream &operator>>(std::istream &is, document &d);

        private:

            /**
             * @brief rapidjson::Document represents a the root of the
             * underlying nodes tree (and owner of all underlying nodes)
             * as well as the source to allocate memory for new nodes
             */
            rapidjson::Document _rjdoc;

            /**
             * @brief unique identifier
             */
            identifier _id;
        };

        inline bool operator==(const document &lh, const document &rh){
            return lh.id() == rh.id();
        }

        inline bool operator<(const document &lh, const document &rh){
            return lh.id() < rh.id();
        }


    }
}

//hash
namespace std{

    template <>
    struct hash<esft::stor::document>{
        std::size_t operator()(const esft::stor::document &d) const{
            return std::hash<std::string>{}(d.id());
        }
    };
}


#endif //ESNPL_STOR_DOCUMENT_H
