//
// Created by devhack on 23/05/16.
//

#ifndef ESNPL_STOR_DOCUMENT_EXCEPTION_H
#define ESNPL_STOR_DOCUMENT_EXCEPTION_H

#include <stdexcept>

namespace esft{
    namespace stor{

        /**
         * @brief document_exception is used to signal an error occurred while using
         * document, node, iterator or const_iterator
         */
        class document_exception: public std::runtime_error{
        public:
            document_exception(const std::string &msg) : runtime_error(msg) { }
        };
    }
}

#endif //ESNPL_STOR_DOCUMENT_EXCEPTION_H
