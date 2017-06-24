#ifndef ESNPL_QUERY_INSTRUCTION_H_INCLUDED
#define ESNPL_QUERY_INSTRUCTION_H_INCLUDED

#include <unordered_set>
#include <string>
#include <memory>
#include <leveldb/options.h>

namespace esft{

namespace stor{

    class collection;

    /**
     * @brief query_instruction represents an operation that
     * obtains or aggregates database documents' keys depending
     * on the implementation of the 'execute' function.
     *
     * query_instruction is a node of the query_instruction tree
     * where results flow from leaves to root, being  processed
     * via each node's 'execute' function.
     */
    class query_instruction{
    public:

        /**
         * @brief Destructor
         */
        virtual ~query_instruction() {}

        /**
         * @brief Executes some rules to either extract or aggregate document's keys out of collection 'c'
         * using ReadOptions 'ro'
         *
         * @param c collection to perform operation on
         * @param ro ReadOptions to be used during extraction operation
         *
         * @returns set of documents' keys
         */
        virtual std::unordered_set<std::string> execute(const collection &c, const leveldb::ReadOptions &ro) const = 0;

        /**
         * @brief Add child to the current query_instruction node
         */
        virtual void add_child(std::unique_ptr<query_instruction> ptr) = 0;

    };

}
}

#endif // ESNPL_QUERY_INSTRUCTION_H_INCLUDED
