//
// Created by efelici on 6/28/2016.
//

#include <stor/store/lt_instruction.h>

namespace esft{
    namespace stor{

        lt_instruction::lt_instruction(index_path &&path, const std::string &value):
                _target{std::move(path)},_value{value}{ }

        std::unordered_set<std::string> lt_instruction::execute(const collection &c,
                                                                 const leveldb::ReadOptions &ro) const
        {
            return c.process(*this,ro);
        }

        void lt_instruction::add_child(std::unique_ptr<query_instruction> ptr) {}

        const index_path& lt_instruction::target() const {return _target; }

        const std::string& lt_instruction::value() const {return _value; }

    }
}