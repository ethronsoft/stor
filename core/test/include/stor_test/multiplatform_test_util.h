//
// Created by efelici on 6/6/2016.
//

#ifndef STOR_MULTIPLATFORM_H
#define STOR_MULTIPLATFORM_H

#include <string>

namespace esft{
    namespace stor_test{

        inline std::string home(){
                return TEMP_DIR;
        }

    }
}


#endif //STOR_MULTIPLATFORM_H
