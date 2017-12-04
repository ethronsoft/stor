

#include <stor/store/collection_visitor.h>
#include <leveldb/env.h>
#include <leveldb/options.h>
#include <algorithm>
#include <stdexcept>
#include <functional>

namespace esft{
    namespace stor{

        collection_visitor::collection_visitor(const std::string &stor_root)
        {
            //files are 2 levels down (0,1) from stor_test root folder
            size_t depth = 1;
            
            std::function<void(const std::string&,leveldb::Env&,std::vector<std::string>&,int)> scan;

            scan = [depth,&scan](const std::string &dir, leveldb::Env &env, std::vector<std::string> &c, size_t level){
                if (level <= depth){
                    std::vector<std::string> tmp;
                    env.GetChildren(dir,&tmp);
                    //add full name of children at level = depth to container c
                    if (level == depth){
                        std::for_each(tmp.cbegin(),tmp.cend(),[&c,&dir](const std::string &fn){
                            if ((fn != ".") & (fn != "..")) c.push_back(dir + "/" + fn);
                        });
                    }
                    //recursive scan
                    for (const auto &p : tmp){
                        if ((p != ".") & (p != "..") & (p != "catalog")){
                            std::string t = dir + "/" + p;
                            scan(t,env,c,level+1);
                        }
                    }
                }
            };
            leveldb::Options opt;
            leveldb::Env *env = opt.env;
            assert(env != nullptr);
            if (env){
                scan(stor_root,*env,_paths,0);
            }
        }

        size_t collection_visitor::size() const{return _paths.size();}

        const std::string &collection_visitor::operator[](size_t indx) const{
            if (indx >= size()) throw std::out_of_range{""};
            return _paths[indx];
        }


        collection_visitor::const_iterator collection_visitor::cbegin() const
        {
            return _paths.cbegin();
        }

        collection_visitor::const_iterator collection_visitor::cend() const
        {
            return _paths.cend();
        }

    }
}