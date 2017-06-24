//
// Created by devhack on 09/06/16.
//

#ifndef ESNPL_STOR_ACCESS_MANAGER_H
#define ESNPL_STOR_ACCESS_MANAGER_H

#include <string>
#include <array>

namespace esft{
    namespace stor{

        /**
         * @brief access_manager is responsible for encryption and decryption of files
         * and can be used to encrypt and decrypt the database
         */
        class access_manager{
        public:

            /**
             * @brief result is a code representing the state of the crypto operation
             */
            typedef int result;

            /**
             * @brief crypto operation successful
             */
            constexpr static int ok = 1;

            /**
             * @brief crypto operation unsuccessful
             */
            constexpr static int cipher_fail = 2;

            /**
             * @brief post-decryption file comparison failed
             */
            constexpr static int checksum_fail = 3;

            /**
             * @brief Constructor
             *
             * @param password key used for encryption
             */
            access_manager(const std::string &password):
                    _pwd(password) {}

            /**
             * @brief Destructor
             */
            virtual ~access_manager() {}

            /**
             * @brief encrypt input_file_name file with name
             *  output_file_name
             *
             *  @param input_file_name name of file to encrypt
             *  @param output_file_name name of resulting encrypted file
             */
            virtual result encrypt(const std::string &input_file_name,
                                 const std::string &output_file_name) const = 0;

            /**
            * @brief decrypt input_file_name file with name
            *  output_file_name
            *
            *  @param input_file_name name of file to decrypt
            *  @param output_file_name name of resulting decrypted file
            */
            virtual result decrypt(const std::string &input_file_name,
                                 const std::string &output_file_name) const = 0;

        protected:

            /**
             * @brief Crypto key
             */
            std::string _pwd;

        };
    }
}

#endif //ESNPL_STOR_ACCESS_MANAGER_H
