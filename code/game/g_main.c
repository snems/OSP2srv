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

#include "g_local.h"
#include "q_shared.h"
#include "g_unimplemented.h"

level_locals_t  level;

#define MAXPACKETSMIN 15
#define MAXPACKETSMAX 125
#define TIMENUDGEMIN -200
#define TIMENUDGEMAX 200

typedef struct
{
	vmCvar_t*   vmCvar;
	char*       cvarName;
	char*       defaultString;
	int         cvarFlags;
	int         modificationCount;  // for tracking changes
	qboolean    trackChange;        // track this variable, and announce if changed
	qboolean    teamShader;         // track and if changed, update shader state
	qboolean    unk;
} cvarTable_t;

gentity_t       g_entities[MAX_GENTITIES];
gclient_t       g_clients[MAX_CLIENTS];

vmCvar_t           g_cheats;
vmCvar_t           osp_gamename;
vmCvar_t           gamename;
vmCvar_t           gamedate;
vmCvar_t           g_restarted;
vmCvar_t           sv_mapname;
vmCvar_t           g_gametype;
vmCvar_t           g_maxclients;
vmCvar_t           g_maxGameClients;
vmCvar_t           gameversion;
vmCvar_t           mapsessionauto;
vmCvar_t           mapsessioncount;
vmCvar_t           ca_session;
vmCvar_t           Players_Red;
vmCvar_t           Players_Blue;
vmCvar_t           Players_Active;
vmCvar_t           Score_Red;
vmCvar_t           Score_Blue;
vmCvar_t           default_change;
vmCvar_t           default_settings;
vmCvar_t           Score_Time;
vmCvar_t           server_promode;
vmCvar_t           server_cq3;
vmCvar_t           g_allowVote;
vmCvar_t           g_banIPs;
vmCvar_t           g_blood;
vmCvar_t           g_capturelimit;
vmCvar_t           g_debugAlloc;
vmCvar_t           g_debugDamage;
vmCvar_t           g_debugMove;
vmCvar_t           g_dedicated;
vmCvar_t           g_dmflags;
vmCvar_t           g_doWarmup;
vmCvar_t           g_filterBan;
vmCvar_t           g_forcerespawn;
vmCvar_t           g_friendlyFire;
vmCvar_t           g_fraglimit;
vmCvar_t           g_gravity;
vmCvar_t           g_inactivity;
vmCvar_t           g_knockback;
vmCvar_t           g_listEntity;
vmCvar_t           g_log;
vmCvar_t           g_logSync;
vmCvar_t           g_motd;
vmCvar_t           g_needpass;
vmCvar_t           g_password;
vmCvar_t           g_podiumDist;
vmCvar_t           g_podiumDrop;
vmCvar_t           g_quadfactor;
vmCvar_t           g_rankings;
vmCvar_t           g_spectatorInactivity;
vmCvar_t           g_speed;
vmCvar_t           g_synchronousClients;
vmCvar_t           g_teamAutoJoin;
vmCvar_t           g_teamForceBalance;
vmCvar_t           g_timelimit;
vmCvar_t           g_warmup;
vmCvar_t           g_weaponRespawn;
vmCvar_t           g_weaponTeamRespawn;
vmCvar_t           g_smoothClients;
vmCvar_t           pmove_fixed;
vmCvar_t           pmove_msec;
vmCvar_t           osp_restart;
vmCvar_t           roundlimit;
vmCvar_t           sv_dcount;
vmCvar_t           sv_hostname;
vmCvar_t           admin_log;
vmCvar_t           armor_q2style;
vmCvar_t           bot_checktime;
vmCvar_t           bot_autoload;
vmCvar_t           decal_directory;
vmCvar_t           filter_file;
vmCvar_t           freezetag_latejoin;
vmCvar_t           freezetag_respawn;
vmCvar_t           gauntlet_damage;
vmCvar_t           gauntlet_disable;
vmCvar_t           hook_delaytime;
vmCvar_t           hook_enable;
vmCvar_t           hook_holdtime;
vmCvar_t           hook_sky;
vmCvar_t           hook_speed;
vmCvar_t           hook_speedpull;
vmCvar_t           instagib_reload;
vmCvar_t           item_disable;
vmCvar_t           log_default;
vmCvar_t           log_stat;
vmCvar_t           map_autoload;
vmCvar_t           map_debug;
vmCvar_t           map_file;
vmCvar_t           map_halt;
vmCvar_t           map_nocount;
vmCvar_t           map_once;
vmCvar_t           map_queue;
vmCvar_t           map_random;
vmCvar_t           map_rotate;
vmCvar_t           match_carespawn;
vmCvar_t           match_cawarmupfire;
vmCvar_t           match_dropitems;
vmCvar_t           match_hurtself;
vmCvar_t           match_instagib;
vmCvar_t           match_latejoin;
vmCvar_t           match_maxsuicides;
vmCvar_t           match_minplayers;
vmCvar_t           match_mutespecs;
vmCvar_t           match_overtime;
vmCvar_t           match_overtimecount;
vmCvar_t           match_overtimelength;
vmCvar_t           match_poweruprespawn;
vmCvar_t           match_readypercent;
vmCvar_t           match_roundtimer;
vmCvar_t           match_timeoutlength;
vmCvar_t           match_timeoutcount;
vmCvar_t           match_winbyscore;
vmCvar_t           mg_damage;
vmCvar_t           mg_damageTeamplay;
vmCvar_t           z_m_current;
vmCvar_t           mode_customfile;
vmCvar_t           z_m_default;
vmCvar_t           mode_start;
vmCvar_t           player_always;
vmCvar_t           player_file;
vmCvar_t           powerup_disable;
vmCvar_t           ref_password;
vmCvar_t           railgun_damage;
vmCvar_t           replace_machinegun;
vmCvar_t           replace_shotgun;
vmCvar_t           replace_glauncher;
vmCvar_t           replace_rlauncher;
vmCvar_t           replace_lightning;
vmCvar_t           replace_railgun;
vmCvar_t           replace_plasmagun;
vmCvar_t           replace_bfg;
vmCvar_t           replace_invis;
vmCvar_t           replace_quad;
vmCvar_t           replace_battlesuit;
vmCvar_t           replace_haste;
vmCvar_t           replace_regen;
vmCvar_t           replace_flight;
vmCvar_t           server_availmodes;
vmCvar_t           server_batcher;
vmCvar_t           server_customclient;
vmCvar_t           server_fastrail;
vmCvar_t           server_freezetag;
vmCvar_t           server_gfxspecfile;
vmCvar_t           server_lgcooldown;
vmCvar_t           server_maxpacketsmin;
vmCvar_t           server_maxpacketsmax;
vmCvar_t           server_motdfile;
vmCvar_t           server_motd1;
vmCvar_t           server_motd2;
vmCvar_t           server_motd3;
vmCvar_t           server_motd4;
vmCvar_t           server_motd5;
vmCvar_t           server_motd6;
vmCvar_t           server_motd7;
vmCvar_t           server_motd8;
vmCvar_t           server_ospauth;
vmCvar_t           server_realping;
vmCvar_t           server_record;
vmCvar_t           server_thrufloors;
vmCvar_t           server_timenudgemax;
vmCvar_t           server_timenudgemin;
vmCvar_t           start_armor;
vmCvar_t           start_bfg;
vmCvar_t           start_bullets;
vmCvar_t           start_cells;
vmCvar_t           start_grenades;
vmCvar_t           start_health;
vmCvar_t           start_lightning;
vmCvar_t           start_rockets;
vmCvar_t           start_shells;
vmCvar_t           start_slugs;
vmCvar_t           team_allcaptain;
vmCvar_t           team_armordamage;
vmCvar_t           team_maxplayers;
vmCvar_t           team_nocontrols;
vmCvar_t           team_noteamready;
vmCvar_t           viewcam_directory;
vmCvar_t           vote_allow_allcaptain;
vmCvar_t           vote_allow_altgraphics;
vmCvar_t           vote_allow_armordamage;
vmCvar_t           vote_allow_capfps;
vmCvar_t           vote_allow_capturelimit;
vmCvar_t           vote_allow_ca_startrespawn;
vmCvar_t           vote_allow_ca_warmupfire;
vmCvar_t           vote_allow_cq3;
vmCvar_t           vote_allow_dropitems;
vmCvar_t           vote_allow_falling;
vmCvar_t           vote_allow_fastrail;
vmCvar_t           vote_allow_fraglimit;
vmCvar_t           vote_allow_hook;
vmCvar_t           vote_allow_items;
vmCvar_t           vote_allow_instagib;
vmCvar_t           vote_allow_kick;
vmCvar_t           vote_allow_lgcooldown;
vmCvar_t           vote_allow_map;
vmCvar_t           vote_allow_maxpackets;
vmCvar_t           vote_allow_machinegun;
vmCvar_t           vote_allow_mutespecs;
vmCvar_t           vote_allow_notimer;
vmCvar_t           vote_allow_osphud;
vmCvar_t           vote_allow_overtime;
vmCvar_t           vote_allow_pmove;
vmCvar_t           vote_allow_poweruprespawn;
vmCvar_t           vote_allow_powerups;
vmCvar_t           vote_allow_promode;
vmCvar_t           vote_allow_referee;
vmCvar_t           vote_allow_reload;
vmCvar_t           vote_allow_respawn;
vmCvar_t           vote_allow_roundlimit;
vmCvar_t           vote_allow_teamdamage;
vmCvar_t           vote_allow_thrufloors;
vmCvar_t           vote_allow_timenudge;
vmCvar_t           vote_allow_timelimit;
vmCvar_t           vote_allow_weaponrespawn;
vmCvar_t           vote_limit;
vmCvar_t           vote_percent;
vmCvar_t           warmup_armor;
vmCvar_t           weapon_deaddrop;
vmCvar_t           weapon_have;
vmCvar_t           weapon_start;


