//
// Created by devhack on 06/06/16.
//

#include <stor_test/query_test.h>
#include <exception>

namespace esft{
    namespace stor_test{

        query_test::query_test(stor::document &d):query(d),_root{test_read(d)} {}

        std::unique_ptr<query_test::query_node> query_test::test_read(stor::document &d)  const
        {
            if (d.empty()) throw std::exception{};
            std::string key = d.begin().key();
            if (key == "$eq"){
                 std::unique_ptr<query_test::query_node> qptr(new query_test::query_node{});
                qptr->_type = query_test::type::EQ;
                return qptr;
            }

            return nullptr;
        }

    }
}