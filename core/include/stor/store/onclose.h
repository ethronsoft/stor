//
// Created by devhack on 05/06/16.
//

#ifndef ESNPL_STOR_AUTODELETE_H
#define ESNPL_STOR_AUTODELETE_H

#include <string>
#include <functional>

namespace esft {
    namespace stor {

        /**
         * @brief onclose is responsible for triggering a certain operation when destructed
         */
        class onclose final{
            friend class store;
        public:

            /**
             * @brief Destructor. Triggers operation
             */
            ~onclose();

            /**
             * @brief Move constructor
             */
            onclose(onclose &&o);

            /**
             * @brief Move assignment
             */
            onclose &operator=(onclose &&o);

            /**
             * @brief Copy constructor. Not usable.
             */
            onclose(const onclose &o) = delete;

            /**
             * @brief Copy assignment. Not usable.
             */
            onclose &operator=(const onclose &o) = delete;

        private:
            typedef std::function<void(const std::string &)> operation;

            /**
             * @brief Constructor
             */
            onclose(const std::string &db_path, operation op);

            /**
             * operation to be triggered
             */
            operation _op;

            /**
             * path used by operation
             */
            std::string _path;
        };
    }
}
#endif //ESNPL_STOR_AUTODELETE_H
