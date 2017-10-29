//
// Created by devhack on 30/05/16.
//

#ifndef ESNPL_STOR_STORE_EXCEPTION_H
#define ESNPL_STOR_STORE_EXCEPTION_H

#include <string>
#include <stdexcept>

namespace esft{
    namespace stor{

        /**
         * @brief store_exception is used to signal an error occurred while
         * using store or collection
         */
        class store_exception: public std::runtime_error{
        public:

            /**
             * @brief Contructor
             * @param msg error message
             */
            store_exception(const std::string &msg) : runtime_error(msg) { }
        };
    }
}

#endif //ESNPL_STOR_STORE_EXCEPTION_H
