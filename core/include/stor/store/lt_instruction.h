

#ifndef ESNPL_STOR_LT_INSTRUCTION_H
#define ESNPL_STOR_LT_INSTRUCTION_H


#include <string>
#include <stor/store/query_instruction.h>
#include <stor/store/index_path.h>
#include <stor/store/collection.h>

namespace esft{
    namespace stor{

        /**
         * @brief lt_instruction returns all store keys for
         * documents that have index_path less than a target value
         */
        class lt_instruction: public query_instruction{
        public:

            /**
             * @brief Constructor
             * @param path path to value to evaluate for `less operation`
             * @param value value to evaluate
             */
            lt_instruction(index_path &&path, const std::string &value);

            virtual std::unordered_set<std::string> execute(const collection &c,const leveldb::ReadOptions &ro) const override;

            /**
             * @brief returns the document's index path to the target value
             * @return  document's index path to the target value
             */
            const index_path &target() const;

            /**
             * @brief returns the value to evaluate for `less operation`
             * @return value to evaluate for `less than operation`
             */
            const std::string &value() const;

            virtual void add_child(std::unique_ptr<query_instruction> ptr);

        private:

            /**
             * @brief document's path to node used as right hand operand to the less than operator
             */
            index_path _target;
            /**
             * @brief target value used as left hand operand to the less than operator
             */
            std::string _value;


        };
    }
}


#endif //ESNPL_STOR_LT_INSTRUCTION_H
