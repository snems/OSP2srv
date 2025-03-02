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
#include "g_unimplemented.h"

#include "../../ui/menudef.h"           // for the voice chats
#include "q_shared.h"
//
static const char* cmdHelp[] =
{
	":^7 Locks/unlocks a player's team"
	, ":^7 Locks/unlocks a player's team from spectators"
	, ":^7 Toggles option to stay as a spectator in 1v1"
	, ":^7 Sets your status to ^5ready^7 to start a match"
	, ":^7 Lists all active players and their IDs/information"
	, " <player_ID>:^7 Invites a player to join a team"
	, ":^7 Invites a player to spectate a spec-locked team"
	, " <player_ID>:^7 Invites a player to coach team"
	, ":^7 Declines coach invitation or resigns coach status"
	, " <player_ID>:^7 Kicks active coach from team"
	, ":^7 Accepts coach invitation/restarts coach view"
	, " <player_ID>:^7 Removes a player from the team"
	, " [player_ID]:^7 Resigns captainship.  Can optionally be given to another teammate"
	, ":^7 Sets an entire team's status to ^5ready^7 to start a match"
	, ":^7 Shows team captains"
	, ":^7 Pause the match or issue a team timeout/timein"
	, " [player_ID]:^7 Shows weapon accuracy stats for a player"
	, " [player_ID]:^7 Shows player stats + match info saved locally to a file"
	, " [password]:^7 Shows/sets team joincode or used to join a locked team"
	, " <item>:^7 Drops available weapon/ammo/flag"
	, ":^7 Shows BEST player for each weapon. Add ^3<weapon_ID>^7 to show all stats for a weapon"
	, ":^7 Creates a demo with a consistant naming scheme (OSP clients only)"
	, ":^7 Creates a screenshot with a consistant naming scheme (OSP clients only)"
	, ":^7 Shows WORST player for each weapon. Add ^3<weapon_ID>^7 to show all stats for a weapon"
	, " <password>:^7 Become a referee (admin access)"
	, ":^7 Switches to cinematic camera mode"
	, " [player_ID]:^7 Puts viewcam in follow mode.  Can optionally to follow a specific player"
	, ":^7 Toggle viewcam between manual/automatic change"
	, ":^7 Toggle ViewCam between static/dynamic views"
	, " <player_ID>:^7 Adds a player to multi-screen view"
	, ":^7 Adds all active players to a multi-screen view"
	, " [player_ID]:^7 Removes current selected or specific player from multi-screen view"
	, ":^7 Disables multiview mode and goes back to spectator mode"
	, ":^7 Moves through active screen in a multi-screen display"
	, ":^7 Cycles through players in current view"
	, ":^7 Follows current highlighted view"
	, ": ^7 Views entire red/blue team"
	, " <r|b|s|none>:^7 Chooses a team or shows current team (s = spectator mode)"
	, " <player_ID|red|blue>:^7 Spectates a particular player or team"
	, ":^7 Sets player's status to ^5not ready^7 to start a match"
	, " <params>:^7 Calls a vote"
	, ":^7 Gives a list of most OSP commands"
};

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

/*
==================
DeathmatchScoreboardMessage

==================
*/
void DeathmatchScoreboardMessage(gentity_t* ent)
{
	char        entry[1024];
	char        string[1400];
	int         stringlength;
	int         i, j;
	gclient_t*   cl;
	int         numSorted, scoreFlags, accuracy, perfect;

	// send the latest information on all clients
	string[0] = 0;
	stringlength = 0;
	scoreFlags = 0;

	numSorted = level.numConnectedClients;

	for (i = 0 ; i < numSorted ; i++)
	{
		int     ping;

		cl = &level.clients[level.sortedClients[i]];

		if (cl->pers.connected == CON_CONNECTING)
		{
			ping = -1;
		}
		else
		{
			ping = cl->ps.ping < 999 ? cl->ps.ping : 999;
		}

		if (cl->accuracy_shots)
		{
			accuracy = cl->accuracy_hits * 100 / cl->accuracy_shots;
		}
		else
		{
			accuracy = 0;
		}
		perfect = (cl->ps.persistant[PERS_RANK] == 0 && cl->ps.persistant[PERS_KILLED] == 0) ? 1 : 0;

		Com_sprintf(entry, sizeof(entry),
		            " %i %i %i %i %i %i %i %i %i %i %i %i %i %i", level.sortedClients[i],
		            cl->ps.persistant[PERS_SCORE], ping, (level.time - cl->pers.enterTime) / 60000,
		            scoreFlags, g_entities[level.sortedClients[i]].s.powerups, accuracy,
		            cl->ps.persistant[PERS_IMPRESSIVE_COUNT],
		            cl->ps.persistant[PERS_EXCELLENT_COUNT],
		            cl->ps.persistant[PERS_GAUNTLET_FRAG_COUNT],
		            cl->ps.persistant[PERS_DEFEND_COUNT],
		            cl->ps.persistant[PERS_ASSIST_COUNT],
		            perfect,
		            cl->ps.persistant[PERS_CAPTURES]);
		j = strlen(entry);
		if (stringlength + j > 1024)
			break;
		strcpy(string + stringlength, entry);
		stringlength += j;
	}

	trap_SendServerCommand(ent - g_entities, va("scores %i %i %i%s", i,
	                       level.teamScores[TEAM_RED], level.teamScores[TEAM_BLUE],
	                       string));
}


/*
==================
Cmd_Score_f

Request current scoreboard information
==================
*/
void Cmd_Score_f(gentity_t* ent)
{
	DeathmatchScoreboardMessage(ent);
}



/*
==================
CheatsOk
==================
*/
qboolean    CheatsOk(gentity_t* ent)
{
	if (!g_cheats.integer)
	{
		trap_SendServerCommand(ent - g_entities, va("print \"Cheats are not enabled on this server.\n\""));
		return qfalse;
	}
	if (ent->health <= 0)
	{
		trap_SendServerCommand(ent - g_entities, va("print \"You must be alive to use this command.\n\""));
		return qfalse;
	}
	return qtrue;
}


/*
==================
ConcatArgs
==================
*/
char*    ConcatArgs(int start)
{
	int     i, c, tlen;
	static char line[MAX_STRING_CHARS];
	int     len;
	char    arg[MAX_STRING_CHARS];

	len = 0;
	c = trap_Argc();
	for (i = start ; i < c ; i++)
	{
		trap_Argv(i, arg, sizeof(arg));
		tlen = strlen(arg);
		if (len + tlen >= MAX_STRING_CHARS - 1)
		{
			break;
		}
		memcpy(line + len, arg, tlen);
		len += tlen;
		if (i != c - 1)
		{
			line[len] = ' ';
			len++;
		}
	}

	line[len] = 0;

	return line;
}

/*
==================
SanitizeString

Remove case and control characters
==================
*/
static void SanitizeString(const char* in, char* out, qboolean toLower)
{
	char tmp;
	while (*in)
	{
		tmp = *in;                                                          /* Address : 0x1ea57 Type : Interium */
		if (tmp == 27 || tmp == Q_COLOR_ESCAPE)
		{
			tmp = *(++in);
			if (tmp == 'x' || tmp == 'X')
			{
				tmp = *(in + 1);
				if ((tmp >= '0' && tmp <= '9') || (tmp >= 'A' && tmp <= 'F') || (tmp >= 'a' && tmp <= 'f'))
				{
					tmp = *(in + 2);
					if ((tmp >= '0' && tmp <= '9') || (tmp >= 'A' && tmp <= 'F') || (tmp >= 'a' && tmp <= 'f'))
					{
						tmp = *(in + 3);
						if ((tmp >= '0' && tmp <= '9') || (tmp >= 'A' && tmp <= 'F') || (tmp >= 'a' && tmp <= 'f'))
						{
							tmp = *(in + 4);
							if ((tmp >= '0' && tmp <= '9') || (tmp >= 'A' && tmp <= 'F') || (tmp >= 'a' && tmp <= 'f'))
							{
								tmp = *(in + 5);
								if ((tmp >= '0' && tmp <= '9') || (tmp >= 'A' && tmp <= 'F') || (tmp >= 'a' && tmp <= 'f'))
								{
									tmp = *(in + 6);
									if ((tmp >= '0' && tmp <= '9') || (tmp >= 'A' && tmp <= 'F') || (tmp >= 'a' && tmp <= 'f'))
									{
										in = in + 7;
									}
								}
							}
						}
					}
				}
			}
			else if (*in)
			{
				++in;
			}
		}
		else if (*in < ' ')
		{
			++in;
		}
		else
		{
			if (toLower)
			{
				*(out++) = tolower(*(in++));
			}
			else
			{
				*(out++) = *(in++);
			}
		}
	}

	*out = 0;                                                                   /* Address : 0x1ec27 Type : Interium */
}

/*
==================
ClientNumberFromString

Returns a player number for either a number or name string
Returns -1 if invalid
==================
*/
int ClientNumberFromString(gentity_t* to, char* s)
{
	gclient_t*   cl;
	int         idnum;
	char        s2[MAX_STRING_CHARS];
	char        n2[MAX_STRING_CHARS];

	// numeric values are just slot numbers
	if (s[0] >= '0' && s[0] <= '9')
	{
		idnum = atoi(s);
		if (idnum < 0 || idnum >= level.maxclients)
		{
			trap_SendServerCommand(to - g_entities, va("print \"Bad client slot: %i\n\"", idnum));
			return -1;
		}

		cl = &level.clients[idnum];
		if (cl->pers.connected != CON_CONNECTED)
		{
			trap_SendServerCommand(to - g_entities, va("print \"Client %i is not active\n\"", idnum));
			return -1;
		}
		return idnum;
	}

	// check for a name match
	SanitizeString(s, s2, qtrue);
	for (idnum = 0, cl = level.clients ; idnum < level.maxclients ; idnum++, cl++)
	{
		if (cl->pers.connected != CON_CONNECTED)
		{
			continue;
		}
		SanitizeString(cl->pers.netname, n2, qtrue);
		if (!strcmp(n2, s2))
		{
			return idnum;
		}
	}

	trap_SendServerCommand(to - g_entities, va("print \"User %s is not on the server\n\"", s));
	return -1;
}

