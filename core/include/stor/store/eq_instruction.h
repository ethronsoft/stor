#ifndef ESNPL_EQ_INSTRUCTION_H_INCLUDED
#define ESNPL_EQ_INSTRUCTION_H_INCLUDED

#include <stor/store/query_instruction.h>
#include <stor/store/collection.h>
#include <stor/store/index_path.h>
#include <string>
#include <unordered_set>

namespace esft{
namespace stor{


/**
 * @brief eq_instruction returns all store keys for
 * documents that have index_path equal to a target value
 */
class eq_instruction: public query_instruction{
public:

    /**
     * @brief Constructor
     * @param path path to value to evaluate for `equality operation`
     * @param value value to evaluate
     */
    eq_instruction(index_path &&path, const std::string &value);

    virtual std::unordered_set<std::string> execute(const collection &c,const leveldb::ReadOptions &ro) const override;

    /**
     * @brief returns the document's index path to the target value
     * @return  document's index path to the target value
     */
    const index_path &target() const;

    /**
     * @brief returns the value to evaluate for `equality operation`
     * @return value to evaluate for `equality operation`
     */
    const std::string &value() const;

    virtual void add_child(std::unique_ptr<query_instruction> ptr) override;

private:

    /**
     * @brief document's path to node used as right hand operand to equality operator
     */
    index_path _target;
    /**
     * @brief target value used as left hand operand to equality operator
     */
    std::string _value;

};

}
}


#endif // ESNPL_EQ_INSTRUCTION_H_INCLUDED
