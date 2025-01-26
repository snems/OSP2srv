/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
//
// g_local.h -- local definitions for game module

#include "q_shared.h"
#include "bg_public.h"
#include "g_public.h"

//==================================================================

// the "gameversion" client command will print this plus compile date
#define GAMEVERSION "baseq3"

#define BODY_QUEUE_SIZE     8

#define INFINITE            1000000

#define FRAMETIME           100                 // msec
#define CARNAGE_REWARD_TIME 3000
#define REWARD_SPRITE_TIME  2000

#define INTERMISSION_DELAY_TIME 1000
#define SP_INTERMISSION_DELAY_TIME  5000

// gentity->flags
#define FL_GODMODE              0x00000010
#define FL_NOTARGET             0x00000020
#define FL_TEAMSLAVE            0x00000400  // not the first on the team
#define FL_NO_KNOCKBACK         0x00000800
#define FL_DROPPED_ITEM         0x00001000
#define FL_NO_BOTS              0x00002000  // spawn point not for bot use
#define FL_NO_HUMANS            0x00004000  // spawn point just for bots
#define FL_FORCE_GESTURE        0x00008000  // force gesture on client

// movers are things like doors, plats, buttons, etc
typedef enum
{
	MOVER_POS1,
	MOVER_POS2,
	MOVER_1TO2,
	MOVER_2TO1
} moverState_t;

#define SP_PODIUM_MODEL     "models/mapobjects/podium/podium4.md3"

//============================================================================

typedef struct gentity_s gentity_t;
typedef struct gclient_s gclient_t;

struct gentity_s
{
	entityState_t   s;              // communicated by server to clients
	entityShared_t  r;              // shared by both the server system and game

	// DO NOT MODIFY ANYTHING ABOVE THIS, THE SERVER
	// EXPECTS THE FIELDS IN THAT ORDER!
	//================================

	struct gclient_s*    client;            // NULL if not a client

	qboolean    inuse;

	char*        classname;         // set in QuakeEd
	int         spawnflags;         // set in QuakeEd

	qboolean    neverFree;          // if true, FreeEntity will only unlink
	// bodyque uses this

	int         flags;              // FL_* variables

	char*        model;
	char*        model2;
	int         freetime;           // level.time when the object was freed

	int         eventTime;          // events will be cleared EVENT_VALID_MSEC after set
	qboolean    freeAfterEvent;
	qboolean    unlinkAfterEvent;

	qboolean    physicsObject;      // if true, it can be pushed by movers and fall off edges
	// all game items are physicsObjects,
	float       physicsBounce;      // 1.0 = continuous bounce, 0.0 = no bounce
	int         clipmask;           // brushes with this content value will be collided against
	// when moving.  items and corpses do not collide against
	// players, for instance

	// movers
	moverState_t moverState;
	int         soundPos1;
	int         sound1to2;
	int         sound2to1;
	int         soundPos2;
	int         soundLoop;
	gentity_t*   parent;
	gentity_t*   nextTrain;
	gentity_t*   prevTrain;
	vec3_t      pos1, pos2;

	char*        message;

	int         timestamp;      // body queue sinking, etc

	float       angle;          // set in editor, -1 = up, -2 = down
	char*        target;
	char*        targetname;
	char*        team;
	char*        targetShaderName;
	char*        targetShaderNewName;
	gentity_t*   target_ent;

	float       speed;
	vec3_t      movedir;

	int         nextthink;
	void (*think)(gentity_t* self);
	void (*reached)(gentity_t* self);           // movers call this when hitting endpoint
	void (*blocked)(gentity_t* self, gentity_t* other);
	void (*touch)(gentity_t* self, gentity_t* other, trace_t* trace);
	void (*use)(gentity_t* self, gentity_t* other, gentity_t* activator);
	void (*pain)(gentity_t* self, gentity_t* attacker, int damage);
	void (*die)(gentity_t* self, gentity_t* inflictor, gentity_t* attacker, int damage, int mod);

	int         pain_debounce_time;
	int         fly_sound_debounce_time;    // wind tunnel
	int         last_move_time;

	int         health;

	qboolean    takedamage;

	int         damage;
	int         splashDamage;   // quad will increase this without increasing radius
	int         splashRadius;
	int         methodOfDeath;
	int         splashMethodOfDeath;

	int         count;

	gentity_t*   chain;
	gentity_t*   enemy;
	gentity_t*   activator;
	gentity_t*   teamchain;     // next entity in team
	gentity_t*   teammaster;    // master of the team

#ifdef MISSIONPACK
	int         kamikazeTime;
	int         kamikazeShockTime;
#endif

	int         watertype;
	int         waterlevel;

	int         noise_index;

	// timing variables
	float       wait;
	float       random;

	gitem_t*     item;          // for bonus items
};


typedef enum
{
	CON_DISCONNECTED,
	CON_CONNECTING,
	CON_CONNECTED
} clientConnected_t;

typedef enum
{
	SPECTATOR_NOT,
	SPECTATOR_FREE,
	SPECTATOR_FOLLOW,
	SPECTATOR_SCOREBOARD
} spectatorState_t;

typedef enum
{
	TEAM_BEGIN,     // Beginning a team game, spawn at base
	TEAM_ACTIVE     // Now actively playing
} playerTeamStateState_t;

typedef struct
{
	playerTeamStateState_t  state;

	int         location;

	int         captures;
	int         basedefense;
	int         carrierdefense;
	int         flagrecovery;
	int         fragcarrier;
	int         assists;

	float       lasthurtcarrier;
	float       lastreturnedflag;
	float       flagsince;
	float       lastfraggedcarrier;
} playerTeamState_t;

// the auto following clients don't follow a specific client
// number, but instead follow the first two active players
#define FOLLOW_ACTIVE1  -1
#define FOLLOW_ACTIVE2  -2

// client data that stays across multiple levels or tournament restarts
// this is achieved by writing all the data to cvar strings at game shutdown
// time and reading them back at connection time.  Anything added here
// MUST be dealt with in G_InitSessionData() / G_ReadSessionData() / G_WriteSessionData()
typedef struct
{
	team_t      sessionTeam;
	int         spectatorTime;      // for determining next-in-line to play
	spectatorState_t    spectatorState;
	int         spectatorClient;    // for chasecam and follow mode
	int         wins, losses;       // tournament stats
	qboolean    teamLeader;         // true when this client is a team leader
} clientSession_t;