// bk001129 - made static to avoid aliasing
static cvarTable_t      gameCvarTable[] =
{
	{ &g_cheats, "sv_cheats", "", 0, qfalse, qfalse, qfalse },
	{ &osp_gamename, "osp_gamename", "1", 0, qfalse, qfalse, qfalse },
	{ &gamename, "gamename", "osp", CVAR_SERVERINFO | CVAR_ROM, qfalse, qfalse, qfalse },
	{ &gamedate, "gamedate", "Mar 04 2003", CVAR_ROM, qfalse, qfalse, qfalse },
	{ &g_restarted, "g_restarted", "0", CVAR_ROM, qfalse, qfalse, qfalse },
	{ &sv_mapname, "sv_mapname", "", CVAR_ROM, qfalse, qfalse, qfalse },
	{ &g_gametype, "g_gametype", "0", CVAR_SERVERINFO | CVAR_LATCH, qfalse, qfalse, qfalse },
	{ &g_maxclients, "sv_maxclients", "8", CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_LATCH, qfalse, qfalse, qfalse },
	{ &g_maxGameClients, "g_maxGameClients", "0", CVAR_ARCHIVE | CVAR_LATCH, qfalse, qfalse, qfalse },
	{ &gameversion, "gameversion", "OSP v1.03a", CVAR_SERVERINFO | CVAR_INIT | CVAR_ROM, qfalse, qfalse, qfalse },
	{ &mapsessionauto, "mapsessionauto", "-1", 0, qfalse, qfalse, qfalse },
	{ &mapsessioncount, "mapsessioncount", "none", 0, qfalse, qfalse, qfalse },
	{ &ca_session, "ca_session", "0 0 0", 0, qfalse, qtrue, qtrue },
	{ &Players_Red, "Players_Red", "", CVAR_SERVERINFO, qfalse, qtrue, qtrue },
	{ &Players_Blue, "Players_Blue", "", CVAR_SERVERINFO, qfalse, qtrue, qtrue },
	{ &Players_Active, "Players_Active", "", CVAR_SERVERINFO, qfalse, qtrue, qtrue },
	{ &Score_Red, "Score_Red", "", CVAR_SERVERINFO, qfalse, qtrue, qtrue },
	{ &Score_Blue, "Score_Blue", "", CVAR_SERVERINFO, qfalse, qtrue, qtrue },
	{ &default_change, "default_change", "0", 0, qfalse, qtrue, qtrue },
	{ &default_settings, "default_settings", "none", 0, qfalse, qtrue, qtrue },
	{ &Score_Time, "Score_Time", "", CVAR_SERVERINFO, qfalse, qtrue, qtrue },
	{ &server_promode, "server_promode", "0", CVAR_SERVERINFO, qfalse, qfalse, qfalse },
	{ &server_cq3, "server_cq3", "0", 0, qfalse, qfalse, qfalse },
	{ &g_allowVote, "g_allowVote", "1", 0, qfalse, qtrue, qtrue },
	{ &g_banIPs, "g_banIPs", "", CVAR_ARCHIVE, qfalse, qfalse, qfalse },
	{ &g_blood, "g_blood", "1", 0, qfalse, qfalse, qfalse },
	{ &g_capturelimit, "capturelimit", "8", CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_NORESTART, qfalse, qtrue, qtrue },
	{ &g_debugAlloc, "g_debugAlloc", "0", 0, qfalse, qfalse, qfalse },
	{ &g_debugDamage, "g_debugDamage", "0", 0, qfalse, qfalse, qfalse },
	{ &g_debugMove, "g_debugMove", "0", 0, qfalse, qfalse, qfalse },
	{ &g_dedicated, "dedicated", "0", 0, qfalse, qfalse, qfalse },
	{ &g_dmflags, "dmflags", "0", CVAR_ARCHIVE | CVAR_SERVERINFO, qfalse, qtrue, qtrue },
	{ &g_doWarmup, "g_doWarmup", "1", CVAR_ARCHIVE, qtrue, qtrue, qtrue },
	{ &g_filterBan, "g_filterBan", "1", CVAR_ARCHIVE, qfalse, qfalse, qfalse },
	{ &g_forcerespawn, "g_forcerespawn", "20", 0, qfalse, qtrue, qtrue },
	{ &g_friendlyFire, "g_friendlyFire", "1", CVAR_ARCHIVE, qfalse, qtrue, qtrue },
	{ &g_fraglimit, "fraglimit", "20", CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_NORESTART, qfalse, qtrue, qtrue },
	{ &g_gravity, "g_gravity", "800", 0, qtrue, qtrue, qtrue },
	{ &g_inactivity, "g_inactivity", "0", 0, qtrue, qfalse, qfalse },
	{ &g_knockback, "g_knockback", "1000", 0, qtrue, qfalse, qfalse },
	{ &g_listEntity, "g_listEntity", "0", 0, qfalse, qfalse, qfalse },
	{ &g_log, "g_log", "games.log", CVAR_ARCHIVE, qfalse, qfalse, qfalse },
	{ &g_logSync, "g_logSync", "0", CVAR_ARCHIVE, qfalse, qfalse, qfalse },
	{ &g_motd, "g_motd", "", 0, qfalse, qfalse, qfalse },
	{ &g_needpass, "g_needpass", "0", CVAR_SERVERINFO | CVAR_ROM, qfalse, qfalse, qfalse },
	{ &g_password, "g_password", "none", CVAR_USERINFO, qfalse, qfalse, qfalse },
	{ &g_podiumDist, "g_podiumDist", "80", 0, qfalse, qfalse, qfalse },
	{ &g_podiumDrop, "g_podiumDrop", "70", 0, qfalse, qfalse, qfalse },
	{ &g_quadfactor, "g_quadfactor", "3", 0, qfalse, qtrue, qtrue },
	{ &g_rankings, "g_rankings", "0", 0, qfalse, qfalse, qfalse },
	{ &g_spectatorInactivity, "g_spectatorInactivity", "0", 0, qtrue, qfalse, qfalse },
	{ &g_speed, "g_speed", "320", 0, qtrue, qfalse, qfalse },
	{ &g_synchronousClients, "g_synchronousClients", "0", CVAR_SYSTEMINFO, qfalse, qfalse, qfalse },
	{ &g_teamAutoJoin, "g_teamAutoJoin", "0", CVAR_ARCHIVE, qfalse, qfalse, qfalse },
	{ &g_teamForceBalance, "g_teamForceBalance", "0", CVAR_ARCHIVE, qfalse, qfalse, qfalse },
	{ &g_timelimit, "timelimit", "0", CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_NORESTART, qfalse, qtrue, qtrue },
	{ &g_warmup, "g_warmup", "20", CVAR_ARCHIVE, qtrue, qtrue, qtrue },
	{ &g_weaponRespawn, "g_weaponrespawn", "5", 0, qfalse, qtrue, qtrue },
	{ &g_weaponTeamRespawn, "g_weaponteamrespawn", "30", 0, qfalse, qtrue, qtrue },
	{ &g_smoothClients, "g_smoothClients", "0", 0, qfalse, qtrue, qtrue },
	{ &pmove_fixed, "pmove_fixed", "0", CVAR_SYSTEMINFO, qfalse, qtrue, qtrue },
	{ &pmove_msec, "pmove_msec", "8", CVAR_SYSTEMINFO, qfalse, qtrue, qtrue },
	{ &osp_restart, "osp_restart", "0", 0, qfalse, qfalse, qfalse },
	{ &roundlimit, "roundlimit", "5", 0, qfalse, qtrue, qtrue },
	{ &sv_dcount, "sv_dcount", "0", 0, qfalse, qtrue, qtrue },
	{ &sv_hostname, "sv_hostname", "noname", 0, qfalse, qfalse, qfalse },
	{ &admin_log, "admin_log", "none", 0, qfalse, qfalse, qfalse },
	{ &armor_q2style, "armor_q2style", "0", 0, qfalse, qtrue, qtrue },
	{ &bot_checktime, "bot_checktime", "10", 0, qfalse, qfalse, qfalse },
	{ &bot_autoload, "bot_autoload", "[none]", 0, qfalse, qfalse, qfalse },
	{ &decal_directory, "decal_directory", "decals", 0, qfalse, qfalse, qfalse },
	{ &filter_file, "filter_file", "filters.txt", 0, qfalse, qfalse, qfalse },
	{ &freezetag_latejoin, "freezetag_latejoin", "0", 0, qfalse, qtrue, qtrue },
	{ &freezetag_respawn, "freezetag_respawn", "8.0", 0, qfalse, qtrue, qtrue },
	{ &gauntlet_damage, "gauntlet_damage", "50", 0, qfalse, qtrue, qtrue },
	{ &gauntlet_disable, "gauntlet_disable", "0", 0, qfalse, qtrue, qtrue },
	{ &hook_delaytime, "hook_delaytime", "750", 0, qfalse, qfalse, qfalse },
	{ &hook_enable, "hook_enable", "0", 0, qfalse, qtrue, qtrue },
	{ &hook_holdtime, "hook_holdtime", "3", 0, qfalse, qfalse, qfalse },
	{ &hook_sky, "hook_sky", "0", 0, qfalse, qfalse, qfalse },
	{ &hook_speed, "hook_speed", "1200", 0, qfalse, qfalse, qfalse },
	{ &hook_speedpull, "hook_speedpull", "950", 0, qfalse, qfalse, qfalse },
	{ &instagib_reload, "instagib_reload", "1500", 0, qtrue, qfalse, qfalse },
	{ &item_disable, "item_disable", "0", 0, qfalse, qtrue, qtrue },
	{ &log_default, "log_default", "0", 0, qfalse, qfalse, qfalse },
	{ &log_stat, "log_stat", "0", 0, qfalse, qfalse, qfalse },
	{ &map_autoload, "map_autoload", "1", 0, qfalse, qfalse, qfalse },
	{ &map_debug, "map_debug", "0", 0, qfalse, qfalse, qfalse },
	{ &map_file, "map_file", "maps.txt", 0, qfalse, qfalse, qfalse },
	{ &map_halt, "map_halt", "0", 0, qfalse, qfalse, qfalse },
	{ &map_nocount, "map_nocount", "0", 0, qfalse, qfalse, qfalse },
	{ &map_once, "map_once", "1", 0, qfalse, qtrue, qtrue },
	{ &map_queue, "map_queue", "1", 0, qfalse, qtrue, qtrue },
	{ &map_random, "map_random", "1", 0, qfalse, qfalse, qfalse },
	{ &map_rotate, "map_rotate", "0", 0, qfalse, qfalse, qfalse },
	{ &match_carespawn, "match_carespawn", "0", 0, qfalse, qtrue, qtrue },
	{ &match_cawarmupfire, "match_cawarmupfire", "1", 0, qfalse, qtrue, qtrue },
	{ &match_dropitems, "match_dropitems", "0", 0, qfalse, qtrue, qtrue },
	{ &match_hurtself, "match_hurtself", "1", 0, qfalse, qtrue, qtrue },
	{ &match_instagib, "match_instagib", "0", 0, qfalse, qfalse, qfalse },
	{ &match_latejoin, "match_latejoin", "0", 0, qfalse, qtrue, qtrue },
	{ &match_maxsuicides, "match_maxsuicides", "0", 0, qfalse, qtrue, qtrue },
	{ &match_minplayers, "match_minplayers", "2", 0, qfalse, qtrue, qtrue },
	{ &match_mutespecs, "match_mutespecs", "0", 0, qfalse, qtrue, qtrue },
	{ &match_overtime, "match_overtime", "0", 0, qfalse, qtrue, qtrue },
	{ &match_overtimecount, "match_overtimecount", "0", 0, qfalse, qtrue, qtrue },
	{ &match_overtimelength, "match_overtimelength", "5", 0, qfalse, qtrue, qtrue },
	{ &match_poweruprespawn, "match_poweruprespawn", "60", 0, qfalse, qfalse, qfalse },
	{ &match_readypercent, "match_readypercent", "51", 0, qfalse, qtrue, qtrue },
	{ &match_roundtimer, "match_roundtimer", "10", 0, qfalse, qtrue, qtrue },
	{ &match_timeoutlength, "match_timeoutlength", "60", 0, qfalse, qtrue, qtrue },
	{ &match_timeoutcount, "match_timeoutcount", "3", 0, qfalse, qtrue, qtrue },
	{ &match_winbyscore, "match_winbyscore", "0", 0, qfalse, qtrue, qtrue },
	{ &mg_damage, "mg_damage", "7", 0, qfalse, qtrue, qtrue },
	{ &mg_damageTeamplay, "mg_damageTeamplay", "5", 0, qfalse, qtrue, qtrue },
	{ &z_m_current, "z_m_current", "-1", 0, qfalse, qfalse, qfalse },
	{ &mode_customfile, "mode_customfile", "none", 0, qfalse, qfalse, qfalse },
	{ &z_m_default, "z_m_default", "-1", 0, qfalse, qfalse, qfalse },
	{ &mode_start, "mode_start", "-1", 0, qfalse, qfalse, qfalse },
	{ &player_always, "player_always", "", 0, qfalse, qfalse, qfalse },
	{ &player_file, "player_file", "none", 0, qfalse, qfalse, qfalse },
	{ &powerup_disable, "powerup_disable", "0", 0, qfalse, qtrue, qtrue },
	{ &ref_password, "ref_password", "none", 0, qfalse, qfalse, qfalse },
	{ &railgun_damage, "railgun_damage", "100", 0, qfalse, qtrue, qtrue },
	{ &replace_machinegun, "replace_machinegun", "0", 0, qfalse, qtrue, qtrue },
	{ &replace_shotgun, "replace_shotgun", "0", 0, qfalse, qtrue, qtrue },
	{ &replace_glauncher, "replace_glauncher", "0", 0, qfalse, qtrue, qtrue },
	{ &replace_rlauncher, "replace_rlauncher", "0", 0, qfalse, qtrue, qtrue },
	{ &replace_lightning, "replace_lightning", "0", 0, qfalse, qtrue, qtrue },
	{ &replace_railgun, "replace_railgun", "0", 0, qfalse, qtrue, qtrue },
	{ &replace_plasmagun, "replace_plasmagun", "0", 0, qfalse, qtrue, qtrue },
	{ &replace_bfg, "replace_bfg", "0", 0, qfalse, qtrue, qtrue },
	{ &replace_invis, "replace_invis", "0", 0, qfalse, qfalse, qfalse },
	{ &replace_quad, "replace_quad", "0", 0, qfalse, qtrue, qtrue },
	{ &replace_battlesuit, "replace_battlesuit", "0", 0, qfalse, qtrue, qtrue },
	{ &replace_haste, "replace_haste", "0", 0, qfalse, qtrue, qtrue },
	{ &replace_regen, "replace_regen", "0", 0, qfalse, qtrue, qtrue },
	{ &replace_flight, "replace_flight", "0", 0, qfalse, qtrue, qtrue },
	{ &server_availmodes, "server_availmodes", "127", 0, qfalse, qfalse, qfalse },
	{ &server_batcher, "server_batcher", "none", 0, qfalse, qfalse, qfalse },
	{ &server_customclient, "server_customclient", "15", 0, qfalse, qfalse, qfalse },
	{ &server_fastrail, "server_fastrail", "0", 0, qfalse, qfalse, qfalse },
	{ &server_freezetag, "server_freezetag", "0", CVAR_SERVERINFO, qfalse, qtrue, qtrue },
	{ &server_gfxspecfile, "server_gfxspecfile", "none", 0, qfalse, qfalse, qfalse },
	{ &server_lgcooldown, "server_lgcooldown", "0", 0, qfalse, qfalse, qfalse },
	{ &server_maxpacketsmin, "server_maxpacketsmin", "0", 0, qfalse, qfalse, qfalse },
	{ &server_maxpacketsmax, "server_maxpacketsmax", "0", 0, qfalse, qfalse, qfalse },
	{ &server_motdfile, "server_motdfile", "none", 0, qfalse, qfalse, qfalse },
	{ &server_motd1, "server_motd1", " ^B^3*** Welcome to OSP Tourney DM/CA/CTF v(1.03a) *** ", 0, qfalse, qfalse, qfalse },
	{ &server_motd2, "server_motd2", "  ", 0, qfalse, qfalse, qfalse },
	{ &server_motd3, "server_motd3", "           http://www.OrangeSmoothie.org", 0, qfalse, qfalse, qfalse },
	{ &server_motd4, "server_motd4", "", 0, qfalse, qfalse, qfalse },
	{ &server_motd5, "server_motd5", "", 0, qfalse, qfalse, qfalse },
	{ &server_motd6, "server_motd6", "", 0, qfalse, qfalse, qfalse },
	{ &server_motd7, "server_motd7", "", 0, qfalse, qfalse, qfalse },
	{ &server_motd8, "server_motd8", "", 0, qfalse, qfalse, qfalse },
	{ &server_ospauth, "server_ospauth", "0", CVAR_SERVERINFO, qfalse, qfalse, qfalse },
	{ &server_realping, "server_realping", "0", 0, qfalse, qfalse, qfalse },
	{ &server_record, "server_record", "0", 0, qfalse, qfalse, qfalse },
	{ &server_thrufloors, "server_thrufloors", "0", 0, qfalse, qtrue, qtrue },
	{ &server_timenudgemax, "server_timenudgemax", "0", 0, qfalse, qfalse, qfalse },
	{ &server_timenudgemin, "server_timenudgemin", "0", 0, qfalse, qfalse, qfalse },
	{ &start_armor, "start_armor", "0", 0, qfalse, qtrue, qtrue },
	{ &start_bfg, "start_bfg", "0", 0, qfalse, qtrue, qtrue },
	{ &start_bullets, "start_bullets", "100", 0, qfalse, qtrue, qtrue },
	{ &start_cells, "start_cells", "0", 0, qfalse, qtrue, qtrue },
	{ &start_grenades, "start_grenades", "0", 0, qfalse, qtrue, qtrue },
	{ &start_health, "start_health", "125", 0, qfalse, qtrue, qtrue },
	{ &start_lightning, "start_lightning", "0", 0, qfalse, qtrue, qtrue },
	{ &start_rockets, "start_rockets", "0", 0, qfalse, qtrue, qtrue },
	{ &start_shells, "start_shells", "0", 0, qfalse, qtrue, qtrue },
	{ &start_slugs, "start_slugs", "0", 0, qfalse, qtrue, qtrue },
	{ &team_allcaptain, "team_allcaptain", "1", 0, qfalse, qtrue, qtrue },
	{ &team_armordamage, "team_armordamage", "0", 0, qfalse, qtrue, qtrue },
	{ &team_maxplayers, "team_maxplayers", "0", 0, qfalse, qtrue, qtrue },
	{ &team_nocontrols, "team_nocontrols", "0", 0, qfalse, qtrue, qtrue },
	{ &team_noteamready, "team_noteamready", "0", 0, qfalse, qfalse, qfalse },
	{ &viewcam_directory, "viewcam_directory", "cfg-viewcam", 0, qfalse, qfalse, qfalse },
	{ &vote_allow_allcaptain, "vote_allow_allcaptain", "1", 0, qfalse, qtrue, qtrue },
	{ &vote_allow_altgraphics, "vote_allow_altgraphics", "1", 0, qfalse, qfalse, qfalse },
	{ &vote_allow_armordamage, "vote_allow_armordamage", "1", 0, qfalse, qtrue, qtrue },
	{ &vote_allow_capfps, "vote_allow_capfps", "0", 0, qfalse, qfalse, qfalse },
	{ &vote_allow_capturelimit, "vote_allow_capturelimit", "1", 0, qfalse, qtrue, qtrue },
	{ &vote_allow_ca_startrespawn, "vote_allow_ca_startrespawn", "1", 0, qfalse, qtrue, qtrue },
	{ &vote_allow_ca_warmupfire, "vote_allow_ca_warmupfire", "1", 0, qfalse, qtrue, qtrue },
	{ &vote_allow_cq3, "vote_allow_cq3", "1", 0, qfalse, qtrue, qtrue },
	{ &vote_allow_dropitems, "vote_allow_dropitems", "1", 0, qfalse, qtrue, qtrue },
	{ &vote_allow_falling, "vote_allow_falling", "1", 0, qfalse, qtrue, qtrue },
	{ &vote_allow_fastrail, "vote_allow_fastrail", "1", 0, qfalse, qtrue, qtrue },
	{ &vote_allow_fraglimit, "vote_allow_fraglimit", "1", 0, qfalse, qtrue, qtrue },
	{ &vote_allow_hook, "vote_allow_hook", "1", 0, qfalse, qfalse, qfalse },
	{ &vote_allow_items, "vote_allow_items", "1023", 0, qfalse, qtrue, qtrue },
	{ &vote_allow_instagib, "vote_allow_instagib", "1", 0, qfalse, qtrue, qtrue },
	{ &vote_allow_kick, "vote_allow_kick", "1", 0, qfalse, qfalse, qfalse },
	{ &vote_allow_lgcooldown, "vote_allow_lgcooldown", "1", 0, qfalse, qtrue, qtrue },
	{ &vote_allow_map, "vote_allow_map", "1", 0, qfalse, qtrue, qtrue },
	{ &vote_allow_maxpackets, "vote_allow_maxpackets", "1", 0, qfalse, qfalse, qfalse },
	{ &vote_allow_machinegun, "vote_allow_machinegun", "1", 0, qfalse, qtrue, qtrue },
	{ &vote_allow_mutespecs, "vote_allow_mutespecs", "1", 0, qfalse, qtrue, qtrue },
	{ &vote_allow_notimer, "vote_allow_notimer", "1", 0, qfalse, qtrue, qtrue },
	{ &vote_allow_osphud, "vote_allow_osphud", "1", 0, qfalse, qfalse, qfalse },
	{ &vote_allow_overtime, "vote_allow_overtime", "1", 0, qfalse, qtrue, qtrue },
	{ &vote_allow_pmove, "vote_allow_pmove", "1", 0, qfalse, qtrue, qtrue },
	{ &vote_allow_poweruprespawn, "vote_allow_poweruprespawn", "1", 0, qfalse, qtrue, qtrue },
	{ &vote_allow_powerups, "vote_allow_powerups", "255", 0, qfalse, qfalse, qfalse },
	{ &vote_allow_promode, "vote_allow_promode", "1", 0, qfalse, qfalse, qfalse },
	{ &vote_allow_referee, "vote_allow_referee", "1", 0, qfalse, qfalse, qfalse },
	{ &vote_allow_reload, "vote_allow_reload", "1", 0, qfalse, qfalse, qfalse },
	{ &vote_allow_respawn, "vote_allow_respawn", "1", 0, qfalse, qtrue, qtrue },
	{ &vote_allow_roundlimit, "vote_allow_roundlimit", "1", 0, qfalse, qtrue, qtrue },
	{ &vote_allow_teamdamage, "vote_allow_teamdamage", "1", 0, qfalse, qfalse, qfalse },
	{ &vote_allow_thrufloors, "vote_allow_thrufloors", "1", 0, qfalse, qfalse, qfalse },
	{ &vote_allow_timenudge, "vote_allow_timenudge", "1", 0, qfalse, qtrue, qtrue },
	{ &vote_allow_timelimit, "vote_allow_timelimit", "1", 0, qfalse, qtrue, qtrue },
	{ &vote_allow_weaponrespawn, "vote_allow_weaponrespawn", "1", 0, qfalse, qfalse, qfalse },
	{ &vote_limit, "vote_limit", "5", 0, qfalse, qtrue, qtrue },
	{ &vote_percent, "vote_percent", "50", 0, qfalse, qtrue, qtrue },
	{ &warmup_armor, "warmup_armor", "100", 0, qfalse, qtrue, qtrue },
	{ &weapon_deaddrop, "weapon_deaddrop", "1", 0, qfalse, qtrue, qtrue },
	{ &weapon_have, "weapon_have", "257", 0, qfalse, qtrue, qtrue },
	{ &weapon_start, "weapon_start", "1", 0, qfalse, qtrue, qtrue },
};

