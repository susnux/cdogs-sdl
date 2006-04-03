/*
    C-Dogs SDL
    A port of the legendary (and fun) action/arcade cdogs.
    Copyright (C) 1995 Ronny Wester
    Copyright (C) 2003 Jeremy Chin 
    Copyright (C) 2003 Lucas Martin-King 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

-------------------------------------------------------------------------------

 cdogs.c - main bits
 
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "SDL.h"

#include "joystick.h"
#include "objs.h"
#include "actors.h"
#include "grafx.h"
#include "pics.h"
#include "sounds.h"
#include "text.h"
#include "gamedata.h"
#include "ai.h"
#include "triggers.h"
#include "input.h"
#include "prep.h"
#include "hiscores.h"
#include "mission.h"
#include "game.h"
#include "keyboard.h"
#include "draw.h"
#include "mainmenu.h"
#include "password.h"
#include "files.h"


static void *bkg = NULL;
static char lastPassword[PASSWORD_MAX + 1] = "";


void DrawObjectiveInfo(int index, int x, int y, struct Mission *mission)
{
	TOffsetPic pic;
	TranslationTable *table = NULL;
	int i = 0;

	switch (mission->objectives[index].type) {
	case OBJECTIVE_KILL:
		i = characterDesc[mission->baddieCount +
				  CHARACTER_OTHERS].facePic;
		table =
		    &characterDesc[mission->baddieCount +
				  CHARACTER_OTHERS].table;
		pic.picIndex = cHeadPic[i][DIRECTION_DOWN][STATE_IDLE];
		pic.dx = cHeadOffset[i][DIRECTION_DOWN].dx;
		pic.dy = cHeadOffset[i][DIRECTION_DOWN].dy;
		break;
	case OBJECTIVE_RESCUE:
		i = characterDesc[CHARACTER_PRISONER].facePic;
		table = &characterDesc[CHARACTER_PRISONER].table;
		pic.picIndex = cHeadPic[i][DIRECTION_DOWN][STATE_IDLE];
		pic.dx = cHeadOffset[i][DIRECTION_DOWN].dx;
		pic.dy = cHeadOffset[i][DIRECTION_DOWN].dy;
		break;
	case OBJECTIVE_COLLECT:
		i = gMission.objectives[index].pickupItem;
		pic = cGeneralPics[i];
		break;
	case OBJECTIVE_DESTROY:
		i = gMission.objectives[index].blowupObject->pic;
		pic = cGeneralPics[i];
		break;
	case OBJECTIVE_INVESTIGATE:
		return;
	default:
		i = gMission.objectives[i].pickupItem;
		pic = cGeneralPics[i];
	}
	if (table)
		DrawTTPic(x + pic.dx, y + pic.dy, gPics[pic.picIndex],
			  table, NULL);
	else
		DrawTPic(x + pic.dx, y + pic.dy, gPics[pic.picIndex],
			 NULL);
}

int MissionDescription(int y, const char *description)
{
	int w, x, lines;
	const char *ws, *word, *p, *s;

	x = 20;
	lines = 1;
	TextGoto(x, y);

	s = ws = word = description;
	while (*s) {
		// Find word
		ws = s;
		while (*s == ' ' || *s == '\n')
			s++;
		word = s;
		while (*s != 0 && *s != ' ' && *s != '\n')
			s++;

		for (w = 0, p = ws; p < s; p++)
			w += TextCharWidth(*p);

		if (x + w > 300 && w < 280) {
			y += TextHeight();
			x = 20;
			lines++;
			ws = word;
		}

		for (p = ws; p < word; p++)
			x += TextCharWidth(*p);
		TextGoto(x, y);

		for (p = word; p < s; p++) {
			TextChar(*p);
			x += TextCharWidth(*p);
		}
	}
	return lines;
}

void CampaignIntro(void *bkg)
{
	printf("Doing campaign intro\n");
	memcpy(GetDstScreen(), bkg, 64000);

	TextStringAt(50, 5, gCampaign.setting->title);
	TextStringAt(60, 15, "by ");
	TextStringWithTable(gCampaign.setting->author, &tableFlamed);

	MissionDescription(30, gCampaign.setting->description);

	CopyToScreen();
	Wait();
}

void MissionBriefing(void *bkg)
{
	char s[128];
	int i, y;

	memcpy(GetDstScreen(), bkg, 64000);

	sprintf(s, "Mission %d", gMission.index + 1);
	TextStringAt(50, 5, s);
	TextStringAt(50, 15, gMission.missionData->title);

	y = 30;
	y += TextHeight() * MissionDescription(y,
					       gMission.missionData->
					       description);

	y += TextHeight();

	for (i = 0; i < gMission.missionData->objectiveCount; i++)
		if (gMission.missionData->objectives[i].required > 0) {
			TextStringAt(50, y,
				     gMission.missionData->objectives[i].
				     description);
			DrawObjectiveInfo(i, 260, y + 8,
					  gMission.missionData);
			y += 20;
		}

	if (gMission.index)
//      (gMission.index % 3) == 0)
	{
		strcpy(lastPassword, MakePassword(gMission.index));
		TextStringAt(150, 5, "Password: ");
		TextString(lastPassword);
	}

	CopyToScreen();

	Wait();
}

void Summary(int x, struct PlayerData *data, int character)
{
	char s[50];
	int y = 50;

	if (lastPassword[0]) {
		TextStringAt(200, 2, "Last password: ");
		TextString(lastPassword);
	}

	if (data->survived) {
		if (data->hp > 150)
			data->totalScore += (data->hp - 150) * 10;
		else if (data->hp <= 0)
			data->totalScore -= 500;

		if (data->friendlies > 5
		    && data->friendlies > data->kills / 2)
			data->totalScore -= 100 * data->friendlies;
		else if (data->weaponCount == 1 &&
			 data->weapons[0] == GUN_KNIFE &&
			 data->friendlies == 0 && data->kills > 5)
			data->totalScore += 50 * data->kills;
		else if (data->kills == 0 && data->friendlies == 0)
			data->totalScore += 500;
	}

	if (data->survived)
		TextStringAt(x, y, "Completed mission");
	else
		TextStringWithTableAt(x, y, "Failed mission", &tableFlamed);

	y += TextHeight();
	DisplayPlayer(x, data, character, 0);
	sprintf(s, "Score: %d", data->score);
	TextStringAt(x, y, s);
	y += TextHeight();
	sprintf(s, "Total: %d", data->totalScore);
	TextStringAt(x, y, s);
	y += TextHeight();
	sprintf(s, "Missions: %d",
		data->missions + (data->survived ? 1 : 0));
	TextStringAt(x, y, s);
	y += TextHeight();
	if (data->survived && (data->hp > 150 || data->hp <= 0)) {
		if (data->hp > (200 * gOptions.playerHp) / 100 - 50)
			sprintf(s, "Health bonus: %d",
				(data->hp + 50 -
				 (200 * gOptions.playerHp) / 100) * 10);
		else if (data->hp <= 0)
			sprintf(s, "Resurrection fee: %d", -500);
		TextStringAt(x, y, s);
		y += TextHeight();
	}
	if (data->friendlies > 0 && data->friendlies > data->kills / 2) {
		sprintf(s, "Butcher penalty: %d", 100 * data->friendlies);
		TextStringAt(x, y, s);
		y += TextHeight();
	} else if (data->weaponCount == 1 &&
		   data->weapons[0] == GUN_KNIFE && data->kills > 0) {
		sprintf(s, "Ninja bonus: %d", 50 * data->kills);
		TextStringAt(x, y, s);
		y += TextHeight();
	} else if (data->kills == 0 && data->friendlies == 0) {
		sprintf(s, "Friendly bonus: %d", 500);
		TextStringAt(x, y, s);
		y += TextHeight();
	}
}

void Bonuses(void)
{
	int i;
	int y = 100;
	int done, req, total;
	int access;
	int index;
	char s[100];

	index = 1;
	for (i = 0; i < gMission.missionData->objectiveCount; i++) {
		done = gMission.objectives[i].done;
		req = gMission.objectives[i].required;
		total = gMission.objectives[i].count;

		if (done > 0 || req > 0) {
			DrawObjectiveInfo(i, 25, y + 8,
					  gMission.missionData);
			sprintf(s, "Objective %d: %d of %d, %d required",
				index, done, total, req);
			if (req > 0)
				TextStringAt(50, y, s);
			else
				TextStringWithTableAt(50, y, s,
						      &tablePurple);
			if (done < req)
				TextStringWithTableAt(240, y, "Failed",
						      &tableFlamed);
			else if (done == total && done > req
				 && (gPlayer1Data.survived
				     || gPlayer2Data.survived)) {
				TextStringAt(240, y, "Perfect: 500");
				if (gPlayer1Data.survived)
					gPlayer1Data.totalScore += 500;
				if (gPlayer2Data.survived)
					gPlayer2Data.totalScore += 500;
			} else if (req > 0)
				TextStringAt(240, y, "Done");
			else
				TextStringAt(240, y, "Bonus!");
			y += 15;
			index++;
		}
	}

	access = 0;
	if ((gMission.flags & FLAGS_KEYCARD_YELLOW) != 0)
		access += 50;
	if ((gMission.flags & FLAGS_KEYCARD_GREEN) != 0)
		access += 100;
	if ((gMission.flags & FLAGS_KEYCARD_BLUE) != 0)
		access += 150;
	if ((gMission.flags & FLAGS_KEYCARD_RED) != 0)
		access += 200;
	if (access > 0 && (gPlayer1Data.survived || gPlayer2Data.survived)) {
		sprintf(s, "Access bonus: %d", access);
		TextStringAt(100, y, s);
		y += TextHeight() + 1;
		if (gPlayer1Data.survived)
			gPlayer1Data.totalScore += access;
		if (gPlayer2Data.survived)
			gPlayer2Data.totalScore += access;
	}

	i = 60 + gMission.missionData->objectiveCount * 30 -
	    missionTime / 70;
	if (i > 0 && (gPlayer1Data.survived || gPlayer2Data.survived)) {
		sprintf(s, "Time bonus: %d secs x 25 = %d", i, i * 25);
		TextStringAt(100, y, s);
		if (gPlayer1Data.survived)
			gPlayer1Data.totalScore += i * 25;
		if (gPlayer2Data.survived)
			gPlayer2Data.totalScore += i * 25;
	}
}

void MissionSummary(void *bkg)
{
	memcpy(GetDstScreen(), bkg, 64000);

	Bonuses();
	if (gOptions.twoPlayers) {
		Summary(50, &gPlayer1Data, CHARACTER_PLAYER1);
		Summary(200, &gPlayer2Data, CHARACTER_PLAYER2);
	} else
		Summary(125, &gPlayer1Data, CHARACTER_PLAYER1);

	CopyToScreen();

	Wait();
}

void ShowScore(void *bkg, int score1, int score2)
{
	char s[10];

	memcpy(GetDstScreen(), bkg, 64000);

	DisplayPlayer(50, &gPlayer1Data, CHARACTER_PLAYER1, 0);
	sprintf(s, "%d", score1);
	TextStringAt(60, 100, s);

	DisplayPlayer(200, &gPlayer2Data, CHARACTER_PLAYER2, 0);
	sprintf(s, "%d", score2);
	TextStringAt(210, 100, s);

	CopyToScreen();
	Wait();
}

void FinalScore(void *bkg, int score1, int score2)
{
	memcpy(GetDstScreen(), bkg, 64000);

	if (score1 == score2) {
		DisplayPlayer(50, &gPlayer1Data, CHARACTER_PLAYER1, 0);
		DisplayPlayer(200, &gPlayer2Data, CHARACTER_PLAYER2, 0);
		TextStringAt(125, 100, "It's a draw!");
	} else if (score1 > score2) {
		DisplayPlayer(125, &gPlayer1Data, CHARACTER_PLAYER1, 0);
		TextStringAt(125, 100, "Winner!");
	} else {
		DisplayPlayer(125, &gPlayer2Data, CHARACTER_PLAYER2, 0);
		TextStringAt(125, 100, "Winner!");
	}
	CopyToScreen();
	Wait();
}


static const char *finalWords1P[] = {
	"Ha, next time I'll use my good hand",
	"Over already? I was just warming up...",
	"There's just no good opposition to be found these days!",
	"Well, maybe I'll just do my monthly reload then",
	"Woof woof",
	"I'll just bury the bones in the back yard, he-he",
	"I just wish they'd let me try bare-handed",
	"Rambo? Who's Rambo?",
	"<in Austrian accent:> I'll be back",
	"Gee, my trigger finger is sore",
	"I need more practice. I think I missed a few shots at times"
};

static const char *finalWords2P[] = {
	"United we stand, divided we conquer",
	"Nothing like good teamwork, is there?",
	"Which way is the camera?",
	"We eat bullets for breakfast and have grenades as dessert",
	"We're so cool we have to wear mittens",
};

#define CONGRATULATIONS "Congratulations, you have completed "

void Victory(void *bkg)
{
	int x, i;
	const char *s;

	memcpy(GetDstScreen(), bkg, 64000);

	x = 160 - TextWidth(CONGRATULATIONS) / 2;
	TextStringAt(x, 100, CONGRATULATIONS);
	x = 160 - TextWidth(gCampaign.setting->title) / 2;
	TextStringWithTableAt(x, 115, gCampaign.setting->title,
			      &tableFlamed);

	if (gOptions.twoPlayers) {
		i = sizeof(finalWords2P) / sizeof(char *);
		i = rand() % i;
		s = finalWords2P[i];
		DisplayPlayer(50, &gPlayer1Data, CHARACTER_PLAYER1, 0);
		DisplayPlayer(200, &gPlayer2Data, CHARACTER_PLAYER2, 0);
		if (gPlayer1Data.survived)
			gPlayer1Data.missions++;
		if (gPlayer2Data.survived)
			gPlayer2Data.missions++;
	} else {
		i = sizeof(finalWords1P) / sizeof(char *);
		i = rand() % i;
		s = finalWords1P[i];
		DisplayPlayer(125, &gPlayer1Data, CHARACTER_PLAYER1, 0);
		gPlayer1Data.missions++;
	}

	x = 160 - TextWidth(s) / 2;
	TextGoto(x, 140);
	TextCharWithTable('"', &tableDarker);
	TextStringWithTable(s, &tablePurple);
	TextCharWithTable('"', &tableDarker);

	PlaySound(SND_HAHAHA, 0, 255);

	CopyToScreen();
	Wait();
}


static void PlaceActor(TActor * actor)
{
	int x, y;

	do {
		x = ((rand() % (XMAX * TILE_WIDTH)) << 8);
		y = ((rand() % (YMAX * TILE_HEIGHT)) << 8);
	}
	while (!OKforPlayer(x, y) || !MoveActor(actor, x, y));
}

void InitData(struct PlayerData *data)
{
	data->totalScore = 0;
	data->missions = 0;
}

void DataUpdate(int mission, struct PlayerData *data)
{
	if (!data->survived) {
		data->totalScore = 0;
		data->missions = 0;
	} else
		data->missions++;
	data->lastMission = mission;
}

static void CleanupMission(void)
{
	KillAllActors();
	KillAllMobileObjects();
	KillAllObjects();
	FreeTriggersAndWatches();
	gPlayer1 = gPlayer2 = gPrisoner = NULL;
}

static void InitPlayers(int twoPlayers, int maxHealth, int mission)
{
	gPlayer1Data.score = 0;
	gPlayer1Data.kills = gPlayer1Data.friendlies = 0;
	gPlayer1Data.allTime = gPlayer1Data.today = -1;
	gPlayer1Data.lastMission = mission;
	gPlayer1 = AddActor(CHARACTER_PLAYER1);
	gPlayer1->gun = gPlayer1Data.weapons[0];
	gPlayer1->flags = FLAGS_PLAYER1;
	PlaceActor(gPlayer1);
	gPlayer1->health = maxHealth;

	if (twoPlayers) {
		gPlayer2Data.score = 0;
		gPlayer2Data.kills = gPlayer2Data.friendlies = 0;
		gPlayer2Data.allTime = gPlayer2Data.today = -1;
		gPlayer2Data.lastMission = mission;
		gPlayer2 = AddActor(CHARACTER_PLAYER2);
		gPlayer2->gun = gPlayer2Data.weapons[0];
		gPlayer2->flags = FLAGS_PLAYER2;
		PlaceActor(gPlayer2);
		gPlayer2->health = maxHealth;
	}
}

static void PlayGameSong(void)
{
	// Play a tune
	// Start by trying to play a mission specific song,
	// otherwise pick one from the general collection...
	if (gMission.missionData->song[0]
	    && PlaySong(gMission.missionData->song))
		/* nothing */ ;
	else if (gGameSongs) {
		PlaySong(gGameSongs->path);
		ShiftSongs(&gGameSongs);
	} else
		PlaySong(NULL);
}

