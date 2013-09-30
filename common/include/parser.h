#ifndef PARSER_H
#define PARSER_H
#include "etypes.h"
#include "common.h"

#define UNKNOWN_TYPE       0x00000000
#define IMMEDIATE_TYPE     0x00010000
#define VARIABLE_TYPE      0x00020000
#define OPERATOR_TYPE      0x00040000
/**
#define INTEGER_TYPE       0x00000001
#define FLOAT_TYPE         0x00000002
#define STRING_TYPE        0x00000004
**/
#define IMMEDIATE_PRIORITY 0x0000000f
#define VARIABLE_PRIORITY  0x000000ff

#define PARSER_DEBUG

#ifdef PARSER_DEBUG
typedef char Symbol;
#define EXCE_SYMBOL 0x00
#else
typedef int SymbolPriority;
typedef int SymbolDataType;
typedef struct _symbol_data
{
    SymbolDataType type;
    void* value;
} SymbolData;
typedef struct _symbol
{
    SymbolPriority priority;
    SymbolData data;
} Symbol;
#endif

Symbol Formula(Symbol* text, Symbol ends);
#endif