//
#define MAX_NETNAME         36
#define MAX_VOTE_COUNT      3

// client data that stays across multiple respawns, but is cleared
// on each level change or team change at ClientBegin()
typedef struct
{
	clientConnected_t   connected;
	usercmd_t   cmd;                // we would lose angles if not persistant
	qboolean    localClient;        // true if "ip" info key is "localhost"
	qboolean    initialSpawn;       // the first spawn should be at a cool location
	qboolean    predictItemPickup;  // based on cg_predictItems userinfo
	qboolean    pmoveFixed;         //
	char        netname[MAX_NETNAME];
	int         maxHealth;          // for handicapping
	int         enterTime;          // level.time the client entered the game
	playerTeamState_t teamState;    // status in teamplay games
	int         voteCount;          // to prevent people from constantly calling votes
	int         teamVoteCount;      // to prevent people from constantly calling votes
	qboolean    teamInfo;           // send team overlay updates?
} clientPersistant_t;


// this structure is cleared on each ClientSpawn(),
// except for 'client->pers' and 'client->sess'
struct gclient_s
{
	// ps MUST be the first element, because the server expects it
	playerState_t   ps;             // communicated by server to clients

	// the rest of the structure is private to game
	clientPersistant_t  pers;
	clientSession_t     sess;

	qboolean    readyToExit;        // wishes to leave the intermission

	qboolean    noclip;

	int         lastCmdTime;        // level.time of last usercmd_t, for EF_CONNECTION
	// we can't just use pers.lastCommand.time, because
	// of the g_sycronousclients case
	int         buttons;
	int         oldbuttons;
	int         latched_buttons;

	vec3_t      oldOrigin;

	// sum up damage over an entire frame, so
	// shotgun blasts give a single big kick
	int         damage_armor;       // damage absorbed by armor
	int         damage_blood;       // damage taken out of health
	int         damage_knockback;   // impact damage
	vec3_t      damage_from;        // origin for vector calculation
	qboolean    damage_fromWorld;   // if true, don't use the damage_from vector

	int         accurateCount;      // for "impressive" reward sound

	int         accuracy_shots;     // total number of shots
	int         accuracy_hits;      // total number of hits

	//
	int         lastkilled_client;  // last client that this client killed
	int         lasthurt_client;    // last client that damaged this client
	int         lasthurt_mod;       // type of damage the client did

	// timers
	int         respawnTime;        // can respawn when time > this, force after g_forcerespwan
	int         OSPAuth;                    //
	int         inactivityTime;     // kick players when time > this
	qboolean    inactivityWarning;  // qtrue if the five seoond warning has been given
	int         rewardTime;         // clear the EF_AWARD_IMPRESSIVE, etc when time > this

	int         airOutTime;

	int         lastKillTime;       // for multiple kill rewards

	qboolean    fireHeld;           // used for hook
	gentity_t*   hook;              // grapple hook if out

	int         switchTeamTime;     // time the player switched teams

	// timeResidual is used to handle events that happen every second
	// like health / armor countdowns and regeneration
	int         timeResidual;

#ifdef MISSIONPACK
	gentity_t*   persistantPowerup;
	int         portalID;
	int         ammoTimes[WP_NUM_WEAPONS];
	int         invulnerabilityTime;
#endif

	char*        areabits;
	char         tail1[344];
	int          unknown1;
	char         tail2[428];
};


//
// this structure is cleared as each map is entered
//
#define MAX_SPAWN_VARS          64
#define MAX_SPAWN_VARS_CHARS    4096

typedef struct
{
	struct gclient_s*    clients;       // [maxclients]

	struct gentity_s*    gentities;
	int         gentitySize;
	int         num_entities;       // current number, <= MAX_GENTITIES

	int         warmupTime;         // restart match at this time

	fileHandle_t    logFile;

	// store latched cvars here that we want to get at often
	int         maxclients;

	int         framenum;
	int         time;                   // in msec
	int         previousTime;           // so movers can back up when blocked

	int         startTime;              // level.time the map was started

	int         teamScores[TEAM_NUM_TEAMS];
	int         lastTeamLocationTime;       // last time of client team location update

	qboolean    newSession;             // don't use any old session data, because
	// we changed gametype

	qboolean    restarted;              // waiting for a map_restart to fire

	int         numConnectedClients;
	int         numNonSpectatorClients; // includes connecting clients
	int         numPlayingClients;      // connected, non-spectators
	int         sortedClients[MAX_CLIENTS];     // sorted by score
	int         follow1, follow2;       // clientNums for auto-follow spectators

	int         snd_fry;                // sound index for standing in lava

	int         warmupModificationCount;    // for detecting if g_warmup is changed

	// voting state
	char        voteString[MAX_STRING_CHARS];
	char        voteDisplayString[MAX_STRING_CHARS];
	int         voteTime;               // level.time vote was called
	int         voteExecuteTime;        // time the vote is executed
	int         voteYes;
	int         voteNo;
	int         numVotingClients;       // set by CalculateRanks

	// team voting state
	char        teamVoteString[2][MAX_STRING_CHARS];
	int         teamVoteTime[2];        // level.time vote was called
	int         teamVoteYes[2];
	int         teamVoteNo[2];
	int         numteamVotingClients[2];// set by CalculateRanks

	// spawn variables
	qboolean    spawning;               // the G_Spawn*() functions are valid
	int         numSpawnVars;
	char*        spawnVars[MAX_SPAWN_VARS][2];  // key / value pairs
	int         numSpawnVarChars;
	char        spawnVarChars[MAX_SPAWN_VARS_CHARS];

	// intermission state
	int         intermissionQueued;     // intermission was qualified, but
	// wait INTERMISSION_DELAY_TIME before
	// actually going there so the last
	// frag can be watched.  Disable future
	// kills during this delay
	int         intermissiontime;       // time the intermission was started
	char*        changemap;
	qboolean    readyToExit;            // at least one client wants to exit
	int         exitTime;
	vec3_t      intermission_origin;    // also used for spectator spawns
	vec3_t      intermission_angle;

	qboolean    locationLinked;         // target_locations get linked
	gentity_t*   locationHead;          // head of the location list
	int         bodyQueIndex;           // dead bodies
	gentity_t*   bodyQue[BODY_QUEUE_SIZE];
	int osp_levelTime;
	int unknown_flag1;
	int unknown1;
	int unknown2;
	int unknown3;
} level_locals_t;


