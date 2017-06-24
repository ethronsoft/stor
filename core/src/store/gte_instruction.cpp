//
// Created by efelici on 6/27/2016.
//

#include <stor/store/gte_instruction.h>

namespace esft{
    namespace stor{

        gte_instruction::gte_instruction(index_path &&path, const std::string &value):
                _target{std::move(path)},_value{value}
        {
        }

        std::unordered_set<std::string> gte_instruction::execute(const collection &c,const leveldb::ReadOptions &ro) const
        {
            return c.process(*this,ro);
        }

        const index_path& gte_instruction::target() const { return _target; }

        const std::string& gte_instruction::value() const { return _value; }

        void gte_instruction::add_child(std::unique_ptr<query_instruction> ptr) { }


    }
}