/*
	C-Dogs SDL
	A port of the legendary (and fun) action/arcade cdogs.
	Copyright (c) 2013-2016, 2019-2020 Cong Xu
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer.
	Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
	ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
	LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
*/
#include "editor_ui.h"

#include <assert.h>

#include <cdogs/door.h>
#include <cdogs/draw/draw.h>
#include <cdogs/draw/draw_actor.h>
#include <cdogs/draw/drawtools.h>
#include <cdogs/events.h>
#include <cdogs/files.h>
#include <cdogs/font.h>
#include <cdogs/mission.h>
#include <cdogs/mission_convert.h>
#include <cdogs/pic_manager.h>

#include "campaign_options.h"
#include "editor_ui_cave.h"
#include "editor_ui_color.h"
#include "editor_ui_common.h"
#include "editor_ui_objectives.h"
#include "editor_ui_static.h"
#include "editor_ui_weapons.h"

#define Y_ABS 200

static void DrawStyleArea(
	struct vec2i pos, const char *name, const Pic *pic, int idx, int count,
	int isHighlighted);

static const char *CampaignGetTitle(UIObject *o, void *data)
{
	UNUSED(o);
	Campaign *co = data;
	return co->Setting.Title;
}
static EditorResult CampaignEdit(void *data, int d)
{
	UNUSED(d);
	Campaign *c = data;
	return EditCampaignOptions(&gEventHandlers, c);
}
static const char *CampaignGetMissionIndexStr(UIObject *o, void *data)
{
	static char s[128];
	UNUSED(o);
	Campaign *co = data;
	Mission *mission = CampaignGetCurrentMission(co);
	if (mission == NULL)
	{
		if (co->Setting.Missions.size == 0)
		{
			return NULL;
		}
		sprintf(s, "End/%d", (int)co->Setting.Missions.size);
	}
	else
	{
		sprintf(
			s, "Mission %d/%d", co->MissionIndex + 1,
			(int)co->Setting.Missions.size);
	}
	return s;
}
static void CheckMission(UIObject *o, void *data)
{
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
	{
		o->IsVisible = false;
		// Need to unhighlight to prevent children being drawn
		UIObjectUnhighlight(o, false);
		return;
	}
	o->IsVisible = true;
}
MISSION_CHECK_TYPE_FUNC(MAPTYPE_CLASSIC)
static char *MissionGetTitle(UIObject *o, void *data)
{
	UNUSED(o);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
	{
		return NULL;
	}
	return CampaignGetCurrentMission(co)->Title;
}
static void MissionCheckVisible(UIObject *o, void *data)
{
	Campaign *co = data;
	o->IsVisible = CampaignGetCurrentMission(co) != NULL;
}
static char **MissionGetTitleSrc(void *data)
{
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
	{
		return NULL;
	}
	return &CampaignGetCurrentMission(co)->Title;
}
static char *MissionGetDescription(UIObject *o, void *data)
{
	UNUSED(o);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
	{
		return NULL;
	}
	return CampaignGetCurrentMission(co)->Description;
}
static char **MissionGetDescriptionSrc(void *data)
{
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
	{
		return NULL;
	}
	return &CampaignGetCurrentMission(co)->Description;
}
static char *MissionGetSong(UIObject *o, void *data)
{
	UNUSED(o);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
	{
		return NULL;
	}
	return CampaignGetCurrentMission(co)->Song;
}
static const char *MissionGetWidthStr(UIObject *o, void *data)
{
	static char s[128];
	UNUSED(o);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
		return NULL;
	sprintf(s, "Width: %d", CampaignGetCurrentMission(co)->Size.x);
	return s;
}
static const char *MissionGetHeightStr(UIObject *o, void *data)
{
	static char s[128];
	UNUSED(o);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
		return NULL;
	sprintf(s, "Height: %d", CampaignGetCurrentMission(co)->Size.y);
	return s;
}
static const char *MissionGetWallCountStr(UIObject *o, void *data)
{
	static char s[128];
	UNUSED(o);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
		return NULL;
	sprintf(s, "Walls: %d", CampaignGetCurrentMission(co)->u.Classic.Walls);
	return s;
}
static const char *MissionGetWallLengthStr(UIObject *o, void *data)
{
	static char s[128];
	UNUSED(o);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
		return NULL;
	sprintf(s, "Len: %d", CampaignGetCurrentMission(co)->u.Classic.WallLength);
	return s;
}
static const char *MissionGetCorridorWidthStr(UIObject *o, void *data)
{
	static char s[128];
	UNUSED(o);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
		return NULL;
	sprintf(
		s, "CorridorWidth: %d",
		CampaignGetCurrentMission(co)->u.Classic.CorridorWidth);
	return s;
}
static const char *MissionGetRoomCountStr(UIObject *o, void *data)
{
	static char s[128];
	UNUSED(o);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
		return NULL;
	sprintf(
		s, "Rooms: %d", CampaignGetCurrentMission(co)->u.Classic.Rooms.Count);
	return s;
}
static const char *MissionGetRoomMinStr(UIObject *o, void *data)
{
	static char s[128];
	UNUSED(o);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
		return NULL;
	sprintf(
		s, "RoomMin: %d", CampaignGetCurrentMission(co)->u.Classic.Rooms.Min);
	return s;
}
static const char *MissionGetRoomMaxStr(UIObject *o, void *data)
{
	static char s[128];
	UNUSED(o);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
		return NULL;
	sprintf(
		s, "RoomMax: %d", CampaignGetCurrentMission(co)->u.Classic.Rooms.Max);
	return s;
}
static void MissionDrawEdgeRooms(
	UIObject *o, GraphicsDevice *g, struct vec2i pos, void *data)
{
	UNUSED(o);
	UNUSED(g);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
		return;
	DisplayFlag(
		svec2i_add(pos, o->Pos), "Edge rooms",
		CampaignGetCurrentMission(co)->u.Classic.Rooms.Edge,
		UIObjectIsHighlighted(o));
}
static void MissionDrawRoomsOverlap(
	UIObject *o, GraphicsDevice *g, struct vec2i pos, void *data)
{
	UNUSED(o);
	UNUSED(g);
	UNUSED(pos);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
		return;
	DisplayFlag(
		svec2i_add(pos, o->Pos), "Room overlap",
		CampaignGetCurrentMission(co)->u.Classic.Rooms.Overlap,
		UIObjectIsHighlighted(o));
}
static const char *MissionGetRoomWallCountStr(UIObject *o, void *data)
{
	static char s[128];
	UNUSED(o);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
		return NULL;
	sprintf(
		s, "RoomWalls: %d",
		CampaignGetCurrentMission(co)->u.Classic.Rooms.Walls);
	return s;
}
static const char *MissionGetRoomWallLenStr(UIObject *o, void *data)
{
	static char s[128];
	UNUSED(o);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
		return NULL;
	sprintf(
		s, "RoomWallLen: %d",
		CampaignGetCurrentMission(co)->u.Classic.Rooms.WallLength);
	return s;
}
static const char *MissionGetRoomWallPadStr(UIObject *o, void *data)
{
	static char s[128];
	UNUSED(o);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
		return NULL;
	sprintf(
		s, "RoomWallPad: %d",
		CampaignGetCurrentMission(co)->u.Classic.Rooms.WallPad);
	return s;
}
static const char *MissionGetSquareCountStr(UIObject *o, void *data)
{
	static char s[128];
	UNUSED(o);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
		return NULL;
	sprintf(s, "Sqr: %d", CampaignGetCurrentMission(co)->u.Classic.Squares);
	return s;
}
static void MissionDrawDoorEnabled(
	UIObject *o, GraphicsDevice *g, struct vec2i pos, void *data)
{
	UNUSED(o);
	UNUSED(g);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
		return;
	DisplayFlag(
		svec2i_add(pos, o->Pos), "Doors",
		CampaignGetCurrentMission(co)->u.Classic.Doors.Enabled,
		UIObjectIsHighlighted(o));
}
static const char *MissionGetDoorMinStr(UIObject *o, void *data)
{
	static char s[128];
	UNUSED(o);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
		return NULL;
	sprintf(
		s, "DoorMin: %d", CampaignGetCurrentMission(co)->u.Classic.Doors.Min);
	return s;
}
static const char *MissionGetDoorMaxStr(UIObject *o, void *data)
{
	static char s[128];
	UNUSED(o);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
		return NULL;
	sprintf(
		s, "DoorMax: %d", CampaignGetCurrentMission(co)->u.Classic.Doors.Max);
	return s;
}
static const char *MissionGetPillarCountStr(UIObject *o, void *data)
{
	static char s[128];
	UNUSED(o);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
		return NULL;
	sprintf(
		s, "Pillars: %d",
		CampaignGetCurrentMission(co)->u.Classic.Pillars.Count);
	return s;
}
static const char *MissionGetPillarMinStr(UIObject *o, void *data)
{
	static char s[128];
	UNUSED(o);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
		return NULL;
	sprintf(
		s, "PillarMin: %d",
		CampaignGetCurrentMission(co)->u.Classic.Pillars.Min);
	return s;
}
static const char *MissionGetPillarMaxStr(UIObject *o, void *data)
{
	static char s[128];
	UNUSED(o);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
		return NULL;
	sprintf(
		s, "PillarMax: %d",
		CampaignGetCurrentMission(co)->u.Classic.Pillars.Max);
	return s;
}
static const char *MissionGetDensityStr(UIObject *o, void *data)
{
	static char s[128];
	UNUSED(o);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
		return NULL;
	sprintf(s, "Dens: %d", CampaignGetCurrentMission(co)->EnemyDensity);
	return s;
}
static const char *MissionGetTypeStr(UIObject *o, void *data)
{
	static char s[128];
	UNUSED(o);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
		return NULL;
	sprintf(s, "Type: %s", MapTypeStr(CampaignGetCurrentMission(co)->Type));
	return s;
}
static void DrawTileStyle(
	UIObject *o, const struct vec2i pos, const TileClass *tc, const char *name,
	const char *styleType, const int idx, const int maxStyles)
{
	DrawStyleArea(
		svec2i_add(pos, o->Pos), name,
		&PicManagerGetMaskedStylePic(
			 &gPicManager, tc->Name, tc->Style, styleType, tc->Mask,
			 tc->MaskAlt)
			 ->pic,
		idx, maxStyles, UIObjectIsHighlighted(o));
}
static void MissionDrawWallStyle(
	UIObject *o, GraphicsDevice *g, struct vec2i pos, void *data)
{
	UNUSED(g);
	Campaign *co = data;
	const Mission *m = CampaignGetCurrentMission(co);
	if (!m)
		return;
	if (m->Type == MAPTYPE_STATIC)
		return;
	const TileClass *tc = m->Type == MAPTYPE_CLASSIC
							  ? &m->u.Classic.TileClasses.Wall
							  : &m->u.Cave.TileClasses.Wall;
	const int idx = PicManagerGetWallStyleIndex(&gPicManager, tc->Style);
	DrawTileStyle(
		o, pos, tc, "Wall", TileClassBaseStyleType(TILE_CLASS_WALL), idx,
		(int)gPicManager.wallStyleNames.size);
}
static void MissionDrawFloorStyle(
	UIObject *o, GraphicsDevice *g, struct vec2i pos, void *data)
{
	UNUSED(g);
	Campaign *co = data;
	const Mission *m = CampaignGetCurrentMission(co);
	if (!m)
		return;
	if (m->Type == MAPTYPE_STATIC)
		return;
	const TileClass *tc = m->Type == MAPTYPE_CLASSIC
							  ? &m->u.Classic.TileClasses.Floor
							  : &m->u.Cave.TileClasses.Floor;
	const int idx = PicManagerGetTileStyleIndex(&gPicManager, tc->Style);
	DrawTileStyle(
		o, pos, tc, "Floor", TileClassBaseStyleType(TILE_CLASS_FLOOR), idx,
		(int)gPicManager.tileStyleNames.size);
}
static void MissionDrawRoomStyle(
	UIObject *o, GraphicsDevice *g, struct vec2i pos, void *data)
{
	UNUSED(g);
	Campaign *co = data;
	const Mission *m = CampaignGetCurrentMission(co);
	if (!m)
		return;
	if (m->Type == MAPTYPE_STATIC)
		return;
	const TileClass *tc = m->Type == MAPTYPE_CLASSIC
							  ? &m->u.Classic.TileClasses.Room
							  : &m->u.Cave.TileClasses.Room;
	const int idx = PicManagerGetTileStyleIndex(&gPicManager, tc->Style);
	DrawTileStyle(
		o, pos, tc, "Rooms", TileClassBaseStyleType(TILE_CLASS_FLOOR), idx,
		(int)gPicManager.tileStyleNames.size);
}
static void MissionDrawDoorStyle(
	UIObject *o, GraphicsDevice *g, struct vec2i pos, void *data)
{
	UNUSED(g);
	Campaign *co = data;
	const Mission *m = CampaignGetCurrentMission(co);
	if (!m)
		return;
	if (m->Type == MAPTYPE_STATIC)
		return;
	const TileClass *tc = m->Type == MAPTYPE_CLASSIC
							  ? &m->u.Classic.TileClasses.Door
							  : &m->u.Cave.TileClasses.Door;
	const int idx = PicManagerGetDoorStyleIndex(&gPicManager, tc->Style);
	DrawTileStyle(
		o, pos, tc, "Doors", TileClassBaseStyleType(TILE_CLASS_DOOR), idx,
		(int)gPicManager.doorStyleNames.size);
}
static void MissionDrawKeyStyle(
	UIObject *o, GraphicsDevice *g, struct vec2i pos, void *data)
{
	UNUSED(g);
	Campaign *co = data;
	Mission *m = CampaignGetCurrentMission(co);
	if (m == NULL)
	{
		return;
	}
	const int idx = PicManagerGetKeyStyleIndex(&gPicManager, m->KeyStyle);
	const Pic *pic = CPicGetPic(&KeyPickupClass(m->KeyStyle, 0)->Pic, 0);
	DrawStyleArea(
		svec2i_add(pos, o->Pos), "Keys", pic, idx,
		(int)gPicManager.keyStyleNames.size, UIObjectIsHighlighted(o));
}
static void MissionDrawExitStyle(
	UIObject *o, GraphicsDevice *g, struct vec2i pos, void *data)
{
	UNUSED(g);
	Campaign *co = data;
	Mission *m = CampaignGetCurrentMission(co);
	if (m == NULL)
	{
		return;
	}
	const int idx = PicManagerGetExitStyleIndex(&gPicManager, m->ExitStyle);
	DrawStyleArea(
		svec2i_add(pos, o->Pos), "Exit",
		TileClassesGetExit(&gTileClasses, &gPicManager, m->ExitStyle, false)
			->Pic,
		idx, (int)gPicManager.exitStyleNames.size, UIObjectIsHighlighted(o));
}
static const char *MissionGetCharacterCountStr(UIObject *o, void *data)
{
	static char s[128];
	UNUSED(o);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
		return NULL;
	sprintf(
		s, "Characters (%d)",
		(int)CampaignGetCurrentMission(co)->Enemies.size);
	return s;
}
static const char *MissionGetSpecialCountStr(UIObject *o, void *data)
{
	static char s[128];
	UNUSED(o);
	Campaign *co = data;
	if (!CampaignGetCurrentMission(co))
		return NULL;
	sprintf(
		s, "Mission objective characters (%d)",
		(int)CampaignGetCurrentMission(co)->SpecialChars.size);
	return s;
}
static const char *GetWeaponCountStr(UIObject *o, void *v)
{
	static char s[128];
	UNUSED(o);
	UNUSED(v);
	Mission *m = CampaignGetCurrentMission(&gCampaign);
	if (!m)
	{
		return NULL;
	}
	int totalWeapons = 0;
	CA_FOREACH(const WeaponClass, wc, gWeaponClasses.Guns)
	if (wc->IsRealGun)
	{
		totalWeapons++;
	}
	CA_FOREACH_END()
	CA_FOREACH(const WeaponClass, wc, gWeaponClasses.CustomGuns)
	if (wc->IsRealGun)
	{
		totalWeapons++;
	}
	CA_FOREACH_END()
	sprintf(
		s, "Available weapons (%d/%d)",
		(int)gMission.missionData->Weapons.size, totalWeapons);
	return s;
}
static const char *GetObjectCountStr(UIObject *o, void *v)
{
	static char s[128];
	UNUSED(o);
	UNUSED(v);
	Mission *m = CampaignGetCurrentMission(&gCampaign);
	if (!m)
	{
		return NULL;
	}
	sprintf(
		s, "Map items (%d)",
		(int)gMission.missionData->MapObjectDensities.size);
	return s;
}
typedef struct
{
	Campaign *co;
	int index;
} MissionIndexData;
static void MissionDrawEnemy(
	UIObject *o, GraphicsDevice *g, struct vec2i pos, void *vData)
{
	UNUSED(g);
	MissionIndexData *data = vData;
	if (!CampaignGetCurrentMission(data->co))
		return;
	if (data->index >= (int)CampaignGetCurrentMission(data->co)->Enemies.size)
	{
		return;
	}
	const CharacterStore *store = &data->co->Setting.characters;
	const int charIndex = *(int *)CArrayGet(&store->baddieIds, data->index);
	DrawCharacterSimple(
		CArrayGet(&store->OtherChars, charIndex),
		svec2i_add(svec2i_add(pos, o->Pos), svec2i_scale_divide(o->Size, 2)),
		DIRECTION_DOWN, UIObjectIsHighlighted(o), true);
}
static void MissionDrawSpecialChar(
	UIObject *o, GraphicsDevice *g, struct vec2i pos, void *vData)
{
	UNUSED(g);
	MissionIndexData *data = vData;
	if (!CampaignGetCurrentMission(data->co))
		return;
	if (data->index >=
		(int)CampaignGetCurrentMission(data->co)->SpecialChars.size)
	{
		return;
	}
	const CharacterStore *store = &data->co->Setting.characters;
	const int charIndex = CharacterStoreGetSpecialId(store, data->index);
	DrawCharacterSimple(
		CArrayGet(&store->OtherChars, charIndex),
		svec2i_add(svec2i_add(pos, o->Pos), svec2i_scale_divide(o->Size, 2)),
		DIRECTION_DOWN, UIObjectIsHighlighted(o), true);
}
static void MissionDrawMapItem(
	UIObject *o, GraphicsDevice *g, struct vec2i pos, void *vData)
{
	UNUSED(g);
	MissionIndexData *data = vData;
	if (!CampaignGetCurrentMission(data->co))
		return;
	const Mission *m = CampaignGetCurrentMission(data->co);
	if (data->index >= (int)m->MapObjectDensities.size)
		return;
	const MapObjectDensity *mod =
		CArrayGet(&m->MapObjectDensities, data->index);
	DisplayMapItemWithDensity(
		svec2i_add(svec2i_add(pos, o->Pos), svec2i_scale_divide(o->Size, 2)),
		mod, UIObjectIsHighlighted(o));
}
static void DrawStyleArea(
	struct vec2i pos, const char *name, const Pic *pic, int idx, int count,
	int isHighlighted)
{
	FontStrMask(name, pos, isHighlighted ? colorRed : colorWhite);
	pos.y += FontH();
	PicRender(
		pic, gGraphicsDevice.gameWindow.renderer, pos, colorWhite, 0,
		svec2_one(), SDL_FLIP_NONE, Rect2iZero());
	// Display style index and count, right aligned
	char buf[16];
	sprintf(buf, "%d/%d", idx + 1, count);
	FontStrMask(
		buf, svec2i(pos.x + 28 - FontStrW(buf), pos.y + 17), colorGray);
}

