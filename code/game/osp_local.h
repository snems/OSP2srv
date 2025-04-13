#ifndef OSP_LOCAL
#define OSP_LOCAL
#include "g_local.h"
#define NYAN_POINT()      G_Printf( "%s:%d: DEBUG point reached\n", __FILE__, __LINE__)
#define NYAN_INT(VALUE)   G_Printf( "%s:%d: %s = %i\n", __FILE__, __LINE__, #VALUE, (int)VALUE)
#define NYAN_FLOAT(VALUE) G_Printf( "%s:%d: %s = %f\n", __FILE__, __LINE__, #VALUE, (float)VALUE)
#define NYAN_VEC4(VALUE) G_Printf( "%s:%d: %s = %f,%f,%f,%f\n", __FILE__, __LINE__, #VALUE, VALUE[0], VALUE[1], VALUE[2], VALUE[3])
#define NYAN_MSG(VALUE)   G_Printf( "%s:%d: %s\n", __FILE__, __LINE__, VALUE)
//
// osp_admin.c
//
extern const char* monthName[12];
extern qboolean adminLogDisabled;
void G_AdminLog(const char* text);

//
// osp_bot.c
//
void osp_bot_2ca01(void);
void osp_bot_2ca2a(void);
void osp_bot_2ca58(void);


//
// osp_cmds.c
//
struct viewcam_s
{
    float unk1;
    float ind;
    float unk2;
    float unk3;
    float unk4;
    float len;
    float unk5;
    float unk6;
    float unk7;
    float unk8;
    float unk9;
    float unk10;
    float unk11;
    vec3_t origin;
    vec3_t angles;
};

typedef struct viewcam_s viewcam_t;
extern viewcam_t viewcams[256];