// bk001129 - made static to avoid aliasing
static int gameCvarTableSize = sizeof(gameCvarTable) / sizeof(gameCvarTable[0]);


void G_InitGame(int levelTime, int randomSeed, int restart);
void G_RunFrame(int levelTime);
void G_ShutdownGame(int restart);
void CheckExitRules(void);


static const char* monthName[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

static fileHandle_t adminLogHandle;
static qboolean adminLogDisabled;


/*
================
vmMain

This is the only way control passes into the module.
This must be the very first function compiled into the .q3vm file
================
*/
int vmMain(int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11)
{
	switch (command)
	{
		case GAME_INIT:
			G_InitGame(arg0, arg1, arg2);
			return 0;
		case GAME_SHUTDOWN:
			G_ShutdownGame(arg0);
			return 0;
		case GAME_CLIENT_CONNECT:
			return (int)ClientConnect(arg0, arg1, arg2);
		case GAME_CLIENT_THINK:
			ClientThink(arg0);
			return 0;
		case GAME_CLIENT_USERINFO_CHANGED:
			ClientUserinfoChanged(arg0);
			return 0;
		case GAME_CLIENT_DISCONNECT:
			ClientDisconnect(arg0);
			return 0;
		case GAME_CLIENT_BEGIN:
			ClientBegin(arg0);
			return 0;
		case GAME_CLIENT_COMMAND:
			ClientCommand(arg0);
			return 0;
		case GAME_RUN_FRAME:
			G_RunFrame(arg0);
			return 0;
		case GAME_CONSOLE_COMMAND:
			return ConsoleCommand();
		case BOTAI_START_FRAME:
			return BotAIStartFrame(arg0);
	}

	return -1;
}


void QDECL G_Printf(const char* fmt, ...)
{
	va_list     argptr;
	char        text[1024];

	va_start(argptr, fmt);
	vsprintf(text, fmt, argptr);
	va_end(argptr);

	trap_Printf(text);
}

void QDECL G_Error(const char* fmt, ...)
{
	va_list     argptr;
	char        text[1024];

	va_start(argptr, fmt);
	vsprintf(text, fmt, argptr);
	va_end(argptr);

	trap_Error(text);
}

/*
================
G_AdminLog
================
*/
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

/*
================
G_FindTeams

Chain together all entities with a matching team field.
Entity teams are used for item groups and multi-entity mover groups.

All but the first will have the FL_TEAMSLAVE flag set and teammaster field set
All but the last will have the teamchain field set to the next one
================
*/
void G_FindTeams(void)
{
	gentity_t*   e, *e2;
	int     i, j;
	int     c, c2;

	c = 0;
	c2 = 0;
	for (i = 1, e = g_entities + i ; i < level.num_entities ; i++, e++)
	{
		if (!e->inuse)
			continue;
		if (!e->team)
			continue;
		if (e->flags & FL_TEAMSLAVE)
			continue;
		e->teammaster = e;
		c++;
		c2++;
		for (j = i + 1, e2 = e + 1 ; j < level.num_entities ; j++, e2++)
		{
			if (!e2->inuse)
				continue;
			if (!e2->team)
				continue;
			if (e2->flags & FL_TEAMSLAVE)
				continue;
			if (!strcmp(e->team, e2->team))
			{
				c2++;
				e2->teamchain = e->teamchain;
				e->teamchain = e2;
				e2->teammaster = e;
				e2->flags |= FL_TEAMSLAVE;

				// make sure that targets only point at the master
				if (e2->targetname)
				{
					e->targetname = e2->targetname;
					e2->targetname = NULL;
				}
			}
		}
	}

	G_Printf("%i teams with %i entities\n", c, c2);
}

void G_RemapTeamShaders()
{
#ifdef MISSIONPACK
	char string[1024];
	float f = level.time * 0.001;
	Com_sprintf(string, sizeof(string), "team_icon/%s_red", g_redteam.string);
	AddRemap("textures/ctf2/redteam01", string, f);
	AddRemap("textures/ctf2/redteam02", string, f);
	Com_sprintf(string, sizeof(string), "team_icon/%s_blue", g_blueteam.string);
	AddRemap("textures/ctf2/blueteam01", string, f);
	AddRemap("textures/ctf2/blueteam02", string, f);
	trap_SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());
#endif
}


