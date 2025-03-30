#include "g_local.h"
#include "osp_local.h"
#include "q_shared.h"

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



/*
=================
Cmd_RefCommand_f
=================
*/
qboolean Cmd_OSPCommands_f(const gentity_t* ent, char* arg, qboolean isReferee)
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

void Cmd_HelpAvailOspGameCommands_f(const gentity_t* ent)
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
Cmd_Help_f
=================
*/
qboolean Cmd_Help_f(const gentity_t* ent, int cmdNumber)
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
void Cmd_Uinfo_f(const gentity_t* ent)
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
	ent->client->isCustomLocEnabled = atoi(buf);

	trap_Argv(5, buf, MAX_STRING_CHARS);
	ui_flags = atoi(buf);
	ent->client->isFollowPowerUp = ui_flags & 1;
	ent->client->isFollowKiller = ui_flags & 2;
	ent->client->isFollowViewCam = ui_flags & 4;

	trap_Argv(6, buf, MAX_STRING_CHARS);
	ent->client->autoAction = atoi(buf);

	trap_Argv(7, buf, MAX_STRING_CHARS);
	ent->client->isUsingJPEG = atoi(buf);
}


void osp_cmds_2d54a(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_2d54a);
}


void Cmd_SpecOnly_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_SpecOnly_f);
}


void Cmd_Lock_f(const gentity_t* ent, char* arg)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_Lock_f);
}


void osp_cmds_2d73a(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_2d73a);
}


void Cmd_SpecLock_f(const gentity_t* ent, char* arg)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_SpecLock_f);
}


void Cmd_Ready_f(const gentity_t* ent, qboolean unk)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_Ready_f);
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
void Cmd_PlayerList_f(const gentity_t* ent)
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
			if (clientLocal->sess.sessionTeam == TEAM_SPECTATOR && clientLocal->pers.connected == CON_CONNECTING)
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
			team = clientLocal->sess.sessionTeam;
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


void Cmd_Invite_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_Invite_f);
}


void Cmd_SpecInvite_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_SpecInvite_f);
}


void Cmd_CoachInvite_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_CoachInvite_f);
}


void Cmd_CoachDecline_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_CoachDecline_f);
}


void Cmd_CoachKick_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_CoachKick_f);
}


void Cmd_Coach_f(const gentity_t* ent, qboolean unk)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_Coach_f);
}


void Cmd_Remove_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_Remove_f);
}


void Cmd_Resign_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_Resign_f);
}


void Cmd_TeamReady_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_TeamReady_f);
}


void Cmd_Capitans_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_Capitans_f);
}


void Cmd_Pause_f(const gentity_t* ent, qboolean unk)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_Pause_f);
}


void Cmd_Stats_f(const gentity_t* ent, int arg)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_Stats_f);
}


void Cmd_StatsAll_f(const gentity_t* ent, int arg)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_StatsAll_f);
}


void osp_cmds_2ee67(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_2ee67);
}


void osp_cmds_2f019(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_2f019);
}


void Cmd_Acc_f(const gentity_t* ent, int arg)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_Acc_f);
}


void Cmd_JoinCode_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_JoinCode_f);
}


void Cmd_Drop_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_Drop_f);
}

/*
=================
Cmd_GetStatsInfo_f
=================
*/
void Cmd_GetStatsInfo_f(gentity_t* ent)
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

	if ((client->sess.sessionTeam != TEAM_SPECTATOR && g_gametype.integer == GT_TEAM && server_freezetag.integer && G_IsSpectator(client)) ||
	        (intermission && G_IsGameTypeOSP(g_gametype.integer) && G_GetSpectatorTeam(ent) != TEAM_SPECTATOR))
	{
		playerID = ent - g_entities;
	}
	else
	{
		if (ent->client->sess.spectatorState == 0x2 || ent->client->sess.spectatorState == 0x5)
		{
			playerID = ent->client->sess.spectatorClient;
		}
		else if (ent->client->sess.spectatorState == 0x7 && ent->client->tail3_25)
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
		team = client->sess.sessionTeam;
	}
	else if (client->sess.sessionTeam == TEAM_SPECTATOR)
	{
		team = client->clanArenaSpectateForTeam;
	}
	else
	{
		team = client->sess.sessionTeam;
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
	                      client->sess.wins,
	                      client->sess.losses,
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


void osp_cmds_2fa77(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_2fa77);
}