static void PlayMenuSong(void)
{
	if (gMenuSongs) {
		PlaySong(gMenuSongs->path);
		ShiftSongs(&gMenuSongs);
	} else
		PlaySong(NULL);
}


int Game(int mission)
{
	int run, gameOver;
	int allTime, todays;
	int maxHealth;

	maxHealth = 200 * (gOptions.playerHp) / 100;

	do {
		SetupMission(mission, 1);

		SetupMap();

		srand(clock());
		InitializeBadGuys();

		MissionBriefing(bkg);
		PlayerEquip(bkg);

		InitPlayers(gOptions.twoPlayers, maxHealth, mission);

		CreateCharacters();

		PlayGameSong();

		run = gameloop();

		gameOver = (!gPlayer1 && !gPlayer2) ||
		    mission == gCampaign.setting->missionCount - 1;

		gPlayer1Data.survived = gPlayer1 != NULL;
		if (gPlayer1)
			gPlayer1Data.hp = gPlayer1->health;
		gPlayer2Data.survived = gPlayer2 != NULL;
		if (gPlayer2)
			gPlayer2Data.hp = gPlayer2->health;

		CleanupMission();

		PlayMenuSong();
		printf(">> Starting\n");

		if (run) {
			MissionSummary(bkg);
			if (gameOver
			    && (gPlayer1Data.survived
				|| gPlayer2Data.survived))
				Victory(bkg);
		}

		allTime = todays = 0;
		if ((run && !gPlayer1Data.survived) || gameOver) {
			EnterHighScore(&gPlayer1Data);
			allTime = gPlayer1Data.allTime >= 0;
			todays = gPlayer1Data.today >= 0;
		}
		if (run && gOptions.twoPlayers
		    && (!gPlayer2Data.survived || gameOver)) {
			EnterHighScore(&gPlayer2Data);
			allTime = gPlayer2Data.allTime >= 0;
			todays = gPlayer2Data.today >= 0;

			// Check if player 1's position(s) in the list(s) need adjustment...
			if (gPlayer2Data.allTime >= 0 &&
			    gPlayer2Data.allTime <= gPlayer1Data.allTime)
				gPlayer1Data.allTime++;
			if (gPlayer2Data.today >= 0 &&
			    gPlayer2Data.today <= gPlayer1Data.today)
				gPlayer1Data.today++;
		}
		if (allTime && !gameOver)
			DisplayAllTimeHighScores(bkg);
		if (todays && !gameOver)
			DisplayTodaysHighScores(bkg);

		mission++;

		DataUpdate(mission, &gPlayer1Data);
		if (gOptions.twoPlayers)
			DataUpdate(mission, &gPlayer2Data);
	}
	while (run && !gameOver);
	return run;
}

