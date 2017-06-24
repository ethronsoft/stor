//
// Created by devhack on 30/05/16.
//

#ifndef ESNPL_STOR_INDEX_PATH_H
#define ESNPL_STOR_INDEX_PATH_H

#include <stor/document/document.h>
#include <string>
#include <functional>
#include <vector>

namespace esft{
    namespace stor{

        /**
         * @brief index_path represents a path to a specific
         * JSON element in a document, with the following structure
         *
         * root.parent1.parent2.leaf
         *
         * i.e. Given the following JSON: {"a":{"b":1}} ,
         * the path to value 1 is "a.b"
         */
        class index_path{

        public:

            /**
             * @brief Constructor 1
             */
            index_path(const std::string &path);

            /**
            * @brief Constructor 2
            */
            index_path(const char *path);

            /**
             * @brief Copy constructor
             */
            index_path(const index_path &o);

            /**
             * @brief Move constructor
             */
            index_path(index_path &&o);

            /**
             * @brief Returns the value of this index_path extracted from document doc
             *
             * @param doc document to scan through, following index_path
             *
             * @return value of index_path extracted from doc
             */
            std::pair<std::string,std::string> extract(const document &doc) const;

            /**
             * @brief Returns string representation of path
             *
             * @return string representation of path
             */
            std::string str() const;



        private:

            /**
             * @brief index_path tokens, which
             * is each string in path separated by '.'
             */
            std::vector<std::string> _tokens;
        };

        inline bool operator==(const index_path &lh, const index_path &rh){
            return lh.str() == rh.str();
        }

        inline bool operator<(const index_path &lh, const index_path &rh){
            return lh.str() < rh.str();
        }

    }
}

//hash
namespace std{

    template <>
    struct hash<esft::stor::index_path>{
        std::size_t operator()(const esft::stor::index_path &p) const{
            return std::hash<std::string>{}(p.str());
        }
    };
}

#endif //ESNPL_STOR_INDEX_PATH_H
