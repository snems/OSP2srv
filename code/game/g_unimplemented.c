#include "g_local.h"


#define OSP2_UNIMPLEMENTED_FUNCTION()  trap_SendServerCommand(-1, va("print \"%s:%d: Unimplemented function called.\n\"", __FILE__, __LINE__));

void G_OSPAuth(const gentity_t* ent)
{
	(void)ent;
	OSP2_UNIMPLEMENTED_FUNCTION();
}

void Cmd_Uinfo_f(const gentity_t* ent)
{
	(void)ent;
	OSP2_UNIMPLEMENTED_FUNCTION();
}

void Cmd_ViewList_f(const gentity_t* ent)
{
	(void)ent;
	OSP2_UNIMPLEMENTED_FUNCTION();
}

void Cmd_PlayerList_f(const gentity_t* ent)
{
	(void)ent;
	OSP2_UNIMPLEMENTED_FUNCTION();
}

void Cmd_GetStatsInfo_f(const gentity_t* ent)
{
	(void)ent;
	OSP2_UNIMPLEMENTED_FUNCTION();
}

void Cmd_Stats_f(const gentity_t* ent, int arg)
{
	(void)ent;
	(void)arg;
	OSP2_UNIMPLEMENTED_FUNCTION();
}

void Cmd_StatsAll_f(const gentity_t* ent, int arg)
{
	(void)ent;
	(void)arg;
	OSP2_UNIMPLEMENTED_FUNCTION();
}

void Cmd_Acc_f(const gentity_t* ent, int arg)
{
	(void)ent;
	(void)arg;
	OSP2_UNIMPLEMENTED_FUNCTION();
}

void Cmd_OSPAuth_f(const gentity_t* ent)
{
	(void)ent;
	OSP2_UNIMPLEMENTED_FUNCTION();
}

void Cmd_AutoScreenshot_f(int arg1, int arg2, int arg3)
{
	(void)arg1;
	(void)arg2;
	(void)arg3;
	OSP2_UNIMPLEMENTED_FUNCTION();
}

void Cmd_ScoresFT_f(const gentity_t* ent, int arg)
{
	(void)ent;
	(void)arg;
	OSP2_UNIMPLEMENTED_FUNCTION();
}

void Cmd_ScoresDM_f(const gentity_t* ent, int arg)
{
	(void)ent;
	(void)arg;
	OSP2_UNIMPLEMENTED_FUNCTION();
}

void Cmd_ScoresCTF_f(const gentity_t* ent, int arg)
{
	(void)ent;
	(void)arg;
	OSP2_UNIMPLEMENTED_FUNCTION();
}

void Cmd_ScoresCA_f(const gentity_t* ent, int arg)
{
	(void)ent;
	(void)arg;
	OSP2_UNIMPLEMENTED_FUNCTION();
}

void Cmd_ScoresFFA_f(const gentity_t* ent, int arg)
{
	(void)ent;
	(void)arg;
	OSP2_UNIMPLEMENTED_FUNCTION();
}

qboolean Cmd_CallVote2_f(const gentity_t* ent, int arg)
{
	(void)ent;
	(void)arg;
	OSP2_UNIMPLEMENTED_FUNCTION();
	return qfalse;
}

void Cmd_RefHelp(const gentity_t* ent)
{
	(void)ent;
	OSP2_UNIMPLEMENTED_FUNCTION();
}

qboolean Cmd_ControlCommands_f(const gentity_t* ent, const char *arg, qboolean unk)
{
	(void)ent;
	(void)arg;
	(void)unk;
	OSP2_UNIMPLEMENTED_FUNCTION();
	return qfalse;
}

qboolean Cmd_RefCommandArg_f(const gentity_t* ent, const char *arg, qboolean unk)
{
	(void)ent;
	(void)arg;
	(void)unk;
	OSP2_UNIMPLEMENTED_FUNCTION();
	return qfalse;
}