/*
==================
Cmd_Give_f

Give items to a client
==================
*/
void Cmd_Give_f(gentity_t* ent)
{
	char*        name;
	gitem_t*     it;
	int         i;
	qboolean    give_all;
	gentity_t*       it_ent;
	trace_t     trace;

	if (!CheatsOk(ent))
	{
		return;
	}

	name = ConcatArgs(1);

	if (Q_stricmp(name, "all") == 0)
		give_all = qtrue;
	else
		give_all = qfalse;

	if (give_all || Q_stricmp(name, "health") == 0)
	{
		ent->health = ent->client->ps.stats[STAT_MAX_HEALTH];
		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp(name, "weapons") == 0)
	{
		ent->client->ps.stats[STAT_WEAPONS] = (1 << WP_NUM_WEAPONS) - 1 -
		                                      (1 << WP_GRAPPLING_HOOK) - (1 << WP_NONE);
		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp(name, "ammo") == 0)
	{
		for (i = 0 ; i < MAX_WEAPONS ; i++)
		{
			ent->client->ps.ammo[i] = 999;
		}
		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp(name, "armor") == 0)
	{
		ent->client->ps.stats[STAT_ARMOR] = 200;

		if (!give_all)
			return;
	}

	if (Q_stricmp(name, "excellent") == 0)
	{
		ent->client->ps.persistant[PERS_EXCELLENT_COUNT]++;
		return;
	}
	if (Q_stricmp(name, "impressive") == 0)
	{
		ent->client->ps.persistant[PERS_IMPRESSIVE_COUNT]++;
		return;
	}
	if (Q_stricmp(name, "gauntletaward") == 0)
	{
		ent->client->ps.persistant[PERS_GAUNTLET_FRAG_COUNT]++;
		return;
	}
	if (Q_stricmp(name, "defend") == 0)
	{
		ent->client->ps.persistant[PERS_DEFEND_COUNT]++;
		return;
	}
	if (Q_stricmp(name, "assist") == 0)
	{
		ent->client->ps.persistant[PERS_ASSIST_COUNT]++;
		return;
	}

	// spawn a specific item right on the player
	if (!give_all)
	{
		it = BG_FindItem(name);
		if (!it)
		{
			return;
		}

		it_ent = G_Spawn();
		VectorCopy(ent->r.currentOrigin, it_ent->s.origin);
		it_ent->classname = it->classname;
		G_SpawnItem(it_ent, it);
		FinishSpawningItem(it_ent);
		memset(&trace, 0, sizeof(trace));
		Touch_Item(it_ent, ent, &trace);
		if (it_ent->inuse)
		{
			G_FreeEntity(it_ent);
		}
	}
}


/*
==================
Cmd_God_f

Sets client to godmode

argv(0) god
==================
*/
void Cmd_God_f(gentity_t* ent)
{
	char*    msg;

	if (!CheatsOk(ent))
	{
		return;
	}

	ent->flags ^= FL_GODMODE;
	if (!(ent->flags & FL_GODMODE))
		msg = "godmode OFF\n";
	else
		msg = "godmode ON\n";

	trap_SendServerCommand(ent - g_entities, va("print \"%s\"", msg));
}


/*
==================
Cmd_Notarget_f

Sets client to notarget

argv(0) notarget
==================
*/
void Cmd_Notarget_f(gentity_t* ent)
{
	char*    msg;

	if (!CheatsOk(ent))
	{
		return;
	}

	ent->flags ^= FL_NOTARGET;
	if (!(ent->flags & FL_NOTARGET))
		msg = "notarget OFF\n";
	else
		msg = "notarget ON\n";

	trap_SendServerCommand(ent - g_entities, va("print \"%s\"", msg));
}


/*
==================
Cmd_Noclip_f

argv(0) noclip
==================
*/
void Cmd_Noclip_f(gentity_t* ent)
{
	char*    msg;

	if (!CheatsOk(ent))
	{
		return;
	}

	if (ent->client->noclip)
	{
		msg = "noclip OFF\n";
	}
	else
	{
		msg = "noclip ON\n";
	}
	ent->client->noclip = !ent->client->noclip;

	trap_SendServerCommand(ent - g_entities, va("print \"%s\"", msg));
}


/*
==================
Cmd_LevelShot_f

This is just to help generate the level pictures
for the menus.  It goes to the intermission immediately
and sends over a command to the client to resize the view,
hide the scoreboard, and take a special screenshot
==================
*/
void Cmd_LevelShot_f(gentity_t* ent)
{
	if (!CheatsOk(ent))
	{
		return;
	}

	// doesn't work in single player
	if (g_gametype.integer != 0)
	{
		trap_SendServerCommand(ent - g_entities,
		                       "print \"Must be in g_gametype 0 for levelshot\n\"");
		return;
	}

	BeginIntermission();
	trap_SendServerCommand(ent - g_entities, "clientLevelShot");
}


/*
==================
Cmd_LevelShot_f

This is just to help generate the level pictures
for the menus.  It goes to the intermission immediately
and sends over a command to the client to resize the view,
hide the scoreboard, and take a special screenshot
==================
*/
void Cmd_TeamTask_f(gentity_t* ent)
{
	char userinfo[MAX_INFO_STRING];
	char        arg[MAX_TOKEN_CHARS];
	int task;
	int client = ent->client - level.clients;

	if (trap_Argc() != 2)
	{
		return;
	}
	trap_Argv(1, arg, sizeof(arg));
	task = atoi(arg);

	trap_GetUserinfo(client, userinfo, sizeof(userinfo));
	Info_SetValueForKey(userinfo, "teamtask", va("%d", task));
	trap_SetUserinfo(client, userinfo);
	ClientUserinfoChanged(client);
}



/*
=================
Cmd_Kill_f
=================
*/
void Cmd_Kill_f(gentity_t* ent)
{
	if (ent->client->sess.sessionTeam == TEAM_SPECTATOR)
	{
		return;
	}
	if (ent->health <= 0)
	{
		return;
	}
	ent->flags &= ~FL_GODMODE;
	ent->client->ps.stats[STAT_HEALTH] = ent->health = -999;
	player_die(ent, ent, ent, 100000, MOD_SUICIDE);
}

/*
=================
BroadCastTeamChange

Let everyone know about a team change
=================
*/
void BroadcastTeamChange(gclient_t* client, int oldTeam)
{
	if (client->sess.sessionTeam == TEAM_RED)
	{
		trap_SendServerCommand(-1, va("cp \"%s" S_COLOR_WHITE " joined the red team.\n\"",
		                              client->pers.netname));
	}
	else if (client->sess.sessionTeam == TEAM_BLUE)
	{
		trap_SendServerCommand(-1, va("cp \"%s" S_COLOR_WHITE " joined the blue team.\n\"",
		                              client->pers.netname));
	}
	else if (client->sess.sessionTeam == TEAM_SPECTATOR && oldTeam != TEAM_SPECTATOR)
	{
		trap_SendServerCommand(-1, va("cp \"%s" S_COLOR_WHITE " joined the spectators.\n\"",
		                              client->pers.netname));
	}
	else if (client->sess.sessionTeam == TEAM_FREE)
	{
		trap_SendServerCommand(-1, va("cp \"%s" S_COLOR_WHITE " joined the battle.\n\"",
		                              client->pers.netname));
	}
}

/*
=================
SetTeam
=================
*/
void SetTeam(gentity_t* ent, char* s)
{
	int                 team, oldTeam;
	gclient_t*           client;
	int                 clientNum;
	spectatorState_t    specState;
	int                 specClient;
	int                 teamLeader;

	//
	// see what change is requested
	//
	client = ent->client;

	clientNum = client - level.clients;
	specClient = 0;
	specState = SPECTATOR_NOT;
	if (!Q_stricmp(s, "scoreboard") || !Q_stricmp(s, "score"))
	{
		team = TEAM_SPECTATOR;
		specState = SPECTATOR_SCOREBOARD;
	}
	else if (!Q_stricmp(s, "follow1"))
	{
		team = TEAM_SPECTATOR;
		specState = SPECTATOR_FOLLOW;
		specClient = -1;
	}
	else if (!Q_stricmp(s, "follow2"))
	{
		team = TEAM_SPECTATOR;
		specState = SPECTATOR_FOLLOW;
		specClient = -2;
	}
	else if (!Q_stricmp(s, "spectator") || !Q_stricmp(s, "s"))
	{
		team = TEAM_SPECTATOR;
		specState = SPECTATOR_FREE;
	}
	else if (g_gametype.integer >= GT_TEAM)
	{
		// if running a team game, assign player to one of the teams
		specState = SPECTATOR_NOT;
		if (!Q_stricmp(s, "red") || !Q_stricmp(s, "r"))
		{
			team = TEAM_RED;
		}
		else if (!Q_stricmp(s, "blue") || !Q_stricmp(s, "b"))
		{
			team = TEAM_BLUE;
		}
		else
		{
			// pick the team with the least number of players
			team = PickTeam(clientNum);
		}

		if (g_teamForceBalance.integer)
		{
			int     counts[TEAM_NUM_TEAMS];

			counts[TEAM_BLUE] = TeamCount(ent->client->ps.clientNum, TEAM_BLUE);
			counts[TEAM_RED] = TeamCount(ent->client->ps.clientNum, TEAM_RED);

			// We allow a spread of two
			if (team == TEAM_RED && counts[TEAM_RED] - counts[TEAM_BLUE] > 1)
			{
				trap_SendServerCommand(ent->client->ps.clientNum,
				                       "cp \"Red team has too many players.\n\"");
				return; // ignore the request
			}
			if (team == TEAM_BLUE && counts[TEAM_BLUE] - counts[TEAM_RED] > 1)
			{
				trap_SendServerCommand(ent->client->ps.clientNum,
				                       "cp \"Blue team has too many players.\n\"");
				return; // ignore the request
			}

			// It's ok, the team we are switching to has less or same number of players
		}

	}
	else
	{
		// force them to spectators if there aren't any spots free
		team = TEAM_FREE;
	}

	// override decision if limiting the players
	if ((g_gametype.integer == GT_TOURNAMENT)
	        && level.numNonSpectatorClients >= 2)
	{
		team = TEAM_SPECTATOR;
	}
	else if (g_maxGameClients.integer > 0 &&
	         level.numNonSpectatorClients >= g_maxGameClients.integer)
	{
		team = TEAM_SPECTATOR;
	}

	//
	// decide if we will allow the change
	//
	oldTeam = client->sess.sessionTeam;
	if (team == oldTeam && team != TEAM_SPECTATOR)
	{
		return;
	}

	//
	// execute the team change
	//

	// if the player was dead leave the body
	if (client->ps.stats[STAT_HEALTH] <= 0)
	{
		CopyToBodyQue(ent);
	}

	// he starts at 'base'
	client->pers.teamState.state = TEAM_BEGIN;
	if (oldTeam != TEAM_SPECTATOR)
	{
		// Kill him (makes sure he loses flags, etc)
		ent->flags &= ~FL_GODMODE;
		ent->client->ps.stats[STAT_HEALTH] = ent->health = 0;
		player_die(ent, ent, ent, 100000, MOD_SUICIDE);

	}
	// they go to the end of the line for tournements
	if (team == TEAM_SPECTATOR)
	{
		client->sess.spectatorTime = level.time;
	}

	client->sess.sessionTeam = team;
	client->sess.spectatorState = specState;
	client->sess.spectatorClient = specClient;

	client->sess.teamLeader = qfalse;
	if (team == TEAM_RED || team == TEAM_BLUE)
	{
		teamLeader = TeamLeader(team);
		// if there is no team leader or the team leader is a bot and this client is not a bot
		if (teamLeader == -1 || (!(g_entities[clientNum].r.svFlags & SVF_BOT) && (g_entities[teamLeader].r.svFlags & SVF_BOT)))
		{
			SetLeader(team, clientNum);
		}
	}
	// make sure there is a team leader on the team the player came from
	if (oldTeam == TEAM_RED || oldTeam == TEAM_BLUE)
	{
		CheckTeamLeader(oldTeam);
	}

	BroadcastTeamChange(client, oldTeam);

	// get and distribute relevent paramters
	ClientUserinfoChanged(clientNum);

	ClientBegin(clientNum);
}

/*
=================
StopFollowing

If the client being followed leaves the game, or you just want to drop
to free floating spectator mode
=================
*/
void StopFollowing(gentity_t* ent)
{
	ent->client->ps.persistant[ PERS_TEAM ] = TEAM_SPECTATOR;
	ent->client->sess.sessionTeam = TEAM_SPECTATOR;
	ent->client->sess.spectatorState = SPECTATOR_FREE;
	ent->client->ps.pm_flags &= ~PMF_FOLLOW;
	ent->r.svFlags &= ~SVF_BOT;
	ent->client->ps.clientNum = ent - g_entities;
}