//
// g_spawn.c
//
qboolean    G_SpawnString(const char* key, const char* defaultString, char** out);
// spawn string returns a temporary reference, you must CopyString() if you want to keep it
qboolean    G_SpawnFloat(const char* key, const char* defaultString, float* out);
qboolean    G_SpawnInt(const char* key, const char* defaultString, int* out);
qboolean    G_SpawnVector(const char* key, const char* defaultString, float* out);
void        G_SpawnEntitiesFromString(void);
char* G_NewString(const char* string);

//
// g_cmds.c
//
void Cmd_Score_f(gentity_t* ent);
void StopFollowing(gentity_t* ent);
void BroadcastTeamChange(gclient_t* client, int oldTeam);
void SetTeam(gentity_t* ent, char* s);
void Cmd_FollowCycle_f(gentity_t* ent, int dir);

//
// g_items.c
//
void G_CheckTeamItems(void);
void G_RunItem(gentity_t* ent);
void RespawnItem(gentity_t* ent);

void UseHoldableItem(gentity_t* ent);
void PrecacheItem(gitem_t* it);
gentity_t* Drop_Item(gentity_t* ent, gitem_t* item, float angle);
gentity_t* LaunchItem(gitem_t* item, vec3_t origin, vec3_t velocity);
void SetRespawn(gentity_t* ent, float delay);
void G_SpawnItem(gentity_t* ent, gitem_t* item);
void FinishSpawningItem(gentity_t* ent);
void Think_Weapon(gentity_t* ent);
int ArmorIndex(gentity_t* ent);
void    Add_Ammo(gentity_t* ent, int weapon, int count);
void Touch_Item(gentity_t* ent, gentity_t* other, trace_t* trace);

void ClearRegisteredItems(void);
void RegisterItem(gitem_t* item);
void SaveRegisteredItems(void);

//
// g_utils.c
//
int G_ModelIndex(char* name);
int     G_SoundIndex(char* name);
void    G_TeamCommand(team_t team, char* cmd);
void    G_KillBox(gentity_t* ent);
gentity_t* G_Find(gentity_t* from, int fieldofs, const char* match);
gentity_t* G_PickTarget(char* targetname);
void    G_UseTargets(gentity_t* ent, gentity_t* activator);
void    G_SetMovedir(vec3_t angles, vec3_t movedir);

void    G_InitGentity(gentity_t* e);
gentity_t*   G_Spawn(void);
gentity_t* G_TempEntity(vec3_t origin, int event);
void    G_Sound(gentity_t* ent, int channel, int soundIndex);
void    G_FreeEntity(gentity_t* e);
qboolean    G_EntitiesFree(void);

void    G_TouchTriggers(gentity_t* ent);
void    G_TouchSolids(gentity_t* ent);

float*   tv(float x, float y, float z);
char*    vtos(const vec3_t v);

float vectoyaw(const vec3_t vec);

void G_AddPredictableEvent(gentity_t* ent, int event, int eventParm);
void G_AddEvent(gentity_t* ent, int event, int eventParm);
void G_SetOrigin(gentity_t* ent, vec3_t origin);
void AddRemap(const char* oldShader, const char* newShader, float timeOffset);
const char* BuildShaderStateConfig();

//
// g_combat.c
//
qboolean CanDamage(gentity_t* targ, vec3_t origin);
void G_Damage(gentity_t* targ, gentity_t* inflictor, gentity_t* attacker, vec3_t dir, vec3_t point, int damage, int dflags, int mod);
qboolean G_RadiusDamage(vec3_t origin, gentity_t* attacker, float damage, float radius, gentity_t* ignore, int mod);
int G_InvulnerabilityEffect(gentity_t* targ, vec3_t dir, vec3_t point, vec3_t impactpoint, vec3_t bouncedir);
void body_die(gentity_t* self, gentity_t* inflictor, gentity_t* attacker, int damage, int meansOfDeath);
void TossClientItems(gentity_t* self);
#ifdef MISSIONPACK
void TossClientPersistantPowerups(gentity_t* self);
#endif
void TossClientCubes(gentity_t* self);

// damage flags
#define DAMAGE_RADIUS               0x00000001  // damage was indirect
#define DAMAGE_NO_ARMOR             0x00000002  // armour does not protect from this damage
#define DAMAGE_NO_KNOCKBACK         0x00000004  // do not affect velocity, just view angles
#define DAMAGE_NO_PROTECTION        0x00000008  // armor, shields, invulnerability, and godmode have no effect
#ifdef MISSIONPACK
#define DAMAGE_NO_TEAM_PROTECTION   0x00000010  // armor, shields, invulnerability, and godmode have no effect
#endif

//
// g_missile.c
//
void G_RunMissile(gentity_t* ent);

gentity_t* fire_blaster(gentity_t* self, vec3_t start, vec3_t aimdir);
gentity_t* fire_plasma(gentity_t* self, vec3_t start, vec3_t aimdir);
gentity_t* fire_grenade(gentity_t* self, vec3_t start, vec3_t aimdir);
gentity_t* fire_rocket(gentity_t* self, vec3_t start, vec3_t dir);
gentity_t* fire_bfg(gentity_t* self, vec3_t start, vec3_t dir);
gentity_t* fire_grapple(gentity_t* self, vec3_t start, vec3_t dir);
#ifdef MISSIONPACK
gentity_t* fire_nail(gentity_t* self, vec3_t start, vec3_t forward, vec3_t right, vec3_t up);
gentity_t* fire_prox(gentity_t* self, vec3_t start, vec3_t aimdir);
#endif


//
// g_mover.c
//
void G_RunMover(gentity_t* ent);
void Touch_DoorTrigger(gentity_t* ent, gentity_t* other, trace_t* trace);

