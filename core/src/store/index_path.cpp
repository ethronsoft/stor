//
// Created by devhack on 05/06/16.
//

#include <stor/store/index_path.h>
#include <sstream>
#include <stor/exceptions/store_exception.h>

namespace esft{
    namespace stor{

        index_path::index_path(const std::string &path)
        {
            if (path.empty()) throw store_exception{"empty path"};
            std::istringstream iss{path};

            std::string token;
            while(std::getline(iss,token,'.')){
                _tokens.push_back(token);
            }
        }

        index_path::index_path(const char *path)
        {
            if (path == nullptr || *path == '\0') throw store_exception{"empty path"};
            std::istringstream iss{path};

            std::string token;
            while(std::getline(iss,token,'.')){
                _tokens.push_back(token);
            }
        }

        index_path::index_path(index_path &&o):_tokens{std::move(o._tokens)}
        {

        }

        index_path::index_path(const index_path &o):_tokens{o._tokens}{

        }

        std::string index_path::str() const
        {
            std::ostringstream oss;
            auto it = _tokens.cbegin();
            oss << *it++;
            for(; it != _tokens.cend(); ++it){
                oss << '.' << *it;
            }
            return oss.str();
        }

        std::pair<std::string,std::string> index_path::extract(const document &doc) const
        {
            const node *curr = &doc;
            for (size_t i = 0; i < _tokens.size(); ++i){
                const std::string &token = _tokens[i];
                if (!curr->has(token)) break;
                node n = (*curr)[token];
                curr = &n;
                if (i == _tokens.size() - 1){

                    if (curr->is_string()){
                        return std::pair<std::string,std::string>(curr->as_string(),"s");
                    }else{
                        std::ostringstream oss;
                        oss << (*curr);
                        if (curr->is_num()){
                            return std::pair<std::string,std::string>(oss.str(),"n");
                        }else if (curr->is_bool()){
                            return std::pair<std::string,std::string>(oss.str(),"b");
                        }
                    }
                }
            }
            return std::pair<std::string,std::string>{};
        }

    }
}