

#ifndef ESNPL_STOR_QUERY_H
#define ESNPL_STOR_QUERY_H

#include <stor/store/query_instruction.h>
#include <unordered_set>
#include <string>
#include <memory>
#include <stor/document/document.h>
#include <stor/document/node.h>

namespace esft{
    namespace stor{

        class query_instruction;

        class collection;
        
        /**
         * @brief query is responsible for evaluating a query_instruction tree,
         * obtained as a JSON string or document, and return as result relevant
         * documents' ids.
         *
         */
        class query{
        public:

            /**
             * @brief Constructor 1
             */
            query(const document &d);

            /**
             * @brief Constructor 2
             */
            query(const char *json);

            /**
             * @brief evaluates query_instructions on collection 'c' using 'ro' as ReadOptions
             *
             * @param c collection to extract from
             * @param ro ReadOptions to use during query_instruction extraction
             */
            std::unordered_set<std::string> answer(const collection &c,const leveldb::ReadOptions &ro) const;

        private:

            /**
             * query_instruction tree
             */
            std::unique_ptr<query_instruction> _root;
        };
    }
}
#endif //ESNPL_STOR_QUERY_H
