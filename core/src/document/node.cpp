//
// Created by efelici on 6/13/2016.
//

#include <sstream>
#include <stor/document/iterator.h>
#include <stor/document/const_iterator.h>
#include <stor/document/node.h>
#include <stor/document/document.h>

namespace esft{
    namespace stor{

        node::node(rapidjson::Value *value, rapidjson::Document *doc):
                _underlying{value}, _doc{doc} {}

        node::node(){}

        node::node(const node &o):
        _underlying{o._underlying},_doc{o._doc}{}

        node::node(node &&o):
        _underlying{o._underlying},_doc{o._doc}{
            o._underlying = nullptr;
            o._doc = nullptr;
        }

        node &node::operator=(const node &o){
            _underlying = o._underlying;
            _doc = o._doc;
            return *this;
        }

        node &node::operator=(node &&o){
            _underlying = o._underlying;
            _doc = o._doc;
            o._underlying = nullptr;
            o._doc = nullptr;
            return *this;
        }

        node::~node() {}

        void node::set_underlying(rapidjson::Value *underlying)
        {
            _underlying = underlying;
        }

        void node::set_document(rapidjson::Document *doc)
        {
            _doc = doc;
        }

        bool node::is_num() const{
            return _underlying->IsNumber();
        }


        bool node::is_int() const{
            return _underlying->IsInt();
        }


        bool node::is_bool() const{
            return _underlying->IsBool();
        }


        bool node::is_long() const{
            return _underlying->IsInt64();
        }


        bool node::is_double() const{
            return _underlying->IsDouble();
        }


        bool node::is_string() const{
            return _underlying->IsString();
        }


        bool node::is_object() const{
            return _underlying->IsObject();
        }


        bool node::is_array() const{
            return _underlying->IsArray();
        }


        bool node::is_null() const{
            return _underlying->IsNull();
        }


        int node::as_int() const{
            if ((_underlying->GetType() == rapidjson::kArrayType) |
                        (_underlying->GetType() == rapidjson::kObjectType)){
                throw document_exception{"invalid request"};
            }
            return _underlying->GetInt();
        }


        int64_t node::as_long() const{
            if ((_underlying->GetType() == rapidjson::kArrayType) |
                (_underlying->GetType() == rapidjson::kObjectType)){
                throw document_exception{"invalid request"};
            }
            return _underlying->GetInt64();
        }


        double node::as_double() const{
            if ((_underlying->GetType() == rapidjson::kArrayType) |
                (_underlying->GetType() == rapidjson::kObjectType)){
                throw document_exception{"invalid request"};
            }
            return _underlying->GetDouble();
        }


        std::string node::as_string() const{
            if ((_underlying->GetType() == rapidjson::kArrayType) |
                (_underlying->GetType() == rapidjson::kObjectType)){
                throw document_exception{"invalid request"};
            }
            return std::string(_underlying->GetString(),_underlying->GetStringLength());
        }

        std::pair<const char *, std::size_t> node::as_cstring() const {
            if ((_underlying->GetType() == rapidjson::kArrayType) |
                (_underlying->GetType() == rapidjson::kObjectType)){
                throw document_exception{"invalid request"};
            }
            return std::make_pair(_underlying->GetString(),_underlying->GetStringLength());
        }

        bool node::as_bool() const{
            if ((_underlying->GetType() == rapidjson::kArrayType) |
                (_underlying->GetType() == rapidjson::kObjectType)){
                throw document_exception{"invalid request"};
            }
            return _underlying->GetBool();
        }


        node node::with(const std::string &key){
            if (_underlying->GetType() != rapidjson::kObjectType ){
                throw document_exception{"invalid request"};
            }
            if (!_underlying->HasMember(key.c_str())) {
                _underlying->AddMember(
                        rapidjson::Value(key.c_str(), key.size(), _doc->GetAllocator()),
                        rapidjson::Value(rapidjson::kObjectType),
                        _doc->GetAllocator()
                );
            }
            return node(&(*_underlying)[key.c_str()],_doc);
        }


