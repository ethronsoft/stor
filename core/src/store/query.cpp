//
// Created by devhack on 05/06/16.
//

#include <sstream>
#include <stor/exceptions/store_exception.h>
#include <stor/store/or_instruction.h>
#include <stor/store/and_instruction.h>
#include <stor/store/eq_instruction.h>
#include <stor/store/gt_instruction.h>
#include <stor/store/gte_instruction.h>
#include <stor/store/neq_instruction.h>
#include <stor/store/lt_instruction.h>
#include <stor/store/lte_instruction.h>
#include <stor/store/query.h>
#include <stor/document/iterator.h>

namespace esft{
    namespace stor{

        namespace impl{
            std::unique_ptr<query_instruction> build(const node &n, const document&d);
            std::unique_ptr<query_instruction> make_leaf(const std::string &op,const node &n, const document&d);
            std::unique_ptr<query_instruction> make_aggregator(const std::string &op,const node &n, const document&d);
        }

        query::query(const char *json)
        {
            stor::document d{json};

            //construct instruction tree
            if (d.empty()) throw store_exception{"empty query."};
            //parse
            _root = impl::build(d,d);

        }

        query::query(const document &d):
        _root{nullptr}
        {
            //construct instruction tree
            if (d.empty()) throw store_exception{"empty query."};

            _root = impl::build(d,d);

        }

        std::unordered_set<std::string> query::answer(const collection &c,const leveldb::ReadOptions &ro) const
        {
            return _root->execute(c,ro);
        }

        //implementation

        /**
         * Iteratively build query_instruction tree
         */
        std::unique_ptr<query_instruction> impl::build(const node &n, const document &d)
        {

            if (n.has("$eq")) {
                return make_leaf("$eq", n, d);
            }else if (n.has("$neq")){
                return make_leaf("$neq", n, d);
            }else if (n.has("$gt")){
                return make_leaf("$gt", n, d);
            }else if (n.has("$gte")){
                return make_leaf("$gte", n, d);
            }else if (n.has("$lt")){
                return make_leaf("$lt", n, d);
            }else if (n.has("$lte")){
                return make_leaf("$lte", n, d);
            }else if (n.has("$or")){
                return make_aggregator("$or",n,d);
            }else if (n.has("$and")){
                return make_aggregator("$and",n,d);
            }else
            {
                throw store_exception{"unrecognized query token"};
            }
        }

        std::unique_ptr<query_instruction> impl::make_leaf(const std::string &op,const node &n, const document &d)
        {
            node eq_obj = n[op];
            //target is the json path that points to the value $eq refers to
            const std::string target = eq_obj.begin().key();

            //json[target] can be represented by many types.
            //we need a string representation so we make use
            //on the overloaded operator<< in case we don't
            //have a string already
            std::ostringstream val;
            if (eq_obj[target].is_string()){
                val << eq_obj[target].as_string();
            }else{

                val << eq_obj[target];
            }

            //path is a combination of the target and the doc id
            if (op == "$eq"){
                return std::unique_ptr<query_instruction>(new eq_instruction{index_path(target),val.str()});
            }else if (op == "$gt"){
                return std::unique_ptr<query_instruction>(new gt_instruction{index_path(target),val.str()});
            }else if (op == "$gte"){
                return std::unique_ptr<query_instruction>(new gte_instruction{index_path(target),val.str()});
            }else if (op == "$lt"){
                return std::unique_ptr<query_instruction>(new lt_instruction{index_path(target),val.str()});
            }else if (op == "$lte"){
                return std::unique_ptr<query_instruction>(new lte_instruction{index_path(target),val.str()});
            }else if (op == "$neq"){
                return std::unique_ptr<query_instruction>(new neq_instruction{index_path(target),val.str()});
            }else{
                throw std::exception{};
            }

        }

        std::unique_ptr<query_instruction> impl::make_aggregator(const std::string &op, const node &n,
                                                                 const document &d) {
            auto x = n[op];
            if (x.size() < 2) throw store_exception{op +  " statement is incomplete."};
            std::unique_ptr<query_instruction> res;
            if (op == "$and"){
                res = std::unique_ptr<query_instruction>(new and_instruction{});
            }else if (op == "$or"){
                res = std::unique_ptr<query_instruction>(new or_instruction{});
            }else {
                throw std::exception{};
            }
            for(auto it = x.cbegin(); it != x.cend(); ++it){
                res->add_child(build(*it,d));
            }
            return res;
        }



    }
}