//
// g_trigger.c
//
void trigger_teleporter_touch(gentity_t* self, gentity_t* other, trace_t* trace);


//
// g_misc.c
//
void TeleportPlayer(gentity_t* player, vec3_t origin, vec3_t angles);
#ifdef MISSIONPACK
void DropPortalSource(gentity_t* ent);
void DropPortalDestination(gentity_t* ent);
#endif


//
// g_weapon.c
//
qboolean LogAccuracyHit(gentity_t* target, gentity_t* attacker);
void CalcMuzzlePoint(gentity_t* ent, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint);
void SnapVectorTowards(vec3_t v, vec3_t to);
qboolean CheckGauntletAttack(gentity_t* ent);
void Weapon_HookFree(gentity_t* ent);
void Weapon_HookThink(gentity_t* ent);


//
// g_client.c
//
team_t TeamCount(int ignoreClientNum, int team);
int TeamLeader(int team);
team_t PickTeam(int ignoreClientNum);
void SetClientViewAngle(gentity_t* ent, vec3_t angle);
gentity_t* SelectSpawnPoint(vec3_t avoidPoint, vec3_t origin, vec3_t angles);
void CopyToBodyQue(gentity_t* ent);
void respawn(gentity_t* ent);
void BeginIntermission(void);
void InitClientPersistant(gclient_t* client);
void InitClientResp(gclient_t* client);
void InitBodyQue(void);
void ClientSpawn(gentity_t* ent);
void player_die(gentity_t* self, gentity_t* inflictor, gentity_t* attacker, int damage, int mod);
void AddScore(gentity_t* ent, vec3_t origin, int score);
void CalculateRanks(void);
qboolean SpotWouldTelefrag(gentity_t* spot);

//
// g_svcmds.c
//
qboolean    ConsoleCommand(void);
void G_ProcessIPBans(void);
qboolean G_FilterPacket(char* from);

//
// g_weapon.c
//
void FireWeapon(gentity_t* ent);
#ifdef MISSIONPACK
void G_StartKamikaze(gentity_t* ent);
#endif

//
// p_hud.c
//
void MoveClientToIntermission(gentity_t* client);
void G_SetStats(gentity_t* ent);
void DeathmatchScoreboardMessage(gentity_t* client);

//
// g_cmds.c
//

//
// g_pweapon.c
//


//
// g_main.c
//
void FindIntermissionPoint(void);
void SetLeader(int team, int client);
void CheckTeamLeader(int team);
void G_RunThink(gentity_t* ent);
void QDECL G_LogPrintf(const char* fmt, ...);
void SendScoreboardMessageToAllClients(void);
void QDECL G_Printf(const char* fmt, ...);
void QDECL G_Error(const char* fmt, ...);

//
// g_client.c
//
char* ClientConnect(int clientNum, qboolean firstTime, qboolean isBot);
void ClientUserinfoChanged(int clientNum);
void ClientDisconnect(int clientNum);
void ClientBegin(int clientNum);
void ClientCommand(int clientNum);

//
// g_active.c
//
void ClientThink(int clientNum);
void ClientEndFrame(gentity_t* ent);
void G_RunClient(gentity_t* ent);

//
// g_team.c
//
qboolean OnSameTeam(gentity_t* ent1, gentity_t* ent2);
void Team_CheckDroppedItem(gentity_t* dropped);
qboolean CheckObeliskAttack(gentity_t* obelisk, gentity_t* attacker);

//
// g_mem.c
//
void* G_Alloc(int size);
void G_InitMemory(void);
void Svcmd_GameMem_f(void);

//
// g_session.c
//
void G_ReadSessionData(gclient_t* client);
void G_InitSessionData(gclient_t* client, char* userinfo);

void G_InitWorldSession(void);
void G_WriteSessionData(void);

//
// g_arenas.c
//
void UpdateTournamentInfo(void);
void SpawnModelsOnVictoryPads(void);
void Svcmd_AbortPodium_f(void);

//
// g_bot.c
//
void G_InitBots(qboolean restart);
char* G_GetBotInfoByNumber(int num);
char* G_GetBotInfoByName(const char* name);
void G_CheckBotSpawn(void);
void G_RemoveQueuedBotBegin(int clientNum);
qboolean G_BotConnect(int clientNum, qboolean restart);
void Svcmd_AddBot_f(void);
void Svcmd_BotList_f(void);
void BotInterbreedEndMatch(void);

// ai_main.c
#define MAX_FILEPATH            144

//bot settings
typedef struct bot_settings_s
{
	char characterfile[MAX_FILEPATH];
	float skill;
	char team[MAX_FILEPATH];
} bot_settings_t;

int BotAISetup(int restart);
int BotAIShutdown(int restart);
int BotAILoadMap(int restart);
int BotAISetupClient(int client, struct bot_settings_s* settings, qboolean restart);
int BotAIShutdownClient(int client, qboolean restart);
int BotAIStartFrame(int time);
void BotTestAAS(vec3_t origin);

#include "g_team.h" // teamplay specific stuff


extern  level_locals_t  level;
extern  gentity_t       g_entities[MAX_GENTITIES];

#define FOFS(x) ((int)&(((gentity_t *)0)->x))