        node node::with_array(const std::string &key){
            if (_underlying->GetType() != rapidjson::kObjectType){
                throw document_exception{"invalid request"};
            }
            if (!_underlying->HasMember(key.c_str())) {
                _underlying->AddMember(
                        rapidjson::Value(key.c_str(), key.size(), _doc->GetAllocator()),
                        rapidjson::Value(rapidjson::kArrayType),
                        _doc->GetAllocator()
                );
            }
            return node(&(*_underlying)[key.c_str()],_doc);
        }


        void node::operator=(int v)
        {
            if ((_underlying->GetType() == rapidjson::kArrayType) |
                        (_underlying->GetType() == rapidjson::kObjectType)){
                throw document_exception{"invalid request"};
            }
            _underlying->SetInt(v);
        }
        void node::operator=(int64_t v)
        {
            if ((_underlying->GetType() == rapidjson::kArrayType) |
                (_underlying->GetType() == rapidjson::kObjectType)){
                throw document_exception{"invalid request"};
            }
            _underlying->SetInt64(v);
        }
        void node::operator=(double v)
        {
            if ((_underlying->GetType() == rapidjson::kArrayType) |
                (_underlying->GetType() == rapidjson::kObjectType)){
                throw document_exception{"invalid request"};
            }
            _underlying->SetDouble(v);
        }
        void node::operator=(bool v)
        {
            if ((_underlying->GetType() == rapidjson::kArrayType) |
                (_underlying->GetType() == rapidjson::kObjectType)){
                throw document_exception{"invalid request"};
            }
            _underlying->SetBool(v);
        }
        void node::operator=(const std::string &v)
        {
//            if ((_underlying->GetType() == rapidjson::kArrayType) |
//                (_underlying->GetType() == rapidjson::kObjectType)){
//                throw document_exception{"invalid request"};
//            }
//            _underlying->SetString(v.c_str(),v.size(),_doc->GetAllocator());
            return (*this) = v.c_str();
        }

        void node::operator=(const char *v)
        {
            if ((_underlying->GetType() == rapidjson::kArrayType) |
                (_underlying->GetType() == rapidjson::kObjectType)){
                throw document_exception{"invalid request"};
            }
            _underlying->SetString(v,_doc->GetAllocator());
        }
        
        node  &node::put(const std::string &key, int v){
            if (_underlying->GetType() != rapidjson::kObjectType ){
                throw document_exception{"invalid request"};
            }
            if (has(key)){
                (*this)[key] = v;
            }else{
                _underlying->AddMember(
                        rapidjson::Value(key.c_str(),key.length(),_doc->GetAllocator()),
                        rapidjson::Value(v),
                        _doc->GetAllocator()
                );
            }
            return *this;
        }

        node  &node::put(const std::string &key, int64_t v){
            if (_underlying->GetType() != rapidjson::kObjectType ){
                throw document_exception{"invalid request"};
            }
            if (has(key)){
                (*this)[key] = v;
            }else{
                _underlying->AddMember(
                        rapidjson::Value(key.c_str(),key.length(),_doc->GetAllocator()),
                        rapidjson::Value(v),
                        _doc->GetAllocator()
                );
            }
            return *this;
        }

        node  &node::put(const std::string &key, double v){
            if (_underlying->GetType() != rapidjson::kObjectType ){
                throw document_exception{"invalid request"};
            }
            if (has(key)){
                (*this)[key] = v;
            }else{
                _underlying->AddMember(
                        rapidjson::Value(key.c_str(),key.length(),_doc->GetAllocator()),
                        rapidjson::Value(v),
                        _doc->GetAllocator()
                );
            }
            return *this;
        }

        node  &node::put(const std::string &key, bool v){
            if (_underlying->GetType() != rapidjson::kObjectType ){
                throw document_exception{"invalid request"};
            }
            if (has(key)){
                (*this)[key] = v;
            }else{
                _underlying->AddMember(
                        rapidjson::Value(key.c_str(),key.length(),_doc->GetAllocator()),
                        rapidjson::Value(v),
                        _doc->GetAllocator()
                );
            }
            return *this;
        }

