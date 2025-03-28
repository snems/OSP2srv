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
#include "osp_local.h"

#include "../../ui/menudef.h"           // for the voice chats
#include "q_shared.h"

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
//

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
void SanitizeString(const char* in, char* out, qboolean toLower)
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
	//if (oldTeam == TEAM_RED || oldTeam == TEAM_BLUE)
	//{
	//	CheckTeamLeader(oldTeam);
	//}

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

/*
==================
Cmd_CallVote_f
==================
*/
qboolean Cmd_CallVote_f(const gentity_t* ent, int unk)
{
	int     i;
	char    arg1[MAX_STRING_TOKENS];
	char    arg2[MAX_STRING_TOKENS];
	qboolean rez = qfalse;

	if (!g_allowVote.integer)
	{
		trap_SendServerCommand(ent - g_entities, "print \"Voting not allowed here.\n\"");
		return rez;
	}

	if (level.voteTime)
	{
		trap_SendServerCommand(ent - g_entities, "print \"A vote is already in progress.\n\"");
		return rez;
	}
	if (ent->client->pers.voteCount >= MAX_VOTE_COUNT)
	{
		trap_SendServerCommand(ent - g_entities, "print \"You have called the maximum number of votes.\n\"");
		return rez;
	}
	if (ent->client->sess.sessionTeam == TEAM_SPECTATOR)
	{
		trap_SendServerCommand(ent - g_entities, "print \"Not allowed to call a vote as spectator.\n\"");
		return rez;
	}

	// make sure it is a valid command to vote on
	trap_Argv(1, arg1, sizeof(arg1));
	trap_Argv(2, arg2, sizeof(arg2));

	if (strchr(arg1, ';') || strchr(arg2, ';'))
	{
		trap_SendServerCommand(ent - g_entities, "print \"Invalid vote string.\n\"");
		return rez;
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
		return rez;
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
			return rez;
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
			return rez;
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
	return rez;
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
			Cmd_RefCommand_f(ent, qfalse);
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
			Cmd_RefCommand_f(ent, qfalse);
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
		Cmd_CallVote_f(ent, 0);
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

