#include <chrono>
#include <sstream>
#include <random>
#include <stor/document/document.h>
#include <stor/exceptions/document_exception.h>
#include <rapidjson/istreamwrapper.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace esft{

    namespace stor{

        document::document(const std::string &json, document::identifier _id):
                node{},_rjdoc{},_id{_id}
        {
            rapidjson::ParseResult p = _rjdoc.Parse(json.c_str());
            if (!p) throw document_exception{"failed parsing."};
            set_document(&_rjdoc);
            set_underlying(&_rjdoc);

        }

        document::document(const char *json):
                node{},_rjdoc{},_id{make_id()}
        {
            rapidjson::ParseResult p = _rjdoc.Parse(json);
            if (!p) throw document_exception{"failed parsing."};
            set_document(&_rjdoc);
            set_underlying(&_rjdoc);

        }


        document::document():_id{make_id()} {}


        document::document(document &&o):
        _id{o._id}
        {
            _rjdoc = std::move(o._rjdoc);
            o._id.clear();
        }

        document &document::operator=(document &&o)
        {
            _id = o._id;
            _rjdoc = std::move(o._rjdoc);
            o._id.clear();
            return *this;
        }



        document document::as_object(){
            document d;
            d._rjdoc = rapidjson::Document(rapidjson::kObjectType);
            d.set_document(&d._rjdoc);
            d.set_underlying(&d._rjdoc);
            return d;
        }

        document document::as_array(){
            document d;
            d._rjdoc = rapidjson::Document(rapidjson::kArrayType);
            d.set_document(&d._rjdoc);
            d.set_underlying(&d._rjdoc);
            return d;
        }

        const document::identifier &document::id() const {
            return _id;
        }

        document::identifier document::make_id()
        {
            using namespace std::chrono;
            //node_key key = get_key(&_rjdoc);

            std::ostringstream oss;

            auto duration = system_clock::now().time_since_epoch();
            auto micro = duration_cast<milliseconds>(duration).count();

            oss << micro;

            boost::uuids::random_generator gen;
            boost::uuids::uuid u = gen();
            oss << u;

            return oss.str();
        }


        std::istream &operator>>(std::istream &is, document &d)
        {
            rapidjson::IStreamWrapper isw{is};
            rapidjson::ParseResult p = d._rjdoc.ParseStream(isw);
            if (!p) {
                is.setstate(std::ios::badbit);
                throw document_exception{"failed parsing."};
            }
            d.set_document(&d._rjdoc);
            d.set_underlying(&d._rjdoc);

            return is;
        }


    }

}
