#include "pch.h"
#include "elog.h"
#include "string.h"

pthread_rwlock_t g_lock;
char g_elog_buffer[ELOG_BUFFER_SIZE];
#ifdef USE_LOG_SYSTEM
static FILE* g_elog_file = NULL;
#endif
void ELog_Init()
{
#ifdef USE_LOG_SYSTEM
#if defined(_WIN32) || defined(_WIN64)
	g_elog_file = SafeFOpen("log.log", "w+");
#elif defined(__APPLE__)
    g_elog_file = SafeFOpen("/Users/joumining/vengine/log.log", "w+");
#endif
	pthread_rwlock_init(&g_lock, NULL);
#endif
}

void ELog_write()
{
#ifdef USE_LOG_SYSTEM
	///pthread_spin_lock(&g_lock);
    snprintf(g_elog_buffer, ELOG_BUFFER_SIZE - 1, "%s\n", g_elog_buffer);
    fwrite(g_elog_buffer, strlen(g_elog_buffer), 1, g_elog_file);
	///pthread_spin_unlock(&g_lock);
#endif
}

void ELog_Release()
{
#ifdef USE_LOG_SYSTEM
	///pthread_spin_lock(&g_lock);
    fclose(g_elog_file);
    g_elog_file = NULL;
	///pthread_spin_unlock(&g_lock);
#endif
}
