/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */
#ifndef less_then
#define less_then(self, t, a, b, ret) \
                        if (!self->key_compare_proc || t != Vptr) \
                        { \
                            switch(t) \
                            {\
                            case Sint8:\
                                ret = a.sint8_var < b.sint8_var;\
                                break;\
                            case Sint16:\
                                ret = a.sint16_var < b.sint16_var;\
                                break;\
                            case Sint32:\
                                ret = a.sint32_var < b.sint32_var;\
                                break;\
                            case Sint64:\
                                ret = a.sint64_var < b.sint64_var;\
                                break;\
                            case Uint8:\
                                ret = a.uint8_var < b.uint8_var;\
                                break;\
                            case Uint16:\
                                ret = a.uint16_var < b.uint16_var;\
                                break;\
                            case Uint32:\
                                ret = a.uint32_var < b.uint32_var;\
                                break;\
                            case Uint64:\
                                ret = a.uint64_var < b.uint64_var;\
                                break;\
                            case Vptr:\
                                ret = (ref_ptr)a.vptr_var < (ref_ptr)b.vptr_var;\
                                break;\
                            case String:\
                                ret = EString_less((EString)a.str_var, (EString)b.str_var); \
                                break;\
                            } \
                        } \
                        else \
                        { \
                            if (self->key_compare_proc(a.vptr_var, b.vptr_var) < 0) \
                            { \
                                ret = true; \
                            } \
                            else \
                            { \
                                ret = false; \
                            } \
                        }

#endif

#ifndef greater_then
#define greater_then(self, t, a, b, ret) \
                          if (!self->key_compare_proc || t != Vptr) \
                          { \
                              switch(t) \
                              {\
                              case Sint8:\
                                  ret = a.sint8_var > b.sint8_var;\
                                  break;\
                              case Sint16:\
                                  ret = a.sint16_var > b.sint16_var;\
                                  break;\
                              case Sint32:\
                                  ret = a.sint32_var > b.sint32_var;\
                                  break;\
                              case Sint64:\
                                  ret = a.sint64_var > b.sint64_var;\
                                  break;\
                              case Uint8:\
                                  ret = a.uint8_var > b.uint8_var;\
                                  break;\
                              case Uint16:\
                                  ret = a.uint16_var > b.uint16_var;\
                                  break;\
                              case Uint32:\
                                  ret = a.uint32_var > b.uint32_var;\
                                  break;\
                              case Uint64:\
                                  ret = a.uint64_var > b.uint64_var;\
                                  break;\
                              case Vptr:\
                                  ret = (ref_ptr)a.vptr_var > (ref_ptr)b.vptr_var;\
                                  break;\
                              case String:\
                                  ret = EString_greater((EString)a.str_var, (EString)b.str_var); \
                                  break;\
                              } \
                          } \
                          else \
                          { \
                              if (self->key_compare_proc(a.vptr_var, b.vptr_var) > 0) \
                              { \
                                  ret = true; \
                              } \
                              else \
                              { \
                                  ret = false; \
                              } \
                          }

#endif

/**
 * Copyright (c) 2011-2013 Xu Haining
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
