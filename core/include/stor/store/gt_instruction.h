//
// Created by devhack on 05/06/16.
//

#ifndef ESNPL_STOR_GT_INSTRUCTION_H
#define ESNPL_STOR_GT_INSTRUCTION_H

#include <string>
#include <stor/store/index_path.h>
#include <stor/store/query_instruction.h>
#include <stor/store/collection.h>

namespace esft{
    namespace stor{


        /**
         * @brief gt_instruction returns all store keys for
         * documents that have index_path greater than a target value
         */
        class gt_instruction: public query_instruction{
        public:

            gt_instruction(index_path &&path, const std::string &value);

            virtual std::unordered_set<std::string> execute(const collection &c,const leveldb::ReadOptions &ro) const override;

            const index_path &target() const;

            const std::string &value() const;

            virtual void add_child(std::unique_ptr<query_instruction> ptr);

        private:

            /**
             * @brief document's path to node used as right hand operand to the greater than operator
             */
            index_path _target;
            /**
             * @brief target value used as left hand operand to the greater than operator
             */
            std::string _value;

        };

    }
}
#endif //ESNPL_STOR_GT_INSTRUCTION_H
