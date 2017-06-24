#include <stor/exceptions/document_exception.h>
#include <stor/document/const_iterator.h>

namespace esft{

    namespace stor{

        const_iterator::const_iterator(rapidjson::Value::ConstValueIterator it, rapidjson::Document *doc):
                _ith{const_iterator::type::VALUE,{nullptr}}, _doc{doc}
        {
            _ith._it.vit = it;
        }

        const_iterator::const_iterator(rapidjson::Value::ConstMemberIterator it, rapidjson::Document *doc):
                _ith{const_iterator::type::MEMBER,{nullptr}}, _doc{doc}
        {
            _ith._it.mit = it;
        }

        const_iterator::const_iterator(const_iterator &&o):_ith{o._ith._tag,{nullptr}} , _doc{o._doc}
        {
            switch(o._ith._tag){
                case const_iterator::type::MEMBER:
                {
                    _ith._it.mit = o._ith._it.mit;
                }
                case const_iterator::type::VALUE:
                {
                    _ith._it.vit = o._ith._it.vit;
                }
            }
            o._doc = nullptr;
        }

        const_iterator const_iterator::operator++(){

            switch(_ith._tag){
                case const_iterator::type::MEMBER:{
                    auto nxt = _ith._it.mit++;
                    return const_iterator(nxt, _doc);
                }
                case const_iterator::type::VALUE:
                {
                    auto nxt = _ith._it.vit++;
                    return const_iterator(nxt, _doc);
                }
                default:
                    throw std::exception{};
            }
        }

        const node const_iterator::operator*() const{
            switch(_ith._tag){
                case const_iterator::type::VALUE:
                {
                    rapidjson::Value *v = const_cast<rapidjson::Value *>(_ith._it.vit);
                    return node(v,_doc);
                }
                case const_iterator::type::MEMBER:
                {
                    rapidjson::Value *v = const_cast<rapidjson::Value *>(&_ith._it.mit->value);
                    return node(v,_doc);
                }
                default:
                    throw std::exception{};
            }
        }

        std::string const_iterator::key() const
        {
            switch (_ith._tag){
                case const_iterator::type::VALUE:
                {
                    throw document_exception{"value not does not have a key"};
                }
                case const_iterator::type::MEMBER:
                {
                    return _ith._it.mit->name.GetString();
                }
                default:
                    throw std::exception{};
            }
        }

        bool const_iterator::operator==(const const_iterator &rh) const
        {
            switch(_ith._tag){
                case const_iterator::type::MEMBER:
                {
                    if (rh._ith._tag != const_iterator::type::MEMBER) throw document_exception{"mismatching iterators."};
                    return _ith._it.mit == rh._ith._it.mit;
                }
                case const_iterator::type::VALUE:
                {
                    if (rh._ith._tag != const_iterator::type::VALUE) throw document_exception{"mismatching iterators."};
                    return _ith._it.vit == rh._ith._it.vit;
                }
                default:
                    throw std::exception{};
            }
        }

        bool const_iterator::operator!=(const const_iterator &rh) const{
            return !(*this == rh);
        }



    }

}
