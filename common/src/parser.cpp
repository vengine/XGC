
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
template <typename S>
class Symbol
{
public:
    typedef int (*CompareProc)(const S&, const S&);
public:
    S m_symbol;
    CompareProc m_compProc;
public:
    Symbol (const Symbol& s)
    : m_symbol(s.m_symbol)
    , m_compProc(s.m_compProc)
    {}
    Symbol (CompareProc proc)
    : m_compProc(proc)
    {}
    Symbol (S sym, CompareProc proc)
    : m_symbol(sym)
    , m_compProc(proc)
    {}
    int Compare(const Symbol& s) { return m_compProc(m_symbol, s.m_symbol); }
    inline bool operator ==(const Symbol& s) { return Compare(s) == 0; }
    inline bool operator ==(Symbol& s)       { return Compare((const Symbol&)s) == 0; }
    inline bool operator !=(const Symbol& s) { return Compare(s) != 0; }
    inline bool operator !=(Symbol& s)       { return Compare((const Symbol&)s) != 0; }
    inline void operator = (const Symbol& s) { m_symbol = s.m_symbol; m_compProc = s.m_compProc; }
    inline S Get()                           { return m_symbol; }

    static std::vector<S> GetString(Symbol* str, Symbol& endSym) {
        std::vector<S> result;
        int count = 0;
        while (str[count] != endSym)
        {
            result.push_back(str[count].Get());
            count++;
        }
        result.push_back(endSym.Get());
        return result;
    }
    static std::vector<Symbol> Convert(S* str, Symbol& endSym) {
        std::vector<Symbol> result;
        int count = 0;
        Symbol s(endSym);
        do
        {
            s = Symbol(str[count], endSym.m_compProc);
            result.push_back(s);
            count++;
        } while (s != endSym);
        return result;
    }
};

int _compare(const char& a, const char& b)
{
    return a - b;
}
int main()
{
    char s[] = "a2b1c0(e1f1g)0h0(i1j)1k";
    Symbol<char> endSym((char)0x00, _compare);
    std::vector< Symbol<char> > str = Symbol<char>::Convert(s, endSym);
    std::vector<char> cstr = Symbol<char>::GetString(&str[0], endSym);
    printf(&cstr[0]);
    getchar();
}