extern vmCvar_t           g_cheats;
extern vmCvar_t           osp_gamename;
extern vmCvar_t           gamename;
extern vmCvar_t           gamedate;
extern vmCvar_t           g_restarted;
extern vmCvar_t           sv_mapname;
extern vmCvar_t           g_gametype;
extern vmCvar_t           g_maxclients;
extern vmCvar_t           g_maxGameClients;
extern vmCvar_t           gameversion;
extern vmCvar_t           mapsessionauto;
extern vmCvar_t           mapsessioncount;
extern vmCvar_t           ca_session;
extern vmCvar_t           Players_Red;
extern vmCvar_t           Players_Blue;
extern vmCvar_t           Players_Active;
extern vmCvar_t           Score_Red;
extern vmCvar_t           Score_Blue;
extern vmCvar_t           default_change;
extern vmCvar_t           default_settings;
extern vmCvar_t           Score_Time;
extern vmCvar_t           server_promode;
extern vmCvar_t           server_cq3;
extern vmCvar_t           g_allowVote;
extern vmCvar_t           g_banIPs;
extern vmCvar_t           g_blood;
extern vmCvar_t           g_capturelimit;
extern vmCvar_t           g_debugAlloc;
extern vmCvar_t           g_debugDamage;
extern vmCvar_t           g_debugMove;
extern vmCvar_t           g_dedicated;
extern vmCvar_t           g_dmflags;
extern vmCvar_t           g_doWarmup;
extern vmCvar_t           g_filterBan;
extern vmCvar_t           g_forcerespawn;
extern vmCvar_t           g_friendlyFire;
extern vmCvar_t           g_fraglimit;
extern vmCvar_t           g_gravity;
extern vmCvar_t           g_inactivity;
extern vmCvar_t           g_knockback;
extern vmCvar_t           g_listEntity;
extern vmCvar_t           g_log;
extern vmCvar_t           g_logSync;
extern vmCvar_t           g_motd;
extern vmCvar_t           g_needpass;
extern vmCvar_t           g_password;
extern vmCvar_t           g_podiumDist;
extern vmCvar_t           g_podiumDrop;
extern vmCvar_t           g_quadfactor;
extern vmCvar_t           g_rankings;
extern vmCvar_t           g_spectatorInactivity;
extern vmCvar_t           g_speed;
extern vmCvar_t           g_synchronousClients;
extern vmCvar_t           g_teamAutoJoin;
extern vmCvar_t           g_teamForceBalance;
extern vmCvar_t           g_timelimit;
extern vmCvar_t           g_warmup;
extern vmCvar_t           g_weaponRespawn;
extern vmCvar_t           g_weaponTeamRespawn;
extern vmCvar_t           g_smoothClients;
extern vmCvar_t           pmove_fixed;
extern vmCvar_t           pmove_msec;
extern vmCvar_t           osp_restart;
extern vmCvar_t           roundlimit;
extern vmCvar_t           sv_dcount;
extern vmCvar_t           sv_hostname;
extern vmCvar_t           admin_log;
extern vmCvar_t           armor_q2style;
extern vmCvar_t           bot_checktime;
extern vmCvar_t           bot_autoload;
extern vmCvar_t           decal_directory;
extern vmCvar_t           filter_file;
extern vmCvar_t           freezetag_latejoin;
extern vmCvar_t           freezetag_respawn;
extern vmCvar_t           gauntlet_damage;
extern vmCvar_t           gauntlet_disable;
extern vmCvar_t           hook_delaytime;
extern vmCvar_t           hook_enable;
extern vmCvar_t           hook_holdtime;
extern vmCvar_t           hook_sky;
extern vmCvar_t           hook_speed;
extern vmCvar_t           hook_speedpull;
extern vmCvar_t           instagib_reload;
extern vmCvar_t           item_disable;
extern vmCvar_t           log_default;
extern vmCvar_t           log_stat;
extern vmCvar_t           map_autoload;
extern vmCvar_t           map_debug;
extern vmCvar_t           map_file;
extern vmCvar_t           map_halt;
extern vmCvar_t           map_nocount;
extern vmCvar_t           map_once;
extern vmCvar_t           map_queue;
extern vmCvar_t           map_random;
extern vmCvar_t           map_rotate;
extern vmCvar_t           match_carespawn;
extern vmCvar_t           match_cawarmupfire;
extern vmCvar_t           match_dropitems;
extern vmCvar_t           match_hurtself;
extern vmCvar_t           match_instagib;
extern vmCvar_t           match_latejoin;
extern vmCvar_t           match_maxsuicides;
extern vmCvar_t           match_minplayers;
extern vmCvar_t           match_mutespecs;
extern vmCvar_t           match_overtime;
extern vmCvar_t           match_overtimecount;
extern vmCvar_t           match_overtimelength;
extern vmCvar_t           match_poweruprespawn;
extern vmCvar_t           match_readypercent;
extern vmCvar_t           match_roundtimer;
extern vmCvar_t           match_timeoutlength;
extern vmCvar_t           match_timeoutcount;
extern vmCvar_t           match_winbyscore;
extern vmCvar_t           mg_damage;
extern vmCvar_t           mg_damageTeamplay;
extern vmCvar_t           z_m_current;
extern vmCvar_t           mode_customfile;
extern vmCvar_t           z_m_default;
extern vmCvar_t           mode_start;
extern vmCvar_t           player_always;
extern vmCvar_t           player_file;
extern vmCvar_t           powerup_disable;
extern vmCvar_t           ref_password;
extern vmCvar_t           railgun_damage;
extern vmCvar_t           replace_machinegun;
extern vmCvar_t           replace_shotgun;
extern vmCvar_t           replace_glauncher;
extern vmCvar_t           replace_rlauncher;
extern vmCvar_t           replace_lightning;
extern vmCvar_t           replace_railgun;
extern vmCvar_t           replace_plasmagun;
extern vmCvar_t           replace_bfg;
extern vmCvar_t           replace_invis;
extern vmCvar_t           replace_quad;
extern vmCvar_t           replace_battlesuit;
extern vmCvar_t           replace_haste;
extern vmCvar_t           replace_regen;
extern vmCvar_t           replace_flight;
extern vmCvar_t           server_availmodes;
extern vmCvar_t           server_batcher;
extern vmCvar_t           server_customclient;
extern vmCvar_t           server_fastrail;
extern vmCvar_t           server_freezetag;
extern vmCvar_t           server_gfxspecfile;
extern vmCvar_t           server_lgcooldown;
extern vmCvar_t           server_maxpacketsmin;
extern vmCvar_t           server_maxpacketsmax;
extern vmCvar_t           server_motdfile;
extern vmCvar_t           server_motd1;
extern vmCvar_t           server_motd2;
extern vmCvar_t           server_motd3;
extern vmCvar_t           server_motd4;
extern vmCvar_t           server_motd5;
extern vmCvar_t           server_motd6;
extern vmCvar_t           server_motd7;
extern vmCvar_t           server_motd8;
extern vmCvar_t           server_ospauth;
extern vmCvar_t           server_realping;
extern vmCvar_t           server_record;
extern vmCvar_t           server_thrufloors;
extern vmCvar_t           server_timenudgemax;
extern vmCvar_t           server_timenudgemin;
extern vmCvar_t           start_armor;
extern vmCvar_t           start_bfg;
extern vmCvar_t           start_bullets;
extern vmCvar_t           start_cells;
extern vmCvar_t           start_grenades;
extern vmCvar_t           start_health;
extern vmCvar_t           start_lightning;
extern vmCvar_t           start_rockets;
extern vmCvar_t           start_shells;
extern vmCvar_t           start_slugs;
extern vmCvar_t           team_allcaptain;
extern vmCvar_t           team_armordamage;
extern vmCvar_t           team_maxplayers;
extern vmCvar_t           team_nocontrols;
extern vmCvar_t           team_noteamready;
extern vmCvar_t           viewcam_directory;
extern vmCvar_t           vote_allow_allcaptain;
extern vmCvar_t           vote_allow_altgraphics;
extern vmCvar_t           vote_allow_armordamage;
extern vmCvar_t           vote_allow_capfps;
extern vmCvar_t           vote_allow_capturelimit;
extern vmCvar_t           vote_allow_ca_startrespawn;
extern vmCvar_t           vote_allow_ca_warmupfire;
extern vmCvar_t           vote_allow_cq3;
extern vmCvar_t           vote_allow_dropitems;
extern vmCvar_t           vote_allow_falling;
extern vmCvar_t           vote_allow_fastrail;
extern vmCvar_t           vote_allow_fraglimit;
extern vmCvar_t           vote_allow_hook;
extern vmCvar_t           vote_allow_items;
extern vmCvar_t           vote_allow_instagib;
extern vmCvar_t           vote_allow_kick;
extern vmCvar_t           vote_allow_lgcooldown;
extern vmCvar_t           vote_allow_map;
extern vmCvar_t           vote_allow_maxpackets;
extern vmCvar_t           vote_allow_machinegun;
extern vmCvar_t           vote_allow_mutespecs;
extern vmCvar_t           vote_allow_notimer;
extern vmCvar_t           vote_allow_osphud;
extern vmCvar_t           vote_allow_overtime;
extern vmCvar_t           vote_allow_pmove;
extern vmCvar_t           vote_allow_poweruprespawn;
extern vmCvar_t           vote_allow_powerups;
extern vmCvar_t           vote_allow_promode;
extern vmCvar_t           vote_allow_referee;
extern vmCvar_t           vote_allow_reload;
extern vmCvar_t           vote_allow_respawn;
extern vmCvar_t           vote_allow_roundlimit;
extern vmCvar_t           vote_allow_teamdamage;
extern vmCvar_t           vote_allow_thrufloors;
extern vmCvar_t           vote_allow_timenudge;
extern vmCvar_t           vote_allow_timelimit;
extern vmCvar_t           vote_allow_weaponrespawn;
extern vmCvar_t           vote_limit;
extern vmCvar_t           vote_percent;
extern vmCvar_t           warmup_armor;
extern vmCvar_t           weapon_deaddrop;
extern vmCvar_t           weapon_have;
extern vmCvar_t           weapon_start;