void osp_cmds_2fc2f(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_2fc2f);
}


void osp_cmds_2fcba(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_2fcba);
}


void osp_cmds_2fd00(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_2fd00);
}


void osp_cmds_30178(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_30178);
}


void osp_cmds_30373(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_30373);
}


void osp_cmds_3048d(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3048d);
}


void osp_cmds_3050e(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3050e);
}


void osp_cmds_306a0(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_306a0);
}


void osp_cmds_307cb(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_307cb);
}


void osp_cmds_30811(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_30811);
}


void osp_cmds_30876(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_30876);
}


void osp_cmds_308b5(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_308b5);
}


void Cmd_Settings_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_Settings_f);
}


void osp_cmds_30b09(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_30b09);
}


void osp_cmds_30c10(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_30c10);
}


void osp_cmds_30c9d(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_30c9d);
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


void osp_cmds_30dd4(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_30dd4);
}


void osp_cmds_30f4f(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_30f4f);
}


void osp_cmds_30f90(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_30f90);
}


void osp_cmds_30f9f(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_30f9f);
}


void osp_cmds_30ff1(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_30ff1);
}


void osp_cmds_31023(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_31023);
}


void osp_cmds_31061(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_31061);
}


void osp_cmds_3114c(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3114c);
}


void osp_cmds_3145e(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3145e);
}


void osp_cmds_31607(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_31607);
}


void osp_cmds_3189f(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3189f);
}


qboolean G_IsItemAllowed(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(G_IsItemAllowed);
	return qfalse;
}


void G_RegisterWeapon(void)
{
	if (weapon_have.integer & 1 || weapon_start.integer & 1)
	{
		RegisterItem(BG_FindItemForWeapon(WP_MACHINEGUN));
	}

	RegisterItem(BG_FindItem("Bullets"));

  /*	WP_SHOTGUN */
	if (weapon_have.integer & 2 || weapon_start.integer & 2)
	{
		RegisterItem(BG_FindItemForWeapon(WP_MACHINEGUN));
	}

	if (weapon_have.integer & 4 || weapon_start.integer & 4)
	{
		RegisterItem(BG_FindItemForWeapon(WP_GRENADE_LAUNCHER));
	}

	if (weapon_have.integer & 8 || weapon_start.integer & 8)
	{
		RegisterItem(BG_FindItemForWeapon(WP_ROCKET_LAUNCHER));
	}

	if (weapon_have.integer & 0x10 || weapon_start.integer & 0x10)
	{
		RegisterItem(BG_FindItemForWeapon(WP_LIGHTNING));
	}

	if (weapon_have.integer & 0x40 || weapon_start.integer & 0x40)
	{
		RegisterItem(BG_FindItemForWeapon(WP_PLASMAGUN));
	}

	if (weapon_have.integer & 0x80 || weapon_start.integer & 0x80)
	{
		RegisterItem(BG_FindItemForWeapon(WP_BFG));
	}

	RegisterItem(BG_FindItemForWeapon(WP_GRAPPLING_HOOK));
	RegisterItem(BG_FindItemForWeapon(WP_RAILGUN));
}


void osp_cmds_31deb(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_31deb);
}


void osp_cmds_3213d(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3213d);
}


void osp_cmds_321e7(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_321e7);
}


qboolean G_IsAttackEnabled(void)
{
	if (g_gametype.integer == GT_CA && level.warmupTime && !match_cawarmupfire.integer)
	{
		return qfalse;
	}
	return qtrue;
}



void osp_cmds_32212(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_32212);
}


void osp_cmds_322a5(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_322a5);
}


void osp_cmds_3235a(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3235a);
}


void osp_cmds_324a9(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_324a9);
}


void osp_cmds_324e2(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_324e2);
}


