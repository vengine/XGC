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
