#ifndef ESTRING_H
#define ESTRING_H
#include "common.h"
#include "etypes.h"

API_EXPORT EString _EString_new(const char* _str, const char* _file_name, euint32 _file_line);
API_EXPORT void _EString_delete(EString _str, const char* _file_name, euint32 _file_line);
API_EXPORT EString _EString_add(EString _str0, const char* _str1, const char* _file_name, euint32 _file_line);

API_EXPORT euint32 EString_size(EString _str);

API_EXPORT EString _EString_add_sint(EString _str, esint _si, const char* _file_name, euint32 _file_line);
API_EXPORT EString _EString_add_uint(EString _str, euint _ui, const char* _file_name, euint32 _file_line);
API_EXPORT EString _EString_add_float(EString _str, float _f, const char* _file_name, euint32 _file_line);

API_EXPORT bool EString_less(EString _str0, EString _str1);
API_EXPORT bool EString_greater(EString _str0, EString _str1);

#define EString_new(s) _EString_new(s, __FILE__, __LINE__)
#define EString_delete(s) _EString_delete(s, __FILE__, __LINE__)
#define EString_add(s0, s1) _EString_add(s0, s1, __FILE__, __LINE__)

#endif