        node  &node::put(const std::string &key, const std::string &v){
            return put(key,v.c_str());
        }

        node &node::put(const std::string &key, const char *v){
            if (_underlying->GetType() != rapidjson::kObjectType ){
                throw document_exception{"invalid request"};
            }
            if (has(key)){
                (*this)[key] = v;
            }else{
                _underlying->AddMember(
                        rapidjson::Value(key.c_str(),key.length(),_doc->GetAllocator()),
                        rapidjson::Value(v,_doc->GetAllocator()),
                        _doc->GetAllocator()
                );
            }
            return *this;
        }

        node &node::add(int v){
            if (_underlying->GetType() != rapidjson::kArrayType ){
                throw document_exception{"invalid request"};
            }
            _underlying->PushBack(
                    rapidjson::Value(v),
                    _doc->GetAllocator()
            );
            return *this;
        }

        node &node::add(int64_t v){
            if (_underlying->GetType() != rapidjson::kArrayType ){
                throw document_exception{"invalid request"};
            }
            _underlying->PushBack(
                    rapidjson::Value(v),
                    _doc->GetAllocator()
            );
            return *this;
        }

        node &node::add(double v){
            if (_underlying->GetType() != rapidjson::kArrayType ){
                throw document_exception{"invalid request"};
            }
            _underlying->PushBack(
                    rapidjson::Value(v),
                    _doc->GetAllocator()
            );
            return *this;
        }

        node &node::add(bool v){
            if (_underlying->GetType() != rapidjson::kArrayType ){
                throw document_exception{"invalid request"};
            }
            _underlying->PushBack(
                    rapidjson::Value(v),
                    _doc->GetAllocator()
            );
            return *this;
        }

        node &node::add(const std::string &v){
            if (_underlying->GetType() != rapidjson::kArrayType ){
                throw document_exception{"invalid request"};
            }
            _underlying->PushBack(
                    rapidjson::Value(v.c_str(), v.length(), _doc->GetAllocator()),
                    _doc->GetAllocator()
            );
            return *this;
        }

        node &node::add(const char * v){
            if (_underlying->GetType() != rapidjson::kArrayType ){
                throw document_exception{"invalid request"};
            }
            _underlying->PushBack(
                    rapidjson::Value(v, _doc->GetAllocator()),
                    _doc->GetAllocator()
            );
            return *this;
        }

        node node::add_array(){
            if (_underlying->GetType() != rapidjson::kArrayType ){
                throw document_exception{"invalid request"};
            }
            int indx = size();
            _underlying->PushBack(
                    rapidjson::Value(rapidjson::kArrayType),
                    _doc->GetAllocator()
            );
            return (*this)[indx];
        }

        node node::add_object(){
            if (_underlying->GetType() != rapidjson::kArrayType ){
                throw document_exception{"invalid request"};
            }
            int indx = size();
            _underlying->PushBack(
                    rapidjson::Value(rapidjson::kObjectType),
                    _doc->GetAllocator()
            );
            return (*this)[indx];
        }
        
        bool node::has(const std::string &key) const{
            if (_underlying->GetType() != rapidjson::kObjectType ){
                throw document_exception{"invalid request"};
            }
            return _underlying->HasMember(key.c_str());
        }

        node node::operator[](const std::string &key) const{
            if (_underlying->GetType() != rapidjson::kObjectType ){
                throw document_exception{"invalid request"};
            }
            if (!_underlying->HasMember(key.c_str())){
                throw document_exception{"key does not exist."};
            }
            rapidjson::Value &v =(*_underlying)[key.c_str()];
            return node(&v,_doc);
        }

        node node::operator[](std::size_t indx) const{
            if (_underlying->GetType() != rapidjson::kArrayType ){
                throw document_exception{"invalid request"};
            }
            if (indx >= size()){
                std::ostringstream oss;
                oss << indx;
                throw std::out_of_range{"index " + oss.str() + " out of range"};
            }
            rapidjson::Value &v = (*_underlying)[indx];
            return node(&v,_doc);
        }

