

#ifndef STOR_QUERY_TEST_H
#define STOR_QUERY_TEST_H

#include <stor/store/query.h>

namespace esft{
    namespace stor_test{
        class query_test: public stor::query{
        public:

            enum class type{
                OR,
                EQ
            };


            struct query_node{
                std::vector<std::unique_ptr<query_node>>_children;
                type _type;
            };


            query_test(stor::document &d);

            std::unique_ptr<query_node> test_read(stor::document &d) const;

            std::unique_ptr<query_node> _root;





        };
    }
}
#endif //STOR_QUERY_TEST_H
