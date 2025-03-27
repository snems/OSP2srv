#include "osp_local.h"
/*
================
G_AdminLog
================
*/
static fileHandle_t adminLogHandle;
qboolean adminLogDisabled = qfalse;
const char* monthName[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };


void G_AdminLog(const char* text)
{
	qtime_t localTime;
	const char* tmp;
	char textToWrite[MAX_STRING_CHARS];
	if (!admin_log.string[0] || Q_stricmp(admin_log.string, "none"))
	{
		adminLogDisabled = qtrue;
		return;
	}

	(void)trap_FS_FOpenFile(admin_log.string, &adminLogHandle, FS_WRITE | FS_APPEND);
	if (!adminLogHandle)
	{
		adminLogDisabled = qtrue;
		G_Printf("*** ERROR: Couldn't open admin log \"%s\"\n\n", admin_log.string);
	}
	else
	{
		trap_RealTime(&localTime);
		tmp = va("%02d %s %d (%02d:%02d:%02d)\t%s\n", localTime.tm_mday, monthName[localTime.tm_mon], localTime.tm_year + 1900, localTime.tm_hour, localTime.tm_min, localTime.tm_sec, text);
		Q_strncpyz(textToWrite, tmp, MAX_STRING_CHARS);
		trap_FS_Write(textToWrite, strlen(textToWrite), adminLogHandle);
		trap_FS_FCloseFile(adminLogHandle);
		if (g_dedicated.integer)
		{
			G_Printf("%s\n", textToWrite);
		}
	}
}
