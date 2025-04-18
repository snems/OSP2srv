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
// bg_local.h -- local definitions for the bg (both games) files

#define MIN_WALK_NORMAL 0.7f        // can't walk on very steep slopes

#define STEPSIZE        18

#define JUMP_VELOCITY   270

#define TIMER_LAND      130
#define TIMER_GESTURE   (34*66+50)

#define OVERCLIP        1.001f

// all of the locals will be zeroed before each
// pmove, just to make damn sure we don't have
// any differences when running on client or server
typedef struct
{
	vec3_t      forward, right, up;
	float       frametime;

	int         msec;

	qboolean    walking;
	qboolean    groundPlane;
	trace_t     groundTrace;

	float       impactSpeed;

	vec3_t      previous_origin;
	vec3_t      previous_velocity;
	int         previous_waterlevel;
} pml_t;

extern  pmove_t*     pm;
extern  pml_t       pml;

// movement parameters
extern  float   pm_stopspeed;
extern  float   pm_duckScale;
extern  float   pm_swimScale;
extern  float   pm_wadeScale;

extern  float   pm_accelerate;
extern  float   pm_airaccelerate;
extern  float   pm_wateraccelerate;
extern  float   pm_flyaccelerate;

extern  float   pm_friction;
extern  float   pm_waterfriction;
extern  float   pm_flightfriction;

extern  int     c_pmove;

void PM_ClipVelocity(vec3_t in, vec3_t normal, vec3_t out, float overbounce);
void PM_AddTouchEnt(int entityNum);
void PM_AddEvent(int newEvent);

qboolean    PM_SlideMove(qboolean gravity);
void        PM_StepSlideMove(qboolean gravity);

extern int   modePickupDistance;
extern float pm_accelerate;
extern float pm_friction;
extern float modePromode_pm_airaccelerate_1;
extern int   modePredictionKoeff2;
extern float modePromode_pm_airaccelerate_2;
extern float modeWishspeedLimit;
extern int   modePredictionKoeff1;
extern float modeWaterFriction;
extern float modeSwimScale1;
extern float modeSwimScale2;
extern float modeWaterAccelerate;
extern float modeOSP1;
extern int   modeOSP2;
extern int   modeOSP3;
extern int   modeOSP4;
extern int   modeOSP5;
extern int   modeOSP6;
extern int   modeOSP7;
extern float modeShotgunKoeff;
extern int   modeShotgunNumberOfPellets;
extern float modeOSP8;
extern int   modeMaxAmmoShotgun;
extern int   modeGrenadeTime;
extern int   modeMaxAmmoGrenade;
extern int   modeOSP9;
extern int   modeMaxAmmoRocket;
extern float modeOSP10;
extern float modeOSP11;
extern float modeOSP12;
extern int   modeMaxAmmoRail;
extern int   modeOSP13;
extern int   modeOSP14;
extern int   modeOSP15;
extern float modeOSP16;
extern int   modeBeginWeaponChangeTime;
extern int   modeFinishWeaponChangeTime;
extern int   modePMNoAmmoTime;
extern qboolean modeDropAllAmmo;
extern int modeOSP20;
extern qboolean modeOSP21;
extern int   pm_armorPromode;
extern float modeOSP17;
extern float modeOSP18;
extern int   modeOSP22;
extern int   modeOSP23;
extern int   modeOSP24;
extern int   modeOSP25;
extern int   modeOSP26;
extern int   modeOSP27;
extern int   modeOSP28;
extern int   modeOSP29;
extern int   modeOSP30;
extern qboolean modeHitLevelSounds;
extern float modeOSP31;
extern float modeOSP32;
extern float modeOSP33;
extern float modeOSP34;
extern int modeOSP35;
extern int modeOSP36;
extern int modeOSP37;
extern int modeShotgunPromode;
extern int modePromodePhysKoeff;

typedef enum 
{
	OSP_SERVER_MODE_VQ3      = 0,
	OSP_SERVER_MODE_PROMODE  = 1,
	OSP_SERVER_MODE_CQ3      = 2,
} ospMode_t;

#define  OSP_SERVER_MODE_PROMODE_OPT1      0x10
#define  OSP_SERVER_MODE_PROMODE_OPT2      0x20

enum OSPArmorType
{
	OSP_ARMOR_GREEN = 0,
	OSP_ARMOR_YELLOW = 1,
	OSP_ARMOR_RED = 2,
};

