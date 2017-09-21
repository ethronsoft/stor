//
// Created by Edd on 21/09/2017.
//

#ifndef PYSTOR_EXPORT_H
#define PYSTOR_EXPORT_H

#ifndef CSTOR_API
# if defined(CSTOR_API_EXPORT)
#  if defined(_WIN32) || defined(__CYGWIN__)
#   if defined(CSTOR_API_EXPORT_BUILD)
#    if defined(__GNUC__)
#     define CSTOR_API __attribute__ ((dllexport))
#    else
#     define CSTOR_API __declspec(dllexport)
#    endif
#   else
#    if defined(__GNUC__)
#     define CSTOR_API __attribute__ ((dllimport))
#    else
#     define CSTOR_API __declspec(dllimport)
#    endif
#   endif
#  elif defined(__GNUC__) && defined(CSTOR_API_EXPORT_BUILD)
#   define CSTOR_API __attribute__ ((visibility ("default")))
#  else
#   define CSTOR_API
#  endif
# else
#  define CSTOR_API
# endif
#endif

#endif //PYSTOR_EXPORT_H