/*
=================
Cmd_Team_f
=================
*/
void Cmd_Team_f(gentity_t* ent)
{
	int         oldTeam;
	char        s[MAX_TOKEN_CHARS];

	if (trap_Argc() != 2)
	{
		oldTeam = ent->client->sess.sessionTeam;
		switch (oldTeam)
		{
			case TEAM_BLUE:
				trap_SendServerCommand(ent - g_entities, "print \"Blue team\n\"");
				break;
			case TEAM_RED:
				trap_SendServerCommand(ent - g_entities, "print \"Red team\n\"");
				break;
			case TEAM_FREE:
				trap_SendServerCommand(ent - g_entities, "print \"Free team\n\"");
				break;
			case TEAM_SPECTATOR:
				trap_SendServerCommand(ent - g_entities, "print \"Spectator team\n\"");
				break;
		}
		return;
	}

	if (ent->client->switchTeamTime > level.time)
	{
		trap_SendServerCommand(ent - g_entities, "print \"May not switch teams more than once per 5 seconds.\n\"");
		return;
	}

	// if they are playing a tournement game, count as a loss
	if ((g_gametype.integer == GT_TOURNAMENT)
	        && ent->client->sess.sessionTeam == TEAM_FREE)
	{
		ent->client->sess.losses++;
	}

	trap_Argv(1, s, sizeof(s));

	SetTeam(ent, s);

	ent->client->switchTeamTime = level.time + 5000;
}


/*
=================
Cmd_Follow_f
=================
*/
void Cmd_Follow_f(gentity_t* ent)
{
	int     i;
	char    arg[MAX_TOKEN_CHARS];

	if (trap_Argc() != 2)
	{
		if (ent->client->sess.spectatorState == SPECTATOR_FOLLOW)
		{
			StopFollowing(ent);
		}
		return;
	}

	trap_Argv(1, arg, sizeof(arg));
	i = ClientNumberFromString(ent, arg);
	if (i == -1)
	{
		return;
	}

	// can't follow self
	if (&level.clients[ i ] == ent->client)
	{
		return;
	}

	// can't follow another spectator
	if (level.clients[ i ].sess.sessionTeam == TEAM_SPECTATOR)
	{
		return;
	}

	// if they are playing a tournement game, count as a loss
	if ((g_gametype.integer == GT_TOURNAMENT)
	        && ent->client->sess.sessionTeam == TEAM_FREE)
	{
		ent->client->sess.losses++;
	}

	// first set them to spectator
	if (ent->client->sess.sessionTeam != TEAM_SPECTATOR)
	{
		SetTeam(ent, "spectator");
	}

	ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
	ent->client->sess.spectatorClient = i;
}

/*
=================
Cmd_FollowCycle_f
=================
*/
void Cmd_FollowCycle_f(gentity_t* ent, int dir)
{
	int     clientnum;
	int     original;

	// if they are playing a tournement game, count as a loss
	if ((g_gametype.integer == GT_TOURNAMENT)
	        && ent->client->sess.sessionTeam == TEAM_FREE)
	{
		ent->client->sess.losses++;
	}
	// first set them to spectator
	if (ent->client->sess.spectatorState == SPECTATOR_NOT)
	{
		SetTeam(ent, "spectator");
	}

	if (dir != 1 && dir != -1)
	{
		G_Error("Cmd_FollowCycle_f: bad dir %i", dir);
	}

	clientnum = ent->client->sess.spectatorClient;
	original = clientnum;
	do
	{
		clientnum += dir;
		if (clientnum >= level.maxclients)
		{
			clientnum = 0;
		}
		if (clientnum < 0)
		{
			clientnum = level.maxclients - 1;
		}

		// can only follow connected clients
		if (level.clients[ clientnum ].pers.connected != CON_CONNECTED)
		{
			continue;
		}

		// can't follow another spectator
		if (level.clients[ clientnum ].sess.sessionTeam == TEAM_SPECTATOR)
		{
			continue;
		}

		// this is good, we can use it
		ent->client->sess.spectatorClient = clientnum;
		ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
		return;
	}
	while (clientnum != original);

	// leave it where it was
}


/*
==================
G_Say
==================
*/

static void G_SayTo(gentity_t* ent, gentity_t* other, int mode, int color, const char* name, const char* message)
{
	if (!other)
	{
		return;
	}
	if (!other->inuse)
	{
		return;
	}
	if (!other->client)
	{
		return;
	}
	if (other->client->pers.connected != CON_CONNECTED)
	{
		return;
	}
	if (mode == SAY_TEAM  && !OnSameTeam(ent, other))
	{
		return;
	}
	// no chatting to players in tournements
	if ((g_gametype.integer == GT_TOURNAMENT)
	        && other->client->sess.sessionTeam == TEAM_FREE
	        && ent->client->sess.sessionTeam != TEAM_FREE)
	{
		return;
	}

	trap_SendServerCommand(other - g_entities, va("%s \"%s%c%c%s\"",
	                       mode == SAY_TEAM ? "tchat" : "chat",
	                       name, Q_COLOR_ESCAPE, color, message));
}

#define EC      "\x19"

void G_Say(gentity_t* ent, gentity_t* target, int mode, const char* chatText)
{
	int         j;
	gentity_t*   other;
	int         color;
	char        name[64];
	// don't let text be too long for malicious reasons
	char        text[MAX_SAY_TEXT];
	char        location[64];

	if (g_gametype.integer < GT_TEAM && mode == SAY_TEAM)
	{
		mode = SAY_ALL;
	}

	switch (mode)
	{
		default:
		case SAY_ALL:
			G_LogPrintf("say: %s: %s\n", ent->client->pers.netname, chatText);
			Com_sprintf(name, sizeof(name), "%s%c%c"EC": ", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE);
			color = COLOR_GREEN;
			break;
		case SAY_TEAM:
			G_LogPrintf("sayteam: %s: %s\n", ent->client->pers.netname, chatText);
			if (Team_GetLocationMsg(ent, location, sizeof(location)))
				Com_sprintf(name, sizeof(name), EC"(%s%c%c"EC") (%s)"EC": ",
				            ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE, location);
			else
				Com_sprintf(name, sizeof(name), EC"(%s%c%c"EC")"EC": ",
				            ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE);
			color = COLOR_CYAN;
			break;
		case SAY_TELL:
			if (target && g_gametype.integer >= GT_TEAM &&
			        target->client->sess.sessionTeam == ent->client->sess.sessionTeam &&
			        Team_GetLocationMsg(ent, location, sizeof(location)))
				Com_sprintf(name, sizeof(name), EC"[%s%c%c"EC"] (%s)"EC": ", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE, location);
			else
				Com_sprintf(name, sizeof(name), EC"[%s%c%c"EC"]"EC": ", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE);
			color = COLOR_MAGENTA;
			break;
	}

	Q_strncpyz(text, chatText, sizeof(text));

	if (target)
	{
		G_SayTo(ent, target, mode, color, name, text);
		return;
	}

	// echo the text to the console
	if (g_dedicated.integer)
	{
		G_Printf("%s%s\n", name, text);
	}

	// send it to all the apropriate clients
	for (j = 0; j < level.maxclients; j++)
	{
		other = &g_entities[j];
		G_SayTo(ent, other, mode, color, name, text);
	}
}


/*
==================
Cmd_Say_f
==================
*/
static void Cmd_Say_f(gentity_t* ent, int mode, qboolean arg0)
{
	char*        p;

	if (trap_Argc() < 2 && !arg0)
	{
		return;
	}

	if (arg0)
	{
		p = ConcatArgs(0);
	}
	else
	{
		p = ConcatArgs(1);
	}

	G_Say(ent, NULL, mode, p);
}

/*
==================
Cmd_Tell_f
==================
*/
static void Cmd_Tell_f(gentity_t* ent)
{
	int         targetNum;
	gentity_t*   target;
	char*        p;
	char        arg[MAX_TOKEN_CHARS];

	if (trap_Argc() < 2)
	{
		return;
	}

	trap_Argv(1, arg, sizeof(arg));
	targetNum = atoi(arg);
	if (targetNum < 0 || targetNum >= level.maxclients)
	{
		return;
	}

	target = &g_entities[targetNum];
	if (!target || !target->inuse || !target->client)
	{
		return;
	}

	p = ConcatArgs(2);

	G_LogPrintf("tell: %s to %s: %s\n", ent->client->pers.netname, target->client->pers.netname, p);
	G_Say(ent, target, SAY_TELL, p);
	// don't tell to the player self if it was already directed to this player
	// also don't send the chat back to a bot
	if (ent != target && !(ent->r.svFlags & SVF_BOT))
	{
		G_Say(ent, ent, SAY_TELL, p);
	}
}


static void G_VoiceTo(gentity_t* ent, gentity_t* other, int mode, const char* id, qboolean voiceonly)
{
	int color;
	char* cmd;

	if (!other)
	{
		return;
	}
	if (!other->inuse)
	{
		return;
	}
	if (!other->client)
	{
		return;
	}
	if (mode == SAY_TEAM && !OnSameTeam(ent, other))
	{
		return;
	}
	// no chatting to players in tournements
	if ((g_gametype.integer == GT_TOURNAMENT))
	{
		return;
	}

	if (mode == SAY_TEAM)
	{
		color = COLOR_CYAN;
		cmd = "vtchat";
	}
	else if (mode == SAY_TELL)
	{
		color = COLOR_MAGENTA;
		cmd = "vtell";
	}
	else
	{
		color = COLOR_GREEN;
		cmd = "vchat";
	}

	trap_SendServerCommand(other - g_entities, va("%s %d %d %d %s", cmd, voiceonly, ent->s.number, color, id));
}

void G_Voice(gentity_t* ent, gentity_t* target, int mode, const char* id, qboolean voiceonly)
{
	int         j;
	gentity_t*   other;

	if (g_gametype.integer < GT_TEAM && mode == SAY_TEAM)
	{
		mode = SAY_ALL;
	}

	if (target)
	{
		G_VoiceTo(ent, target, mode, id, voiceonly);
		return;
	}

	// echo the text to the console
	if (g_dedicated.integer)
	{
		G_Printf("voice: %s %s\n", ent->client->pers.netname, id);
	}

	// send it to all the apropriate clients
	for (j = 0; j < level.maxclients; j++)
	{
		other = &g_entities[j];
		G_VoiceTo(ent, other, mode, id, voiceonly);
	}
}

/*
==================
Cmd_Voice_f
==================
*/
static void Cmd_Voice_f(gentity_t* ent, int mode, qboolean arg0, qboolean voiceonly)
{
	char*        p;

	if (trap_Argc() < 2 && !arg0)
	{
		return;
	}

	if (arg0)
	{
		p = ConcatArgs(0);
	}
	else
	{
		p = ConcatArgs(1);
	}

	G_Voice(ent, NULL, mode, p, voiceonly);
}