#define CMDHELP_0   0
#define CMDHELP_1   1
#define CMDHELP_SPECONLY   2
#define CMDHELP_3   3
#define CMDHELP_PLAYER_LIST 4
#define CMDHELP_5   5
#define CMDHELP_6   6
#define CMDHELP_7   7
#define CMDHELP_8   8
#define CMDHELP_9   9
#define CMDHELP_10  10 //0a
#define CMDHELP_11  11 //0b
#define CMDHELP_12  12 //0c
#define CMDHELP_13  13 //0d
#define CMDHELP_14  14 //0e
#define CMDHELP_15  15 //0f
#define CMDHELP_16  16 //10
#define CMDHELP_17  17 //11
#define CMDHELP_18  18 //12
#define CMDHELP_19  19 //13
#define CMDHELP_20  20 //14
#define CMDHELP_AUTORECORD  21
#define CMDHELP_AUTOSCREENSHOT  22
#define CMDHELP_23  23 //17
#define CMDHELP_REF  24 //18
#define CMDHELP_VIEWCAM  25 //19
#define CMDHELP_VCFOLLOW  26 //1a
#define CMDHELP_VCFREE  27 //1b
#define CMDHELP_VCVIEW  28 //1c
#define CMDHELP_VIEWADD  29 //1d
#define CMDHELP_VIEWALL  30 //1e
#define CMDHELP_VIEWREMOVE  31 //1f
#define CMDHELP_VIEWNONE  32 //20
#define CMDHELP_VIEWNEXT  33 //21
#define CMDHELP_VIEWCYCLENEXT  34 //22
#define CMDHELP_VIEWFOLLOW  35 //23
#define CMDHELP_VIEWTEAM  36 //24
#define CMDHELP_TEAM  37 //25
#define CMDHELP_FOLLOW  38 //26
#define CMDHELP_39  39 //27
#define CMDHELP_CALLVOTE  40 //28
#define CMDHELP_MOST  41 //29
qboolean Cmd_OSPCommands_f(const gentity_t* ent, char* arg, qboolean isReferee);
void Cmd_HelpAvailOspGameCommands_f(const gentity_t* ent);
qboolean Cmd_Help_f(const gentity_t* ent, int cmdNumber);
void Cmd_Uinfo_f(const gentity_t* ent);
void osp_cmds_2d54a(void);
void Cmd_SpecOnly_f(const gentity_t* ent);
void Cmd_Lock_f(const gentity_t* ent, char* arg);
void osp_cmds_2d73a(void);
void Cmd_SpecLock_f(const gentity_t* ent, char* arg);
void Cmd_Ready_f(const gentity_t* ent, qboolean unk);
void Cmd_PlayerList_f(const gentity_t* ent);
void Cmd_Invite_f(const gentity_t* ent);
void Cmd_SpecInvite_f(const gentity_t* ent);
void Cmd_CoachInvite_f(const gentity_t* ent);
void Cmd_CoachDecline_f(const gentity_t* ent);
void Cmd_CoachKick_f(const gentity_t* ent);
void Cmd_Coach_f(const gentity_t* ent, qboolean unk);
void Cmd_Remove_f(const gentity_t* ent);
void Cmd_Resign_f(const gentity_t* ent);
void Cmd_TeamReady_f(const gentity_t* ent);
void Cmd_Capitans_f(const gentity_t* ent);
void Cmd_Pause_f(const gentity_t* ent, qboolean unk);
void Cmd_Stats_f(const gentity_t* ent, int arg);
void Cmd_StatsAll_f(const gentity_t* ent, int arg);
void osp_cmds_2ee67(void);
void osp_cmds_2f019(void);
void Cmd_Acc_f(const gentity_t* ent, int arg);
void Cmd_JoinCode_f(const gentity_t* ent);
void Cmd_Drop_f(const gentity_t* ent);
void Cmd_GetStatsInfo_f(gentity_t* ent);
void osp_cmds_2fa77(void);
void osp_cmds_2fc2f(void);
void osp_cmds_2fcba(void);
void osp_cmds_2fd00(void);
void osp_cmds_30178(void);
void osp_cmds_30373(void);
void osp_cmds_3048d(void);
void osp_cmds_3050e(void);
void osp_cmds_306a0(void);
void osp_cmds_307cb(void);
void osp_cmds_30811(void);
void osp_cmds_30876(void);
void osp_cmds_308b5(void);
void Cmd_Settings_f(const gentity_t* ent);
void osp_cmds_30b09(void);
void osp_cmds_30c10(void);
void osp_cmds_30c9d(void);
qboolean G_IsGameTypeOSP(int gametype);
void osp_cmds_30dd4(void);
void osp_cmds_30f4f(void);
void osp_cmds_30f90(void);
void osp_cmds_30f9f(void);
void osp_cmds_30ff1(void);
void osp_cmds_31023(void);
void osp_cmds_31061(void);
void osp_cmds_3114c(void);
void osp_cmds_3145e(void);
void osp_cmds_31607(void);
void osp_cmds_3189f(void);
qboolean G_IsItemAllowed(const gentity_t* ent);
void G_RegisterWeapon(void);
void G_ClientSpawnHPAndGuns(gentity_t* ent);
void osp_cmds_3213d(gentity_t* ent);
void osp_cmds_321e7(void);
qboolean G_IsAttackEnabled(void);
void osp_cmds_32212(void);
void osp_cmds_322a5(void);
void osp_cmds_3235a(void);
void osp_cmds_324a9(void);
void osp_cmds_324e2(void);
void osp_cmds_32548(void);
void osp_cmds_3257c(void);
void osp_cmds_32682(void);
void osp_cmds_3269b(void);
void osp_cmds_32776(void);
void osp_cmds_32854(void);
void osp_cmds_3285c(void);
void osp_cmds_328fb(void);
void G_UpdateReadyStatus(void);
void osp_cmds_329e9(void);
void osp_cmds_32a7e(void);
void osp_cmds_32d19(void);
void osp_cmds_32d5b(void);
void Cmd_AutoScreenshot_f(int entityNum, qboolean unk1, qboolean unk2);
void osp_cmds_3303c(void);
void osp_cmds_330e2(void);
void osp_cmds_331c9(void);
void osp_cmds_33222(void);
void osp_cmds_332db(void);
void osp_cmds_3344e(void);
void osp_cmds_33482(void);
void osp_cmds_334b6(void);
void G_OSPAuth(const gentity_t* ent);
void Cmd_OSPAuth_f(const gentity_t* ent);
void osp_cmds_33674(void);
void osp_cmds_33677(void);
void osp_cmds_337ae(void);
void osp_cmds_3398b(void);
void osp_cmds_339b6(void);
void osp_cmds_339ff(void);
void osp_cmds_33aa6(void);
void osp_cmds_33b68(void);
qboolean G_GetClientVersion(char *version, unsigned int size);
void osp_cmds_33c92(gentity_t* self);
void osp_cmds_33cc5(int arg1, int modelindex);
void osp_cmds_33d12(int modelindex);
void osp_cmds_33d90(void);
void osp_cmds_33dc1(qboolean flag);
void osp_cmds_33fa0(void);
void osp_cmds_34159(void);
void osp_cmds_341ad(void);
void osp_cmds_341e1(void);
void osp_cmds_34210(void);
void osp_cmds_3424f(void);
void osp_cmds_3427c(void);
void osp_cmds_3461e(void);
void osp_cmds_34627(void);
void osp_cmds_3466d(void);
void osp_cmds_3497d(void);
void osp_cmds_349dc(void);
void osp_cmds_34c31(void);
void osp_cmds_34d12(void);
void osp_cmds_34d98(void);
void osp_cmds_34dac(void);
void Cmd_MapLoad_f(qboolean unk);
void osp_cmds_35068(void);
void Cmd_MapList_f(const gentity_t* ent);
void osp_cmds_3525b(void);
void osp_cmds_352ff(void);
void osp_cmds_35309(void);
void osp_cmds_3532f(void);
void osp_cmds_35419(void);
void Cmd_ViewAdd_f(const gentity_t* ent);
void Cmd_ViewerMove_f(const gentity_t* ent);
void Cmd_ViewFollow(const gentity_t* ent);
void Cmd_ViewNext_f(const gentity_t* ent, int unk);
void Cmd_ViewFrame_f(const gentity_t* ent);
void Cmd_ViewCycleNext_f(const gentity_t* ent, int unk);
void Cmd_ViewNone_f(const gentity_t* ent);
void Cmd_ViewList_f(const gentity_t* ent);
void Cmd_ViewAll_f(const gentity_t* ent);
void Cmd_ViewTeam_f(const gentity_t* ent, int unk1, int unk2);
void osp_cmds_35d93(void);
void osp_cmds_35fb6(void);
void osp_cmds_3601a(void);
void osp_cmds_361d8(void);
void osp_cmds_36285(void);
void osp_cmds_3646f(void);
void osp_cmds_36551(void);
qboolean Cmd_RefCommandArg_f(const gentity_t* ent, const char* arg, qboolean unk);
void Cmd_RefCommand_f(const gentity_t* ent, qboolean skipOSPCommands);
void Cmd_RefHelp(const gentity_t* ent);
void osp_cmds_36b67(void);
void osp_cmds_36cfa(void);
void osp_cmds_36edf(void);
void osp_cmds_36f6e(void);
void osp_cmds_3705d(void);
void osp_cmds_3710c(void);
void osp_cmds_371be(void);
void osp_cmds_371cb(void);
void osp_cmds_372a0(void);
void osp_cmds_3740f(void);
void osp_cmds_37506(void);
void osp_cmds_375e1(void);
void osp_cmds_376a1(void);
void osp_cmds_376d7(void);
void osp_cmds_37731(void);
void osp_cmds_3775b(void);
void osp_cmds_378e0(void);
void osp_cmds_37910(void);
void osp_cmds_37a00(void);
void osp_cmds_37c09(void);
void osp_cmds_37e43(void);
void osp_cmds_37e7f(void);
void osp_cmds_37fd9(void);
void osp_cmds_38260(void);
void osp_cmds_384cb(void);
void osp_cmds_385cf(void);
void osp_cmds_386d3(void);
void osp_cmds_387f6(void);
void osp_cmds_388eb(void);
void osp_cmds_38964(void);
void osp_cmds_38972(void);
void osp_cmds_3897a(void);
void osp_cmds_38985(void);
team_t G_GetSpectatorTeam(const gentity_t* ent);
void osp_cmds_38ac2(void);
void osp_cmds_38c01(void);
void osp_cmds_38c45(void);
void osp_cmds_38c7b(void);
void osp_cmds_38d3c(void);
void osp_cmds_38dbe(void);
void osp_cmds_38dea(void);
void Cmd_ScoresDM_f(const gentity_t* ent, int arg);
void Cmd_ScoresFT_f(const gentity_t* ent, int arg);
void Cmd_ScoresFFA_f(const gentity_t* ent, int arg);
void Cmd_ScoresCTF_f(const gentity_t* ent, int arg);
void Cmd_ScoresCA_f(const gentity_t* ent, int arg);
void osp_cmds_39844(void);
void osp_cmds_3986a(void);
void osp_cmds_3994e(void);
void osp_cmds_39979(void);
void osp_cmds_39a71(void);
void osp_cmds_39c0b(void);
void osp_cmds_39cf2(void);
void osp_cmds_39e12(void);
void osp_cmds_3a004(void);
void osp_cmds_3a168(void);
void osp_cmds_3a413(void);
void Cmd_Snapshot_f(const gentity_t* ent);
void Cmd_SnapshotDump_f(const gentity_t* ent);
void Cmd_VcProximity_f(const gentity_t* ent);
void Cmd_Range_f(const gentity_t* ent);
void Cmd_ViewCam_f(const gentity_t* ent);
void Cmd_VcFollow_f(const gentity_t* ent);



//
// osp_unsorted.c
//
extern int time_in_game;
extern int game_paused;
extern int time_in_pause;

void osp_unsorted_3aa10(void);
void osp_unsorted_3abd0(void);
void osp_unsorted_3ac27(void);
void osp_unsorted_3ac40(void);
void osp_unsorted_3ac65(void);
void osp_unsorted_3ac74(void);
void osp_unsorted_3ac90(void);
void osp_unsorted_3ad17(void);
void osp_unsorted_3c957(void);
void osp_unsorted_3cb1a(void);
void osp_unsorted_3cfb7(void);
void osp_unsorted_3d019(void);
void osp_unsorted_3d04f(void);
void osp_unsorted_3d0c5(void);
void osp_unsorted_3d131(void);
void osp_unsorted_3d174(void);
void osp_unsorted_3d1bb(void);
void osp_unsorted_3d3bc(void);
void osp_unsorted_3d418(void);
void osp_unsorted_3d4ca(void);
void osp_unsorted_3d4d9(void);
void osp_unsorted_3d5da(void);
void osp_unsorted_3d61f(void);

#include "osp_unimpl.h"
#endif