void    trap_Printf(const char* fmt);
void    trap_Error(const char* fmt);
int     trap_Milliseconds(void);
int     trap_Argc(void);
void    trap_Argv(int n, char* buffer, int bufferLength);
void    trap_Args(char* buffer, int bufferLength);
int     trap_FS_FOpenFile(const char* qpath, fileHandle_t* f, fsMode_t mode);
void    trap_FS_Read(void* buffer, int len, fileHandle_t f);
void    trap_FS_Write(const void* buffer, int len, fileHandle_t f);
void    trap_FS_FCloseFile(fileHandle_t f);
int     trap_FS_GetFileList(const char* path, const char* extension, char* listbuf, int bufsize);
int     trap_FS_Seek(fileHandle_t f, long offset, int origin);   // fsOrigin_t
void    trap_SendConsoleCommand(int exec_when, const char* text);
void    trap_Cvar_Register(vmCvar_t* cvar, const char* var_name, const char* value, int flags);
void    trap_Cvar_Update(vmCvar_t* cvar);
void    trap_Cvar_Set(const char* var_name, const char* value);
int     trap_Cvar_VariableIntegerValue(const char* var_name);
float   trap_Cvar_VariableValue(const char* var_name);
void    trap_Cvar_VariableStringBuffer(const char* var_name, char* buffer, int bufsize);
void    trap_LocateGameData(gentity_t* gEnts, int numGEntities, int sizeofGEntity_t, playerState_t* gameClients, int sizeofGameClient);
void    trap_DropClient(int clientNum, const char* reason);
void    trap_SendServerCommand(int clientNum, const char* text);
void    trap_SetConfigstring(int num, const char* string);
void    trap_GetConfigstring(int num, char* buffer, int bufferSize);
void    trap_GetUserinfo(int num, char* buffer, int bufferSize);
void    trap_SetUserinfo(int num, const char* buffer);
void    trap_GetServerinfo(char* buffer, int bufferSize);
void    trap_SetBrushModel(gentity_t* ent, const char* name);
void    trap_Trace(trace_t* results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask);
int     trap_PointContents(const vec3_t point, int passEntityNum);
qboolean trap_InPVS(const vec3_t p1, const vec3_t p2);
qboolean trap_InPVSIgnorePortals(const vec3_t p1, const vec3_t p2);
void    trap_AdjustAreaPortalState(gentity_t* ent, qboolean open);
qboolean trap_AreasConnected(int area1, int area2);
void    trap_LinkEntity(gentity_t* ent);
void    trap_UnlinkEntity(gentity_t* ent);
int     trap_EntitiesInBox(const vec3_t mins, const vec3_t maxs, int* entityList, int maxcount);
qboolean trap_EntityContact(const vec3_t mins, const vec3_t maxs, const gentity_t* ent);
int     trap_BotAllocateClient(void);
void    trap_BotFreeClient(int clientNum);
void    trap_GetUsercmd(int clientNum, usercmd_t* cmd);
qboolean    trap_GetEntityToken(char* buffer, int bufferSize);

int     trap_DebugPolygonCreate(int color, int numPoints, vec3_t* points);
void    trap_DebugPolygonDelete(int id);

