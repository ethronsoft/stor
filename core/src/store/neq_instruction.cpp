//
// Created by efelici on 6/28/2016.
//

#include <stor/store/neq_instruction.h>

namespace esft{
    namespace stor{

        neq_instruction::neq_instruction(index_path &&path, const std::string &value):
        _target{std::move(path)},_value{value}{ }

        void neq_instruction::add_child(std::unique_ptr<query_instruction> ptr) {}

        std::unordered_set<std::string> neq_instruction::execute(const collection &c,
                                                                 const leveldb::ReadOptions &ro) const
        {
            return c.process(*this,ro);
        }

        const index_path& neq_instruction::target() const {return _target; }

        const std::string& neq_instruction::value() const {return _value; }

    }
}