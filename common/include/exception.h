#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>

extern jmp_buf s_jmp_buf;
extern const char* s_exce_info;
extern int s_exce_id;
#ifndef __cplusplus
#define TRY(i) \
signal(SIGSEGV, SIGSEGV_handle); \
{ if (setjmp(s_jmp_buf)) goto catch_exception##i; }

#define CATCH(i) \
if (0) catch_exception##i:
#else
#define TRY(i) try {
#define CATCH(i) } catch(...)
#endif
#define GET_EXCE \
( s_exce_info )

#define GET_EXCE_ID \
( s_exce_id )

extern void SIGSEGV_handle(int s);
extern void throw_exception(int id, const char* what);

#endif
