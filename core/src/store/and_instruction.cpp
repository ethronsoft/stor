//
// Created by devhack on 05/06/16.
//


#include <algorithm>
#include <stor/store/query_instruction.h>
#include <stor/store/and_instruction.h>

namespace esft{
    namespace stor{

        and_instruction::and_instruction() {}

        std::unordered_set<std::string> and_instruction::execute(const collection &c,const leveldb::ReadOptions &ro) const
        {
            std::unordered_set<std::string> res;

            //intersection specifically for unorederd_set (std:set_intersection requires sorted input)
            typedef  std::unordered_set<std::string> us;
            auto intersection = [](const us &s1, const us &s2) {
                us res;
                if (s1.empty() && s2.empty()) return res;
                for (const auto &x : s1){
                    if (s2.count(x) > 0) res.insert(x);
                }
                return res;
            };

            for (size_t i = 0; i < _children.size(); ++i){
                auto &child = _children[i];
                std::unordered_set<std::string> child_res = child->execute(c,ro);
                if (i == 0){
                    res = std::move(child_res);
                }else{
                    res = intersection(res,child_res);
                }

            }
            return res;
        }

        void and_instruction::add_child(std::unique_ptr<query_instruction> ptr)
        {
            _children.push_back(std::move(ptr));
        }

    }
}