int     trap_BotLibSetup(void);
int     trap_BotLibShutdown(void);
int     trap_BotLibVarSet(char* var_name, char* value);
int     trap_BotLibVarGet(char* var_name, char* value, int size);
int     trap_BotLibDefine(char* string);
int     trap_BotLibStartFrame(float time);
int     trap_BotLibLoadMap(const char* mapname);
int     trap_BotLibUpdateEntity(int ent, void /* struct bot_updateentity_s */ *bue);
int     trap_BotLibTest(int parm0, char* parm1, vec3_t parm2, vec3_t parm3);

int     trap_BotGetSnapshotEntity(int clientNum, int sequence);
int     trap_BotGetServerCommand(int clientNum, char* message, int size);
void    trap_BotUserCommand(int client, usercmd_t* ucmd);

int     trap_AAS_BBoxAreas(vec3_t absmins, vec3_t absmaxs, int* areas, int maxareas);
int     trap_AAS_AreaInfo(int areanum, void /* struct aas_areainfo_s */ *info);
void    trap_AAS_EntityInfo(int entnum, void /* struct aas_entityinfo_s */ *info);

int     trap_AAS_Initialized(void);
void    trap_AAS_PresenceTypeBoundingBox(int presencetype, vec3_t mins, vec3_t maxs);
float   trap_AAS_Time(void);

int     trap_AAS_PointAreaNum(vec3_t point);
int     trap_AAS_PointReachabilityAreaIndex(vec3_t point);
int     trap_AAS_TraceAreas(vec3_t start, vec3_t end, int* areas, vec3_t* points, int maxareas);

int     trap_AAS_PointContents(vec3_t point);
int     trap_AAS_NextBSPEntity(int ent);
int     trap_AAS_ValueForBSPEpairKey(int ent, char* key, char* value, int size);
int     trap_AAS_VectorForBSPEpairKey(int ent, char* key, vec3_t v);
int     trap_AAS_FloatForBSPEpairKey(int ent, char* key, float* value);
int     trap_AAS_IntForBSPEpairKey(int ent, char* key, int* value);

int     trap_AAS_AreaReachability(int areanum);

int     trap_AAS_AreaTravelTimeToGoalArea(int areanum, vec3_t origin, int goalareanum, int travelflags);
int     trap_AAS_EnableRoutingArea(int areanum, int enable);
int     trap_AAS_PredictRoute(void /*struct aas_predictroute_s*/ *route, int areanum, vec3_t origin,
                              int goalareanum, int travelflags, int maxareas, int maxtime,
                              int stopevent, int stopcontents, int stoptfl, int stopareanum);

int     trap_AAS_AlternativeRouteGoals(vec3_t start, int startareanum, vec3_t goal, int goalareanum, int travelflags,
                                       void /*struct aas_altroutegoal_s*/ *altroutegoals, int maxaltroutegoals,
                                       int type);
int     trap_AAS_Swimming(vec3_t origin);
int     trap_AAS_PredictClientMovement(void /* aas_clientmove_s */ *move, int entnum, vec3_t origin, int presencetype, int onground, vec3_t velocity, vec3_t cmdmove, int cmdframes, int maxframes, float frametime, int stopevent, int stopareanum, int visualize);


void    trap_EA_Say(int client, char* str);
void    trap_EA_SayTeam(int client, char* str);
void    trap_EA_Command(int client, char* command);

void    trap_EA_Action(int client, int action);
void    trap_EA_Gesture(int client);
void    trap_EA_Talk(int client);
void    trap_EA_Attack(int client);
void    trap_EA_Use(int client);
void    trap_EA_Respawn(int client);
void    trap_EA_Crouch(int client);
void    trap_EA_MoveUp(int client);
void    trap_EA_MoveDown(int client);
void    trap_EA_MoveForward(int client);
void    trap_EA_MoveBack(int client);
void    trap_EA_MoveLeft(int client);
void    trap_EA_MoveRight(int client);
void    trap_EA_SelectWeapon(int client, int weapon);
void    trap_EA_Jump(int client);
void    trap_EA_DelayedJump(int client);
void    trap_EA_Move(int client, vec3_t dir, float speed);
void    trap_EA_View(int client, vec3_t viewangles);

void    trap_EA_EndRegular(int client, float thinktime);
void    trap_EA_GetInput(int client, float thinktime, void /* struct bot_input_s */ *input);
void    trap_EA_ResetInput(int client);


int     trap_BotLoadCharacter(char* charfile, float skill);
void    trap_BotFreeCharacter(int character);
float   trap_Characteristic_Float(int character, int index);
float   trap_Characteristic_BFloat(int character, int index, float min, float max);
int     trap_Characteristic_Integer(int character, int index);
int     trap_Characteristic_BInteger(int character, int index, int min, int max);
void    trap_Characteristic_String(int character, int index, char* buf, int size);

