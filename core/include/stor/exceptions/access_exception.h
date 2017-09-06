//
// Created by devhack on 23/05/16.
//

#ifndef ESNPL_STOR_ACCESS_EXCEPTION_H
#define ESNPL_STOR_ACCESS_EXCEPTION_H

#include <stdexcept>

namespace esft{
    namespace stor{

        /**
         * @brief access_exception is used to signal an error occurred while
         * encrypting or decrypting the database
         */
        class access_exception: public std::runtime_error{
        public:
            access_exception(const std::string &msg) : runtime_error(msg) { }
        };
    }
}

#endif //ESNPL_STOR_ACCESS_EXCEPTION_H
