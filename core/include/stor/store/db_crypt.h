//
// Created by efelici on 6/15/2016.
//

#ifndef ESNPL_STOR_DB_CRYPT_H
#define ESNPL_STOR_DB_CRYPT_H

#include <memory>
#include <stor/store/access_manager.h>
#include <string>

namespace esft{
    namespace stor{

        /**
         * @brief db_crypt is a functor responsible to either encrypt or decrypt
         * the whole database, with the help of access_manager.
         */
        class db_crypt{
        public:

            enum class mode{
                ENCRYPT,
                DECRYPT
            };

            /**
             * @brief Constructor
             *
             * @param m crypto mode
             * @param am shared pointer to access_manager.
             */
            db_crypt(mode m, const std::shared_ptr<access_manager> &am);

            /**
             * @brief apply crypto function to file with name 'fn'
             *
             * @param name of file to apply crypto function to
             */
            bool operator()(const std::string &fn) const;


        private:
            std::shared_ptr<access_manager> _am;

            size_t _thread_pool_size;

            std::string _crypted_file_name_ext;

            std::function<int(const std::string &,const std::string &)> _crypto_funct;
        protected:

        };
    }
}

#endif //ESNPL_STOR_DB_CRYPT_H
