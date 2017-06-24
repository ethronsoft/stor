//
// Created by efelici on 6/28/2016.
//

#ifndef ESNPL_STOR_NEQ_INSTRUCTION_H
#define ESNPL_STOR_NEQ_INSTRUCTION_H

#include <string>
#include <stor/store/query_instruction.h>
#include <stor/store/index_path.h>
#include <stor/store/collection.h>

namespace esft{
    namespace stor{

        /**
         * @brief neq_instruction returns all store keys for
         * documents that don't have index_path equal to a target value
         */
        class neq_instruction: public query_instruction{
        public:
            neq_instruction(index_path &&path, const std::string &value);

            virtual std::unordered_set<std::string> execute(const collection &c,const leveldb::ReadOptions &ro) const override;

            const index_path &target() const;

            const std::string &value() const;

            virtual void add_child(std::unique_ptr<query_instruction> ptr);

        private:

            /**
             * @brief document's path to node used as right hand operand to non-equality operator
             */
            index_path _target;
            /**
             * @brief target value used as left hand operand to non-equality operator
             */
            std::string _value;


        };
    }
}




#endif //ESNPL_STOR_NEQ_INSTRUCTION_H
