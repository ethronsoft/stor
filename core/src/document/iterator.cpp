#include <stor/document/iterator.h>
#include <stor/exceptions/document_exception.h>

namespace esft{

namespace stor{

    iterator::iterator(rapidjson::Value::ValueIterator it, rapidjson::Document *doc):
        _ith{iterator::type::VALUE,{nullptr}}, _doc{doc}
    {
        _ith._it.vit = it;
    }

    iterator::iterator(rapidjson::Value::MemberIterator it, rapidjson::Document *doc):
        _ith{iterator::type::MEMBER,{nullptr}}, _doc{doc}
    {
        _ith._it.mit = it;
    }

    iterator::iterator(iterator &&o):_ith{o._ith._tag,{nullptr}} , _doc{o._doc}
    {
        switch(o._ith._tag){
        case iterator::type::MEMBER:
            {
                _ith._it.mit = o._ith._it.mit;
            }
        case iterator::type::VALUE:
            {
                _ith._it.vit = o._ith._it.vit;
            }
        }
        o._doc = nullptr;
    }

    iterator iterator::operator++(){

        switch(_ith._tag){
            case iterator::type::MEMBER:
                return iterator(++_ith._it.mit, _doc);
            case iterator::type::VALUE:
                return iterator(++_ith._it.vit, _doc);
           default:
                 throw std::exception{};
        }
    }

    node iterator::operator*(){
        switch(_ith._tag){
            case iterator::type::VALUE:
            {
                rapidjson::Value *v = (_ith._it.vit);
                return node(v,_doc);
            }
            case iterator::type::MEMBER:
            {
                rapidjson::Value *v = &_ith._it.mit->value;
                return node(v,_doc);
            }
            default:
                throw std::exception{};
        }
    }

        std::string iterator::key() const
        {
            switch (_ith._tag){
                case iterator::type::VALUE:
                {
                    throw document_exception{"value not does not have a key"};
                }
                case iterator::type::MEMBER:
                {
                    return _ith._it.mit->name.GetString();
                }
                default:
                    throw std::exception{};
            }
        }

    bool iterator::operator==(const iterator &rh) const
    {
        switch(_ith._tag){
            case iterator::type::MEMBER:
            {
                if (rh._ith._tag != iterator::type::MEMBER) throw document_exception{"mismatching iterators."};
                return _ith._it.mit == rh._ith._it.mit;
            }
            case iterator::type::VALUE:
            {
                if (rh._ith._tag != iterator::type::VALUE) throw document_exception{"mismatching iterators."};
                return _ith._it.vit == rh._ith._it.vit;
            }
            default:
                throw std::exception{};
        }
    }

    bool iterator::operator!=(const iterator &rh) const{
        return !(*this == rh);
    }



}

}
