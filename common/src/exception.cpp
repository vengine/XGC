#include "pch.h"
#include "exception.h"
#include <stdio.h>
#include <signal.h>
#include "elog.h"
#include "eassert.h"
jmp_buf s_jmp_buf;
const char* s_exce_info = NULL;
int s_exce_id = 0;

void SIGSEGV_handle(int s)
{
    s_exce_info = "SIGSEGV";
    s_exce_id = s;
    longjmp(s_jmp_buf, s);
}
void throw_exception(int id, const char* what)
{
    EAssert(id != 0, "exception id is zero!");
    s_exce_info = what;
    s_exce_id = id;
    longjmp(s_jmp_buf, id);
}