/*
=================
G_IsGameTypeOSP
=================
*/
qboolean G_IsGameTypeOSP(int gametype)
{
	return gametype >= GT_CA;
}

/*
=================
G_RegisterCvars
=================
*/
void G_RegisterCvars(void)
{
	int         i;
	cvarTable_t* cv;
	qboolean remapped = qfalse;

	for (i = 0, cv = gameCvarTable ; i < gameCvarTableSize ; i++, cv++)
	{
		//skip if not team game and cvar name start with Score_ but not gametype 1 and name "Score_Time"
		if (g_gametype.integer < GT_TEAM && cv->cvarName == strstr(cv->cvarName, "Score_") && (g_gametype.integer != 1 || !Q_stricmp(cv->cvarName, "Score_Time")))
		{
			continue;
		}
		//skip if not team game and name is Players_Red or Players_Blue
		if (g_gametype.integer < GT_TEAM && (!Q_stricmp(cv->cvarName, "Players_Red") || !Q_stricmp(cv->cvarName, "Players_Blue")))
		{
			continue;
		}
		//skip if team game and name Players_Active
		if (g_gametype.integer >= GT_TEAM && !Q_stricmp(cv->cvarName, "Players_Active"))
		{
			continue;
		}

		if (!Q_stricmp(cv->cvarName, "gamename"))
		{
			trap_Cvar_Register(cv->vmCvar, cv->cvarName, "baseq3", cv->cvarFlags);
		}
		else if (cv->vmCvar == &start_health && server_promode.integer <= 0 && server_cq3.integer <= 0)
		{
			trap_Cvar_Register(cv->vmCvar, cv->cvarName, "100", cv->cvarFlags);
		}
		else if (cv->vmCvar == &g_weaponRespawn && (server_promode.integer > 0 || server_cq3.integer > 0))
		{
			const char* wprStr = "5";
			switch (g_gametype.integer)
			{
				default:
					break;
				case GT_TEAM:
					wprStr = "30";
					break;
				case GT_TOURNAMENT:
					wprStr = "15";
					break;
				case GT_CTF:
					wprStr = "10";
					break;
			}
			trap_Cvar_Register(cv->vmCvar, cv->cvarName, wprStr, cv->cvarFlags);
		}
		else if (g_gametype.integer == GT_TEAM && cv->vmCvar == &start_bullets)
		{
			trap_Cvar_Register(cv->vmCvar, cv->cvarName, "50", cv->cvarFlags);
		}
		else if (g_gametype.integer == GT_CTF && cv->vmCvar == &match_poweruprespawn)
		{
			trap_Cvar_Register(cv->vmCvar, cv->cvarName, "120", cv->cvarFlags);
		}
		else if (cv->vmCvar == &g_forcerespawn)
		{
			const char* respStr = "5";
			if (g_gametype.integer == GT_TOURNAMENT)
			{
				respStr = "10";
			}
			else
			{
				respStr = "20";
			}
			trap_Cvar_Register(cv->vmCvar, cv->cvarName, respStr, cv->cvarFlags);                                       /* Address : 0x354 Type : Interium */
		}
		else if (cv->vmCvar == &match_mutespecs && g_gametype.integer == GT_TOURNAMENT)
		{
			trap_Cvar_Register(cv->vmCvar, cv->cvarName, "1", cv->cvarFlags);                                               /* Address : 0x374 Type : Interium */
		}
		else if (g_gametype.integer == GT_CA && cv->vmCvar == &start_armor)
		{
			trap_Cvar_Register(cv->vmCvar, cv->cvarName, "200", cv->cvarFlags);                                                 /* Address : 0x394 Type : Interium */
		}
		else if (G_IsGameTypeOSP(g_gametype.integer) && cv->vmCvar == &roundlimit)
		{
			trap_Cvar_Register(cv->vmCvar, cv->cvarName, cv->defaultString, cv->cvarFlags);
			trap_Cvar_Set("fraglimit", roundlimit.string);
		}
		else if (cv->vmCvar == &match_overtime)
		{
			const char* moStr;
			if (g_gametype.integer == GT_TEAM)
			{
				moStr = "1";
			}
			else
			{
				moStr = "0";
			}
			trap_Cvar_Register(cv->vmCvar, cv->cvarName, moStr, cv->cvarFlags);
		}
		else
		{
			trap_Cvar_Register(cv->vmCvar, cv->cvarName, cv->defaultString, cv->cvarFlags);
		}
		if (cv->vmCvar)
			cv->modificationCount = cv->vmCvar->modificationCount;

		if (cv->teamShader)
		{
			remapped = qtrue;
		}

	}

	if (remapped)
	{
		G_RemapTeamShaders();
	}

	trap_Cvar_Update(&server_promode);
	trap_Cvar_Update(&server_cq3);
	g_unk_33677();                                                                  /* Address : 0x449 Type : Interium */
	g_unk_33dc1(0);                                                                 /* Address : 0x44e Type : Interium */

	// check some things
	if (g_gametype.integer < 0 || g_gametype.integer >= GT_MAX_GAME_TYPE)
	{
		G_Printf("g_gametype %i is out of range, defaulting to 0\n", g_gametype.integer);
		trap_Cvar_Set("g_gametype", "0");
	}

	level.warmupModificationCount = g_warmup.modificationCount;
}

/*
=================
G_UpdateCvars
=================
*/
void G_UpdateCvars(void)
{
	int         i;
	cvarTable_t* cv;
	qboolean remapped = qfalse;

	for (i = 0, cv = gameCvarTable ; i < gameCvarTableSize ; i++, cv++)
	{
		if (cv->vmCvar)
		{
			trap_Cvar_Update(cv->vmCvar);

			if (cv->vmCvar == &admin_log)
			{
				adminLogDisabled = qfalse;
			}

			if (cv->modificationCount != cv->vmCvar->modificationCount)
			{
				cv->modificationCount = cv->vmCvar->modificationCount;

				if (cv->trackChange)
				{
					trap_SendServerCommand(-1, va("print \"Server: %s changed to %s\n\"",
					                              cv->cvarName, cv->vmCvar->string));
				}

				if (cv->teamShader)
				{
					remapped = qtrue;
				}
			}
		}
	}

	if (remapped)
	{
		G_RemapTeamShaders();
	}
}