void osp_cmds_32548(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_32548);
}


void osp_cmds_3257c(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3257c);
}


void osp_cmds_32682(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_32682);
}


void osp_cmds_3269b(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3269b);
}


void osp_cmds_32776(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_32776);
}


void osp_cmds_32854(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_32854);
}


void osp_cmds_3285c(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3285c);
}


void osp_cmds_328fb(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_328fb);
}


void osp_cmds_3294d(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3294d);
}


void osp_cmds_329e9(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_329e9);
}


void osp_cmds_32a7e(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_32a7e);
}


void osp_cmds_32d19(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_32d19);
}


void osp_cmds_32d5b(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_32d5b);
}


void Cmd_AutoScreenshot_f(int entityNum, qboolean unk1, qboolean unk2)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_AutoScreenshot_f);
}


void osp_cmds_3303c(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3303c);
}


void osp_cmds_330e2(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_330e2);
}


void osp_cmds_331c9(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_331c9);
}


void osp_cmds_33222(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_33222);
}


void osp_cmds_332db(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_332db);
}


void osp_cmds_3344e(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3344e);
}


void osp_cmds_33482(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_33482);
}


void osp_cmds_334b6(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_334b6);
}


void G_OSPAuth(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(G_OSPAuth);
}


void Cmd_OSPAuth_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_OSPAuth_f);
}


void osp_cmds_33674(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_33674);
}


void osp_cmds_33677(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_33677);
}


void osp_cmds_337ae(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_337ae);
}


void osp_cmds_3398b(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3398b);
}


void osp_cmds_339b6(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_339b6);
}


void osp_cmds_339ff(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_339ff);
}


void osp_cmds_33aa6(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_33aa6);
}


void osp_cmds_33b68(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_33b68);
}


void osp_cmds_33c18(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_33c18);
}


void osp_cmds_33c92(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_33c92);
}


void osp_cmds_33cc5(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_33cc5);
}


void osp_cmds_33d12(qboolean arg)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_33d12);
}


void osp_cmds_33d90(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_33d90);
}


void osp_cmds_33dc1(qboolean flag)
{
	char* instagibPrefix;
	int i;
	gentity_t* ent;

	instagibPrefix = match_instagib.integer > 0 ? "EN" : "DIS";

	if ((time_in_game - level.startTime) > 10000)
	{
		trap_SendServerCommand(-1, va("cp \"^1Instagib %sABLED!\n\"", instagibPrefix));
		trap_SendServerCommand(-1, va("print \"^1Instagib %sABLED!\n\"", instagibPrefix));
	}

	G_Printf("Instagib %sABLED!\n", instagibPrefix);

	if (match_instagib.integer > 0)
	{
		int i;
		gentity_t* ent;
		for (i = level.maxclients, ent = &g_entities[level.maxclients]; i < level.num_entities; ++i, ++ent)
		{
			if (ent->inuse && ent->tail0 && !G_IsItemAllowed(ent))
			{
					if (!(ent->s.eFlags & EF_NODRAW) && ent->r.contents)
					{
							ent->r.svFlags |= SVF_NOCLIENT;
							ent->s.eFlags |= EF_NODRAW;

							ent->r.contents = 0;
							trap_LinkEntity(ent);
							if (ent->targetShaderName != 0)
							{
								RespawnItem(ent);
							}
					}
					else
					{
						if (ent->think && ent->reached && !ent->targetShaderName)
						{
							ent->think = 0;
							ent->reached = 0;
							trap_LinkEntity(ent);
						}
					}

			}
		}
	}
	else if (flag)
	{
		osp_cmds_32a7e();
		for (i = level.maxclients, ent = &g_entities[level.maxclients]; i < level.num_entities; ++i, ++ent)
		{
			if (ent->inuse && ent->tail0 && G_IsItemAllowed(ent))
			{
				ent->r.contents = CONTENTS_TRIGGER;
				ent->s.eFlags &= ~SVF_NOCLIENT;
				ent->r.svFlags &= ~EF_NODRAW;
				FinishSpawningItem(ent);
			}
		}
	}
	if (flag)
	{
		for (i = 0, ent = &g_entities[level.maxclients]; i < level.maxclients; ++i, ++ent)
		{
			if (ent->inuse && ent->client->pers.connected == CON_CONNECTED && ent->client->sess.sessionTeam != TEAM_SPECTATOR && ent->client->ps.pm_type != PM_DEAD)
			{
				int p;
				osp_cmds_31deb(ent);
				osp_cmds_3213d(ent);
				for (p = 0; p < 16; ++p)
				{
					ent->client->ps.powerups[p] = 0;
				}
			}
		}
	}
}


