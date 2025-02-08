# OSP2 server

- [X] osp variables list
- [X] osp commands list
- [ ] wstats
- [ ] freezetag
- [ ] clan arena


## Commands:

### CLIENT

- [ ] `uinfo`
- [ ] `uvinfo`
- [X] `ref/admin/referee`
- [ ] `players`
- [ ] `playerlist`
- [ ] `getstatsinfo`
- [ ] `stats`
- [ ] `stat`
- [ ] `statsdump`
- [ ] `statsall`
- [ ] `statsblue`
- [ ] `statsred`
- [ ] `topshots`
- [ ] `acc`
- [ ] `accuracy`
- [ ] `bottomshots`
- [ ] `autoscreenshot`
- [ ] `scores`
- [ ] `viewlist`
- [ ] `cv`
- [ ] `follow`
- [ ] `follownext`
- [ ] `followprev`

### OSP commands

- [ ] `lock`
- [ ] `unlock`
- [ ] `speclock`
- [ ] `specunlock`
- [ ] `ready`
- [ ] `notready`
- [ ] `unready`
- [ ] `teamready`
- [ ] `readyteam`
- [ ] `invite`
- [ ] `pickplayer`
- [ ] `specinvite`
- [ ] `invitespec`
- [ ] `remove`
- [ ] `kickplayer`
- [ ] `captains`
- [ ] `resign`
- [ ] `joincode`
- [ ] `time`
- [ ] `pause`
- [ ] `timeout`
- [ ] `timein`
- [ ] `unpause`
- [ ] `help`
- [ ] `commands`
- [ ] `followpowerup`
- [ ] `speconly`
- [ ] `autorecord`
- [ ] `maplist`
- [ ] `mapload`
- [ ] `range`
- [ ] `snapshot`
- [ ] `snapshotdump`
- [ ] `viewadd`
- [ ] `viewall`
- [ ] `viewremove`
- [ ] `viewfollow`
- [ ] `viewnone`
- [ ] `viewframe`
- [ ] `viewnext`
- [ ] `viewprev`
- [ ] `viewcyclenext`
- [ ] `viewcycleprev`
- [ ] `viewred`
- [ ] `viewblue`
- [ ] `coach`
- [ ] `coachinvite`
- [ ] `coachdecline`
- [ ] `coachkick`
- [ ] `vc_proximity`
- [ ] `vc_info`
- [ ] `vc_follow`
- [ ] `vc_free`
- [ ] `vc_view`
- [ ] `drop`
- [ ] `settings`
- [X] `rhea/osp/dess/shod`
- [X] `arqon`

### Referee commands

- [ ] `r_pause/pause`
- [ ] `r_kick`
- [ ] `r_map`
- [ ] `r_ban/ban`
- [ ] `r_abort/abort`
- [ ] `r_help`
- [ ] `r_allcaptain/allcaptain`
- [ ] `r_allready/allready`
- [ ] `r_lock`
- [ ] `r_unlock`
- [ ] `r_speclock`
- [ ] `r_specunlock`
- [ ] `r_lockred/lockred`
- [ ] `r_unlockred/unlockred`
- [ ] `r_lockblue/lockblue`
- [ ] `r_unlockblue/unlockblue`
- [ ] `r_speclockred/speclockred`
- [ ] `r_specunlockred/specunlockred`
- [ ] `r_specklockblue/speclockblue`
- [ ] `r_specunlockblue/specunlockblue`
- [ ] `r_mutespecs/mutespecs`
- [ ] `r_promote/promote`
- [ ] `r_putred/putred`
- [ ] `r_putblue/putblue`
- [ ] `r_remove/remove`
- [ ] `r_restart/restart`

## Variables:

### STARTUP (Reference)

- [X] `admin_log <logname>`
- [ ] `dmflags <df_mask>`
- [ ] `filter_file <filename>`
- [ ] `freezetag_latejoin <0|1>`
- [ ] `freezetag_respawn <1.7-10.0>`
- [ ] `g_spectatorInactivity <value>`
- [ ] `player_file <filename>`
- [ ] `player_always <name>`
- [ ] `pmove_fixed <0|1>`
- [ ] `osp_gamename <0|1>`
- [ ] `server_availmodes <flags>`
- [ ] `server_cq3 <0|1>`
- [ ] `server_customclient <flags>`
- [ ] `server_fastrail <0|1>`
- [ ] `server_freezetag <0|1|2>`
- [ ] `server_gfxspecfile <filename>`
- [ ] `server_lgcooldown <0|1>`
- [ ] `server_maxpacketsmin <value>`
- [ ] `server_maxpacketsmax <value>`
- [ ] `server_motdfile <filename>`
- [ ] `server_motdX <string> (X = 1 through 8)`
- [ ] `server_ospauth <value>`
- [ ] `server_promode <0|1>`
- [ ] `server_realping <0|1>`
- [ ] `server_record <bitmask>`
- [ ] `server_thrufloors <0|1>`
- [ ] `server_timenudgemin <value>`
- [ ] `server_timenudgemax <value>`