static const char* G_GetMapName(void)
{
	char    serverinfo[MAX_INFO_STRING];
	static char mapname[128];

	trap_GetServerinfo(serverinfo, sizeof(serverinfo));
	strncpy(mapname, Info_ValueForKey(serverinfo, "mapname"), 127);

	return mapname;
}

/*
============
G_InitGame

============
*/
void G_InitGame(int levelTime, int randomSeed, int restart)
{
	int                 i;
	int                 maxpacketsmin;
	int                 maxpacketsmax;
	int                 timenudgemin;
	int                 timenudgemax;
	int                 freezeGameType;

	G_Printf("------- Game Initialization -------\n");

	srand(randomSeed);

	G_RegisterCvars();

	G_ProcessIPBans();

	G_InitMemory();

	trap_SetConfigstring(CS_OSP_VERSION_STR, "OSP Tourney DM/CA/CTF v(1.03a)");
	trap_SetConfigstring(CS_OSP_ALLOW_PMOVE, va("%d", pmove_fixed.integer));
	trap_SetConfigstring(CS_OSP_CUSTOM_CLIENT, va("%d", server_customclient.integer));
	trap_SetConfigstring(CS_OSP_AUTH, va("%d", server_ospauth.integer));

	if (server_maxpacketsmin.integer > MAXPACKETSMIN)
	{
		maxpacketsmin = server_maxpacketsmin.integer;
	}
	else
	{
		maxpacketsmin = MAXPACKETSMIN;
	}
	trap_SetConfigstring(CS_OSP_MAXPACKETS_MIN, va("%d", maxpacketsmin));

	if (server_maxpacketsmax.integer < MAXPACKETSMAX)
	{
		maxpacketsmax = server_maxpacketsmax.integer;
	}
	else
	{
		maxpacketsmax = MAXPACKETSMAX;
	}
	trap_SetConfigstring(CS_OSP_MAXPACKETS_MAX, va("%d", maxpacketsmax));

	if (server_timenudgemin.integer > TIMENUDGEMIN)
	{
		timenudgemin = server_timenudgemin.integer;
	}
	else
	{
		timenudgemin = TIMENUDGEMIN;
	}
	trap_SetConfigstring(CS_OSP_TIMENUDGE_MIN, va("%d", timenudgemin));

	if (server_timenudgemax.integer < TIMENUDGEMAX)
	{
		timenudgemax = server_timenudgemax.integer;
	}
	else
	{
		timenudgemax = TIMENUDGEMAX;
	}
	trap_SetConfigstring(CS_OSP_TIMENUDGE_MAX, va("%d", timenudgemax));

	trap_SetConfigstring(CS_OSP_CLAN_BASE_TEAM_DM, va("%d", z_m_current.integer == 2 ? 1 : 0));

	if (server_freezetag.integer)
	{
		freezeGameType = 0;
	}
	else
	{
		if (server_freezetag.integer == 2)
		{
			freezeGameType = 2;
		}
		else
		{
			freezeGameType = 1;
		}
	}

	trap_SetConfigstring(CS_OSP_FREEZE_GAME_TYPE, va("%d", freezeGameType));

	G_Printf("gamename: %s\n", osp_gamename.integer ? "osp" : "baseq3 -- osp");
	G_Printf("gamedate: %s\n", __DATE__);

	trap_SetConfigstring(CS_INTERMISSION, "0");

	// set some level globals
	memset(&level, 0, sizeof(level));

	OSP_UNK_CODE("g_unk_33677()");

	level.time = levelTime;
	level.startTime = levelTime;
	level.osp_levelTime = levelTime;
	level.unknown2 = 0;
	level.unknown1 = 7900;

	level.snd_fry = G_SoundIndex("sound/player/fry.wav");   // FIXME standing in lava / slime
	//
	if (g_gametype.integer != GT_SINGLE_PLAYER && g_log.string[0])
	{
		if (g_logSync.integer)
		{
			trap_FS_FOpenFile(g_log.string, &level.logFile, FS_APPEND_SYNC);
		}
		else
		{
			trap_FS_FOpenFile(g_log.string, &level.logFile, FS_APPEND);
		}
		if (!level.logFile)
		{
			G_Printf("WARNING: Couldn't open logfile: %s\n", g_log.string);
		}
		else
		{
			char    serverinfo[MAX_INFO_STRING];
			qtime_t localTime;

			trap_GetServerinfo(serverinfo, sizeof(serverinfo));
			G_LogPrintf("------------------------------------------------------------\n");
			G_LogPrintf("InitGame: %s\n", serverinfo);
			trap_RealTime(&localTime);
			G_LogPrintf("ServerTime:\t%d%02d%02d%02d%02d%02d\t%02d:%02d:%02d (%02d %s %d)\n",
			            localTime.tm_year + 1900,
			            localTime.tm_mon + 1,
			            localTime.tm_mday,
			            localTime.tm_hour,
			            localTime.tm_min,
			            localTime.tm_sec,
			            localTime.tm_hour,
			            localTime.tm_min,
			            localTime.tm_sec,
			            localTime.tm_mday,
			            monthName[localTime.tm_mon],
			            localTime.tm_year + 1900);
		}
	}
	else
	{
		G_Printf("Not logging to disk.\n");
	}

	G_InitWorldSession();

	// initialize all entities for this game
	memset(g_entities, 0, MAX_GENTITIES * sizeof(g_entities[0]));
	level.gentities = g_entities;

	// initialize all clients for this game
	level.maxclients = g_maxclients.integer;
	memset(g_clients, 0, MAX_CLIENTS * sizeof(g_clients[0]));
	level.clients = g_clients;

	// set client fields on player ents
	for (i = 0 ; i < level.maxclients ; i++)
	{
		g_entities[i].client = level.clients + i;
	}

	// always leave room for the max number of clients,
	// even if they aren't all used, so numbers inside that
	// range are NEVER anything but clients
	level.num_entities = MAX_CLIENTS;

	// let the server system know where the entites are
	trap_LocateGameData(level.gentities, level.num_entities, sizeof(gentity_t),
	                    &level.clients[0].ps, sizeof(level.clients[0]));

	// reserve some spots for dead player bodies
	InitBodyQue();

	ClearRegisteredItems();

	OSP_UNK_CODE("g_unk_3285c()");
	if (Q_stricmp(G_GetMapName(), "ospdm4") == 0)
	{
		level.unknown_flag1 |= 1;
	}
	if (Q_stricmp(G_GetMapName(), "ospdm2") == 0)
	{
		level.unknown_flag1 |= 2;
	}

	// parse the key/value pairs and spawn gentities
	G_SpawnEntitiesFromString();

	// general initialization
	G_FindTeams();

	// make sure we have flags for CTF, etc
	if (g_gametype.integer >= GT_TEAM)
	{
		G_CheckTeamItems();
	}

	SaveRegisteredItems();

	G_Printf("-----------------------------------\n");

	if (g_gametype.integer == GT_SINGLE_PLAYER || trap_Cvar_VariableIntegerValue("com_buildScript"))
	{
		G_ModelIndex(SP_PODIUM_MODEL);
		G_SoundIndex("sound/player/gurp1.wav");
		G_SoundIndex("sound/player/gurp2.wav");
	}

	if (trap_Cvar_VariableIntegerValue("bot_enable"))
	{
		BotAISetup(restart);
		BotAILoadMap(restart);
		G_InitBots(restart);
	}

	G_RemapTeamShaders();
	OSP_UNK_CODE("g_unk_30dd4()");
	G_UpdateCvars();
}



/*
=================
G_ShutdownGame
=================
*/
void G_ShutdownGame(int restart)
{
	G_Printf("==== ShutdownGame ====\n");

	if (level.logFile)
	{
		G_LogPrintf("ShutdownGame:\n");
		G_LogPrintf("------------------------------------------------------------\n");
		trap_FS_FCloseFile(level.logFile);
	}

	// write all the client session data so we can get it back
	G_WriteSessionData();

	if (trap_Cvar_VariableIntegerValue("bot_enable"))
	{
		BotAIShutdown(restart);
	}
}



//===================================================================

#ifndef GAME_HARD_LINKED
// this is only here so the functions in q_shared.c and bg_*.c can link

void QDECL Com_Error(int level, const char* error, ...)
{
	va_list     argptr;
	char        text[1024];

	va_start(argptr, error);
	vsprintf(text, error, argptr);
	va_end(argptr);

	G_Error("%s", text);
}

void QDECL Com_Printf(const char* msg, ...)
{
	va_list     argptr;
	char        text[1024];

	va_start(argptr, msg);
	vsprintf(text, msg, argptr);
	va_end(argptr);

	G_Printf("%s", text);
}

#endif

/*
========================================================================

PLAYER COUNTING / SCORE SORTING

========================================================================
*/

/*
=============
AddTournamentPlayer

If there are less than two tournament players, put a
spectator in the game and restart
=============
*/
void AddTournamentPlayer(void)
{
	int         i;
	gclient_t*   client;
	gclient_t*   nextInLine;

	if (level.numPlayingClients >= 2)
	{
		return;
	}

	// never change during intermission
	if (level.intermissiontime)
	{
		return;
	}

	nextInLine = NULL;

	for (i = 0 ; i < level.maxclients ; i++)
	{
		client = &level.clients[i];
		if (client->pers.connected != CON_CONNECTED)
		{
			continue;
		}
		if (client->sess.sessionTeam != TEAM_SPECTATOR)
		{
			continue;
		}
		// never select the dedicated follow or scoreboard clients
		if (client->sess.spectatorState == SPECTATOR_SCOREBOARD ||
		        client->sess.spectatorClient < 0)
		{
			continue;
		}

		if (!nextInLine || client->sess.spectatorTime < nextInLine->sess.spectatorTime)
		{
			nextInLine = client;
		}
	}

	if (!nextInLine)
	{
		return;
	}

	level.warmupTime = -1;

	// set them to free-for-all team
	SetTeam(&g_entities[ nextInLine - level.clients ], "f");
}

/*
=======================
RemoveTournamentLoser

Make the loser a spectator at the back of the line
=======================
*/
void RemoveTournamentLoser(void)
{
	int         clientNum;

	if (level.numPlayingClients != 2)
	{
		return;
	}

	clientNum = level.sortedClients[1];

	if (level.clients[ clientNum ].pers.connected != CON_CONNECTED)
	{
		return;
	}

	// make them a spectator
	SetTeam(&g_entities[ clientNum ], "s");
}

/*
=======================
RemoveTournamentWinner
=======================
*/
void RemoveTournamentWinner(void)
{
	int         clientNum;

	if (level.numPlayingClients != 2)
	{
		return;
	}

	clientNum = level.sortedClients[0];

	if (level.clients[ clientNum ].pers.connected != CON_CONNECTED)
	{
		return;
	}

	// make them a spectator
	SetTeam(&g_entities[ clientNum ], "s");
}