/*
==================
Cmd_VoiceTell_f
==================
*/
static void Cmd_VoiceTell_f(gentity_t* ent, qboolean voiceonly)
{
	int         targetNum;
	gentity_t*   target;
	char*        id;
	char        arg[MAX_TOKEN_CHARS];

	if (trap_Argc() < 2)
	{
		return;
	}

	trap_Argv(1, arg, sizeof(arg));
	targetNum = atoi(arg);
	if (targetNum < 0 || targetNum >= level.maxclients)
	{
		return;
	}

	target = &g_entities[targetNum];
	if (!target || !target->inuse || !target->client)
	{
		return;
	}

	id = ConcatArgs(2);

	G_LogPrintf("vtell: %s to %s: %s\n", ent->client->pers.netname, target->client->pers.netname, id);
	G_Voice(ent, target, SAY_TELL, id, voiceonly);
	// don't tell to the player self if it was already directed to this player
	// also don't send the chat back to a bot
	if (ent != target && !(ent->r.svFlags & SVF_BOT))
	{
		G_Voice(ent, ent, SAY_TELL, id, voiceonly);
	}
}


/*
==================
Cmd_VoiceTaunt_f
==================
*/
static void Cmd_VoiceTaunt_f(gentity_t* ent)
{
	gentity_t* who;
	int i;

	if (!ent->client)
	{
		return;
	}

	// insult someone who just killed you
	if (ent->enemy && ent->enemy->client && ent->enemy->client->lastkilled_client == ent->s.number)
	{
		// i am a dead corpse
		if (!(ent->enemy->r.svFlags & SVF_BOT))
		{
			G_Voice(ent, ent->enemy, SAY_TELL, VOICECHAT_DEATHINSULT, qfalse);
		}
		if (!(ent->r.svFlags & SVF_BOT))
		{
			G_Voice(ent, ent,        SAY_TELL, VOICECHAT_DEATHINSULT, qfalse);
		}
		ent->enemy = NULL;
		return;
	}
	// insult someone you just killed
	if (ent->client->lastkilled_client >= 0 && ent->client->lastkilled_client != ent->s.number)
	{
		who = g_entities + ent->client->lastkilled_client;
		if (who->client)
		{
			// who is the person I just killed
			if (who->client->lasthurt_mod == MOD_GAUNTLET)
			{
				if (!(who->r.svFlags & SVF_BOT))
				{
					G_Voice(ent, who, SAY_TELL, VOICECHAT_KILLGAUNTLET, qfalse);     // and I killed them with a gauntlet
				}
				if (!(ent->r.svFlags & SVF_BOT))
				{
					G_Voice(ent, ent, SAY_TELL, VOICECHAT_KILLGAUNTLET, qfalse);
				}
			}
			else
			{
				if (!(who->r.svFlags & SVF_BOT))
				{
					G_Voice(ent, who, SAY_TELL, VOICECHAT_KILLINSULT, qfalse);   // and I killed them with something else
				}
				if (!(ent->r.svFlags & SVF_BOT))
				{
					G_Voice(ent, ent, SAY_TELL, VOICECHAT_KILLINSULT, qfalse);
				}
			}
			ent->client->lastkilled_client = -1;
			return;
		}
	}

	if (g_gametype.integer >= GT_TEAM)
	{
		// praise a team mate who just got a reward
		for (i = 0; i < MAX_CLIENTS; i++)
		{
			who = g_entities + i;
			if (who->client && who != ent && who->client->sess.sessionTeam == ent->client->sess.sessionTeam)
			{
				if (who->client->rewardTime > level.time)
				{
					if (!(who->r.svFlags & SVF_BOT))
					{
						G_Voice(ent, who, SAY_TELL, VOICECHAT_PRAISE, qfalse);
					}
					if (!(ent->r.svFlags & SVF_BOT))
					{
						G_Voice(ent, ent, SAY_TELL, VOICECHAT_PRAISE, qfalse);
					}
					return;
				}
			}
		}
	}

	// just say something
	G_Voice(ent, NULL, SAY_ALL, VOICECHAT_TAUNT, qfalse);
}



static char* gc_orders[] =
{
	"hold your position",
	"hold this position",
	"come here",
	"cover me",
	"guard location",
	"search and destroy",
	"report"
};

void Cmd_GameCommand_f(gentity_t* ent)
{
	int     player;
	int     order;
	char    str[MAX_TOKEN_CHARS];

	trap_Argv(1, str, sizeof(str));
	player = atoi(str);
	trap_Argv(2, str, sizeof(str));
	order = atoi(str);

	if (player < 0 || player >= MAX_CLIENTS)
	{
		return;
	}
	if (order < 0 || order > sizeof(gc_orders) / sizeof(char*))
	{
		return;
	}
	G_Say(ent, &g_entities[player], SAY_TELL, gc_orders[order]);
	G_Say(ent, ent, SAY_TELL, gc_orders[order]);
}

/*
==================
Cmd_Where_f
==================
*/
void Cmd_Where_f(gentity_t* ent)
{
	trap_SendServerCommand(ent - g_entities, va("print \"%s\n\"", vtos(ent->s.origin)));
}

static const char* gameNames[] =
{
	"Free For All",
	"Tournament",
	"Single Player",
	"Team Deathmatch",
	"Capture the Flag",
	"One Flag CTF",
	"Overload",
	"Harvester"
};

/*
==================
Cmd_CallVote_f
==================
*/
void Cmd_CallVote_f(gentity_t* ent)
{
	int     i;
	char    arg1[MAX_STRING_TOKENS];
	char    arg2[MAX_STRING_TOKENS];

	if (!g_allowVote.integer)
	{
		trap_SendServerCommand(ent - g_entities, "print \"Voting not allowed here.\n\"");
		return;
	}

	if (level.voteTime)
	{
		trap_SendServerCommand(ent - g_entities, "print \"A vote is already in progress.\n\"");
		return;
	}
	if (ent->client->pers.voteCount >= MAX_VOTE_COUNT)
	{
		trap_SendServerCommand(ent - g_entities, "print \"You have called the maximum number of votes.\n\"");
		return;
	}
	if (ent->client->sess.sessionTeam == TEAM_SPECTATOR)
	{
		trap_SendServerCommand(ent - g_entities, "print \"Not allowed to call a vote as spectator.\n\"");
		return;
	}

	// make sure it is a valid command to vote on
	trap_Argv(1, arg1, sizeof(arg1));
	trap_Argv(2, arg2, sizeof(arg2));

	if (strchr(arg1, ';') || strchr(arg2, ';'))
	{
		trap_SendServerCommand(ent - g_entities, "print \"Invalid vote string.\n\"");
		return;
	}

	if (!Q_stricmp(arg1, "map_restart"))
	{
	}
	else if (!Q_stricmp(arg1, "nextmap"))
	{
	}
	else if (!Q_stricmp(arg1, "map"))
	{
	}
	else if (!Q_stricmp(arg1, "g_gametype"))
	{
	}
	else if (!Q_stricmp(arg1, "kick"))
	{
	}
	else if (!Q_stricmp(arg1, "clientkick"))
	{
	}
	else if (!Q_stricmp(arg1, "g_doWarmup"))
	{
	}
	else if (!Q_stricmp(arg1, "timelimit"))
	{
	}
	else if (!Q_stricmp(arg1, "fraglimit"))
	{
	}
	else
	{
		trap_SendServerCommand(ent - g_entities, "print \"Invalid vote string.\n\"");
		trap_SendServerCommand(ent - g_entities, "print \"Vote commands are: map_restart, nextmap, map <mapname>, g_gametype <n>, kick <player>, clientkick <clientnum>, g_doWarmup, timelimit <time>, fraglimit <frags>.\n\"");
		return;
	}

	// if there is still a vote to be executed
	if (level.voteExecuteTime)
	{
		level.voteExecuteTime = 0;
		trap_SendConsoleCommand(EXEC_APPEND, va("%s\n", level.voteString));
	}

	// special case for g_gametype, check for bad values
	if (!Q_stricmp(arg1, "g_gametype"))
	{
		i = atoi(arg2);
		if (i == GT_SINGLE_PLAYER || i < GT_FFA || i >= GT_MAX_GAME_TYPE)
		{
			trap_SendServerCommand(ent - g_entities, "print \"Invalid gametype.\n\"");
			return;
		}

		Com_sprintf(level.voteString, sizeof(level.voteString), "%s %d", arg1, i);
		Com_sprintf(level.voteDisplayString, sizeof(level.voteDisplayString), "%s %s", arg1, gameNames[i]);
	}
	else if (!Q_stricmp(arg1, "map"))
	{
		// special case for map changes, we want to reset the nextmap setting
		// this allows a player to change maps, but not upset the map rotation
		char    s[MAX_STRING_CHARS];

		trap_Cvar_VariableStringBuffer("nextmap", s, sizeof(s));
		if (*s)
		{
			Com_sprintf(level.voteString, sizeof(level.voteString), "%s %s; set nextmap \"%s\"", arg1, arg2, s);
		}
		else
		{
			Com_sprintf(level.voteString, sizeof(level.voteString), "%s %s", arg1, arg2);
		}
		Com_sprintf(level.voteDisplayString, sizeof(level.voteDisplayString), "%s", level.voteString);
	}
	else if (!Q_stricmp(arg1, "nextmap"))
	{
		char    s[MAX_STRING_CHARS];

		trap_Cvar_VariableStringBuffer("nextmap", s, sizeof(s));
		if (!*s)
		{
			trap_SendServerCommand(ent - g_entities, "print \"nextmap not set.\n\"");
			return;
		}
		Com_sprintf(level.voteString, sizeof(level.voteString), "vstr nextmap");
		Com_sprintf(level.voteDisplayString, sizeof(level.voteDisplayString), "%s", level.voteString);
	}
	else
	{
		Com_sprintf(level.voteString, sizeof(level.voteString), "%s \"%s\"", arg1, arg2);
		Com_sprintf(level.voteDisplayString, sizeof(level.voteDisplayString), "%s", level.voteString);
	}

	trap_SendServerCommand(-1, va("print \"%s called a vote.\n\"", ent->client->pers.netname));

	// start the voting, the caller autoamtically votes yes
	level.voteTime = level.time;
	level.voteYes = 1;
	level.voteNo = 0;

	for (i = 0 ; i < level.maxclients ; i++)
	{
		level.clients[i].ps.eFlags &= ~EF_VOTED;
	}
	ent->client->ps.eFlags |= EF_VOTED;

	trap_SetConfigstring(CS_VOTE_TIME, va("%i", level.voteTime));
	trap_SetConfigstring(CS_VOTE_STRING, level.voteDisplayString);
	trap_SetConfigstring(CS_VOTE_YES, va("%i", level.voteYes));
	trap_SetConfigstring(CS_VOTE_NO, va("%i", level.voteNo));
}