void osp_cmds_33fa0(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_33fa0);
}


void osp_cmds_34159(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_34159);
}


void osp_cmds_341ad(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_341ad);
}


void osp_cmds_341e1(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_341e1);
}


void osp_cmds_34210(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_34210);
}


void osp_cmds_3424f(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3424f);
}


void osp_cmds_3427c(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3427c);
}


void osp_cmds_3461e(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3461e);
}


void osp_cmds_34627(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_34627);
}


void osp_cmds_3466d(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3466d);
}


void osp_cmds_3497d(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3497d);
}


void osp_cmds_349dc(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_349dc);
}


void osp_cmds_34c31(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_34c31);
}


void osp_cmds_34d12(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_34d12);
}


void osp_cmds_34d98(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_34d98);
}


void osp_cmds_34dac(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_34dac);
}


void Cmd_MapLoad_f(qboolean unk)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_MapLoad_f);
}


void osp_cmds_35068(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_35068);
}


void Cmd_MapList_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_MapList_f);
}


void osp_cmds_3525b(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3525b);
}


void osp_cmds_352ff(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_352ff);
}


void osp_cmds_35309(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_35309);
}


void osp_cmds_3532f(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3532f);
}


void osp_cmds_35419(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_35419);
}


void Cmd_ViewAdd_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_ViewAdd_f);
}


void Cmd_ViewerMove_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_ViewerMove_f);
}


void Cmd_ViewFollow(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_ViewFollow);
}


void Cmd_ViewNext_f(const gentity_t* ent, int unk)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_ViewNext_f);
}


void Cmd_ViewFrame_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_ViewFrame_f);
}


void Cmd_ViewCycleNext_f(const gentity_t* ent, int unk)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_ViewCycleNext_f);
}


void Cmd_ViewNone_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_ViewNone_f);
}


void Cmd_ViewList_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_ViewList_f);
}


void Cmd_ViewAll_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_ViewAll_f);
}


void Cmd_ViewTeam_f(const gentity_t* ent, int unk1, int unk2)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_ViewTeam_f);
}


void osp_cmds_35d93(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_35d93);
}


void osp_cmds_35fb6(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_35fb6);
}


void osp_cmds_3601a(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3601a);
}


void osp_cmds_361d8(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_361d8);
}


void osp_cmds_36285(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_36285);
}


void osp_cmds_3646f(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3646f);
}


void osp_cmds_36551(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_36551);
}


qboolean Cmd_RefCommandArg_f(const gentity_t* ent, const char* arg, qboolean unk)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_RefCommandArg_f);
	return (qboolean)0;
}
/*
=================
Cmd_RefCommand_f
=================
*/
void Cmd_RefCommand_f(const gentity_t* ent, qboolean skipOSPCommands)
{
	char text[MAX_STRING_CHARS];
	if (ent->client->isReferee)
	{
		if (!skipOSPCommands)
		{
			trap_Argv(1, &text[0], MAX_STRING_CHARS);
			if (Cmd_OSPCommands_f(ent, &text[0], qtrue) && Cmd_RefCommandArg_f(ent, &text[0], 1) && !Cmd_CallVote_f(ent, 1))
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


void Cmd_RefHelp(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_RefHelp);
}


void osp_cmds_36b67(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_36b67);
}


void osp_cmds_36cfa(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_36cfa);
}


void osp_cmds_36edf(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_36edf);
}


void osp_cmds_36f6e(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_36f6e);
}


void osp_cmds_3705d(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3705d);
}