/*
=======================
AdjustTournamentScores
=======================
*/
void AdjustTournamentScores(void)
{
	int         clientNum;

	clientNum = level.sortedClients[0];
	if (level.clients[ clientNum ].pers.connected == CON_CONNECTED)
	{
		level.clients[ clientNum ].sess.wins++;
		ClientUserinfoChanged(clientNum);
	}

	clientNum = level.sortedClients[1];
	if (level.clients[ clientNum ].pers.connected == CON_CONNECTED)
	{
		level.clients[ clientNum ].sess.losses++;
		ClientUserinfoChanged(clientNum);
	}

}

/*
=============
SortRanks

=============
*/
int QDECL SortRanks(const void* a, const void* b)
{
	gclient_t*   ca, *cb;

	ca = &level.clients[*(int*)a];
	cb = &level.clients[*(int*)b];

	// sort special clients last
	if (ca->sess.spectatorState == SPECTATOR_SCOREBOARD || ca->sess.spectatorClient < 0)
	{
		return 1;
	}
	if (cb->sess.spectatorState == SPECTATOR_SCOREBOARD || cb->sess.spectatorClient < 0)
	{
		return -1;
	}

	// then connecting clients
	if (ca->pers.connected == CON_CONNECTING)
	{
		return 1;
	}
	if (cb->pers.connected == CON_CONNECTING)
	{
		return -1;
	}


	// then spectators
	if (ca->sess.sessionTeam == TEAM_SPECTATOR && cb->sess.sessionTeam == TEAM_SPECTATOR)
	{
		if (ca->sess.spectatorTime < cb->sess.spectatorTime)
		{
			return -1;
		}
		if (ca->sess.spectatorTime > cb->sess.spectatorTime)
		{
			return 1;
		}
		return 0;
	}
	if (ca->sess.sessionTeam == TEAM_SPECTATOR)
	{
		return 1;
	}
	if (cb->sess.sessionTeam == TEAM_SPECTATOR)
	{
		return -1;
	}

	// then sort by score
	if (ca->ps.persistant[PERS_SCORE]
	        > cb->ps.persistant[PERS_SCORE])
	{
		return -1;
	}
	if (ca->ps.persistant[PERS_SCORE]
	        < cb->ps.persistant[PERS_SCORE])
	{
		return 1;
	}
	return 0;
}

/*
============
CalculateRanks

Recalculates the score ranks of all players
This will be called on every client connect, begin, disconnect, death,
and team change.
============
*/
void CalculateRanks(void)
{
	int     i;
	int     rank;
	int     score;
	int     newScore;
	gclient_t*   cl;

	level.follow1 = -1;
	level.follow2 = -1;
	level.numConnectedClients = 0;
	level.numNonSpectatorClients = 0;
	level.numPlayingClients = 0;
	level.numVotingClients = 0;     // don't count bots
	for (i = 0; i < TEAM_NUM_TEAMS; i++)
	{
		level.numteamVotingClients[i] = 0;
	}
	for (i = 0 ; i < level.maxclients ; i++)
	{
		if (level.clients[i].pers.connected != CON_DISCONNECTED)
		{
			level.sortedClients[level.numConnectedClients] = i;
			level.numConnectedClients++;

			if (level.clients[i].sess.sessionTeam != TEAM_SPECTATOR)
			{
				level.numNonSpectatorClients++;

				// decide if this should be auto-followed
				if (level.clients[i].pers.connected == CON_CONNECTED)
				{
					level.numPlayingClients++;
					if (!(g_entities[i].r.svFlags & SVF_BOT))
					{
						level.numVotingClients++;
						if (level.clients[i].sess.sessionTeam == TEAM_RED)
							level.numteamVotingClients[0]++;
						else if (level.clients[i].sess.sessionTeam == TEAM_BLUE)
							level.numteamVotingClients[1]++;
					}
					if (level.follow1 == -1)
					{
						level.follow1 = i;
					}
					else if (level.follow2 == -1)
					{
						level.follow2 = i;
					}
				}
			}
		}
	}

	qsort(level.sortedClients, level.numConnectedClients,
	      sizeof(level.sortedClients[0]), SortRanks);

	// set the rank value for all clients that are connected and not spectators
	if (g_gametype.integer >= GT_TEAM)
	{
		// in team games, rank is just the order of the teams, 0=red, 1=blue, 2=tied
		for (i = 0;  i < level.numConnectedClients; i++)
		{
			cl = &level.clients[ level.sortedClients[i] ];
			if (level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE])
			{
				cl->ps.persistant[PERS_RANK] = 2;
			}
			else if (level.teamScores[TEAM_RED] > level.teamScores[TEAM_BLUE])
			{
				cl->ps.persistant[PERS_RANK] = 0;
			}
			else
			{
				cl->ps.persistant[PERS_RANK] = 1;
			}
		}
	}
	else
	{
		rank = -1;
		score = 0;
		for (i = 0;  i < level.numPlayingClients; i++)
		{
			cl = &level.clients[ level.sortedClients[i] ];
			newScore = cl->ps.persistant[PERS_SCORE];
			if (i == 0 || newScore != score)
			{
				rank = i;
				// assume we aren't tied until the next client is checked
				level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank;
			}
			else
			{
				// we are tied with the previous client
				level.clients[ level.sortedClients[i - 1] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
				level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
			}
			score = newScore;
			if (g_gametype.integer == GT_SINGLE_PLAYER && level.numPlayingClients == 1)
			{
				level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
			}
		}
	}

	// set the CS_SCORES1/2 configstrings, which will be visible to everyone
	if (g_gametype.integer >= GT_TEAM)
	{
		trap_SetConfigstring(CS_SCORES1, va("%i", level.teamScores[TEAM_RED]));
		trap_SetConfigstring(CS_SCORES2, va("%i", level.teamScores[TEAM_BLUE]));
	}
	else
	{
		if (level.numConnectedClients == 0)
		{
			trap_SetConfigstring(CS_SCORES1, va("%i", SCORE_NOT_PRESENT));
			trap_SetConfigstring(CS_SCORES2, va("%i", SCORE_NOT_PRESENT));
		}
		else if (level.numConnectedClients == 1)
		{
			trap_SetConfigstring(CS_SCORES1, va("%i", level.clients[ level.sortedClients[0] ].ps.persistant[PERS_SCORE]));
			trap_SetConfigstring(CS_SCORES2, va("%i", SCORE_NOT_PRESENT));
		}
		else
		{
			trap_SetConfigstring(CS_SCORES1, va("%i", level.clients[ level.sortedClients[0] ].ps.persistant[PERS_SCORE]));
			trap_SetConfigstring(CS_SCORES2, va("%i", level.clients[ level.sortedClients[1] ].ps.persistant[PERS_SCORE]));
		}
	}

	// see if it is time to end the level
	CheckExitRules();

	// if we are at the intermission, send the new info to everyone
	if (level.intermissiontime)
	{
		SendScoreboardMessageToAllClients();
	}
}


/*
========================================================================

MAP CHANGING

========================================================================
*/

/*
========================
SendScoreboardMessageToAllClients

Do this at BeginIntermission time and whenever ranks are recalculated
due to enters/exits/forced team changes
========================
*/
void SendScoreboardMessageToAllClients(void)
{
	int     i;

	for (i = 0 ; i < level.maxclients ; i++)
	{
		if (level.clients[ i ].pers.connected == CON_CONNECTED)
		{
			DeathmatchScoreboardMessage(g_entities + i);
		}
	}
}

/*
========================
MoveClientToIntermission

When the intermission starts, this will be called for all players.
If a new client connects, this will be called after the spawn function.
========================
*/
void MoveClientToIntermission(gentity_t* ent)
{
	// take out of follow mode if needed
	if (ent->client->sess.spectatorState == SPECTATOR_FOLLOW)
	{
		StopFollowing(ent);
	}


	// move to the spot
	VectorCopy(level.intermission_origin, ent->s.origin);
	VectorCopy(level.intermission_origin, ent->client->ps.origin);
	VectorCopy(level.intermission_angle, ent->client->ps.viewangles);
	ent->client->ps.pm_type = PM_INTERMISSION;

	// clean up powerup info
	memset(ent->client->ps.powerups, 0, sizeof(ent->client->ps.powerups));

	ent->client->ps.eFlags = 0;
	ent->s.eFlags = 0;
	ent->s.eType = ET_GENERAL;
	ent->s.modelindex = 0;
	ent->s.loopSound = 0;
	ent->s.event = 0;
	ent->r.contents = 0;
}

/*
==================
FindIntermissionPoint

This is also used for spectator spawns
==================
*/
void FindIntermissionPoint(void)
{
	gentity_t*   ent, *target;
	vec3_t      dir;

	// find the intermission spot
	ent = G_Find(NULL, FOFS(classname), "info_player_intermission");
	if (!ent)        // the map creator forgot to put in an intermission point...
	{
		SelectSpawnPoint(vec3_origin, level.intermission_origin, level.intermission_angle);
	}
	else
	{
		VectorCopy(ent->s.origin, level.intermission_origin);
		VectorCopy(ent->s.angles, level.intermission_angle);
		// if it has a target, look towards it
		if (ent->target)
		{
			target = G_PickTarget(ent->target);
			if (target)
			{
				VectorSubtract(target->s.origin, level.intermission_origin, dir);
				vectoangles(dir, level.intermission_angle);
			}
		}
	}

}

/*
==================
BeginIntermission
==================
*/
void BeginIntermission(void)
{
	int         i;
	gentity_t*   client;

	if (level.intermissiontime)
	{
		return;     // already active
	}

	// if in tournement mode, change the wins / losses
	if (g_gametype.integer == GT_TOURNAMENT)
	{
		AdjustTournamentScores();
	}

	level.intermissiontime = level.time;
	FindIntermissionPoint();

#ifdef MISSIONPACK
	if (g_singlePlayer.integer)
	{
		trap_Cvar_Set("ui_singlePlayerActive", "0");
		UpdateTournamentInfo();
	}
#else
	// if single player game
	if (g_gametype.integer == GT_SINGLE_PLAYER)
	{
		UpdateTournamentInfo();
		SpawnModelsOnVictoryPads();
	}
#endif

	// move all clients to the intermission point
	for (i = 0 ; i < level.maxclients ; i++)
	{
		client = g_entities + i;
		if (!client->inuse)
			continue;
		// respawn if dead
		if (client->health <= 0)
		{
			respawn(client);
		}
		MoveClientToIntermission(client);
	}

	// send the current scoring to all clients
	SendScoreboardMessageToAllClients();

}


