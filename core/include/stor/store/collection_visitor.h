//
// Created by devhack on 11/06/16.
//

#ifndef ESNPL_STOR_DIRECTORY_ITERATOR_H
#define ESNPL_STOR_DIRECTORY_ITERATOR_H

#include <string>
#include <vector>

namespace esft {
    namespace stor{

        /**
         * @brief Iterates over all relevant files in a collection
         */
        class collection_visitor{
        public:

            typedef const std::vector<std::string>::const_iterator const_iterator;

            /**
             * @brief Constructor
             *
             * @param stor_root path to root of store
             */
            collection_visitor(const std::string &stor_root);

            /**
             * @brief Return number of files
             *
             * @return number of files
             */
            size_t size() const;

            /**
             * @brief Return file name at index t
             *
             * @param t index of file name to retrieve
             */
            const std::string &operator[](size_t t) const;

            /**
             * @brief const_iterator pointing to first file name
             */
            const_iterator cbegin() const;

            /**
             * @brief const_iterator pointing to one elemeent after last file name
             */
            const_iterator cend() const;

        private:

            /**
             * file names
             */
            std::vector<std::string> _paths;
        };
    }
}
#endif //ESNPL_STOR_DIRECTORY_ITERATOR_H