int     trap_BotAllocChatState(void);
void    trap_BotFreeChatState(int handle);
void    trap_BotQueueConsoleMessage(int chatstate, int type, char* message);
void    trap_BotRemoveConsoleMessage(int chatstate, int handle);
int     trap_BotNextConsoleMessage(int chatstate, void /* struct bot_consolemessage_s */ *cm);
int     trap_BotNumConsoleMessages(int chatstate);
void    trap_BotInitialChat(int chatstate, char* type, int mcontext, char* var0, char* var1, char* var2, char* var3, char* var4, char* var5, char* var6, char* var7);
int     trap_BotNumInitialChats(int chatstate, char* type);
int     trap_BotReplyChat(int chatstate, char* message, int mcontext, int vcontext, char* var0, char* var1, char* var2, char* var3, char* var4, char* var5, char* var6, char* var7);
int     trap_BotChatLength(int chatstate);
void    trap_BotEnterChat(int chatstate, int client, int sendto);
void    trap_BotGetChatMessage(int chatstate, char* buf, int size);
int     trap_StringContains(char* str1, char* str2, int casesensitive);
int     trap_BotFindMatch(char* str, void /* struct bot_match_s */ *match, unsigned long int context);
void    trap_BotMatchVariable(void /* struct bot_match_s */ *match, int variable, char* buf, int size);
void    trap_UnifyWhiteSpaces(char* string);
void    trap_BotReplaceSynonyms(char* string, unsigned long int context);
int     trap_BotLoadChatFile(int chatstate, char* chatfile, char* chatname);
void    trap_BotSetChatGender(int chatstate, int gender);
void    trap_BotSetChatName(int chatstate, char* name, int client);
void    trap_BotResetGoalState(int goalstate);
void    trap_BotRemoveFromAvoidGoals(int goalstate, int number);
void    trap_BotResetAvoidGoals(int goalstate);
void    trap_BotPushGoal(int goalstate, void /* struct bot_goal_s */ *goal);
void    trap_BotPopGoal(int goalstate);
void    trap_BotEmptyGoalStack(int goalstate);
void    trap_BotDumpAvoidGoals(int goalstate);
void    trap_BotDumpGoalStack(int goalstate);
void    trap_BotGoalName(int number, char* name, int size);
int     trap_BotGetTopGoal(int goalstate, void /* struct bot_goal_s */ *goal);
int     trap_BotGetSecondGoal(int goalstate, void /* struct bot_goal_s */ *goal);
int     trap_BotChooseLTGItem(int goalstate, vec3_t origin, int* inventory, int travelflags);
int     trap_BotChooseNBGItem(int goalstate, vec3_t origin, int* inventory, int travelflags, void /* struct bot_goal_s */ *ltg, float maxtime);
int     trap_BotTouchingGoal(vec3_t origin, void /* struct bot_goal_s */ *goal);
int     trap_BotItemGoalInVisButNotVisible(int viewer, vec3_t eye, vec3_t viewangles, void /* struct bot_goal_s */ *goal);
int     trap_BotGetNextCampSpotGoal(int num, void /* struct bot_goal_s */ *goal);
int     trap_BotGetMapLocationGoal(char* name, void /* struct bot_goal_s */ *goal);
int     trap_BotGetLevelItemGoal(int index, char* classname, void /* struct bot_goal_s */ *goal);
float   trap_BotAvoidGoalTime(int goalstate, int number);
void    trap_BotSetAvoidGoalTime(int goalstate, int number, float avoidtime);
void    trap_BotInitLevelItems(void);
void    trap_BotUpdateEntityItems(void);
int     trap_BotLoadItemWeights(int goalstate, char* filename);
void    trap_BotFreeItemWeights(int goalstate);
void    trap_BotInterbreedGoalFuzzyLogic(int parent1, int parent2, int child);
void    trap_BotSaveGoalFuzzyLogic(int goalstate, char* filename);
void    trap_BotMutateGoalFuzzyLogic(int goalstate, float range);
int     trap_BotAllocGoalState(int state);
void    trap_BotFreeGoalState(int handle);

void    trap_BotResetMoveState(int movestate);
void    trap_BotMoveToGoal(void /* struct bot_moveresult_s */ *result, int movestate, void /* struct bot_goal_s */ *goal, int travelflags);
int     trap_BotMoveInDirection(int movestate, vec3_t dir, float speed, int type);
void    trap_BotResetAvoidReach(int movestate);
void    trap_BotResetLastAvoidReach(int movestate);
int     trap_BotReachabilityArea(vec3_t origin, int testground);
int     trap_BotMovementViewTarget(int movestate, void /* struct bot_goal_s */ *goal, int travelflags, float lookahead, vec3_t target);
int     trap_BotPredictVisiblePosition(vec3_t origin, int areanum, void /* struct bot_goal_s */ *goal, int travelflags, vec3_t target);
int     trap_BotAllocMoveState(void);
void    trap_BotFreeMoveState(int handle);
void    trap_BotInitMoveState(int handle, void /* struct bot_initmove_s */ *initmove);
void    trap_BotAddAvoidSpot(int movestate, vec3_t origin, float radius, int type);

int     trap_BotChooseBestFightWeapon(int weaponstate, int* inventory);
void    trap_BotGetWeaponInfo(int weaponstate, int weapon, void /* struct weaponinfo_s */ *weaponinfo);
int     trap_BotLoadWeaponWeights(int weaponstate, char* filename);
int     trap_BotAllocWeaponState(void);
void    trap_BotFreeWeaponState(int weaponstate);
void    trap_BotResetWeaponState(int weaponstate);

int     trap_GeneticParentsAndChildSelection(int numranks, float* ranks, int* parent1, int* parent2, int* child);

void    trap_SnapVector(float* v);

int     trap_RealTime(qtime_t* qtime);

/*--------debug stuff-------------*/
#define OSP_UNK_CODE(MSG) G_Printf( "^1%s:%d: unknown code: %s\n", __FILE__, __LINE__, MSG)


// UNIMPLEMENTED CODE
//=============================================
void G_OSPAuth(const gentity_t* ent);
void Cmd_Uinfo_f(const gentity_t* ent);
void Cmd_ViewList_f(const gentity_t* ent);
qboolean Cmd_Ref_f(const gentity_t* ent, int arg);
void g_unk_369d2(const gentity_t* ent, int arg);
void Cmd_PlayerList_f(const gentity_t* ent);
void Cmd_PlayerList_f(const gentity_t* ent);
void Cmd_GetStatsInfo_f(const gentity_t* ent);
void Cmd_Stats_f(const gentity_t* ent, int arg);
void Cmd_StatsAll_f(const gentity_t* ent, int arg);
void Cmd_Acc_f(const gentity_t* ent, int arg);
void Cmd_OSPAuth_f(const gentity_t* ent);
void Cmd_AutoScreenshot_f(int arg1, int arg2, int arg3);
void Cmd_ScoresFT_f(const gentity_t* ent, int arg);
void Cmd_ScoresDM_f(const gentity_t* ent, int arg);
void Cmd_ScoresCTF_f(const gentity_t* ent, int arg);
void Cmd_ScoresCA_f(const gentity_t* ent, int arg);
void Cmd_ScoresFFA_f(const gentity_t* ent, int arg);
qboolean g_unk_3660b(const gentity_t* ent, char* cmd, int arg);
void Cmd_CallVote2_f(const gentity_t* ent, int arg);

/*--------------------------------*/

