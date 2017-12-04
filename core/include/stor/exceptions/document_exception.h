

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

            /**
             * @brief Contructor
             * @param msg error message
             */
            document_exception(const std::string &msg) : runtime_error(msg) { }
        };
    }
}

#endif //ESNPL_STOR_DOCUMENT_EXCEPTION_H