int Campaign(void)
{
	int mission;

	InitData(&gPlayer1Data);
	InitData(&gPlayer2Data);

	mission = EnterPassword(bkg, lastPassword);
	lastPassword[0] = 0;

	return Game(mission);
}

void DogFight(void)
{
	int run;
	int score1 = 0, score2 = 0;
	int twoPlayers = gOptions.twoPlayers;

	InitData(&gPlayer1Data);
	InitData(&gPlayer2Data);

	gOptions.badGuys = 0;
	gOptions.twoPlayers = 1;

	do {
		SetupMission(0, 1);
		SetupMap();
		PlayerEquip(bkg);

		srand(clock());
		InitPlayers(YES, 500, 0);
/*
    gPlayer1 = AddActor( CHARACTER_PLAYER1);
    gPlayer1->gun = gPlayer1Data.weapons[0];
    gPlayer1->flags = FLAGS_PLAYER1; // | FLAGS_HURTALWAYS;
    PlaceActor( gPlayer1);
    gPlayer1->health = 500;

    gPlayer2 = AddActor( CHARACTER_PLAYER2);
    gPlayer2->gun = gPlayer2Data.weapons[0];
    gPlayer2->flags = FLAGS_PLAYER2; // | FLAGS_HURTALWAYS;
    PlaceActor( gPlayer2);
    gPlayer2->health = 500;
*/

		PlayGameSong();

		run = gameloop();

		if (gPlayer1 != NULL)
			score1++;
		if (gPlayer2 != NULL)
			score2++;

		CleanupMission();
		PlayMenuSong();

		if (run)
			ShowScore(bkg, score1, score2);
	}
	while (run && score1 < 5 && score2 < 5);

	gOptions.badGuys = 1;
	gOptions.twoPlayers = twoPlayers;

	if (run)
		FinalScore(bkg, score1, score2);
}

