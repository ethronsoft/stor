//
// Created by devhack on 05/06/16.
//

#ifndef ESNPL_STOR_OR_INSTRUCTION_H
#define ESNPL_STOR_OR_INSTRUCTION_H

#include <stor/store/query_instruction.h>
#include <stor/store/collection.h>
#include <stor/store/index_path.h>
#include <string>
#include <memory>


namespace esft{
    namespace stor{


        /**
         * @brief and_instruction aggregates all children's results with
         * the set UNION operation
         */
        class or_instruction: public query_instruction{
        public:

            or_instruction();

            virtual std::unordered_set<std::string> execute(const collection &c,const leveldb::ReadOptions &ro) const override;

            virtual void add_child(std::unique_ptr<query_instruction> ptr);

        private:
            std::vector<std::unique_ptr<query_instruction>> _children;
        };

    }
}
#endif //ESNPL_STOR_OR_INSTRUCTION_H