void osp_cmds_3710c(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3710c);
}


void osp_cmds_371be(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_371be);
}


void osp_cmds_371cb(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_371cb);
}


void osp_cmds_372a0(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_372a0);
}


void osp_cmds_3740f(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3740f);
}


void osp_cmds_37506(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_37506);
}


void osp_cmds_375e1(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_375e1);
}


void osp_cmds_376a1(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_376a1);
}


void osp_cmds_376d7(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_376d7);
}


void osp_cmds_37731(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_37731);
}


void osp_cmds_3775b(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3775b);
}


void osp_cmds_378e0(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_378e0);
}


void osp_cmds_37910(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_37910);
}


void osp_cmds_37a00(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_37a00);
}


void osp_cmds_37c09(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_37c09);
}


void osp_cmds_37e43(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_37e43);
}


void osp_cmds_37e7f(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_37e7f);
}


void osp_cmds_37fd9(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_37fd9);
}


void osp_cmds_38260(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_38260);
}


void osp_cmds_384cb(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_384cb);
}


void osp_cmds_385cf(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_385cf);
}


void osp_cmds_386d3(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_386d3);
}


void osp_cmds_387f6(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_387f6);
}


void osp_cmds_388eb(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_388eb);
}


void osp_cmds_38964(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_38964);
}


void osp_cmds_38972(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_38972);
}


void osp_cmds_3897a(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3897a);
}


void osp_cmds_38985(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_38985);
}

/*
=================
G_GetSpectatorTeam
=================
*/
team_t G_GetSpectatorTeam(const gentity_t* ent)
{
	if (ent->client->viewTeam)
	{
		return ent->client->viewTeam;
	}
	if (G_IsGameTypeOSP(g_gametype.integer) && ent->client->sess.sessionTeam != TEAM_SPECTATOR && ent->client->clanArenaSpectateForTeam != TEAM_SPECTATOR)
	{
		return ent->client->clanArenaSpectateForTeam;
	}
	return ent->client->sess.sessionTeam;
}


void osp_cmds_38ac2(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_38ac2);
}


void osp_cmds_38c01(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_38c01);
}


void osp_cmds_38c45(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_38c45);
}


void osp_cmds_38c7b(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_38c7b);
}


void osp_cmds_38d3c(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_38d3c);
}


void osp_cmds_38dbe(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_38dbe);
}


void osp_cmds_38dea(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_38dea);
}


void Cmd_ScoresDM_f(const gentity_t* ent, int arg)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_ScoresDM_f);
}


void Cmd_ScoresFT_f(const gentity_t* ent, int arg)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_ScoresFT_f);
}


void Cmd_ScoresFFA_f(const gentity_t* ent, int arg)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_ScoresFFA_f);
}


void Cmd_ScoresCTF_f(const gentity_t* ent, int arg)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_ScoresCTF_f);
}


void Cmd_ScoresCA_f(const gentity_t* ent, int arg)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_ScoresCA_f);
}


void osp_cmds_39844(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_39844);
}


void osp_cmds_3986a(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3986a);
}


void osp_cmds_3994e(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3994e);
}


void osp_cmds_39979(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_39979);
}


void osp_cmds_39a71(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_39a71);
}


void osp_cmds_39c0b(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_39c0b);
}


void osp_cmds_39cf2(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_39cf2);
}


void osp_cmds_39e12(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_39e12);
}


void osp_cmds_3a004(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3a004);
}


void osp_cmds_3a168(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3a168);
}


void osp_cmds_3a413(void)
{
	OSP2_UNIMPLEMENTED_FUNCTION(osp_cmds_3a413);
}


void Cmd_Snapshot_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_Snapshot_f);
}


void Cmd_SnapshotDump_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_SnapshotDump_f);
}


void Cmd_VcProximity_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_VcProximity_f);
}


void Cmd_Range_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_Range_f);
}


void Cmd_ViewCam_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_ViewCam_f);
}


void Cmd_VcFollow_f(const gentity_t* ent)
{
	OSP2_UNIMPLEMENTED_FUNCTION(Cmd_VcFollow_f);
}