/*
=============
ExitLevel

When the intermission has been exited, the server is either killed
or moved to a new level based on the "nextmap" cvar

=============
*/
void ExitLevel(void)
{
	int     i;
	gclient_t* cl;

	//bot interbreeding
	BotInterbreedEndMatch();

	// if we are running a tournement map, kick the loser to spectator status,
	// which will automatically grab the next spectator and restart
	if (g_gametype.integer == GT_TOURNAMENT)
	{
		if (!level.restarted)
		{
			RemoveTournamentLoser();
			trap_SendConsoleCommand(EXEC_APPEND, "map_restart 0\n");
			level.restarted = qtrue;
			level.changemap = NULL;
			level.intermissiontime = 0;
		}
		return;
	}


	trap_SendConsoleCommand(EXEC_APPEND, "vstr nextmap\n");
	level.changemap = NULL;
	level.intermissiontime = 0;

	// reset all the scores so we don't enter the intermission again
	level.teamScores[TEAM_RED] = 0;
	level.teamScores[TEAM_BLUE] = 0;
	for (i = 0 ; i < g_maxclients.integer ; i++)
	{
		cl = level.clients + i;
		if (cl->pers.connected != CON_CONNECTED)
		{
			continue;
		}
		cl->ps.persistant[PERS_SCORE] = 0;
	}

	// we need to do this here before chaning to CON_CONNECTING
	G_WriteSessionData();

	// change all client states to connecting, so the early players into the
	// next level will know the others aren't done reconnecting
	for (i = 0 ; i < g_maxclients.integer ; i++)
	{
		if (level.clients[i].pers.connected == CON_CONNECTED)
		{
			level.clients[i].pers.connected = CON_CONNECTING;
		}
	}

}

/*
=================
G_LogPrintf

Print to the logfile with a time stamp if it is open
=================
*/
void QDECL G_LogPrintf(const char* fmt, ...)
{
	va_list     argptr;
	char        string[1024];
	int         min, tens, sec;

	sec = level.time / 1000;

	min = sec / 60;
	sec -= min * 60;
	tens = sec / 10;
	sec -= tens * 10;

	Com_sprintf(string, sizeof(string), "%3i:%i%i ", min, tens, sec);

	va_start(argptr, fmt);
	vsprintf(string + 7, fmt, argptr);
	va_end(argptr);

	if (g_dedicated.integer)
	{
		G_Printf("%s", string + 7);
	}

	if (!level.logFile)
	{
		return;
	}

	trap_FS_Write(string, strlen(string), level.logFile);
}

/*
================
LogExit

Append information about this game to the log file
================
*/
void LogExit(const char* string)
{
	int             i, numSorted;
	gclient_t*       cl;
#ifdef MISSIONPACK // bk001205
	qboolean won = qtrue;
#endif
	G_LogPrintf("Exit: %s\n", string);

	level.intermissionQueued = level.time;

	// this will keep the clients from playing any voice sounds
	// that will get cut off when the queued intermission starts
	trap_SetConfigstring(CS_INTERMISSION, "1");

	// don't send more than 32 scores (FIXME?)
	numSorted = level.numConnectedClients;
	if (numSorted > 32)
	{
		numSorted = 32;
	}

	if (g_gametype.integer >= GT_TEAM)
	{
		G_LogPrintf("red:%i  blue:%i\n",
		            level.teamScores[TEAM_RED], level.teamScores[TEAM_BLUE]);
	}

	for (i = 0 ; i < numSorted ; i++)
	{
		int     ping;

		cl = &level.clients[level.sortedClients[i]];

		if (cl->sess.sessionTeam == TEAM_SPECTATOR)
		{
			continue;
		}
		if (cl->pers.connected == CON_CONNECTING)
		{
			continue;
		}

		ping = cl->ps.ping < 999 ? cl->ps.ping : 999;

		G_LogPrintf("score: %i  ping: %i  client: %i %s\n", cl->ps.persistant[PERS_SCORE], ping, level.sortedClients[i],    cl->pers.netname);
#ifdef MISSIONPACK
		if (g_singlePlayer.integer && g_gametype.integer == GT_TOURNAMENT)
		{
			if (g_entities[cl - level.clients].r.svFlags & SVF_BOT && cl->ps.persistant[PERS_RANK] == 0)
			{
				won = qfalse;
			}
		}
#endif

	}

#ifdef MISSIONPACK
	if (g_singlePlayer.integer)
	{
		if (g_gametype.integer >= GT_CTF)
		{
			won = level.teamScores[TEAM_RED] > level.teamScores[TEAM_BLUE];
		}
		trap_SendConsoleCommand(EXEC_APPEND, (won) ? "spWin\n" : "spLose\n");
	}
#endif


}


/*
=================
CheckIntermissionExit

The level will stay at the intermission for a minimum of 5 seconds
If all players wish to continue, the level will then exit.
If one or more players have not acknowledged the continue, the game will
wait 10 seconds before going on.
=================
*/
void CheckIntermissionExit(void)
{
	int         ready, notReady;
	int         i;
	gclient_t*   cl;
	int         readyMask;

	if (g_gametype.integer == GT_SINGLE_PLAYER)
	{
		return;
	}

	// see which players are ready
	ready = 0;
	notReady = 0;
	readyMask = 0;
	for (i = 0 ; i < g_maxclients.integer ; i++)
	{
		cl = level.clients + i;
		if (cl->pers.connected != CON_CONNECTED)
		{
			continue;
		}
		if (g_entities[cl->ps.clientNum].r.svFlags & SVF_BOT)
		{
			continue;
		}

		if (cl->readyToExit)
		{
			ready++;
			if (i < 16)
			{
				readyMask |= 1 << i;
			}
		}
		else
		{
			notReady++;
		}
	}

	// copy the readyMask to each player's stats so
	// it can be displayed on the scoreboard
	for (i = 0 ; i < g_maxclients.integer ; i++)
	{
		cl = level.clients + i;
		if (cl->pers.connected != CON_CONNECTED)
		{
			continue;
		}
		cl->ps.stats[STAT_CLIENTS_READY] = readyMask;
	}

	// never exit in less than five seconds
	if (level.time < level.intermissiontime + 5000)
	{
		return;
	}

	// if nobody wants to go, clear timer
	if (!ready)
	{
		level.readyToExit = qfalse;
		return;
	}

	// if everyone wants to go, go now
	if (!notReady)
	{
		ExitLevel();
		return;
	}

	// the first person to ready starts the ten second timeout
	if (!level.readyToExit)
	{
		level.readyToExit = qtrue;
		level.exitTime = level.time;
	}

	// if we have waited ten seconds since at least one player
	// wanted to exit, go ahead
	if (level.time < level.exitTime + 10000)
	{
		return;
	}

	ExitLevel();
}

/*
=============
ScoreIsTied
=============
*/
qboolean ScoreIsTied(void)
{
	int     a, b;

	if (level.numPlayingClients < 2)
	{
		return qfalse;
	}

	if (g_gametype.integer >= GT_TEAM)
	{
		return level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE];
	}

	a = level.clients[level.sortedClients[0]].ps.persistant[PERS_SCORE];
	b = level.clients[level.sortedClients[1]].ps.persistant[PERS_SCORE];

	return a == b;
}

/*
=================
CheckExitRules

There will be a delay between the time the exit is qualified for
and the time everyone is moved to the intermission spot, so you
can see the last frag.
=================
*/
void CheckExitRules(void)
{
	int         i;
	gclient_t*   cl;
	// if at the intermission, wait for all non-bots to
	// signal ready, then go to next level
	if (level.intermissiontime)
	{
		CheckIntermissionExit();
		return;
	}

	if (level.intermissionQueued)
	{
#ifdef MISSIONPACK
		int time = (g_singlePlayer.integer) ? SP_INTERMISSION_DELAY_TIME : INTERMISSION_DELAY_TIME;
		if (level.time - level.intermissionQueued >= time)
		{
			level.intermissionQueued = 0;
			BeginIntermission();
		}
#else
		if (level.time - level.intermissionQueued >= INTERMISSION_DELAY_TIME)
		{
			level.intermissionQueued = 0;
			BeginIntermission();
		}
#endif
		return;
	}

	// check for sudden death
	if (ScoreIsTied())
	{
		// always wait for sudden death
		return;
	}

	if (g_timelimit.integer && !level.warmupTime)
	{
		if (level.time - level.startTime >= g_timelimit.integer * 60000)
		{
			trap_SendServerCommand(-1, "print \"Timelimit hit.\n\"");
			LogExit("Timelimit hit.");
			return;
		}
	}

	if (level.numPlayingClients < 2)
	{
		return;
	}

	if (g_gametype.integer < GT_CTF && g_fraglimit.integer)
	{
		if (level.teamScores[TEAM_RED] >= g_fraglimit.integer)
		{
			trap_SendServerCommand(-1, "print \"Red hit the fraglimit.\n\"");
			LogExit("Fraglimit hit.");
			return;
		}

		if (level.teamScores[TEAM_BLUE] >= g_fraglimit.integer)
		{
			trap_SendServerCommand(-1, "print \"Blue hit the fraglimit.\n\"");
			LogExit("Fraglimit hit.");
			return;
		}

		for (i = 0 ; i < g_maxclients.integer ; i++)
		{
			cl = level.clients + i;
			if (cl->pers.connected != CON_CONNECTED)
			{
				continue;
			}
			if (cl->sess.sessionTeam != TEAM_FREE)
			{
				continue;
			}

			if (cl->ps.persistant[PERS_SCORE] >= g_fraglimit.integer)
			{
				LogExit("Fraglimit hit.");
				trap_SendServerCommand(-1, va("print \"%s" S_COLOR_WHITE " hit the fraglimit.\n\"",
				                              cl->pers.netname));
				return;
			}
		}
	}

	if (g_gametype.integer >= GT_CTF && g_capturelimit.integer)
	{

		if (level.teamScores[TEAM_RED] >= g_capturelimit.integer)
		{
			trap_SendServerCommand(-1, "print \"Red hit the capturelimit.\n\"");
			LogExit("Capturelimit hit.");
			return;
		}

		if (level.teamScores[TEAM_BLUE] >= g_capturelimit.integer)
		{
			trap_SendServerCommand(-1, "print \"Blue hit the capturelimit.\n\"");
			LogExit("Capturelimit hit.");
			return;
		}
	}
}



/*
========================================================================

FUNCTIONS CALLED EVERY FRAME

========================================================================
*/