void *MakeBkg(void)
{
	void *bkg = malloc(64000);
	struct Buffer *buffer;
	unsigned char *p;
	int i;

	SetupBuiltinDogfight(0);
	SetupMission(0, 1);
	SetupMap();
	SetDstScreen(bkg);
	buffer = malloc(sizeof(struct Buffer));
	SetBuffer(512, 384, buffer, X_TILES);
	FixBuffer(buffer, 255);
	DrawBuffer(buffer, 0);
	free(buffer);
	KillAllObjects();
	FreeTriggersAndWatches();

	p = bkg;
	for (i = 0; i < 64000; i++)
		p[i] = tableGreen[p[i] & 0xFF];

	return bkg;
}

void MainLoop(void)
{
	void *myScreen;

	bkg = MakeBkg();
	myScreen = malloc(64000);
	memset(myScreen, 0, 64000);
	SetDstScreen(myScreen);

	SetupBuiltinCampaign(1);

	while (MainMenu(bkg)) {
		printf(">> Entering Campaign\n");
		ResetCampaign();
		if (!gCampaign.dogFight)
			CampaignIntro(bkg);
		printf(">> Entering selection\n");
		PlayerSelection(gOptions.twoPlayers
				|| gCampaign.dogFight, bkg);
		printf(">> Starting campaign\n");
		if (gCampaign.dogFight)
			DogFight();
		else if (Campaign()) {
			DisplayAllTimeHighScores(bkg);
			DisplayTodaysHighScores(bkg);
		}
	}
	printf(">> Leaving Main Game Loop\n");
//	free(myScreen);
//	free(bkg);
}