/*
==================
Cmd_Vote_f
==================
*/
void Cmd_Vote_f(gentity_t* ent)
{
	char        msg[64];

	if (!level.voteTime)
	{
		trap_SendServerCommand(ent - g_entities, "print \"No vote in progress.\n\"");
		return;
	}
	if (ent->client->ps.eFlags & EF_VOTED)
	{
		trap_SendServerCommand(ent - g_entities, "print \"Vote already cast.\n\"");
		return;
	}
	if (ent->client->sess.sessionTeam == TEAM_SPECTATOR)
	{
		trap_SendServerCommand(ent - g_entities, "print \"Not allowed to vote as spectator.\n\"");
		return;
	}

	trap_SendServerCommand(ent - g_entities, "print \"Vote cast.\n\"");

	ent->client->ps.eFlags |= EF_VOTED;

	trap_Argv(1, msg, sizeof(msg));

	if (msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1')
	{
		level.voteYes++;
		trap_SetConfigstring(CS_VOTE_YES, va("%i", level.voteYes));
	}
	else
	{
		level.voteNo++;
		trap_SetConfigstring(CS_VOTE_NO, va("%i", level.voteNo));
	}

	// a majority will be determined in CheckVote, which will also account
	// for players entering or leaving
}

/*
==================
Cmd_CallTeamVote_f
==================
*/
void Cmd_CallTeamVote_f(gentity_t* ent)
{
	int     i, team, cs_offset;
	char    arg1[MAX_STRING_TOKENS];
	char    arg2[MAX_STRING_TOKENS];

	team = ent->client->sess.sessionTeam;
	if (team == TEAM_RED)
		cs_offset = 0;
	else if (team == TEAM_BLUE)
		cs_offset = 1;
	else
		return;

	if (!g_allowVote.integer)
	{
		trap_SendServerCommand(ent - g_entities, "print \"Voting not allowed here.\n\"");
		return;
	}

	if (level.teamVoteTime[cs_offset])
	{
		trap_SendServerCommand(ent - g_entities, "print \"A team vote is already in progress.\n\"");
		return;
	}
	if (ent->client->pers.teamVoteCount >= MAX_VOTE_COUNT)
	{
		trap_SendServerCommand(ent - g_entities, "print \"You have called the maximum number of team votes.\n\"");
		return;
	}
	if (ent->client->sess.sessionTeam == TEAM_SPECTATOR)
	{
		trap_SendServerCommand(ent - g_entities, "print \"Not allowed to call a vote as spectator.\n\"");
		return;
	}

	// make sure it is a valid command to vote on
	trap_Argv(1, arg1, sizeof(arg1));
	arg2[0] = '\0';
	for (i = 2; i < trap_Argc(); i++)
	{
		if (i > 2)
			strcat(arg2, " ");
		trap_Argv(i, &arg2[strlen(arg2)], sizeof(arg2) - strlen(arg2));
	}

	if (strchr(arg1, ';') || strchr(arg2, ';'))
	{
		trap_SendServerCommand(ent - g_entities, "print \"Invalid vote string.\n\"");
		return;
	}

	if (!Q_stricmp(arg1, "leader"))
	{
		char netname[MAX_NETNAME], leader[MAX_NETNAME];

		if (!arg2[0])
		{
			i = ent->client->ps.clientNum;
		}
		else
		{
			// numeric values are just slot numbers
			for (i = 0; i < 3; i++)
			{
				if (!arg2[i] || arg2[i] < '0' || arg2[i] > '9')
					break;
			}
			if (i >= 3 || !arg2[i])
			{
				i = atoi(arg2);
				if (i < 0 || i >= level.maxclients)
				{
					trap_SendServerCommand(ent - g_entities, va("print \"Bad client slot: %i\n\"", i));
					return;
				}

				if (!g_entities[i].inuse)
				{
					trap_SendServerCommand(ent - g_entities, va("print \"Client %i is not active\n\"", i));
					return;
				}
			}
			else
			{
				Q_strncpyz(leader, arg2, sizeof(leader));
				Q_CleanStr(leader);
				for (i = 0 ; i < level.maxclients ; i++)
				{
					if (level.clients[i].pers.connected == CON_DISCONNECTED)
						continue;
					if (level.clients[i].sess.sessionTeam != team)
						continue;
					Q_strncpyz(netname, level.clients[i].pers.netname, sizeof(netname));
					Q_CleanStr(netname);
					if (!Q_stricmp(netname, leader))
					{
						break;
					}
				}
				if (i >= level.maxclients)
				{
					trap_SendServerCommand(ent - g_entities, va("print \"%s is not a valid player on your team.\n\"", arg2));
					return;
				}
			}
		}
		Com_sprintf(arg2, sizeof(arg2), "%d", i);
	}
	else
	{
		trap_SendServerCommand(ent - g_entities, "print \"Invalid vote string.\n\"");
		trap_SendServerCommand(ent - g_entities, "print \"Team vote commands are: leader <player>.\n\"");
		return;
	}

	Com_sprintf(level.teamVoteString[cs_offset], sizeof(level.teamVoteString[cs_offset]), "%s %s", arg1, arg2);

	for (i = 0 ; i < level.maxclients ; i++)
	{
		if (level.clients[i].pers.connected == CON_DISCONNECTED)
			continue;
		if (level.clients[i].sess.sessionTeam == team)
			trap_SendServerCommand(i, va("print \"%s called a team vote.\n\"", ent->client->pers.netname));
	}

	// start the voting, the caller autoamtically votes yes
	level.teamVoteTime[cs_offset] = level.time;
	level.teamVoteYes[cs_offset] = 1;
	level.teamVoteNo[cs_offset] = 0;

	for (i = 0 ; i < level.maxclients ; i++)
	{
		if (level.clients[i].sess.sessionTeam == team)
			level.clients[i].ps.eFlags &= ~EF_TEAMVOTED;
	}
	ent->client->ps.eFlags |= EF_TEAMVOTED;

	trap_SetConfigstring(CS_TEAMVOTE_TIME + cs_offset, va("%i", level.teamVoteTime[cs_offset]));
	trap_SetConfigstring(CS_TEAMVOTE_STRING + cs_offset, level.teamVoteString[cs_offset]);
	trap_SetConfigstring(CS_TEAMVOTE_YES + cs_offset, va("%i", level.teamVoteYes[cs_offset]));
	trap_SetConfigstring(CS_TEAMVOTE_NO + cs_offset, va("%i", level.teamVoteNo[cs_offset]));
}

/*
==================
Cmd_TeamVote_f
==================
*/
void Cmd_TeamVote_f(gentity_t* ent)
{
	int         team, cs_offset;
	char        msg[64];

	team = ent->client->sess.sessionTeam;
	if (team == TEAM_RED)
		cs_offset = 0;
	else if (team == TEAM_BLUE)
		cs_offset = 1;
	else
		return;

	if (!level.teamVoteTime[cs_offset])
	{
		trap_SendServerCommand(ent - g_entities, "print \"No team vote in progress.\n\"");
		return;
	}
	if (ent->client->ps.eFlags & EF_TEAMVOTED)
	{
		trap_SendServerCommand(ent - g_entities, "print \"Team vote already cast.\n\"");
		return;
	}
	if (ent->client->sess.sessionTeam == TEAM_SPECTATOR)
	{
		trap_SendServerCommand(ent - g_entities, "print \"Not allowed to vote as spectator.\n\"");
		return;
	}

	trap_SendServerCommand(ent - g_entities, "print \"Team vote cast.\n\"");

	ent->client->ps.eFlags |= EF_TEAMVOTED;

	trap_Argv(1, msg, sizeof(msg));

	if (msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1')
	{
		level.teamVoteYes[cs_offset]++;
		trap_SetConfigstring(CS_TEAMVOTE_YES + cs_offset, va("%i", level.teamVoteYes[cs_offset]));
	}
	else
	{
		level.teamVoteNo[cs_offset]++;
		trap_SetConfigstring(CS_TEAMVOTE_NO + cs_offset, va("%i", level.teamVoteNo[cs_offset]));
	}

	// a majority will be determined in TeamCheckVote, which will also account
	// for players entering or leaving
}


/*
=================
Cmd_SetViewpos_f
=================
*/
void Cmd_SetViewpos_f(gentity_t* ent)
{
	vec3_t      origin, angles;
	char        buffer[MAX_TOKEN_CHARS];
	int         i;

	if (!g_cheats.integer)
	{
		trap_SendServerCommand(ent - g_entities, va("print \"Cheats are not enabled on this server.\n\""));
		return;
	}
	if (trap_Argc() != 5)
	{
		trap_SendServerCommand(ent - g_entities, va("print \"usage: setviewpos x y z yaw\n\""));
		return;
	}

	VectorClear(angles);
	for (i = 0 ; i < 3 ; i++)
	{
		trap_Argv(i + 1, buffer, sizeof(buffer));
		origin[i] = atof(buffer);
	}

	trap_Argv(4, buffer, sizeof(buffer));
	angles[YAW] = atof(buffer);

	TeleportPlayer(ent, origin, angles);
}

/*
=================
Cmd_Help_f
=================
*/
static qboolean Cmd_Help_f(const gentity_t* ent, int cmdNumber)
{
	int rc;
	char text[MAX_STRING_CHARS];

	if (!ent)
	{
		return qfalse;
	}

	trap_Argv(1, text, MAX_STRING_CHARS);

	if (Q_stricmp(text, "?") == 0)
	{
		trap_Argv(0, text, MAX_STRING_CHARS);
		trap_SendServerCommand(ent - g_entities, va("print \"\n^3%s%s\n\n\"", text, cmdHelp[cmdNumber]));
		return qtrue;
	}
	return qfalse;
}

/*
=================
Cmd_HelpAvailOspGameCommands_f
=================
\help
^5
Available OSP Game-Commands:
----------------------------
^3autorecord       invite           specinvite       vc_free
^3autoscreenshot   joincode         speclock         vc_view
^3bottomshots      lock/unlock      speconly         viewcam
^3callvote         menu             specunlock       viewadd
^3captains         ready            stats            viewblue
^3coach            notready         statsall         viewred
^3coachdecline     players          statsblue        viewfollow
^3coachinvite      readyteam        statsdump        viewnext
^3coachkick        ref/admin        statsred         viewnone
^3credits          remove           time/pause       viewprev
^3currenttime      resign           timein/unpause   viewremove
^3drop             scores           topshots         viewcyclenext
^3help/commands    settings         vc_follow        viewcycleprev

Type: ^3\command_name ?^7 for more information

^6OSP Tourney DM for Quake3 v(1.03a)
^5http://www.OrangeSmoothie.org/^7
*/

static void Cmd_HelpAvailOspGameCommands_f(const gentity_t* ent)
{
	static const char* availOspGameCommands[] =
	{
		"autorecord", "autoscreenshot", "bottomshots", "callvote",
		"captains", "coach", "coachdecline", "coachinvite",
		"coachkick", "credits", "currenttime", "drop",
		"help/commands", "invite", "joincode", "lock/unlock",
		"menu", "ready", "notready", "players",
		"readyteam", "ref/admin", "remove", "resign",
		"scores", "settings", "specinvite", "speclock",
		"speconly", "specunlock", "stats", "statsall",
		"statsblue", "statsdump", "statsred", "time/pause",
		"timein/unpause", "topshots", "vc_follow", "vc_free",
		"vc_view", "viewcam", "viewadd", "viewblue",
		"viewred", "viewfollow", "viewnext", "viewnone",
		"viewprev", "viewremove", "viewcyclenext", "viewcycleprev",
	};

	static const int numberOfCommands = sizeof(availOspGameCommands) / sizeof(char*);
	static const int numberOfColums = 4;
	const int numberOfRows = (numberOfCommands / 4) + ((numberOfCommands % 4) ? 1 : 0);
	int i = 0;
	const char* c1;
	const char* c2;
	const char* c3;
	const char* c4;

	trap_SendServerCommand(ent - g_entities, "print \"^5\nAvailable OSP Game-Commands:\n----------------------------\n\"");

	while (i < numberOfRows)
	{
		c1 = availOspGameCommands[i];
		c2 = (i + numberOfRows < numberOfCommands) ? availOspGameCommands[i + numberOfRows] : "";
		c3 = (i + numberOfRows * 2 < numberOfCommands) ? availOspGameCommands[i + numberOfRows * 2] : "";
		c4 = (i + numberOfRows * 3 < numberOfCommands) ? availOspGameCommands[i + numberOfRows * 3] : "";

		trap_SendServerCommand(ent - g_entities, va("print \"^3%-17s%-17s%-17s%-17s\n\"", c1, c2, c3, c4));
		++i;
	}

	trap_SendServerCommand(ent - g_entities, "print \"\nType: ^3\ncommand_name ?^7 for more information\n\"");
	trap_SendServerCommand(ent - g_entities, va("print \"\n^6OSP Tourney DM for Quake3 %s\n\"", OSP_VERSION_STR));
	trap_SendServerCommand(ent - g_entities, "print \"^5http://www.OrangeSmoothie.org/^7\n\n\"");


	if (level.serverMode & 1)
	{
		trap_SendServerCommand(ent - g_entities, "print \"^3ProMode^7 settings provided by:\n\"");
		trap_SendServerCommand(ent - g_entities, "print \"Challenge ProMode - http://www.promode.org\n\"");
	}
	else
	{
		if (level.serverMode & 2)
		{
			trap_SendServerCommand(ent - g_entities, "print \"^3CQ3^7 settings provided by:\n\"");
			trap_SendServerCommand(ent - g_entities, "print \"Challenge Q3 - http://www.promode.org\n\"");
		}
	}
}

/*
=================
Cmd_RefCommand_f
=================
*/
static qboolean Cmd_OSPCommands_f(const gentity_t* ent, char* arg, qboolean isReferee)
{
	if (g_gametype.integer >= GT_TEAM)
	{
		if (!Q_stricmp(arg, "lock") || !Q_stricmp(arg, "unlock"))
		{
			Cmd_Lock_f(ent, arg);
			return qtrue;
		}
		else if (!Q_stricmp(arg, "lock") || !Q_stricmp(arg, "unlock"))
		{
			Cmd_SpecLock_f(ent, arg);
			return qtrue;
		}
		else if (!Q_stricmp(arg, "teamready") || !Q_stricmp(arg, "readyteam"))
		{
			Cmd_TeamReady_f(ent);
			return qtrue;
		}
		else if (!Q_stricmp(arg, "invite") || !Q_stricmp(arg, "pickplayer"))
		{
			Cmd_Invite_f(ent);
			return qtrue;
		}
		else if (!Q_stricmp(arg, "specinvite") || !Q_stricmp(arg, "invitespec"))
		{
			Cmd_SpecInvite_f(ent);
			return qtrue;
		}
		else if (!Q_stricmp(arg, "remove") || !Q_stricmp(arg, "kickplayer"))
		{
			Cmd_Remove_f(ent);
			return qtrue;
		}
		else if (!Q_stricmp(arg, "capitans"))
		{
			Cmd_Capitans_f(ent);
			return qtrue;
		}
		else if (!Q_stricmp(arg, "resign"))
		{
			Cmd_Resign_f(ent);
			return qtrue;
		}
		else if (!Q_stricmp(arg, "joincode"))
		{
			Cmd_JoinCode_f(ent);
			return qtrue;
		}
	}

	if (g_gametype.integer != GT_SINGLE_PLAYER)
	{
		if (!Q_stricmp(arg, "ready"))
		{
			Cmd_Ready_f(ent, qtrue);
			return qtrue;
		}
		else if (!Q_stricmp(arg, "notready") || !Q_stricmp(arg, "unready"))
		{
			Cmd_Ready_f(ent, qfalse);
			return qtrue;
		}
		else if (!Q_stricmp(arg, "time") || !Q_stricmp(arg, "pause") || !Q_stricmp(arg, "timeout"))
		{
			Cmd_Pause_f(ent, qtrue);
			return qtrue;
		}
		else if (!Q_stricmp(arg, "timein") || !Q_stricmp(arg, "unpause"))
		{
			Cmd_Pause_f(ent, qfalse);
			return qtrue;
		}
	}

	if (!Q_stricmp(arg, "help") || !Q_stricmp(arg, "commands") || !Q_stricmp(arg, "?"))
	{
		if (!Cmd_Help_f(ent, CMDHELP_MOST))
		{
			Cmd_HelpAvailOspGameCommands_f(ent);
		}
		return qtrue;
	}
	else if (!Q_stricmp(arg, "followpowerup"))
	{
		ent->client->isFollowPowerUp = !ent->client->isFollowPowerUp;
		trap_SendServerCommand(ent - g_entities, va("print \"\nAuto-follow of powerup pickups is: ^3%s\n\n\"", ent->client->isFollowPowerUp ? "ENABLED" : "DISABLED"));
		return qtrue;
	}
	else if (!Q_stricmp(arg, "speconly"))
	{
		if (!Cmd_Help_f(ent, CMDHELP_SPECONLY))
		{
			Cmd_SpecOnly_f(ent);
		}
		return qtrue;
	}
	else if (!Q_stricmp(arg, "autorecord"))
	{
		if (!Cmd_Help_f(ent, CMDHELP_AUTORECORD))
		{
			Cmd_AutoScreenshot_f(ent - g_entities, qtrue, qtrue);
		}
		return qtrue;
	}
	else if (!Q_stricmp(arg, "maplist"))
	{
		Cmd_MapList_f(ent);
		return qtrue;
	}
	else if (!Q_stricmp(arg, "mapload"))
	{
		Cmd_MapLoad_f(qtrue);
		return qtrue;
	}
	else if (!Q_stricmp(arg, "range"))
	{
		Cmd_Range_f(ent);
		return qtrue;
	}
	else if (!Q_stricmp(arg, "snapshot"))
	{
		Cmd_Snapshot_f(ent);
		return qtrue;
	}
	else if (!Q_stricmp(arg, "snapshotdump"))
	{
		Cmd_SnapshotDump_f(ent);
		return qtrue;
	}
	else if (!Q_stricmp(arg, "viewadd"))
	{
		if (!Cmd_Help_f(ent, CMDHELP_VIEWADD))
		{
			Cmd_ViewAdd_f(ent);
		}
		return qtrue;
	}
	else if (!Q_stricmp(arg, "viewall"))
	{
		if (!Cmd_Help_f(ent, CMDHELP_VIEWALL) && !ent->client->viewTeam)
		{
			Cmd_ViewAll_f(ent);
		}
		return qtrue;
	}
	else if (!Q_stricmp(arg, "viewremove"))
	{
		if (!Cmd_Help_f(ent, CMDHELP_VIEWREMOVE) && !ent->client->viewTeam)
		{
			Cmd_ViewerMove_f(ent);
		}
		return qtrue;
	}
	else if (!Q_stricmp(arg, "viewfollow"))
	{
		if (!Cmd_Help_f(ent, CMDHELP_VIEWFOLLOW))
		{
			Cmd_ViewFollow(ent);
		}
		return qtrue;
	}
	else if (!Q_stricmp(arg, "viewnone"))
	{
		if (!Cmd_Help_f(ent, CMDHELP_VIEWNONE) && !ent->client->viewTeam)
		{
			Cmd_ViewNone_f(ent);
			ent->client->isHaveView = qfalse;
			ClientUserinfoChanged(ent - g_entities);
		}
		return qtrue;
	}
	else if (!Q_stricmp(arg, "viewframe"))
	{
		Cmd_ViewFrame_f(ent);
		return qtrue;
	}
	else if (!Q_stricmp(arg, "viewnext"))
	{
		if (!Cmd_Help_f(ent, CMDHELP_VIEWNEXT))
		{
			Cmd_ViewNext_f(ent, 1);
		}
		return qtrue;
	}
	else if (!Q_stricmp(arg, "viewprev"))
	{
		if (!Cmd_Help_f(ent, CMDHELP_VIEWNEXT))
		{
			Cmd_ViewNext_f(ent, -1);
		}
		return qtrue;
	}
	else if (!Q_stricmp(arg, "viewcyclenext"))
	{
		if (!Cmd_Help_f(ent, CMDHELP_VIEWCYCLENEXT))
		{
			Cmd_ViewCycleNext_f(ent, 1);
		}
		return qtrue;
	}
	else if (!Q_stricmp(arg, "viewcycleprev"))
	{
		if (!Cmd_Help_f(ent, CMDHELP_VIEWCYCLENEXT))
		{
			Cmd_ViewCycleNext_f(ent, -1);
		}
		return qtrue;
	}
	else if (!Q_stricmp(arg, "viewred"))
	{
		if (!Cmd_Help_f(ent, CMDHELP_VIEWTEAM))
		{
			Cmd_ViewTeam_f(ent, 0, 0);
		}
		return qtrue;
	}
	else if (!Q_stricmp(arg, "viewblue"))
	{
		if (!Cmd_Help_f(ent, CMDHELP_VIEWTEAM))
		{
			Cmd_ViewTeam_f(ent, 2, 1);
		}
		return qtrue;
	}
	else if (!Q_stricmp(arg, "coach"))
	{
		Cmd_Coach_f(ent, qtrue);
		return qtrue;
	}
	else if (!Q_stricmp(arg, "coachinvite"))
	{
		Cmd_CoachInvite_f(ent);
		return qtrue;
	}
	else if (!Q_stricmp(arg, "coachdecline"))
	{
		Cmd_CoachDecline_f(ent);
		return qtrue;
	}
	else if (!Q_stricmp(arg, "coachkick"))
	{
		Cmd_CoachKick_f(ent);
		return qtrue;
	}
	else if (!Q_stricmp(arg, "vc_proximity"))
	{
		Cmd_VcProximity_f(ent);
		return qtrue;
	}
	else if (!Q_stricmp(arg, "vc_info"))
	{
		ent->client->isVcInfoEnabled = !ent->client->isVcInfoEnabled;
		trap_SendServerCommand(ent - g_entities, va("print \"ViewCam info is ^5%s\n\"", ent->client->isVcInfoEnabled ? "ENABLED" : "DISABLED"));
		return qtrue;
	}
	else if (!Q_stricmp(arg, "vc_follw"))
	{
		if (!Cmd_Help_f(ent, CMDHELP_VCFOLLOW))
		{
			Cmd_ViewTeam_f(ent, 0, 0);
		}
		return qtrue;
	}
	else if (!Q_stricmp(arg, "vc_free"))
	{
		if (!Cmd_Help_f(ent, CMDHELP_VCFREE))
		{
			ent->client->isVcFreeEnabled = !ent->client->isVcFreeEnabled;
			trap_SendServerCommand(ent - g_entities, va("print \"ViewCam is now ^5%s\n\"", ent->client->isVcFreeEnabled ? "ENABLED" : "DISABLED"));
		}
		return qtrue;
	}
	else if (!Q_stricmp(arg, "vc_view"))
	{
		if (!Cmd_Help_f(ent, CMDHELP_VCVIEW))
		{
			ent->client->isVcViewEnabled = !ent->client->isVcViewEnabled;
			trap_SendServerCommand(ent - g_entities, va("print \"ViewCam is now ^5%s\n\"", ent->client->isVcViewEnabled ? "ENABLED" : "DISABLED"));
		}
		return qtrue;
	}
	else if (!Q_stricmp(arg, "drop"))
	{
		Cmd_Drop_f(ent);
		return qtrue;
	}
	else if (!Q_stricmp(arg, "viewcam"))
	{
		if (!Cmd_Help_f(ent, CMDHELP_VIEWCAM))
		{
			Cmd_ViewCam_f(ent);
		}
		return qtrue;
	}
	else if (!Q_stricmp(arg, "settings"))
	{
		Cmd_Settings_f(ent);
		return qtrue;
	}
	else if (!Q_stricmp(arg, "rhea") || !Q_stricmp(arg, "osp")  || !Q_stricmp(arg, "dess") || !Q_stricmp(arg, "shod"))
	{
		trap_SendServerCommand(ent - g_entities, va("print \"OSP2 Tourney DM for Quake3!\n\""));
		trap_SendServerCommand(ent - g_entities, "print \"^5https://github.com/snems/osp2^7\n\n\"");
		return qtrue;
	}
	else if (!Q_stricmp(arg, "arqon"))
	{
		trap_SendServerCommand(ent - g_entities, "print \"\n^3 OSP2 > OSP > CPMA !!!\n\n\"");
		return qtrue;
	}

	return qfalse;
}
/*
=================
Cmd_RefCommand_f
=================
*/
static void Cmd_RefCommand_f(const gentity_t* ent, qboolean skipOSPCommands)
{
	char text[MAX_STRING_CHARS];
	if (ent->client->isReferee)
	{
		if (!skipOSPCommands)
		{
			trap_Argv(1, &text[0], MAX_STRING_CHARS);
			if (Cmd_OSPCommands_f(ent, &text[0], qtrue) && Cmd_RefCommandArg_f(ent, &text[0], 1) && !Cmd_CallVote2_f(ent, 1))
			{
				Cmd_RefHelp(ent);
			}
		}
	}
	else
	{
		if (Q_stricmp(ref_password.string, "none") == 0 || ref_password.string[0] == 0)
		{
			trap_SendServerCommand(ent - g_entities, "print \"Sorry, referee status disabled on this server.\n\"");
			return;
		}

		if (trap_Argc() < 2)
		{
			trap_SendServerCommand(ent - g_entities, "print \"Usage: ref [password]\n\"");
		}
		else
		{
			trap_Argv(1, &text[0], 0x400);
			if (Q_stricmp(&text[0], ref_password.string))
			{
				trap_SendServerCommand(ent - g_entities, "print \"Invalid referee password!\n\"");
				G_AdminLog(va("Referee_Attempt_Fail\t%s\t(%s)", ent->client->pers.netname, &text[0]));
			}
			else
			{
				ent->client->isReferee = qtrue;
				ent->client->unknown2 = 3;
				trap_SendServerCommand(ent - g_entities, "print \"Type: ^3ref ?^7 for a list of referee commands.\n\"");
				trap_SendServerCommand(-1, va("cp \"%s\n^3has become a referee\n\"", ent->client->pers.netname));
				G_AdminLog(va("Referee_Attempt_Success\t%s\t(%s)", ent->client->pers.netname, text));
			}
		}
	}
}

/*
=================
Cmd_Uinfo_f

There is eight values:

- cl_maxpackets.integer
- snaps.integer
- cl_timenudge.integer
- customLocationsEnabled
- flags (follow mode)
- cg_autoAction.integer
- using_jpeg

=================
*/
void Cmd_Uinfo_f(gentity_t* ent)
{
	char buf[MAX_STRING_CHARS];
	int ui_flags;

	if (trap_Argc() < 8)
	{
		return;
	}

	trap_Argv(1, buf, MAX_STRING_CHARS);
	ent->client->maxPackets = atoi(buf);

	trap_Argv(2, buf, MAX_STRING_CHARS);
	ent->client->snaps = atoi(buf);

	trap_Argv(3, buf, MAX_STRING_CHARS);
	ent->client->timeNudge = atoi(buf);

	trap_Argv(4, buf, MAX_STRING_CHARS);
	ent->client->customLoc = atoi(buf);

	trap_Argv(5, buf, MAX_STRING_CHARS);
	ui_flags = atoi(buf);
	ent->client->followPowerup = ui_flags & 1;
	ent->client->followKiller = ui_flags & 2;
	ent->client->followViewCam = ui_flags & 4;

	trap_Argv(6, buf, MAX_STRING_CHARS);
	ent->client->autoAction = atoi(buf);

	trap_Argv(7, buf, MAX_STRING_CHARS);
	ent->client->usingJPEG = atoi(buf);
}

/*
=================
Cmd_PlayerList_f
=================
]\players

^3Status^1   : ^3ID^1 : ^3Player                    Nudge  Rate  MaxPkts  Snaps
^1---------------------------------------------------------------------^7
^3(READY)^1  :^1R^7 5 ^1:^7 Grunt                     ^7[BOT] -----       --     --  ^3
^3(READY)^1  :^4B^7 1 ^1:^7 Grunt                     ^7[BOT] -----       --     --  ^3
^3(READY)^1  :^1R^7 6 ^1:^7 Major                     ^7[BOT] -----       --     --  ^3
^3(READY)^1  :^4B^7 2 ^1:^7 Major                     ^7[BOT] -----       --     --  ^3
^3(READY)^1  :^1R^7 4 ^1:^7 Sarge                     ^7[BOT] -----       --     --  ^3
^3(READY)^1  :^1R^7 7 ^1:^7 Visor                     ^7[BOT] -----       --     --  ^3
^3(READY)^1  :^4B^7 3 ^1:^7 Visor                     ^7[BOT] -----       --     --  ^3
NOTREADY^1 :^4B^7 0 ^1:^7 snems                     ^7    0 25000      125     40  ^3

^3 8^7 total players

]quit
*/
static void Cmd_PlayerList_f(const gentity_t* ent)
{
	const gclient_t* clientLocal;
	int clientID;
	char readyStr[16];
	char refStr[16];
	const char* viewStr;
	const char* colorStr;
	const gentity_t* clientEntity;
	char* teamStr = NULL;
	int maxRate;
	int i;
	int numberOfPlayers;
	char playerNameSanitized[MAX_NETNAME];
	char tmp[256];
	char userinfo[MAX_INFO_STRING];
	int clientRate;
	qboolean isBot;
	int team;

	if (ent && Cmd_Help_f(ent, CMDHELP_PLAYER_LIST)) return;

	maxRate =   trap_Cvar_VariableIntegerValue("sv_maxrate");

	if (level.warmupTime)
	{
		if (ent)
		{
			trap_SendServerCommand(ent - g_entities, "print \"\n^3Status^1   : ^3ID^1 : ^3Player                    Nudge  Rate  MaxPkts  Snaps\n\"");
			trap_SendServerCommand(ent - g_entities, "print \"^1---------------------------------------------------------------------^7\n\"");
		}
		else
		{
			G_Printf("Status   : ID : Player                    Nudge  Rate  MaxPkts  Snaps\n");
			G_Printf("---------------------------------------------------------------------\n");
		}
	}
	else
	{
		if (ent)
		{
			trap_SendServerCommand(ent - g_entities, "print \"\n^3 ID^1 : ^3Player                    Nudge  Rate  MaxPkts  Snaps\n\"");
			trap_SendServerCommand(ent - g_entities, "print \"^1-----------------------------------------------------------^7\n\"");
		}
		else
		{
			G_Printf(" ID : Player                    Nudge  Rate  MaxPkts  Snaps\n");
			G_Printf("-----------------------------------------------------------\n");
		}
	}
	for (i = 0, numberOfPlayers = 0; i < level.numConnectedClients; ++i, ++numberOfPlayers)
	{
		clientID = level.sortedClients[i];
		clientLocal = &level.clients[clientID];
		clientEntity = &g_entities[clientID];
		isBot = clientEntity->r.svFlags & SVF_BOT;

		G_Printf("netname before %s\n", clientLocal->pers.netname);
		SanitizeString(clientLocal->pers.netname, playerNameSanitized, qfalse);
		G_Printf("netname after %s\n", playerNameSanitized);

		if (isBot)
		{
			strcpy(tmp, va("%s%s%s%s", "[BOT]", " -----", "       --", "     --"));
		}
		else if (clientLocal->pers.connected == CON_CONNECTING)
		{
			strcpy(tmp, va("%s", "^3>>> CONNECTING <<<"));
		}
		else
		{
			trap_GetUserinfo(clientID, userinfo, sizeof(userinfo));
			clientRate = atoi(Info_ValueForKey(userinfo, "rate"));
			if (maxRate && clientRate > maxRate)
			{
				clientRate = maxRate;
			}
			strcpy(tmp, va("%5d%6d%9d%7d", clientLocal->timeNudge, clientRate, clientLocal->maxPackets, clientLocal->snaps));
		}
		/* Ready status */
		readyStr[0] = 0;
		if (level.warmupTime)
		{
			if (clientLocal->team == TEAM_SPECTATOR && clientLocal->pers.connected == CON_CONNECTING)
			{
				if (ent)
				{
					strcpy(readyStr, "^5--------^1 :");
				}
				else
				{
					strcpy(readyStr, "-------- :");
				}
			}
			else
			{
				if (clientLocal->playerReady || isBot)
				{
					if (ent)
					{
						strcpy(readyStr, "^3(READY)^1  :");
					}
					else
					{
						strcpy(readyStr, "(READY)  :");
					}
				}
				else
				{
					if (ent)
					{
						strcpy(readyStr, "NOTREADY^1 :");
					}
					else
					{
						strcpy(readyStr, "NOTREADY :");
					}
				}
			}
		}
		/* Ref */
		if (clientLocal->isReferee != 0)
		{
			strcpy(refStr, "REF");
		}
		else
		{
			refStr[0] = 0;
		}
		/* Client view */
		if (clientLocal->viewTeam)
		{
			team = clientLocal->viewTeam;
			if (ent)
			{
				viewStr = "^3C";
			}
			else
			{
				viewStr = "C";
			}
		}
		else
		{
			team = clientLocal->team;
			viewStr = " ";
		}
		/* Team */
		if (g_gametype.integer < GT_TEAM)
		{
			if (ent)
			{
				teamStr = "^7 ";
			}
			else
			{
				teamStr = " ";
			}
		}
		else if (team == TEAM_RED)
		{
			if (ent)
			{
				teamStr = "^1R^7";
			}
			else
			{
				teamStr = "R";
			}
		}
		else if (team == TEAM_RED)
		{
			if (ent)
			{
				teamStr = "^4B^7";
			}
			else
			{
				teamStr = "B";
			}
		}
		else
		{
			if (ent)
			{
				teamStr = "^7 ";
			}
			else
			{
				teamStr = " ";
			}
		}

		if (ent)
		{
			if (refStr[0])
			{
				colorStr = "^3";
			}
			else
			{
				colorStr = "^7";
			}
			trap_SendServerCommand(ent - g_entities, va("print \"%s%s%2d%s^1:%s %-26s^7%s  ^3%s\n\"", readyStr, teamStr, clientID, viewStr, colorStr, playerNameSanitized, tmp, refStr));
		}
		else
		{
			G_Printf("%s%s%2d%s: %-26s%s  %s\n", readyStr, teamStr, clientID, viewStr, playerNameSanitized, tmp, refStr);
		}
	}


	if (ent)
	{
		trap_SendServerCommand(ent - g_entities, va("print \"\n^3%2d^7 total players\n\n\"", numberOfPlayers));
	}
	else
	{
		G_Printf("\n%2d total players\n\n", numberOfPlayers);
	}
}

static qboolean G_IsGameTypeOSP(int gametype)
{
	return gametype >= GT_CA;
}

static qboolean IsGameTypeOSP(int gametype)
{
	return G_IsGameTypeOSP(gametype);                                           /* Address : 0x30dcb Type : Interium */
}
/*
=================
G_IsSpectator
=================
*/
static qboolean G_IsSpectator(const gclient_t* client)
{
	return !(client->team != TEAM_SPECTATOR && client->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR  && client->unknown3 != 2);
}

/*
=================
G_GetSpectatorTeam
=================
*/
static team_t G_GetSpectatorTeam(const gentity_t* ent)
{
	if (ent->client->viewTeam)
	{
		return ent->client->viewTeam;
	}
	if (IsGameTypeOSP(g_gametype.integer) && ent->client->team != TEAM_SPECTATOR && ent->client->clanArenaSpectateForTeam != TEAM_SPECTATOR)
	{
		return ent->client->clanArenaSpectateForTeam;
	}
	return ent->client->team;
}
/*
=================
Cmd_GetStatsInfo_f
=================
*/
static void Cmd_GetStatsInfo_f(gentity_t* ent)
{
	gclient_t* client;
	char wstatsString[1024];
	char wstatsHead[1024];
	int weaponmask;
	int playerID;
	int team;
	int wp;
	int assists_plus;
	qboolean intermission;
	clientWeaponStats_t* ws;

	intermission = level.intermissiontime || level.intermissionQueued;

	if ((client->team != TEAM_SPECTATOR && g_gametype.integer == GT_TEAM && server_freezetag.integer && G_IsSpectator(client)) ||
	        (intermission && IsGameTypeOSP(g_gametype.integer) && G_GetSpectatorTeam(ent) != TEAM_SPECTATOR))
	{
		playerID = ent - g_entities;
	}
	else
	{
		if (ent->client->unknown3 == 0x2 || ent->client->unknown3 == 0x5)
		{
			playerID = ent->client->unknown4;
		}
		else if (ent->client->unknown3 == 0x7 && ent->client->tail3_25)
		{
			//playerID = ent->client->tail3_39[ent->client->tail3_26];                      /* Address : 0x2f8a5 Type : Interium */
		}
		else
		{
			team_t st_team;
			if (st_team != TEAM_RED && st_team != TEAM_BLUE && st_team != TEAM_SPECTATOR && g_gametype.integer == GT_TEAM && !server_freezetag.integer)
			{
				trap_SendServerCommand(ent - g_entities, "statsinfo 0");
				return;
			}
			else
			{
				playerID = ent - g_entities;
			}
		}
	}

	client = &level.clients[playerID];

	if (!client->ps.powerups[PW_REDFLAG] && !client->ps.powerups[PW_BLUEFLAG])
	{
		assists_plus = 0;
	}
	else
	{
		assists_plus = level.time - client->pers.teamState.lastfraggedcarrier;
	}

	if (g_gametype.integer != GT_CA)
	{
		team = client->team;                                                    /* Address : 0x2f937 Type : Interium */
	}
	else if (client->team == TEAM_SPECTATOR)
	{
		team = client->clanArenaSpectateForTeam;                                    /* Address : 0x2f947 Type : Interium */
	}
	else
	{
		team = client->team;                                                        /* Address : 0x2f950 Type : Interium */
	}

	strcpy(wstatsHead, va("%i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i ",
	                      1,
	                      client->ps.persistant[PERS_SCORE],
	                      team,
	                      client->stats.kills,
	                      client->stats.deaths,
	                      client->stats.suicides,
	                      client->stats.teamKills,
	                      client->stats.damageTeam,
	                      client->stats,
	                      client->stats.damageRecieved,
	                      client->wins,
	                      client->loses,
	                      client->pers.teamState.captures,
	                      client->pers.teamState.lasthurtcarrier,
	                      client->pers.teamState.basedefense + client->pers.teamState.carrierdefense,
	                      client->pers.teamState.flagrecovery,
	                      client->pers.teamState.assists + assists_plus,  // <<-------------------------
	                      client->stats.mh,
	                      client->stats.ga,
	                      client->stats.ra,
	                      client->stats.ya));

	wstatsString[0] = 0;
	weaponmask = 0;
	for (wp = 1; wp < 10; ++wp)
	{
		ws = &client->clientWeaponStats[wp];
		if (ws->attacks || ws->hits || ws->deaths)
		{
			weaponmask |= 1 << wp;
			Q_strcat(wstatsString, MAX_STRING_CHARS, va(" %d %d %d %d", ws->hits, ws->attacks, ws->kills, ws->deaths));
		}
	}
	trap_SendServerCommand(ent - g_entities, va("statsinfo %s %d%s", wstatsHead, weaponmask, wstatsString));
}
/*
=================
ClientCommand
=================
*/

void ClientCommand(int clientNum)
{
	gentity_t* ent;
	char    cmd[MAX_TOKEN_CHARS];


	ent = g_entities + clientNum;
	if (!ent->client)
	{
		return;     // not fully in game yet
	}

	trap_Argv(0, cmd, sizeof(cmd));

	if (ent->client->OSPAuth > 0 && server_ospauth.integer)
	{
		G_OSPAuth(ent);
	}
	trap_Argv(0, &cmd[0], 0x400);
	if (Q_stricmp(cmd, "score") == 0)
	{
		Cmd_Score_f(ent);
	}
	else if (Q_stricmp(cmd, "say") == 0)
	{
		Cmd_Say_f(ent, 0, 0);
	}
	else if (Q_stricmp(cmd, "say_team") == 0)
	{
		Cmd_Say_f(ent, 1, 0);
	}
	else if (Q_stricmp(cmd, "tell") == 0)
	{
		Cmd_Say_f(ent, 1, 0);
	}
	else if (Q_stricmp(cmd, "team") == 0)
	{
		if (!Cmd_Help_f(ent, CMDHELP_TEAM))
		{
			Cmd_Team_f(ent);
		}
	}
	else if (Q_stricmp(cmd, "uinfo") == 0)
	{
		Cmd_Uinfo_f(ent);
	}
	else if (Q_stricmp(cmd, "uvinfo") == 0)
	{
		Cmd_Uinfo_f(ent);
		Cmd_ViewList_f(ent);
	}
	else if ((Q_stricmp(cmd, "ref") == 0) || (Q_stricmp(cmd, "admin") == 0) || (Q_stricmp(cmd, "referee") == 0))
	{
		if (!Cmd_Help_f(ent, CMDHELP_REF))
		{
			Cmd_RefCommand_f(ent, 0);
		}
	}
	else if ((Q_stricmp(cmd, "players") == 0) || (Q_stricmp(cmd, "playerslist") == 0))
	{
		Cmd_PlayerList_f(ent);
	}
	else if (Q_stricmp(cmd, "getstatsinfo") == 0)
	{
		Cmd_GetStatsInfo_f(ent);
	}
	else if ((Q_stricmp(cmd, "stats") == 0) || (Q_stricmp(cmd, "stat") == 0))
	{
		Cmd_Stats_f(ent, 0);
	}
	else if (Q_stricmp(cmd, "statsdump") == 0)
	{
		Cmd_Stats_f(ent, 1);
	}
	else if (Q_stricmp(cmd, "statsall") == 0)
	{
		Cmd_StatsAll_f(ent, 0);
	}
	else if (Q_stricmp(cmd, "statsblue") == 0)
	{
		Cmd_StatsAll_f(ent, 2);
	}
	else if (Q_stricmp(cmd, "statsred") == 0)
	{
		Cmd_StatsAll_f(ent, 1);
	}
	else if ((Q_stricmp(cmd, "topshots") == 0) || (Q_stricmp(cmd, "acc") == 0) || (Q_stricmp(cmd, "accuracy") == 0))
	{
		Cmd_Acc_f(ent, 1);
	}
	else if (Q_stricmp(cmd, "bottomshots") == 0)
	{
		Cmd_Acc_f(ent, 0);
	}
	else if (server_ospauth.integer && Q_stricmp(cmd, "htuatneilc") == 0)
	{
		Cmd_OSPAuth_f(ent);
	}
	else if (Q_stricmp(cmd, "autoscreenshot") == 0)
	{
		if (!Cmd_Help_f(ent, CMDHELP_AUTOSCREENSHOT))
		{
			Cmd_RefCommand_f(ent, 0);
			Cmd_AutoScreenshot_f(ent - g_entities, 0, 1);
		}
	}
	else if (Q_stricmp(cmd, "scores") == 0)
	{
		switch (g_gametype.integer)
		{
			case GT_TEAM:
				if (server_freezetag.integer)
				{
					Cmd_ScoresFT_f(ent, 0);
				}
				else
				{
					Cmd_ScoresDM_f(ent, 0);
				}
				break;
			case GT_CTF:
				Cmd_ScoresCTF_f(ent, 0);
				break;
			case GT_CA:
				Cmd_ScoresCA_f(ent, 0);
				break;
			case GT_FFA:
			case GT_TOURNAMENT:
				Cmd_ScoresFFA_f(ent, 0);
				break;
			default:
				break;
		}

	}
	else if (Q_stricmp(cmd, "viewlist") == 0)
	{
		Cmd_ViewList_f(ent);
	}
	else if (ent->client->isReferee && Cmd_RefCommandArg_f(ent, cmd, 0))
	{
		return;
	}
	else if ((Q_stricmp(cmd, "callvote") == 0) || (Q_stricmp(cmd, "vote") == 0))
	{
		Cmd_Help_f(ent, CMDHELP_CALLVOTE);
		Cmd_CallVote2_f(ent, 0);
	}
	else if (Q_stricmp(cmd, "vote") == 0)
	{
		Cmd_Vote_f(ent);
	}
	else if (level.intermissiontime)
	{
		Cmd_Say_f(ent, qfalse, qtrue);
	}
	else if (Q_stricmp(cmd, "give") == 0)
	{
		Cmd_Give_f(ent);
	}
	else if (Q_stricmp(cmd, "god") == 0)
	{
		Cmd_God_f(ent);
	}
	else if (Q_stricmp(cmd, "notarget") == 0)
	{
		Cmd_Notarget_f(ent);
	}
	else if (Q_stricmp(cmd, "noclip") == 0)
	{
		Cmd_Noclip_f(ent);
	}
	else if (Q_stricmp(cmd, "kill") == 0)
	{
		Cmd_Kill_f(ent);
	}
	else if (Q_stricmp(cmd, "follow") == 0)
	{
		if (!Cmd_Help_f(ent, CMDHELP_FOLLOW))
		{
			Cmd_Follow_f(ent);
		}
	}
	else if (Q_stricmp(cmd, "follownext") == 0)
	{
		Cmd_FollowCycle_f(ent, 1);
	}
	else if (Q_stricmp(cmd, "followprev") == 0)
	{
		Cmd_FollowCycle_f(ent, -1);
	}
	else if (Q_stricmp(cmd, "followprev") == 0)
	{
		Cmd_FollowCycle_f(ent, -1);
	}
	else if (Q_stricmp(cmd, "where") == 0)
	{
		Cmd_Where_f(ent);
	}
	else if (Q_stricmp(cmd, "gc") == 0)
	{
		Cmd_GameCommand_f(ent);
	}
	else if (!Cmd_OSPCommands_f(ent, cmd, qfalse))
	{
		trap_SendServerCommand(clientNum, va("print \"Unknown command: ^5%s^7\nType ^3\\help^7 for a list of all commands.\n\"", &cmd[0]));
	}
}

