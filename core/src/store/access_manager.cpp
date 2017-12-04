

#include <string>
#include <stor/store/access_manager.h>
#include <stor/exceptions/access_exception.h>


#ifdef STOR_CRYPTO
#include <ios>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>
#include <fstream>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#endif


namespace esft{
    namespace stor{


#ifndef STOR_CRYPTO

        struct access_manager::impl{

            impl(const std::string &pwd): _pwd{pwd} {}

            access_manager::result encrypt(const std::string &input_file_name,
                           const std::string &output_file_name) const{
                throw access_exception{"stor compiled without crypto facilities"};
            }

            result decrypt(const std::string &input_file_name,
                           const std::string &output_file_name) const{
                throw access_exception{"stor compiled without crypto facilities"};
            }

            void dispose() noexcept {}

            std::string _pwd;
        };

#else

        std::fstream::pos_type file_size(const std::string &file_name) {
            std::ifstream ifs{file_name, std::ios_base::ate | std::ios_base::binary};
            return ifs.tellg();
        }


        struct access_manager::impl{

            constexpr static size_t hash_size = 32;

            constexpr static int decrypt_mode = 0;

            constexpr static int encrypt_mode = 1;

            constexpr static size_t key_size = 16;

            constexpr static size_t IV_size = 16;

            typedef std::array<unsigned char,hash_size> hash;

            /**
             * must be multiple of cipher_block_size
             * (16 in current implementation)
             */
            constexpr static int _buffer_size = 1024;


            access_manager::result encrypt(const std::string &input_file_name,
                                           const std::string &output_file_name) const{

                std::fstream in{input_file_name,
                                std::ios_base::binary | std::ios_base::in};
                std::fstream out{output_file_name,
                                 std::ios_base::binary | std::ios_base::out};

                if (!in.is_open() | !out.is_open()) {
                    throw access_exception{"failed opening streams"};
                }

                unsigned char *key = const_cast<unsigned char *>(
                        reinterpret_cast<const unsigned char *>(_pwd.c_str())
                );

                //generate initializing vector
                auto _iv = iv();
                //write to output stream
                for (size_t i = 0; i < IV_size; ++i) {
                    out << _iv[i];
                }

                //checksum
                hash checksum;

                //initialize EVP context
                EVP_CIPHER_CTX *ctx;
                if (!(ctx = EVP_CIPHER_CTX_new())) {
                    EVP_CIPHER_CTX_free(ctx);
                    return access_manager::cipher_fail;
                }
                if (EVP_CipherInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, _iv.data(), encrypt_mode) != 1) {
                    EVP_CIPHER_CTX_free(ctx);
                    return access_manager::cipher_fail;
                }

                //initializing SHA context for checksum
                SHA256_CTX sha256_ctx;
                if (SHA256_Init(&sha256_ctx) != 1) {
                    EVP_CIPHER_CTX_free(ctx);
                    return access_manager::checksum_fail;
                }


                //encryption
                {
                    int bytes_encrypted = 0;
                    std::array<unsigned char, _buffer_size> buffer;
                    while (true) {
                        //make sure output buffer will have at least as much space to contain input + up to EVP_MAX_BLOCK_LENGTH
                        in.read(reinterpret_cast<char *>(&buffer[0]), (_buffer_size - EVP_MAX_BLOCK_LENGTH));
                        size_t read = in.gcount();//number of bytes actually read by above read()
                        if (read < 1) break;//we are done

                        //checksum accrual
                        if (SHA256_Update(&sha256_ctx, &buffer[0], read) != 1) {
                            EVP_CIPHER_CTX_free(ctx);
                            return access_manager::checksum_fail;
                        }

                        //data encryption
                        if (EVP_CipherUpdate(ctx, &buffer[0], &bytes_encrypted, &buffer[0], read) != 1) {
                            EVP_CIPHER_CTX_free(ctx);
                            return access_manager::cipher_fail;
                        }
                        for (int i = 0; i < bytes_encrypted; ++i) {
                            out << buffer[i];
                        }

                        if (!in.good()) break;
                    }
                    //flush remaining bytes
                    if (EVP_CipherFinal_ex(ctx, &buffer[0], &bytes_encrypted) != 1) {
                        EVP_CIPHER_CTX_free(ctx);
                        return access_manager::cipher_fail;
                    }
                    for (int i = 0; i < bytes_encrypted; ++i) {
                        out << buffer[i];
                    }
                }

                //Clean up
                EVP_CIPHER_CTX_free(ctx);

                //append checksum to end of file
                if (SHA256_Final(checksum.data(), &sha256_ctx) != 1) {
                    return access_manager::checksum_fail;
                }
                for (size_t i = 0; i < hash_size; ++i) {
                    out << checksum[i];
                }

                return access_manager::ok;

            }

