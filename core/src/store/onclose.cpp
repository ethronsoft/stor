//
// Created by devhack on 05/06/16.
//

#include <leveldb/db.h>
#include <stor/store/onclose.h>
#include <leveldb/env.h>
#include <iostream>

namespace esft{
    namespace stor{


        onclose::onclose(const std::string &db_path,operation op):_op{op},_path{db_path} {}

        onclose::onclose(onclose &&o): _op{std::move(o._op)}, _path{std::move(o._path)}
        {

        }

        onclose &onclose::operator=(onclose &&o){
            _path = std::move(o._path);
            return *this;
        }


        onclose::~onclose()
        {
            //no exceptions in destructor
            try {
                if (_op){
                    _op(_path);
                }
            }catch (...){
                //log anomaly
                std::cerr << "exception thrown in onclose() destructor" << std::endl;
            }

        }

    }
}