        std::size_t node::size() const{
            if ((_underlying->GetType()== rapidjson::kArrayType)){
                return _underlying->Size();
            }else if (_underlying->GetType() == rapidjson::kObjectType) {
                size_t res = 0;
                for(auto it = _underlying->MemberBegin(); it != _underlying->MemberEnd(); ++it){
                    res++;
                }
                return res;
            }else{
                throw document_exception{"invalid request"};
            }
        }

        bool node::remove(const std::string &key) {
            if (_underlying->GetType() == rapidjson::kObjectType){
                return _underlying->RemoveMember(key.c_str());
            }else{
                throw document_exception{"invalid request"};
            }
        }

        const_iterator node::remove(const_iterator it) {
            if (it._ith._tag == const_iterator::type::VALUE){
                return const_iterator(_underlying->Erase(it._ith._it.vit),_doc);
            }else{
                return const_iterator(_underlying->EraseMember(it._ith._it.mit),_doc);
            }
        }

        const_iterator node::remove(const_iterator first, const_iterator last) {
            if (first._ith._tag != last._ith._tag){
                throw document_exception{"iterators must both refer to the same type of node"};
            }
            if (first._ith._tag == const_iterator::type::VALUE){
                return const_iterator(_underlying->Erase(first._ith._it.vit, last._ith._it.vit),_doc);
            }else{
                return const_iterator(_underlying->EraseMember(first._ith._it.mit, last._ith._it.mit),_doc);
            }
        }

        void node::remove_all() const {
            if (_underlying->GetType() == rapidjson::kObjectType){
                _underlying->RemoveAllMembers();
            }else if (_underlying->GetType() == rapidjson::kArrayType){
                _underlying->Clear();
            }else{
                throw document_exception{"invalid request"};
            }
        }

        bool node::empty() const{
            return size() == 0;
        }


        std::string node::json() const{
            rapidjson::StringBuffer sb;
            rapidjson::Writer<rapidjson::StringBuffer> w{sb};
            _underlying->Accept(w);
            return sb.GetString();
        }

        void node::json(const std::string &jsn) {
            copy(document(jsn,document::identifier{0}));
        }

        void node::copy(const node &v) {
            _underlying->CopyFrom(*v._underlying,_doc->GetAllocator());
        }

        std::ostream &node::write_to_stream(std::ostream&os) const{
            rapidjson::OStreamWrapper osw{os};
            rapidjson::Writer<rapidjson::OStreamWrapper> w(osw);
            _underlying->Accept(w);
            return os;
        }


        iterator node::begin(){
            if      (_underlying->GetType() == rapidjson::kArrayType ){
                return iterator(_underlying->Begin(),_doc);
            }else if(_underlying->GetType() == rapidjson::kObjectType ){
                return iterator(_underlying->MemberBegin(),_doc);
            }else{
                throw document_exception{"invalid request"};
            }
        }

        iterator node::end(){
            if      (_underlying->GetType() == rapidjson::kArrayType ){
                return iterator(_underlying->End(),_doc);
            }else if(_underlying->GetType() == rapidjson::kObjectType ){
                return iterator(_underlying->MemberEnd(),_doc);
            }else{
                throw document_exception{"invalid request"};
            }
        }

        const_iterator node::cbegin() const{
            if      (_underlying->GetType() == rapidjson::kArrayType ){
                return const_iterator(_underlying->Begin(),_doc);
            }else if(_underlying->GetType() == rapidjson::kObjectType ){
                return const_iterator(_underlying->MemberBegin(),_doc);
            }else{
                throw document_exception{"invalid request"};
            }
        }

        const_iterator node::cend() const{
            if      (_underlying->GetType() == rapidjson::kArrayType ){
                return const_iterator(_underlying->End(),_doc);
            }else if(_underlying->GetType() == rapidjson::kObjectType ){
                return const_iterator(_underlying->MemberEnd(),_doc);
            }else{
                throw document_exception{"invalid request"};
            }
        }
        

    }
}