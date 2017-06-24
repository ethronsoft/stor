//
// Created by devhack on 05/06/16.
//

#include <algorithm>
#include <stor/store/query_instruction.h>
#include <stor/store/or_instruction.h>

namespace esft{
    namespace stor{

        or_instruction::or_instruction() {}

        std::unordered_set<std::string> or_instruction::execute(const collection &c,const leveldb::ReadOptions &ro) const
        {
            std::unordered_set<std::string> res;

            typedef std::unordered_set<std::string> us;
            auto unionf = [](const us &s1, us &res){
                res.insert(s1.cbegin(),s1.cend());
            };

            for (const auto &child : _children){
                std::unordered_set<std::string> child_res = child->execute(c,ro);
                unionf(child_res,res);
            }
            return res;
        }

        void or_instruction::add_child(std::unique_ptr<query_instruction> ptr)
        {
            _children.push_back(std::move(ptr));
        }

    }
}