            result decrypt(const std::string &input_file_name,
                           const std::string &output_file_name) const{
                unsigned char *key = const_cast<unsigned char *>(
                        reinterpret_cast<const unsigned char *>(_pwd.c_str())
                );

                std::fstream in{input_file_name,
                                std::ios_base::binary | std::ios_base::in};
                std::fstream out{output_file_name,
                                 std::ios_base::binary | std::ios_base::out};

                if (!in.is_open() | !out.is_open()) {
                    throw access_exception{"failed opening streams"};
                }
                auto in_size = file_size(input_file_name);

                //first IV_SIZE bytes are IV. extract IV_SIZE bytes to get IV used for encryption.
                std::array<unsigned char, IV_size> _iv;
                //extract IV
                {
                    int bytes_read = 0;

                    char iv_buffer[IV_size];
                    if (!in.read(iv_buffer, IV_size)) {
                        return access_manager::cipher_fail;
                    }
                    bytes_read = in.gcount();
                    if (bytes_read != IV_size) {
                        return access_manager::cipher_fail;
                    }
                    for (size_t i = 0; i < IV_size; ++i) {
                        unsigned char *tmp = reinterpret_cast<unsigned char *>(&iv_buffer[i]);
                        _iv[i] = *tmp;
                    }
                }


                //with the obtained IV, and provided key, initialize EVP_Context for decryption
                EVP_CIPHER_CTX *ctx;
                if (!(ctx = EVP_CIPHER_CTX_new())) {
                    EVP_CIPHER_CTX_free(ctx);
                    return access_manager::cipher_fail;
                }
                if (EVP_CipherInit_ex(ctx, EVP_aes_128_cbc(), nullptr, key, _iv.data(), decrypt_mode) != 1) {
                    EVP_CIPHER_CTX_free(ctx);
                    return access_manager::cipher_fail;
                }

                //initializing SHA context for checksum
                SHA256_CTX sha256_ctx;
                if (SHA256_Init(&sha256_ctx) != 1) {
                    EVP_CIPHER_CTX_free(ctx);
                    return access_manager::checksum_fail;
                }

                //decrypt
                //After the first IV_size bytes, the encrypted file presents the data + checksum at the end
                //encrypt data and calculate checksum for it, which will be then used to compare the two checksums
                {

                    std::array<unsigned char, _buffer_size> buffer;
                    //make sure output buffer will have at least as much space to contain input + up to EVP_MAX_BLOCK_LENGTH
                    std::array<unsigned char, _buffer_size + EVP_MAX_BLOCK_LENGTH> obuffer;
                    int decrypted_bytes = 0;
                    //read the next 'in_size - IV_size - checksum size' bytes these are the content of the original data
                    std::size_t data_len = size_t(in_size) - IV_size - hash_size;
                    std::size_t to_read = std::min(data_len, size_t(_buffer_size));
                    while (to_read > 0) {

                        in.read(reinterpret_cast<char *>(&buffer[0]), to_read);
                        size_t read = in.gcount();
                        data_len -= read;
                        to_read = std::min(data_len, size_t(_buffer_size));

                        if (EVP_CipherUpdate(ctx, &obuffer[0], &decrypted_bytes, &buffer[0], read) != 1) {
                            EVP_CIPHER_CTX_free(ctx);
                            return access_manager::cipher_fail;
                        }
                        for (int i = 0; i < decrypted_bytes; ++i) {
                            out << obuffer[i];
                        }

                        //checksum accrual
                        if (SHA256_Update(&sha256_ctx, &obuffer[0], decrypted_bytes) != 1) {
                            EVP_CIPHER_CTX_free(ctx);
                            return access_manager::checksum_fail;
                        }

                    }
                    //flush remaining bytes
                    if (EVP_CipherFinal_ex(ctx, &obuffer[0], &decrypted_bytes) != 1) {
                        EVP_CIPHER_CTX_free(ctx);
                        return access_manager::cipher_fail;
                    }
                    for (int i = 0; i < decrypted_bytes; ++i) {
                        out << obuffer[i];
                    }

                    //checksum accrual
                    if (SHA256_Update(&sha256_ctx, &obuffer[0], decrypted_bytes) != 1) {
                        EVP_CIPHER_CTX_free(ctx);
                        return access_manager::checksum_fail;
                    }
                }

                EVP_CIPHER_CTX_free(ctx);

                //decrypted checksum
                hash checksum;
                if (SHA256_Final(checksum.data(), &sha256_ctx) != 1) {
                    return access_manager::checksum_fail;
                }

                //file checksum, last hash_size bytes
                hash fchecksum;
                {
                    in.read((reinterpret_cast<char *>(fchecksum.data())), hash_size);
                    size_t read = in.gcount();
                    if (read != hash_size) {
                        return access_manager::checksum_fail;
                    }
                }

                //compare checksums
                for (size_t i = 0; i < hash_size; ++i) {
                    if (checksum[i] != fchecksum[i]) {
                        return access_manager::checksum_fail | access_manager::cipher_fail;
                    }
                }

                return access_manager::ok;
            }

            /**
             * generates random IV
             *
             * @return InitializingVector of IV_size
             */
            std::array<unsigned char,IV_size> iv() const{
                std::array<unsigned char, IV_size> res;
                RAND_bytes(res.data(), IV_size);
                return res;
            };

            impl(const std::string & pwd): _pwd{pwd}{
                if (pwd.size() != key_size) {
                    std::ostringstream oss;
                    oss << key_size;
                    throw access_exception{"illegal password. Length must be " + oss.str()};
                }
            }

            void dispose() noexcept{
                EVP_cleanup();
                CRYPTO_cleanup_all_ex_data();
                ERR_free_strings();
            }


            const std::string _pwd;

        };

#endif


        access_manager::access_manager(const std::string &pwd) :
                _impl{std::unique_ptr<access_manager::impl>{new access_manager::impl{pwd}}}
        {}

        access_manager::access_manager(access_manager &&o): _impl{std::move(o._impl)} {}

        access_manager& access_manager::operator=(access_manager &&o) {
            _impl = std::move(o._impl);
            return *this;
        }

        access_manager::~access_manager() {_impl->dispose();}

        access_manager::result access_manager::encrypt(const std::string &input_file_name, const std::string &output_file_name) const {
            return _impl->encrypt(input_file_name,output_file_name);
        }

        access_manager::result access_manager::decrypt(const std::string &input_file_name, const std::string &output_file_name) const {
            return _impl->decrypt(input_file_name,output_file_name);
        }


    }
}