### REFEREE

- [X] `ref_password <string>`

### GENERAL Q3 OPTIONS

- [ ] `bot_autoload <string>`
- [ ] `bot_checktime <value>`
- [ ] `g_gametype <value>`
- [ ] `g_inactivity <seconds>`
- [ ] `g_log <logfilename>`
- [ ] `log_default <0|1>`
- [ ] `log_stat <0|1|2>`

### MATCH-RELATED PARAMETERS

- [ ] `g_doWarmup <0|1>`
- [ ] `g_warmup <seconds>`
- [ ] `match_carespawn <0|1>`
- [ ] `match_cawarmupfire <0|1>`
- [ ] `match_dropitems <0|1>`
- [ ] `match_hurtself <0|1>`
- [ ] `match_instagib <0|1>`
- [ ] `match_lateJoin <0|1>`
- [ ] `match_maxsuicides <value>`
- [ ] `match_minplayers <value>`
- [ ] `match_mutespecs <0|1>`
- [ ] `match_poweruprespawn <60|90|120>`
- [ ] `match_overtime <0|1|2|3>`
- [ ] `match_overtimecount <value>`
- [ ] `match_overtimelength <minutes>`
- [ ] `match_readypercent <percentage from 1-100>`
- [ ] `match_roundtimer <seconds>`
- [ ] `match_timeoutcount <value>`
- [ ] `match_timeoutlength <seconds>`
- [ ] `match_winbyscore <scorespread>`
- [ ] `warmup_armor <value>`

### TEAMPLAY

- [ ] `team_allcaptain <0|1>`
- [ ] `team_armordamage <0|1>`
- [ ] `team_maxplayers <value>`
- [ ] `team_nocontrols <value>`
- [ ] `team_noteamready <0|1>`
- [ ] `g_weaponTeamrespawn <value>`
- [ ] `g_weaponrespawn <value>`
- [ ] `g_friendlyFire <0|1>`
- [ ] `g_teamAutoJoin <0|1>`
- [ ] `g_teamForceBalance <0|1>`

### HOOK

- [ ] `hook_enable <0|1>`
- [ ] `hook_delaytime <milliseconds>`
- [ ] `hook_holdtime <seconds>`
- [ ] `hook_sky <0|1>`
- [ ] `hook_speed <value>`
- [ ] `hook_speedpull <value>`

### MODE CONFIG VOTING

- [ ] `mode_start <mode_id>`
- [ ] `mode_customfile <filename>`

### VOTING

- [ ] `g_allowVote <0|1>`
- [ ] `vote_limit <value>`
- [ ] `vote_allow_armordamage <0|1>`
- [ ] `vote_allow_ca_startrespawn <0|1>`
- [ ] `vote_allow_ca_warmupfire <0|1>`
- [ ] `vote_allow_capfps <0|1>`
- [ ] `vote_allow_cq3 <0|1>`
- [ ] `vote_allow_falling <0|1>`
- [ ] `vote_allow_items <item_mask>`
- [ ] `vote_allow_mutespecs <0|1>`
- [ ] `vote_allow_pmove <0|1>`
- [ ] `vote_allow_poweruprespawn <0|1>`
- [ ] `vote_allow_powerups <powerup_mask>`
- [ ] `vote_allow_reload <0|1>`
- [ ] `vote_allow_respawn <0|1>`
- [ ] `vote_percent <1-100>`

### MAP SETTINGS

- [ ] `map_file	<filename>`
- [ ] `map_autoload <0|1|2|3>`
- [ ] `map_queue <0|1>`
- [ ] `map_rotate <0|1>`
- [ ] `map_random <0|1>`
- [ ] `map_once	<0|1>`
- [ ] `map_nocount <0|1>`
- [ ] `map_halt <0|1>`

### ITEM/WEAPON SETTINGS

- [ ] `armor_q2style <0|1>`
- [ ] `gauntlet_damage <value>`
- [ ] `gauntlet_disable <0|1>`
- [ ] `instagib_reload <100-5000>`
- [ ] `item_disable <item_mask>`
- [ ] `mg_damage <value>`
- [ ] `mg_damageTeamplay <value>`
- [ ] `powerup_disable <powerup_mask>`
- [ ] `railgun_damage <value>`
- [ ] `replace_*: <replace_mask>`
- [ ] `start_*: <value>`
- [ ] `weapon_deaddrop <0|1>`
- [ ] `weapon_have <weapon_mask>`
- [ ] `weapon_start <weapon_mask>`