/**
int symbol_compare(Symbol a, Symbol b)
{
#ifdef PARSER_DEBUG
    return a - b;
#else
    return a.priority - b.priority;
#endif
}

typedef struct _parser
{
    Symbol* text;
    Symbol end_symbol;
    int symbol_count;
} Parser;

typedef struct _result
{
    Symbol operator_symbol;
    Symbol return_value;
} Result;

void Parser_Init(Parser* _self, Symbol* s, Symbol ends, int offs)
{
    _self->text = s;
    _self->end_symbol = ends;
    _self->symbol_count = offs;
}
#ifdef PARSER_DEBUG
static Symbol s_symbol = 'A';
static Symbol s_left_parentheses = '(';
static Symbol s_right_parentheses = ')';
#else
static char* s_symbol_string = "ABCDEFGHIJKL";
static char* s_spec_string = "()";
static Symbol s_symbol = {IMMEDIATE_PRIORITY, {IMMEDIATE_TYPE, NULL}};
static Symbol s_left_parentheses = {IMMEDIATE_PRIORITY, {IMMEDIATE_TYPE, NULL}};
static Symbol s_right_parentheses = {IMMEDIATE_PRIORITY, {IMMEDIATE_TYPE, NULL}};
#endif

void symbol_init()
{
#ifdef PARSER_DEBUG
#else
    s_symbol.data.value = s_symbol_string;
    s_left_parentheses.data.value = &s_spec_string[0];
    s_right_parentheses.data.value = &s_spec_string[1];
#endif
}


Symbol alloc_symbol()
{
#ifdef PARSER_DEBUG
    return s_symbol++;
#else
    s_symbol.data.value = (void*)((uint)s_symbol.data.value + 1);
    return s_symbol;
#endif
}

///typedef Symbol (*ParserExec)(Parser*);
///ParserExec s_Parser_analyze = NULL;
Symbol Parser_analyze(Parser* _self);

Result Parser_operate(Parser* _self, Symbol previous_operator_symbol)
{
    Symbol left_symbol = _self->text[_self->symbol_count];
    Symbol operator_symbol = _self->text[_self->symbol_count + 1];

    Result ret;
    ret.operator_symbol = operator_symbol;
    ret.return_value = _self->end_symbol;
    ///if (operator_symbol == _self->end_symbol)
    if (symbol_compare(operator_symbol, _self->end_symbol) == 0)
    {
        _self->symbol_count++;
        ret.return_value = left_symbol;
        return ret;
    }

    ret.return_value = alloc_symbol();
    Symbol right_symbol = _self->text[_self->symbol_count + 2];

    ///if (right_symbol == '(')
    if (symbol_compare(right_symbol, s_left_parentheses) == 0)
    {
        Parser p;
        ///Parser_Init(&p, _self->text, ')', _self->symbol_count + 3);
        Parser_Init(&p, _self->text, s_right_parentheses, _self->symbol_count + 3);
        right_symbol = Parser_analyze(&p);
        _self->symbol_count = p.symbol_count;
        _self->text[_self->symbol_count] = right_symbol;
    }
    else
        _self->symbol_count += 2;

    Symbol next_operator_symbol = _self->text[_self->symbol_count + 1];

    ///if (next_operator_symbol == _self->end_symbol || symbol_compare(operator_symbol, next_operator_symbol) <= 0)
    if (symbol_compare(next_operator_symbol, _self->end_symbol) == 0 || symbol_compare(operator_symbol, next_operator_symbol) <= 0)
    {
        _self->text[_self->symbol_count] = ret.return_value;
#ifdef PARSER_DEBUG
        printf("%c = %c %c %c prev %c\n", ret.return_value, left_symbol, operator_symbol, right_symbol, previous_operator_symbol);
#endif
        return ret;
    }
    else
    {
        Result r = Parser_operate(_self, operator_symbol);
        Symbol n = _self->text[_self->symbol_count + 1];
        ///if (n == _self->end_symbol || symbol_compare(previous_operator_symbol, n) <= 0)
        if (symbol_compare(n, _self->end_symbol) == 0 || symbol_compare(previous_operator_symbol, n) <= 0)
        {
#ifdef PARSER_DEBUG
            printf("%c = %c %c %c prev %c\n", ret.return_value, left_symbol, operator_symbol, r.return_value, previous_operator_symbol);
#endif
            return ret;
        }
        else
        {
            r = Parser_operate(_self, previous_operator_symbol);
#ifdef PARSER_DEBUG
            printf("%c = %c %c %c prev %c\n", ret.return_value, left_symbol, operator_symbol, r.return_value, previous_operator_symbol);
#endif
            return ret;
        }
    }
}

Symbol Parser_analyze(Parser* _self)
{
#ifdef PARSER_DEBUG
    Symbol o = 0x00;
    Symbol r = 0x00;
#else
    Symbol o = {IMMEDIATE_PRIORITY, {IMMEDIATE_TYPE, NULL}};
    Symbol r = {IMMEDIATE_PRIORITY, {IMMEDIATE_TYPE, NULL}};
#endif
    ///while (_self->text[_self->symbol_count] != _self->end_symbol)
    while (symbol_compare(_self->text[_self->symbol_count], _self->end_symbol) != 0)
    {
        Result ret = Parser_operate(_self, o);
        o = ret.operator_symbol;
        r = ret.return_value;
        ///if (_self->text[_self->symbol_count] != _self->end_symbol)
        if (symbol_compare(_self->text[_self->symbol_count], _self->end_symbol) != 0)
        {
            _self->text[_self->symbol_count] = r;
        }
    }
    return r;
}

#ifdef PARSER_DEBUG
void Parser_test()
{
    char s[] = "a2b1c0(e1f1g)0h0(i1j)1k";

    printf("%s\n", s);

    Parser p;
    Parser_Init(&p, s, 0x00, 0);
    Parser_analyze(&p);

    getchar();
}
#endif

Symbol Formula(Symbol* text, Symbol ends)
{
    symbol_init();
    ///s_Parser_analyze = Parser_analyze;
#ifdef PARSER_DEBUG
    Parser_test();
    return 0x00;
#else
    Parser p;
    Parser_Init(&p, text, ends, 0);
    return Parser_analyze(&p);
#endif
}
**/