/*
=============
CheckTournament

Once a frame, check for changes in tournement player state
=============
*/
void CheckTournament(void)
{
	// check because we run 3 game frames before calling Connect and/or ClientBegin
	// for clients on a map_restart
	if (level.numPlayingClients == 0)
	{
		return;
	}

	if (g_gametype.integer == GT_TOURNAMENT)
	{

		// pull in a spectator if needed
		if (level.numPlayingClients < 2)
		{
			AddTournamentPlayer();
		}

		// if we don't have two players, go back to "waiting for players"
		if (level.numPlayingClients != 2)
		{
			if (level.warmupTime != -1)
			{
				level.warmupTime = -1;
				trap_SetConfigstring(CS_WARMUP, va("%i", level.warmupTime));
				G_LogPrintf("Warmup:\n");
			}
			return;
		}

		if (level.warmupTime == 0)
		{
			return;
		}

		// if the warmup is changed at the console, restart it
		if (g_warmup.modificationCount != level.warmupModificationCount)
		{
			level.warmupModificationCount = g_warmup.modificationCount;
			level.warmupTime = -1;
		}

		// if all players have arrived, start the countdown
		if (level.warmupTime < 0)
		{
			if (level.numPlayingClients == 2)
			{
				// fudge by -1 to account for extra delays
				level.warmupTime = level.time + (g_warmup.integer - 1) * 1000;
				trap_SetConfigstring(CS_WARMUP, va("%i", level.warmupTime));
			}
			return;
		}

		// if the warmup time has counted down, restart
		if (level.time > level.warmupTime)
		{
			level.warmupTime += 10000;
			trap_Cvar_Set("g_restarted", "1");
			trap_SendConsoleCommand(EXEC_APPEND, "map_restart 0\n");
			level.restarted = qtrue;
			return;
		}
	}
	else if (g_gametype.integer != GT_SINGLE_PLAYER && level.warmupTime != 0)
	{
		int     counts[TEAM_NUM_TEAMS];
		qboolean    notEnough = qfalse;

		if (g_gametype.integer > GT_TEAM)
		{
			counts[TEAM_BLUE] = TeamCount(-1, TEAM_BLUE);
			counts[TEAM_RED] = TeamCount(-1, TEAM_RED);

			if (counts[TEAM_RED] < 1 || counts[TEAM_BLUE] < 1)
			{
				notEnough = qtrue;
			}
		}
		else if (level.numPlayingClients < 2)
		{
			notEnough = qtrue;
		}

		if (notEnough)
		{
			if (level.warmupTime != -1)
			{
				level.warmupTime = -1;
				trap_SetConfigstring(CS_WARMUP, va("%i", level.warmupTime));
				G_LogPrintf("Warmup:\n");
			}
			return; // still waiting for team members
		}

		if (level.warmupTime == 0)
		{
			return;
		}

		// if the warmup is changed at the console, restart it
		if (g_warmup.modificationCount != level.warmupModificationCount)
		{
			level.warmupModificationCount = g_warmup.modificationCount;
			level.warmupTime = -1;
		}

		// if all players have arrived, start the countdown
		if (level.warmupTime < 0)
		{
			// fudge by -1 to account for extra delays
			level.warmupTime = level.time + (g_warmup.integer - 1) * 1000;
			trap_SetConfigstring(CS_WARMUP, va("%i", level.warmupTime));
			return;
		}

		// if the warmup time has counted down, restart
		if (level.time > level.warmupTime)
		{
			level.warmupTime += 10000;
			trap_Cvar_Set("g_restarted", "1");
			trap_SendConsoleCommand(EXEC_APPEND, "map_restart 0\n");
			level.restarted = qtrue;
			return;
		}
	}
}


/*
==================
CheckVote
==================
*/
void CheckVote(void)
{
	if (level.voteExecuteTime && level.voteExecuteTime < level.time)
	{
		level.voteExecuteTime = 0;
		trap_SendConsoleCommand(EXEC_APPEND, va("%s\n", level.voteString));
	}
	if (!level.voteTime)
	{
		return;
	}
	if (level.time - level.voteTime >= VOTE_TIME)
	{
		trap_SendServerCommand(-1, "print \"Vote failed.\n\"");
	}
	else
	{
		// ATVI Q3 1.32 Patch #9, WNF
		if (level.voteYes > level.numVotingClients / 2)
		{
			// execute the command, then remove the vote
			trap_SendServerCommand(-1, "print \"Vote passed.\n\"");
			level.voteExecuteTime = level.time + 3000;
		}
		else if (level.voteNo >= level.numVotingClients / 2)
		{
			// same behavior as a timeout
			trap_SendServerCommand(-1, "print \"Vote failed.\n\"");
		}
		else
		{
			// still waiting for a majority
			return;
		}
	}
	level.voteTime = 0;
	trap_SetConfigstring(CS_VOTE_TIME, "");

}

/*
==================
PrintTeam
==================
*/
void PrintTeam(int team, char* message)
{
	int i;

	for (i = 0 ; i < level.maxclients ; i++)
	{
		if (level.clients[i].sess.sessionTeam != team)
			continue;
		trap_SendServerCommand(i, message);
	}
}

/*
==================
SetLeader
==================
*/
void SetLeader(int team, int client)
{
	int i;

	if (level.clients[client].pers.connected == CON_DISCONNECTED)
	{
		PrintTeam(team, va("print \"%s is not connected\n\"", level.clients[client].pers.netname));
		return;
	}
	if (level.clients[client].sess.sessionTeam != team)
	{
		PrintTeam(team, va("print \"%s is not on the team anymore\n\"", level.clients[client].pers.netname));
		return;
	}
	for (i = 0 ; i < level.maxclients ; i++)
	{
		if (level.clients[i].sess.sessionTeam != team)
			continue;
		if (level.clients[i].sess.teamLeader)
		{
			level.clients[i].sess.teamLeader = qfalse;
			ClientUserinfoChanged(i);
		}
	}
	level.clients[client].sess.teamLeader = qtrue;
	ClientUserinfoChanged(client);
	PrintTeam(team, va("print \"%s is the new team leader\n\"", level.clients[client].pers.netname));
}

/*
==================
CheckTeamLeader
==================
*/
void CheckTeamLeader(int team)
{
	int i;

	for (i = 0 ; i < level.maxclients ; i++)
	{
		if (level.clients[i].sess.sessionTeam != team)
			continue;
		if (level.clients[i].sess.teamLeader)
			break;
	}
	if (i >= level.maxclients)
	{
		for (i = 0 ; i < level.maxclients ; i++)
		{
			if (level.clients[i].sess.sessionTeam != team)
				continue;
			if (!(g_entities[i].r.svFlags & SVF_BOT))
			{
				level.clients[i].sess.teamLeader = qtrue;
				break;
			}
		}
		for (i = 0 ; i < level.maxclients ; i++)
		{
			if (level.clients[i].sess.sessionTeam != team)
				continue;
			level.clients[i].sess.teamLeader = qtrue;
			break;
		}
	}
}

/*
==================
CheckTeamVote
==================
*/
void CheckTeamVote(int team)
{
	int cs_offset;

	if (team == TEAM_RED)
		cs_offset = 0;
	else if (team == TEAM_BLUE)
		cs_offset = 1;
	else
		return;

	if (!level.teamVoteTime[cs_offset])
	{
		return;
	}
	if (level.time - level.teamVoteTime[cs_offset] >= VOTE_TIME)
	{
		trap_SendServerCommand(-1, "print \"Team vote failed.\n\"");
	}
	else
	{
		if (level.teamVoteYes[cs_offset] > level.numteamVotingClients[cs_offset] / 2)
		{
			// execute the command, then remove the vote
			trap_SendServerCommand(-1, "print \"Team vote passed.\n\"");
			//
			if (!Q_strncmp("leader", level.teamVoteString[cs_offset], 6))
			{
				//set the team leader
				SetLeader(team, atoi(level.teamVoteString[cs_offset] + 7));
			}
			else
			{
				trap_SendConsoleCommand(EXEC_APPEND, va("%s\n", level.teamVoteString[cs_offset]));
			}
		}
		else if (level.teamVoteNo[cs_offset] >= level.numteamVotingClients[cs_offset] / 2)
		{
			// same behavior as a timeout
			trap_SendServerCommand(-1, "print \"Team vote failed.\n\"");
		}
		else
		{
			// still waiting for a majority
			return;
		}
	}
	level.teamVoteTime[cs_offset] = 0;
	trap_SetConfigstring(CS_TEAMVOTE_TIME + cs_offset, "");

}


/*
==================
CheckCvars
==================
*/
void CheckCvars(void)
{
	static int lastMod = -1;

	if (g_password.modificationCount != lastMod)
	{
		lastMod = g_password.modificationCount;
		if (*g_password.string && Q_stricmp(g_password.string, "none"))
		{
			trap_Cvar_Set("g_needpass", "1");
		}
		else
		{
			trap_Cvar_Set("g_needpass", "0");
		}
	}
}

/*
=============
G_RunThink

Runs thinking code for this frame if necessary
=============
*/
void G_RunThink(gentity_t* ent)
{
	float   thinktime;

	thinktime = ent->nextthink;
	if (thinktime <= 0)
	{
		return;
	}
	if (thinktime > level.time)
	{
		return;
	}

	ent->nextthink = 0;
	if (!ent->think)
	{
		G_Error("NULL ent->think");
	}
	ent->think(ent);
}

/*
================
G_RunFrame

Advances the non-player objects in the world
================
*/
void G_RunFrame(int levelTime)
{
	int         i;
	gentity_t*   ent;
	int         msec;
	int start, end;

	// if we are waiting for the level to restart, do nothing
	if (level.restarted)
	{
		return;
	}

	level.framenum++;
	level.previousTime = level.time;
	level.time = levelTime;
	msec = level.time - level.previousTime;

	// get any cvar changes
	G_UpdateCvars();

	//
	// go through all allocated objects
	//
	start = trap_Milliseconds();
	ent = &g_entities[0];
	for (i = 0 ; i < level.num_entities ; i++, ent++)
	{
		if (!ent->inuse)
		{
			continue;
		}

		// clear events that are too old
		if (level.time - ent->eventTime > EVENT_VALID_MSEC)
		{
			if (ent->s.event)
			{
				ent->s.event = 0;   // &= EV_EVENT_BITS;
				if (ent->client)
				{
					ent->client->ps.externalEvent = 0;
					// predicted events should never be set to zero
					//ent->client->ps.events[0] = 0;
					//ent->client->ps.events[1] = 0;
				}
			}
			if (ent->freeAfterEvent)
			{
				// tempEntities or dropped items completely go away after their event
				G_FreeEntity(ent);
				continue;
			}
			else if (ent->unlinkAfterEvent)
			{
				// items that will respawn will hide themselves after their pickup event
				ent->unlinkAfterEvent = qfalse;
				trap_UnlinkEntity(ent);
			}
		}

		// temporary entities don't think
		if (ent->freeAfterEvent)
		{
			continue;
		}

		if (!ent->r.linked && ent->neverFree)
		{
			continue;
		}

		if (ent->s.eType == ET_MISSILE)
		{
			G_RunMissile(ent);
			continue;
		}

		if (ent->s.eType == ET_ITEM || ent->physicsObject)
		{
			G_RunItem(ent);
			continue;
		}

		if (ent->s.eType == ET_MOVER)
		{
			G_RunMover(ent);
			continue;
		}

		if (i < MAX_CLIENTS)
		{
			G_RunClient(ent);
			continue;
		}

		G_RunThink(ent);
	}
	end = trap_Milliseconds();

	start = trap_Milliseconds();
	// perform final fixups on the players
	ent = &g_entities[0];
	for (i = 0 ; i < level.maxclients ; i++, ent++)
	{
		if (ent->inuse)
		{
			ClientEndFrame(ent);
		}
	}
	end = trap_Milliseconds();

	// see if it is time to do a tournement restart
	CheckTournament();

	// see if it is time to end the level
	CheckExitRules();

	// update to team status?
	CheckTeamStatus();

	// cancel vote if timed out
	CheckVote();

	// check team votes
	CheckTeamVote(TEAM_RED);
	CheckTeamVote(TEAM_BLUE);

	// for tracking changes
	CheckCvars();

	if (g_listEntity.integer)
	{
		for (i = 0; i < MAX_GENTITIES; i++)
		{
			G_Printf("%4i: %s\n", i, g_entities[i].classname);
		}
		trap_Cvar_Set("g_listEntity", "0");
	}
}



