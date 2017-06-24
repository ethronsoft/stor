//
// Created by efelici on 6/28/2016.
//

#include <stor/store/lte_instruction.h>


namespace esft{
    namespace stor{

        lte_instruction::lte_instruction(index_path &&path, const std::string &value):
                _target{std::move(path)},_value{value}
        {
        }

        std::unordered_set<std::string> lte_instruction::execute(const collection &c,const leveldb::ReadOptions &ro) const
        {
            return c.process(*this,ro);
        }

        const index_path& lte_instruction::target() const { return _target; }

        const std::string& lte_instruction::value() const { return _value; }

        void lte_instruction::add_child(std::unique_ptr<query_instruction> ptr) { }


    }
}