static EditorResult CampaignChangeMission(void *data, int d)
{
	Campaign *co = data;
	co->MissionIndex =
		CLAMP(co->MissionIndex + d, 0, (int)co->Setting.Missions.size);
	return EDITOR_RESULT_RELOAD;
}
static EditorResult MissionInsertNew(void *data, int d)
{
	UNUSED(d);
	Campaign *co = data;
	InsertMission(co, NULL, co->MissionIndex);
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionDelete(void *data, int d)
{
	UNUSED(d);
	Campaign *co = data;
	if (co->Setting.Missions.size > 0 &&
		ConfirmScreen("", "Delete mission? (Y/N)"))
	{
		DeleteMission(co);
		return EDITOR_RESULT_CHANGED;
	}
	return EDITOR_RESULT_NONE;
}
static EditorResult MissionChangeWidth(void *data, int d)
{
	Campaign *co = data;
	Mission *m = CampaignGetCurrentMission(co);
	int old = m->Size.x;
	if (gEventHandlers.keyboard.modState & KMOD_SHIFT)
	{
		d *= 10;
	}
	m->Size.x = CLAMP(m->Size.x + d, 16, 256);
	if (m->Type == MAPTYPE_STATIC)
	{
		MissionStaticLayout(&m->u.Static, m->Size, svec2i(old, m->Size.y));
	}
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangeHeight(void *data, int d)
{
	Campaign *co = data;
	Mission *m = CampaignGetCurrentMission(co);
	int old = m->Size.y;
	if (gEventHandlers.keyboard.modState & KMOD_SHIFT)
	{
		d *= 10;
	}
	m->Size.y = CLAMP(m->Size.y + d, 16, 256);
	if (m->Type == MAPTYPE_STATIC)
	{
		MissionStaticLayout(&m->u.Static, m->Size, svec2i(m->Size.x, old));
	}
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangeWallCount(void *data, int d)
{
	Campaign *co = data;
	CampaignGetCurrentMission(co)->u.Classic.Walls =
		CLAMP(CampaignGetCurrentMission(co)->u.Classic.Walls + d, 0, 200);
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangeWallLength(void *data, int d)
{
	Campaign *co = data;
	CampaignGetCurrentMission(co)->u.Classic.WallLength =
		CLAMP(CampaignGetCurrentMission(co)->u.Classic.WallLength + d, 1, 100);
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangeCorridorWidth(void *data, int d)
{
	Campaign *co = data;
	CampaignGetCurrentMission(co)->u.Classic.CorridorWidth = CLAMP(
		CampaignGetCurrentMission(co)->u.Classic.CorridorWidth + d, 1, 5);
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangeRoomCount(void *data, int d)
{
	Campaign *co = data;
	CampaignGetCurrentMission(co)->u.Classic.Rooms.Count = CLAMP(
		CampaignGetCurrentMission(co)->u.Classic.Rooms.Count + d, 0, 100);
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangeRoomMin(void *data, int d)
{
	Campaign *co = data;
	CampaignGetCurrentMission(co)->u.Classic.Rooms.Min =
		CLAMP(CampaignGetCurrentMission(co)->u.Classic.Rooms.Min + d, 5, 50);
	CampaignGetCurrentMission(co)->u.Classic.Rooms.Max =
		MAX(CampaignGetCurrentMission(co)->u.Classic.Rooms.Min,
			CampaignGetCurrentMission(co)->u.Classic.Rooms.Max);
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangeRoomMax(void *data, int d)
{
	Campaign *co = data;
	CampaignGetCurrentMission(co)->u.Classic.Rooms.Max =
		CLAMP(CampaignGetCurrentMission(co)->u.Classic.Rooms.Max + d, 5, 50);
	CampaignGetCurrentMission(co)->u.Classic.Rooms.Min =
		MIN(CampaignGetCurrentMission(co)->u.Classic.Rooms.Min,
			CampaignGetCurrentMission(co)->u.Classic.Rooms.Max);
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangeEdgeRooms(void *data, int d)
{
	UNUSED(d);
	Campaign *co = data;
	CampaignGetCurrentMission(co)->u.Classic.Rooms.Edge =
		!CampaignGetCurrentMission(co)->u.Classic.Rooms.Edge;
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangeRoomsOverlap(void *data, int d)
{
	UNUSED(d);
	Campaign *co = data;
	CampaignGetCurrentMission(co)->u.Classic.Rooms.Overlap =
		!CampaignGetCurrentMission(co)->u.Classic.Rooms.Overlap;
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangeRoomWallCount(void *data, int d)
{
	Campaign *co = data;
	CampaignGetCurrentMission(co)->u.Classic.Rooms.Walls =
		CLAMP(CampaignGetCurrentMission(co)->u.Classic.Rooms.Walls + d, 0, 50);
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangeRoomWallLen(void *data, int d)
{
	Campaign *co = data;
	CampaignGetCurrentMission(co)->u.Classic.Rooms.WallLength = CLAMP(
		CampaignGetCurrentMission(co)->u.Classic.Rooms.WallLength + d, 1, 50);
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangeRoomWallPad(void *data, int d)
{
	Campaign *co = data;
	CampaignGetCurrentMission(co)->u.Classic.Rooms.WallPad = CLAMP(
		CampaignGetCurrentMission(co)->u.Classic.Rooms.WallPad + d, 1, 10);
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangeSquareCount(void *data, int d)
{
	Campaign *co = data;
	CampaignGetCurrentMission(co)->u.Classic.Squares =
		CLAMP(CampaignGetCurrentMission(co)->u.Classic.Squares + d, 0, 100);
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangeDoorEnabled(void *data, int d)
{
	UNUSED(d);
	Campaign *co = data;
	CampaignGetCurrentMission(co)->u.Classic.Doors.Enabled =
		!CampaignGetCurrentMission(co)->u.Classic.Doors.Enabled;
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangeDoorMin(void *data, int d)
{
	Campaign *co = data;
	CampaignGetCurrentMission(co)->u.Classic.Doors.Min =
		CLAMP(CampaignGetCurrentMission(co)->u.Classic.Doors.Min + d, 1, 6);
	CampaignGetCurrentMission(co)->u.Classic.Doors.Max =
		MAX(CampaignGetCurrentMission(co)->u.Classic.Doors.Min,
			CampaignGetCurrentMission(co)->u.Classic.Doors.Max);
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangeDoorMax(void *data, int d)
{
	Campaign *co = data;
	CampaignGetCurrentMission(co)->u.Classic.Doors.Max =
		CLAMP(CampaignGetCurrentMission(co)->u.Classic.Doors.Max + d, 1, 6);
	CampaignGetCurrentMission(co)->u.Classic.Doors.Min =
		MIN(CampaignGetCurrentMission(co)->u.Classic.Doors.Min,
			CampaignGetCurrentMission(co)->u.Classic.Doors.Max);
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangePillarCount(void *data, int d)
{
	Campaign *co = data;
	CampaignGetCurrentMission(co)->u.Classic.Pillars.Count = CLAMP(
		CampaignGetCurrentMission(co)->u.Classic.Pillars.Count + d, 0, 50);
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangePillarMin(void *data, int d)
{
	Campaign *co = data;
	CampaignGetCurrentMission(co)->u.Classic.Pillars.Min =
		CLAMP(CampaignGetCurrentMission(co)->u.Classic.Pillars.Min + d, 1, 50);
	CampaignGetCurrentMission(co)->u.Classic.Pillars.Max =
		MAX(CampaignGetCurrentMission(co)->u.Classic.Pillars.Min,
			CampaignGetCurrentMission(co)->u.Classic.Pillars.Max);
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangePillarMax(void *data, int d)
{
	Campaign *co = data;
	CampaignGetCurrentMission(co)->u.Classic.Pillars.Max =
		CLAMP(CampaignGetCurrentMission(co)->u.Classic.Pillars.Max + d, 1, 50);
	CampaignGetCurrentMission(co)->u.Classic.Pillars.Min =
		MIN(CampaignGetCurrentMission(co)->u.Classic.Pillars.Min,
			CampaignGetCurrentMission(co)->u.Classic.Pillars.Max);
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangeDensity(void *data, int d)
{
	Campaign *co = data;
	CampaignGetCurrentMission(co)->EnemyDensity =
		CLAMP(CampaignGetCurrentMission(co)->EnemyDensity + d, 0, 100);
	return EDITOR_RESULT_CHANGED;
}
typedef struct
{
	Campaign *C;
	MapType Type;
} MissionChangeTypeData;
static EditorResult MissionChangeType(void *data, int d)
{
	UNUSED(d);
	MissionChangeTypeData *mct = data;
	if (mct->Type == gMission.missionData->Type)
	{
		return EDITOR_RESULT_NONE;
	}
	Map map;
	memset(&map, 0, sizeof map);
	MissionOptionsTerminate(&gMission);
	CampaignAndMissionSetup(mct->C, &gMission);
	MapBuild(&map, gMission.missionData, mct->C, gMission.index);
	MissionConvertToType(gMission.missionData, &map, mct->Type);
	MapTerminate(&map);
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangeWallStyle(void *data, int d)
{
	Campaign *co = data;
	Mission *m = CampaignGetCurrentMission(co);
	TileClass *tc = m->Type == MAPTYPE_CLASSIC ? &m->u.Classic.TileClasses.Wall
											   : &m->u.Cave.TileClasses.Wall;
	const int idx = CLAMP_OPPOSITE(
		PicManagerGetWallStyleIndex(&gPicManager, tc->Style) + d, 0,
		(int)gPicManager.wallStyleNames.size - 1);
	CFREE(tc->Style);
	CSTRDUP(tc->Style, *(char **)CArrayGet(&gPicManager.wallStyleNames, idx));
	tc->Pic = TileClassGetPic(&gPicManager, tc);
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangeFloorStyle(void *data, int d)
{
	Campaign *co = data;
	Mission *m = CampaignGetCurrentMission(co);
	TileClass *tc = m->Type == MAPTYPE_CLASSIC
						? &m->u.Classic.TileClasses.Floor
						: &m->u.Cave.TileClasses.Floor;
	const int idx = CLAMP_OPPOSITE(
		PicManagerGetTileStyleIndex(&gPicManager, tc->Style) + d, 0,
		(int)gPicManager.tileStyleNames.size - 1);
	CFREE(tc->Style);
	CSTRDUP(tc->Style, *(char **)CArrayGet(&gPicManager.tileStyleNames, idx));
	tc->Pic = TileClassGetPic(&gPicManager, tc);
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangeRoomStyle(void *data, int d)
{
	Campaign *co = data;
	Mission *m = CampaignGetCurrentMission(co);
	TileClass *tc = m->Type == MAPTYPE_CLASSIC ? &m->u.Classic.TileClasses.Room
											   : &m->u.Cave.TileClasses.Room;
	const int idx = CLAMP_OPPOSITE(
		PicManagerGetTileStyleIndex(&gPicManager, tc->Style) + d, 0,
		(int)gPicManager.tileStyleNames.size - 1);
	CFREE(tc->Style);
	CSTRDUP(tc->Style, *(char **)CArrayGet(&gPicManager.tileStyleNames, idx));
	tc->Pic = TileClassGetPic(&gPicManager, tc);
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangeDoorStyle(void *data, int d)
{
	Campaign *co = data;
	Mission *m = CampaignGetCurrentMission(co);
	TileClass *tc = m->Type == MAPTYPE_CLASSIC ? &m->u.Classic.TileClasses.Door
											   : &m->u.Cave.TileClasses.Door;
	const int idx = CLAMP_OPPOSITE(
		PicManagerGetDoorStyleIndex(&gPicManager, tc->Style) + d, 0,
		(int)gPicManager.doorStyleNames.size - 1);
	CFREE(tc->Style);
	CSTRDUP(tc->Style, *(char **)CArrayGet(&gPicManager.doorStyleNames, idx));
	tc->Pic = TileClassGetPic(&gPicManager, tc);
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangeKeyStyle(void *data, int d)
{
	Campaign *co = data;
	Mission *m = CampaignGetCurrentMission(co);
	const int idx = CLAMP_OPPOSITE(
		PicManagerGetKeyStyleIndex(&gPicManager, m->KeyStyle) + d, 0,
		(int)gPicManager.keyStyleNames.size - 1);
	strcpy(m->KeyStyle, *(char **)CArrayGet(&gPicManager.keyStyleNames, idx));
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangeExitStyle(void *data, int d)
{
	Campaign *co = data;
	Mission *m = CampaignGetCurrentMission(co);
	const int idx = CLAMP_OPPOSITE(
		PicManagerGetExitStyleIndex(&gPicManager, m->ExitStyle) + d, 0,
		(int)gPicManager.exitStyleNames.size - 1);
	strcpy(
		m->ExitStyle, *(char **)CArrayGet(&gPicManager.exitStyleNames, idx));
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangeEnemy(void *vData, int d)
{
	MissionIndexData *data = vData;
	CArray *enemies = &CampaignGetCurrentMission(data->co)->Enemies;
	if (data->index >= (int)enemies->size)
	{
		return EDITOR_RESULT_NONE;
	}
	int enemy = *(int *)CArrayGet(enemies, data->index);
	enemy = CLAMP_OPPOSITE(
		enemy + d, 0, (int)data->co->Setting.characters.OtherChars.size - 1);
	*(int *)CArrayGet(enemies, data->index) = enemy;
	*(int *)CArrayGet(&data->co->Setting.characters.baddieIds, data->index) =
		enemy;
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionChangeSpecialChar(void *vData, int d)
{
	MissionIndexData *data = vData;
	int c = *(int *)CArrayGet(
		&CampaignGetCurrentMission(data->co)->SpecialChars, data->index);
	c = CLAMP_OPPOSITE(
		c + d, 0, (int)data->co->Setting.characters.OtherChars.size - 1);
	*(int *)CArrayGet(
		&CampaignGetCurrentMission(data->co)->SpecialChars, data->index) = c;
	*(int *)CArrayGet(&data->co->Setting.characters.specialIds, data->index) =
		c;
	return EDITOR_RESULT_CHANGED;
}
static void DeactivateBrush(UIObject *o, void *data)
{
	UNUSED(o);
	EditorBrush *b = data;
	b->IsActive = false;
}

static UIObject *CreateMissionObjs(Campaign *co);
static UIObject *CreateClassicMapObjs(struct vec2i pos, Campaign *co);
static UIObject *CreateMapItemObjs(Campaign *co, int dy);
static UIObject *CreateCharacterObjs(Campaign *co, int dy);
static UIObject *CreateSpecialCharacterObjs(Campaign *co, int dy);

typedef struct
{
	bool IsCollapsed;
	struct vec2i Size;
	Pic *collapsePic;
	Pic *expandPic;
	UIObject *collapseButton;
	UIObject *child;
	UIObject *background;
} CollapsedData;
static EditorResult ToggleCollapse(void *data, int d);
static void DrawBackground(
	UIObject *o, GraphicsDevice *g, struct vec2i pos, void *data);
static UIObject *CreateEditorObjs(Campaign *co, EditorBrush *brush);
UIObject *CreateMainObjs(Campaign *co, EditorBrush *brush, struct vec2i size)
{
	UIObject *cc =
		UIObjectCreate(UITYPE_NONE, 0, svec2i_zero(), svec2i_zero());
	CollapsedData *cData;
	CMALLOC(cData, sizeof(CollapsedData));
	cData->IsCollapsed = false;
	cData->Size = size;
	cc->Data = cData;
	cc->IsDynamicData = true;

	UIObject *o;

	// Collapse button
	cData->collapsePic = PicManagerGetPic(&gPicManager, "editor/collapse");
	cData->expandPic = PicManagerGetPic(&gPicManager, "editor/expand");
	o = UIObjectCreate(
		UITYPE_BUTTON, 0, svec2i_subtract(size, cData->collapsePic->size),
		svec2i_zero());
	UIButtonSetPic(o, cData->collapsePic);
	o->DoNotHighlight = true;
	o->ChangeFunc = ToggleCollapse;
	o->Data = cData;
	CSTRDUP(o->Tooltip, "Collapse/expand");
	UIObjectAddChild(cc, o);
	cData->collapseButton = o;

	// The rest of the UI
	o = CreateEditorObjs(co, brush);
	UIObjectAddChild(cc, o);
	cData->child = o;

	// Background
	o = UIObjectCreate(UITYPE_CUSTOM, 0, svec2i_zero(), size);
	o->IsBackground = true;
	o->u.CustomDrawFunc = DrawBackground;
	o->OnFocusFunc = DeactivateBrush;
	o->Data = brush;
	UIObjectAddChild(cc, o);
	cData->background = o;

	return cc;
}
static EditorResult ToggleCollapse(void *data, int d)
{
	UNUSED(d);
	CollapsedData *cData = data;
	cData->IsCollapsed = !cData->IsCollapsed;
	if (cData->IsCollapsed)
	{
		// Change button pic, move button
		cData->collapseButton->u.Button.Pic = cData->expandPic;
		cData->collapseButton->Pos = svec2i_zero();
	}
	else
	{
		// Change button pic, move button
		cData->collapseButton->u.Button.Pic = cData->collapsePic;
		cData->collapseButton->Pos =
			svec2i_subtract(cData->Size, cData->collapsePic->size);
	}
	cData->child->IsVisible = !cData->IsCollapsed;
	cData->background->IsVisible = !cData->IsCollapsed;
	return EDITOR_RESULT_NONE;
}
static void DrawBackground(
	UIObject *o, GraphicsDevice *g, struct vec2i pos, void *data)
{
	UNUSED(data);
	const color_t c = {32, 32, 64, 196};
	DrawRectangle(g, pos, o->Size, c, true);
}
static UIObject *CreateEditorObjs(Campaign *co, EditorBrush *brush)
{
	const int th = FontH();
	UIObject *c;
	UIObject *o;
	UIObject *o2;
	UIObject *oc;
	struct vec2i pos = svec2i_zero();
	UIObject *cc =
		UIObjectCreate(UITYPE_NONE, 0, svec2i_zero(), svec2i_zero());

	// Titles

	pos.y = 5;

	o = UIObjectCreate(
		UITYPE_LABEL, YC_CAMPAIGNTITLE, svec2i(25, pos.y), svec2i(140, th));
	o->u.LabelFunc = CampaignGetTitle;
	CSTRDUP(o->Tooltip, "Click to edit campaign options");
	o->ChangeFunc = CampaignEdit;
	o->Data = co;
	UIObjectAddChild(cc, o);

	// Mission insert/delete/index, current mission
	// Layout from right to left
	o = UIObjectCreate(
		UITYPE_LABEL, 0, svec2i(270, pos.y),
		svec2i(FontStrW("Mission 99/99"), th));
	o->u.LabelFunc = CampaignGetMissionIndexStr;
	o->ChangeFunc = CampaignChangeMission;
	o->Data = co;
	UIObjectAddChild(cc, o);
	o = UIObjectCreate(
		UITYPE_LABEL, 0, svec2i(o->Pos.x, pos.y), svec2i(0, th));
	o->Label = "Insert mission";
	o->Size.x = FontStrW(o->Label);
	o->Pos.x -= o->Size.x + 10;
	o->ChangeFunc = MissionInsertNew;
	o->Data = co;
	UIObjectAddChild(cc, o);
	o = UIObjectCreate(
		UITYPE_LABEL, 0, svec2i(o->Pos.x, pos.y), svec2i(0, th));
	o->Label = "Delete mission";
	o->Size.x = FontStrW(o->Label);
	o->Pos.x -= o->Size.x + 10;
	o->ChangeFunc = MissionDelete;
	o->Data = co;
	UIObjectAddChild(cc, o);

	pos.y = 2 * th;

	// Mission-only controls
	// Only visible if the current mission is valid
	c = UIObjectCreate(UITYPE_NONE, 0, svec2i_zero(), svec2i_zero());
	c->CheckVisible = CheckMission;
	c->Data = co;
	UIObjectAddChild(cc, c);

	o = UIObjectCreate(
		UITYPE_TEXTBOX, YC_MISSIONTITLE, svec2i(25, pos.y), svec2i(175, th));
	o->Id2 = XC_MISSIONTITLE;
	o->u.Textbox.TextLinkFunc = MissionGetTitle;
	o->u.Textbox.TextSourceFunc = MissionGetTitleSrc;
	o->Data = co;
	CSTRDUP(o->u.Textbox.Hint, "(Mission title)");
	o->CheckVisible = MissionCheckVisible;
	UIObjectAddChild(o, CreateMissionObjs(co));
	UIObjectAddChild(c, o);

	// mission properties
	// size, walls, rooms etc.

	pos.y = 10 + 2 * th;

	o = UIObjectCreate(UITYPE_LABEL, 0, svec2i_zero(), svec2i(40, th));

	pos.x = 20;
	o2 = UIObjectCopy(o);
	o2->u.LabelFunc = MissionGetWidthStr;
	o2->Data = co;
	o2->ChangeFunc = MissionChangeWidth;
	o2->Pos = pos;
	UIObjectAddChild(c, o2);
	pos.x += 40;
	o2 = UIObjectCopy(o);
	o2->u.LabelFunc = MissionGetHeightStr;
	o2->Data = co;
	o2->ChangeFunc = MissionChangeHeight;
	o2->Pos = pos;
	UIObjectAddChild(c, o2);
	pos.x += 40;
	o2 = UIObjectCopy(o);
	o2->u.LabelFunc = MissionGetDensityStr;
	o2->Data = co;
	o2->ChangeFunc = MissionChangeDensity;
	o2->Pos = pos;
	CSTRDUP(o2->Tooltip, "Number of non-objective characters");
	UIObjectAddChild(c, o2);

	// Drop-down menu for map type
	pos.x += 40;
	o2 = UIObjectCopy(o);
	o2->Size.x = 50;
	o2->u.LabelFunc = MissionGetTypeStr;
	o2->Data = co;
	o2->Pos = pos;
	CSTRDUP(
		o2->Tooltip,
		"WARNING: changing map type will\nlose your previous map settings");
	UIObject *oMapType =
		UIObjectCreate(UITYPE_CONTEXT_MENU, 0, svec2i_zero(), svec2i_zero());
	for (int i = 0; i < (int)MAPTYPE_COUNT; i++)
	{
		UIObject *oMapTypeChild = UIObjectCopy(o);
		oMapTypeChild->Pos.y = i * th;
		UIObjectSetDynamicLabel(oMapTypeChild, MapTypeStr((MapType)i));
		oMapTypeChild->IsDynamicData = true;
		CMALLOC(oMapTypeChild->Data, sizeof(MissionChangeTypeData));
		((MissionChangeTypeData *)oMapTypeChild->Data)->C = co;
		((MissionChangeTypeData *)oMapTypeChild->Data)->Type = (MapType)i;
		oMapTypeChild->ChangeFunc = MissionChangeType;
		UIObjectAddChild(oMapType, oMapTypeChild);
	}
	UIObjectAddChild(o2, oMapType);
	// HACK: add another container UI object so that the BFS draw order doesn't
	// draw the context menu below the type-specific objs
	UIObject *hackContainer =
		UIObjectCreate(UITYPE_NONE, 0, svec2i_zero(), svec2i_zero());
	UIObjectAddChild(hackContainer, o2);
	UIObjectAddChild(c, hackContainer);
	pos.x = 20;
	pos.y += th;
	UIObjectAddChild(c, CreateClassicMapObjs(pos, co));
	UIObjectAddChild(c, CreateStaticMapObjs(pos, co, brush));
	UIObjectAddChild(c, CreateCaveMapObjs(pos, co));

	// Mission looks
	// wall/floor styles etc.

	pos.y += th * 6 + 2;

	UIObjectDestroy(o);
	o = UIObjectCreate(
		UITYPE_CUSTOM, YC_MISSIONLOOKS, svec2i_zero(), svec2i(25, 25 + th));

	pos.x = 20;
	o2 = UIObjectCopy(o);
	o2->Id2 = XC_WALL;
	o2->u.CustomDrawFunc = MissionDrawWallStyle;
	o2->Data = co;
	o2->ChangeFunc = MissionChangeWallStyle;
	o2->Pos = pos;
	UIObjectAddChild(c, o2);
	pos.x += 30;
	o2 = UIObjectCopy(o);
	o2->Id2 = XC_FLOOR;
	o2->u.CustomDrawFunc = MissionDrawFloorStyle;
	o2->Data = co;
	o2->ChangeFunc = MissionChangeFloorStyle;
	o2->Pos = pos;
	o2->CheckVisible = MissionCheckVisible;
	UIObjectAddChild(c, o2);
	pos.x += 30;
	o2 = UIObjectCopy(o);
	o2->Id2 = XC_ROOM;
	o2->u.CustomDrawFunc = MissionDrawRoomStyle;
	o2->Data = co;
	o2->ChangeFunc = MissionChangeRoomStyle;
	o2->Pos = pos;
	o2->CheckVisible = MissionCheckVisible;
	UIObjectAddChild(c, o2);
	pos.x += 30;
	o2 = UIObjectCopy(o);
	o2->Id2 = XC_DOORS;
	o2->u.CustomDrawFunc = MissionDrawDoorStyle;
	o2->Data = co;
	o2->ChangeFunc = MissionChangeDoorStyle;
	o2->Pos = pos;
	o2->CheckVisible = MissionCheckVisible;
	UIObjectAddChild(c, o2);
	pos.x += 30;
	o2 = UIObjectCopy(o);
	o2->Id2 = XC_KEYS;
	o2->u.CustomDrawFunc = MissionDrawKeyStyle;
	o2->Data = co;
	o2->ChangeFunc = MissionChangeKeyStyle;
	o2->Pos = pos;
	o2->CheckVisible = MissionCheckVisible;
	UIObjectAddChild(c, o2);
	pos.x += 30;
	o2 = UIObjectCopy(o);
	o2->Id2 = XC_EXIT;
	o2->u.CustomDrawFunc = MissionDrawExitStyle;
	o2->Data = co;
	o2->ChangeFunc = MissionChangeExitStyle;
	o2->Pos = pos;
	o2->CheckVisible = MissionCheckVisible;
	UIObjectAddChild(c, o2);

	// colours

	pos.x = 200;
	pos = CreateColorObjs(co, c, pos);

	// mission data

	pos.x = 20;
	pos.y += th + 5;

	UIObjectDestroy(o);
	o = UIObjectCreate(UITYPE_LABEL, 0, svec2i_zero(), svec2i(189, th));
	o->Flags = UI_SELECT_ONLY;

	o2 = UIObjectCopy(o);
	o2->Label = "Mission description";
	o2->Id = YC_MISSIONDESC;
	o2->Pos = pos;
	o2->Size = FontStrSize(o2->Label);
	oc = UIObjectCreate(
		UITYPE_TEXTBOX, YC_MISSIONDESC, svec2i(0, Y_ABS - pos.y),
		svec2i(295, 5 * th));
	oc->Flags = UI_ENABLED_WHEN_PARENT_HIGHLIGHTED_ONLY;
	oc->u.Textbox.TextLinkFunc = MissionGetDescription;
	oc->u.Textbox.TextSourceFunc = MissionGetDescriptionSrc;
	oc->Data = co;
	CSTRDUP(oc->u.Textbox.Hint, "(Mission description)");
	oc->CheckVisible = MissionCheckVisible;
	UIObjectAddChild(o2, oc);
	UIObjectAddChild(c, o2);

	pos.y += th;
	o2 = UIObjectCopy(o);
	o2->u.LabelFunc = MissionGetCharacterCountStr;
	o2->Data = co;
	o2->Id = YC_CHARACTERS;
	o2->Pos = pos;
	CSTRDUP(
		o2->Tooltip, "Use Insert/CTRL+i, Delete/CTRL+d and PageUp/PageDown");
	UIObjectAddChild(o2, CreateCharacterObjs(co, pos.y));
	UIObjectAddChild(c, o2);
	pos.y += th;
	o2 = UIObjectCopy(o);
	o2->u.LabelFunc = MissionGetSpecialCountStr;
	o2->Data = co;
	o2->Id = YC_SPECIALS;
	o2->Pos = pos;
	CSTRDUP(
		o2->Tooltip, "Use Insert/CTRL+i, Delete/CTRL+d and PageUp/PageDown");
	UIObjectAddChild(o2, CreateSpecialCharacterObjs(co, pos.y));
	UIObjectAddChild(c, o2);
	pos.y += th;
	o2 = UIObjectCopy(o);
	o2->u.LabelFunc = GetWeaponCountStr;
	o2->Data = NULL;
	o2->Pos = pos;
	UIObjectAddChild(o2, CreateWeaponObjs(co));
	UIObjectAddChild(c, o2);
	pos.y += th;
	o2 = UIObjectCopy(o);
	o2->u.LabelFunc = GetObjectCountStr;
	o2->Data = NULL;
	o2->Id = YC_ITEMS;
	o2->Pos = pos;
	CSTRDUP(
		o2->Tooltip, "Use Insert/CTRL+i, Delete/CTRL+d and PageUp/PageDown\n"
					 "Shift+click to change amounts");
	UIObjectAddChild(o2, CreateMapItemObjs(co, pos.y));
	UIObjectAddChild(c, o2);

	UIObjectDestroy(o);

	// objectives
	pos.y += 2 + th;
	CreateObjectivesObjs(co, c, pos);

	return cc;
}
static UIObject *CreateMissionObjs(Campaign *co)
{
	const int th = FontH();
	UIObject *c;
	UIObject *o;
	c = UIObjectCreate(UITYPE_NONE, 0, svec2i_zero(), svec2i_zero());
	c->Flags = UI_ENABLED_WHEN_PARENT_HIGHLIGHTED_ONLY;

	o = UIObjectCreate(
		UITYPE_TEXTBOX, YC_MISSIONTITLE, svec2i(0, Y_ABS), svec2i(319, th));
	o->u.Textbox.TextLinkFunc = MissionGetSong;
	o->u.Textbox.MaxLen = sizeof((Mission *)0)->Song - 1;
	o->Data = co;
	CSTRDUP(o->u.Textbox.Hint, "(Mission song)");
	o->Id2 = XC_MUSICFILE;
	o->Flags = UI_SELECT_ONLY;
	o->CheckVisible = MissionCheckVisible;
	UIObjectAddChild(c, o);

	return c;
}
static UIObject *CreateClassicMapObjs(struct vec2i pos, Campaign *co)
{
	const int th = FontH();
	UIObject *c = UIObjectCreate(UITYPE_NONE, 0, svec2i_zero(), svec2i_zero());
	UIObject *o =
		UIObjectCreate(UITYPE_LABEL, 0, svec2i_zero(), svec2i(50, th));
	const int x = pos.x;
	// Check whether the map type matches, and set visibility
	c->CheckVisible = MissionCheckTypeFunc;
	c->Data = co;

	UIObject *o2 = CreateCampaignSeedObj(pos, co);
	UIObjectAddChild(c, o2);

	pos.x += o2->Size.x;
	o2 = UIObjectCopy(o);
	o2->u.LabelFunc = MissionGetWallCountStr;
	o2->Data = co;
	o2->ChangeFunc = MissionChangeWallCount;
	o2->Pos = pos;
	UIObjectAddChild(c, o2);
	pos.x += o2->Size.x;
	o2 = UIObjectCopy(o);
	o2->u.LabelFunc = MissionGetWallLengthStr;
	o2->Data = co;
	o2->ChangeFunc = MissionChangeWallLength;
	o2->Pos = pos;
	UIObjectAddChild(c, o2);
	pos.x += o2->Size.x;
	o2 = UIObjectCopy(o);
	o2->u.LabelFunc = MissionGetCorridorWidthStr;
	o2->Data = co;
	o2->ChangeFunc = MissionChangeCorridorWidth;
	o2->Pos = pos;
	o2->Size.x = 60;
	UIObjectAddChild(c, o2);

	pos.x = x;
	pos.y += th;
	o2 = UIObjectCopy(o);
	o2->u.LabelFunc = MissionGetRoomCountStr;
	o2->Data = co;
	o2->ChangeFunc = MissionChangeRoomCount;
	o2->Pos = pos;
	UIObjectAddChild(c, o2);
	pos.x += o2->Size.x;
	o2 = UIObjectCopy(o);
	o2->u.LabelFunc = MissionGetRoomMinStr;
	o2->Data = co;
	o2->ChangeFunc = MissionChangeRoomMin;
	o2->Pos = pos;
	UIObjectAddChild(c, o2);
	pos.x += o2->Size.x;
	o2 = UIObjectCopy(o);
	o2->u.LabelFunc = MissionGetRoomMaxStr;
	o2->Data = co;
	o2->ChangeFunc = MissionChangeRoomMax;
	o2->Pos = pos;
	UIObjectAddChild(c, o2);
	pos.x += o2->Size.x;
	o2 = UIObjectCreate(UITYPE_CUSTOM, 0, pos, svec2i(60, th));
	o2->u.CustomDrawFunc = MissionDrawEdgeRooms;
	o2->Data = co;
	o2->ChangeFunc = MissionChangeEdgeRooms;
	UIObjectAddChild(c, o2);
	pos.x += o2->Size.x;
	o2 = UIObjectCreate(UITYPE_CUSTOM, 0, pos, svec2i(60, th));
	o2->u.CustomDrawFunc = MissionDrawRoomsOverlap;
	o2->Data = co;
	o2->ChangeFunc = MissionChangeRoomsOverlap;
	UIObjectAddChild(c, o2);

	pos.x = x;
	pos.y += th;
	o2 = UIObjectCopy(o);
	o2->u.LabelFunc = MissionGetRoomWallCountStr;
	o2->Data = co;
	o2->ChangeFunc = MissionChangeRoomWallCount;
	o2->Pos = pos;
	UIObjectAddChild(c, o2);
	pos.x += o2->Size.x;
	o2 = UIObjectCopy(o);
	o2->u.LabelFunc = MissionGetRoomWallLenStr;
	o2->Data = co;
	o2->ChangeFunc = MissionChangeRoomWallLen;
	o2->Pos = pos;
	o2->Size.x = 60;
	UIObjectAddChild(c, o2);
	pos.x += o2->Size.x;
	o2 = UIObjectCopy(o);
	o2->u.LabelFunc = MissionGetRoomWallPadStr;
	o2->Data = co;
	o2->ChangeFunc = MissionChangeRoomWallPad;
	o2->Pos = pos;
	o2->Size.x = 60;
	UIObjectAddChild(c, o2);

	pos.x = x;
	pos.y += th;
	o2 = UIObjectCopy(o);
	o2->u.LabelFunc = MissionGetSquareCountStr;
	o2->Data = co;
	o2->ChangeFunc = MissionChangeSquareCount;
	o2->Pos = pos;
	UIObjectAddChild(c, o2);

	pos.x = x;
	pos.y += th;
	o2 = UIObjectCreate(UITYPE_CUSTOM, 0, pos, o->Size);
	o2->u.CustomDrawFunc = MissionDrawDoorEnabled;
	o2->Data = co;
	o2->ChangeFunc = MissionChangeDoorEnabled;
	o2->Pos = pos;
	UIObjectAddChild(c, o2);
	pos.x += o2->Size.x;
	o2 = UIObjectCopy(o);
	o2->u.LabelFunc = MissionGetDoorMinStr;
	o2->Data = co;
	o2->ChangeFunc = MissionChangeDoorMin;
	o2->Pos = pos;
	UIObjectAddChild(c, o2);
	pos.x += o2->Size.x;
	o2 = UIObjectCopy(o);
	o2->u.LabelFunc = MissionGetDoorMaxStr;
	o2->Data = co;
	o2->ChangeFunc = MissionChangeDoorMax;
	o2->Pos = pos;
	UIObjectAddChild(c, o2);

	pos.x = x;
	pos.y += th;
	o2 = UIObjectCopy(o);
	o2->u.LabelFunc = MissionGetPillarCountStr;
	o2->Data = co;
	o2->ChangeFunc = MissionChangePillarCount;
	o2->Pos = pos;
	UIObjectAddChild(c, o2);
	pos.x += o2->Size.x;
	o2 = UIObjectCopy(o);
	o2->u.LabelFunc = MissionGetPillarMinStr;
	o2->Data = co;
	o2->ChangeFunc = MissionChangePillarMin;
	o2->Pos = pos;
	UIObjectAddChild(c, o2);
	pos.x += o2->Size.x;
	o2 = UIObjectCopy(o);
	o2->u.LabelFunc = MissionGetPillarMaxStr;
	o2->Data = co;
	o2->ChangeFunc = MissionChangePillarMax;
	o2->Pos = pos;
	UIObjectAddChild(c, o2);

	UIObjectDestroy(o);
	return c;
}

typedef struct
{
	Campaign *C;
	int Idx;
	MapObject *M;
} MapItemIndexData;
static EditorResult MissionChangeMapItemDensity(void *vData, int d);
static bool MapItemObjFunc(UIObject *o, MapObject *mo, void *vData);
static UIObject *CreateMapItemObjs(Campaign *co, int dy)
{
	UIObject *c = UIObjectCreate(UITYPE_NONE, 0, svec2i_zero(), svec2i_zero());
	c->Flags = UI_ENABLED_WHEN_PARENT_HIGHLIGHTED_ONLY;

	UIObject *o =
		UIObjectCreate(UITYPE_CUSTOM, 0, svec2i_zero(), svec2i(20, 40));
	o->u.CustomDrawFunc = MissionDrawMapItem;
	o->ChangeFuncAlt = MissionChangeMapItemDensity;
	o->Flags = UI_LEAVE_YC;
	for (int i = 0; i < 32; i++) // TODO: no limit to objects
	{
		const int x = 10 + i * 20;
		// Drop-down menu for objective type
		UIObject *o2 = UIObjectCopy(o);
		o2->Id2 = i;
		CMALLOC(o2->Data, sizeof(MissionIndexData));
		o2->IsDynamicData = 1;
		((MissionIndexData *)o2->Data)->co = co;
		((MissionIndexData *)o2->Data)->index = i;
		o2->Pos = svec2i(x, Y_ABS - dy);
		CSTRDUP(
			o2->Tooltip,
			"Click: change map object; Shift+Click: change density");
		UIObjectAddChild(
			o2, CreateAddMapItemObjs(
					svec2i(o2->Size.x, o2->Size.y / 2), MapItemObjFunc,
					o2->Data, sizeof(MapItemIndexData), false));
		UIObjectAddChild(c, o2);
	}

	UIObjectDestroy(o);
	return c;
}
static EditorResult MissionChangeMapItemDensity(void *vData, int d)
{
	MissionIndexData *data = vData;
	Mission *m = CampaignGetCurrentMission(data->co);
	if (data->index >= (int)m->MapObjectDensities.size)
	{
		return EDITOR_RESULT_NONE;
	}
	MapObjectDensity *mod = CArrayGet(&m->MapObjectDensities, data->index);
	mod->Density = CLAMP(mod->Density + 5 * d, 0, 512);
	return EDITOR_RESULT_CHANGED;
}
static EditorResult MissionSetMapItem(void *vData, int d);
static void DrawMapItem(
	UIObject *o, GraphicsDevice *g, struct vec2i pos, void *vData);
static bool MapItemObjFunc(UIObject *o, MapObject *mo, void *vData)
{
	o->ChangeFunc = MissionSetMapItem;
	o->u.CustomDrawFunc = DrawMapItem;
	MissionIndexData *data = vData;
	((MapItemIndexData *)o->Data)->C = data->co;
	((MapItemIndexData *)o->Data)->Idx = data->index;
	((MapItemIndexData *)o->Data)->M = mo;
	o->Tooltip = MakePlacementFlagTooltip(mo);
	return true;
}
static EditorResult MissionSetMapItem(void *vData, int d)
{
	UNUSED(d);
	MapItemIndexData *data = vData;
	Mission *m = CampaignGetCurrentMission(data->C);
	if (data->Idx >= (int)m->MapObjectDensities.size)
	{
		return EDITOR_RESULT_NONE;
	}
	MapObjectDensity *mod = CArrayGet(&m->MapObjectDensities, data->Idx);
	mod->M = data->M;
	return EDITOR_RESULT_CHANGED;
}
static void DrawMapItem(
	UIObject *o, GraphicsDevice *g, struct vec2i pos, void *vData)
{
	const MapItemIndexData *data = vData;
	DisplayMapItem(
		svec2i_add(svec2i_add(pos, o->Pos), svec2i_scale_divide(o->Size, 2)),
		data->M);
	if (data->M->Type != MAP_OBJECT_TYPE_PICKUP_SPAWNER)
	{
		return;
	}
	// Also draw the pickup object spawned by this spawner
	const Pic *pic = CPicGetPic(&data->M->u.PickupClass->Pic, 0);
	pos = svec2i_subtract(pos, svec2i_scale_divide(pic->size, 2));
	PicRender(
		pic, g->gameWindow.renderer,
		svec2i_add(svec2i_add(pos, o->Pos), svec2i_scale_divide(o->Size, 2)),
		colorWhite, 0, svec2_one(), SDL_FLIP_NONE, Rect2iZero());
}

static UIObject *CreateCharacterObjs(Campaign *co, int dy)
{
	UIObject *c;
	UIObject *o;
	UIObject *o2;
	int i;
	c = UIObjectCreate(UITYPE_NONE, 0, svec2i_zero(), svec2i_zero());
	c->Flags = UI_ENABLED_WHEN_PARENT_HIGHLIGHTED_ONLY;

	o = UIObjectCreate(UITYPE_CUSTOM, 0, svec2i_zero(), svec2i(20, 40));
	o->u.CustomDrawFunc = MissionDrawEnemy;
	o->ChangeFunc = MissionChangeEnemy;
	o->Flags = UI_LEAVE_YC | UI_SELECT_ONLY_FIRST;
	for (i = 0; i < 15; i++)
	{
		int x = 10 + i * 20;
		o2 = UIObjectCopy(o);
		o2->Id2 = i;
		o2->IsDynamicData = 1;
		CMALLOC(o2->Data, sizeof(MissionIndexData));
		((MissionIndexData *)o2->Data)->co = co;
		((MissionIndexData *)o2->Data)->index = i;
		o2->Pos = svec2i(x, Y_ABS - dy);
		UIObjectAddChild(c, o2);
	}

	UIObjectDestroy(o);
	return c;
}
static UIObject *CreateSpecialCharacterObjs(Campaign *co, int dy)
{
	UIObject *c;
	UIObject *o;
	UIObject *o2;
	int i;
	c = UIObjectCreate(UITYPE_NONE, 0, svec2i_zero(), svec2i_zero());
	c->Flags = UI_ENABLED_WHEN_PARENT_HIGHLIGHTED_ONLY;

	o = UIObjectCreate(UITYPE_CUSTOM, 0, svec2i_zero(), svec2i(20, 40));
	o->u.CustomDrawFunc = MissionDrawSpecialChar;
	o->ChangeFunc = MissionChangeSpecialChar;
	o->Flags = UI_LEAVE_YC | UI_SELECT_ONLY_FIRST;
	for (i = 0; i < 15; i++)
	{
		int x = 10 + i * 20;
		o2 = UIObjectCopy(o);
		o2->Id2 = i;
		o2->IsDynamicData = 1;
		CMALLOC(o2->Data, sizeof(MissionIndexData));
		((MissionIndexData *)o2->Data)->co = co;
		((MissionIndexData *)o2->Data)->index = i;
		o2->Pos = svec2i(x, Y_ABS - dy);
		UIObjectAddChild(c, o2);
	}

	UIObjectDestroy(o);
	return c;
}
