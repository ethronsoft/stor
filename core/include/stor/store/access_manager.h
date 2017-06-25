//
// Created by devhack on 09/06/16.
//

#ifndef ESNPL_STOR_ACCESS_MANAGER_H
#define ESNPL_STOR_ACCESS_MANAGER_H

#include <string>
#include <array>
#include <memory>

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
            constexpr static result ok = 1;

            /**
             * @brief crypto operation unsuccessful
             */
            constexpr static result cipher_fail = 2;

            /**
             * @brief post-decryption file comparison failed
             */
            constexpr static result checksum_fail = 3;

            /**
             * @brief Constructor
             *
             * @param password key used for encryption
             */
            access_manager(const std::string &password);

            /**
             * @brief Move Constructor
             * @param o object to move
             */
            access_manager(access_manager &&o);

            /**
             * @brief Move assignment
             * @param o object to move
             */
            access_manager &operator=(access_manager &&o);

            /**
             * @brief Destructor
             */
            virtual ~access_manager();

            /**
             * @brief encrypt input_file_name file with name
             *  output_file_name
             *
             *  @param input_file_name name of file to encrypt
             *  @param output_file_name name of resulting encrypted file
             */
            virtual result encrypt(const std::string &input_file_name,
                                 const std::string &output_file_name) const;

            /**
            * @brief decrypt input_file_name file with name
            *  output_file_name
            *
            *  @param input_file_name name of file to decrypt
            *  @param output_file_name name of resulting decrypted file
            */
            virtual result decrypt(const std::string &input_file_name,
                                 const std::string &output_file_name) const;

        protected:

            //Pointer To Implementation idiom
            struct impl;

            std::unique_ptr<impl> _impl;

        };
    }
}

#endif //ESNPL_STOR_ACCESS_MANAGER_H
