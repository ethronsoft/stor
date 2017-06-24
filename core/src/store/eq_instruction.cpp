#include <stor/store/eq_instruction.h>

namespace esft
{
    namespace stor{

        eq_instruction::eq_instruction(index_path &&path, const std::string &value):
                _target{std::move(path)},_value{value}
              {
              }

        std::unordered_set<std::string> eq_instruction::execute(const collection &c,const leveldb::ReadOptions &ro) const{
            return c.process(*this,ro);
        }


        void eq_instruction::add_child(std::unique_ptr<query_instruction> ptr) { }

//        std::unique_ptr<query_instruction> eq_instruction::next() const{
//            return nullptr;
//        }

        const index_path &eq_instruction::target() const {
            return _target;
        }

        const std::string &eq_instruction::value() const {
            return _value;
        }


     }
}