void PrintTitle(void)
{
	printf("C-Dogs %s, (SDL Port %s)\n", CDOGS_VERSION, CDOGS_SDL_VERSION);

	printf("Original Code Copyright Ronny Wester 1995\n");
	printf("Game Data Copyright Ronny Wester 1995\n");
	printf("SDL Port by Jeremy Chin and Lucas Martin-King, Copyright 2003-2006\n\n");
	printf("%s%s%s%s",
		"C-Dogs SDL comes with ABSOLUTELY NO WARRANTY;\n",
		"see the file COPYING that came with this distibution...\n",
		"This is free software, and you are welcome to redistribute it\n",
		"under certain conditions; for details see COPYING.\n\n");
}

void PrintHelp (void)
{
	printf("%s%s%s\n",
		"Video Options:\n",
		"    -fullscreen     Try and use a fullscreen video mode.\n",
		"    -scale          Scale the window resolution up.\n");

	printf("%s%s%s%s\n",
		"Game Options:\n",
		"    -savecampaigns  Save builtin campaigns as files in the current directory.\n",
		"    -wait           Wait for a key hit before initialising video.\n",
		"    -slices         Display CPU slices [*broken*]");
}

int main(int argc, char *argv[])
{
	int i, wait = 0;
	char s[13];
	int compile = 1, rle = 1;
	int vid_mode = VID_WIN_NORMAL;

	PrintTitle();
	
	for (i = 1; i < argc; i++) {
		if ((strlen(argv[i]) > 1 && *(argv[i]) == '-')
		    || *(argv[i]) == '/') {
			if (strcmp(argv[i] + 1, "slices") == 0) {
				printf("Displaying CPU slices\n");
				gOptions.displaySlices = 1;
			}
			if (strstr(argv[i] + 1, "shakemult=")) {
				char *val = strchr(argv[i], '=');
				int nval;
				extern int shakeMultiplier;

				val++;
				nval = atoi(val);
				printf("Shake multiplier: %d\n", nval);
				shakeMultiplier = nval;	
			}
			if (strcmp(argv[i] + 1, "savecampaigns") == 0) {
				int j = 0;
				
				printf("Saving builtin campaigns as files: \n");
				while (SetupBuiltinCampaign(j)) {
					sprintf(s, "bltin%02d.cpn", j);
					printf(" -> Saving [%s]\n", s);
					SaveCampaign(s, gCampaign.setting);
					j++;
				}
				printf("Done\n\n");
			}
			if (strcmp(argv[i] + 1, "wait") == 0)
				wait = 1;
			if (strcmp(argv[i] + 1, "nocompile") == 0)
				compile = 0;
			if (strcmp(argv[i] + 1, "norle") == 0)
				rle = 0;
			if (strcmp(argv[i] + 1, "fullscreen") == 0) {
				if (vid_mode == VID_WIN_SCALE)
					printf("Warning: -fullscreen and -scale are mutually exclusive...\n\n");
				vid_mode = VID_FULLSCREEN;
			}
			if (strcmp(argv[i] + 1, "scale") == 0) {
				if (vid_mode == VID_FULLSCREEN)
					printf("Warning: -fullscreen and -scale are mutually exclusive...\n");
				vid_mode = VID_WIN_SCALE;
			}
			if (strcmp(argv[i] + 1, "help") == 0 ||
				strcmp(argv[i] + 1, "h") == 0 || 
				strcmp(argv[i] + 1, "-help") == 0) {
				PrintHelp();
				SDL_Quit();
				return 0;
			}
		}
	}

	/* moved here because -savecampaigns causes a crash, why? */
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) {
		printf("Failed to start SDL!\n");
		return -1;
	}
	
	compile = 0;
	rle = 0;

	SetupConfigDir();

	printf("Data directory:\t\t%s\n",	GetDataFilePath(""));
	printf(" -> Missions:\t\t%s\n",		GetDataFilePath("missions/"));
	printf(" -> Dogfights:\t\t%s\n",	GetDataFilePath("dogfights/"));
	printf(" -> Sounds:\t\t%s\n",		GetDataFilePath("sounds/"));
	printf(" -> Graphics:\t\t%s\n",		GetDataFilePath("graphics/"));
	printf("Config directory:\t%s\n\n",	GetConfigFilePath(""));

	i = ReadPics(GetDataFilePath("graphics/cdogs.px"), gPics, PIC_COUNT1, gPalette);
	if (!i) {
		printf("Unable to read CDOGS.PX (%s)\n", GetDataFilePath("graphics/cdogs.px"));
		exit(0);
	}
	if (!AppendPics(GetDataFilePath("graphics/cdogs2.px"), gPics, PIC_COUNT1, PIC_MAX)) {
		printf("Unable to read CDOGS2.PX (%s)\n", GetDataFilePath("graphics/cdogs2.px"));
		exit(0);
	}
	gPalette[0].red = gPalette[0].green = gPalette[0].blue = 0;
	memcpy(origPalette, gPalette, sizeof(origPalette));
	InitializeTranslationTables();

	if (compile)
		printf("Size of compiled sprites: %d bytes\n",
		       CompilePics(PIC_MAX, gPics, gCompiledPics));
	else {
		memset(gCompiledPics, 0, sizeof(gCompiledPics));
		printf("Not using compiled sprites\n");
	}

	if (rle)
		printf("Size of RLE sprites: %d bytes\n",
		       RLEncodePics(PIC_MAX, gPics, gRLEPics));
	else {
		memset(gRLEPics, 0, sizeof(gRLEPics));
		printf("Not using RLE\n");
	}

	TextInit(GetDataFilePath("graphics/font.px"), -2, compile, rle);

	if (!InitializeSound()) {
		printf("Sound initialization failed!\n");
		getchar();
	}

	LoadHighScores();

	LoadSongs(GetConfigFilePath("menusong.cfg"), &gMenuSongs);
	LoadSongs(GetConfigFilePath("gamesong.cfg"), &gGameSongs);
	LoadTemplates();

	PlayMenuSong();

	LookForCustomCampaigns();

//      InitSticks();
	if (gSticks[0].present)
		printf("Joystick 1 detected\n");
	if (gSticks[1].present)
		printf("Joystick 2 detected\n");

/*	if (gSticks[0].present) {
		gPlayer1Data.controls = JOYSTICK_ONE;
		if (gSticks[1].present)
			gPlayer2Data.controls = JOYSTICK_TWO;
	} else if (gSticks[1].present)
		gPlayer1Data.controls = JOYSTICK_TWO;
*/
	LoadConfig();

	if (wait) {
		printf("Press Enter...\n");
		getchar();
	}


	if (InitVideo(vid_mode) == -1) {
		printf("Video didn't init!\n");
	} else {
		SetPalette(gPalette);
		printf(">> Entering main loop\n");
		InitMutex();
		SDL_AddTimer(5, synchronizer, NULL);
		MainLoop();
	}
	printf(">> Shutting Down...\n");

	SaveConfig();
	SaveTemplates();
//	FreeSongs(&gMenuSongs);
//	FreeSongs(&gGameSongs);
	SaveHighScores();

	SDL_Quit();

	printf("Bye :)\n");

	return 0;
}

