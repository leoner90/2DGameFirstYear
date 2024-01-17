#include "stdafx.h"
#include "headers/MyGame.h"
#include "headers/Enemy.h"
#include "headers/mapGen.h"
#include "headers/playerStats.h"
#include "headers/playerInterface.h"
#include "headers/Player.h"

//Global for class
MapGen  mapGen;
playerStats playerstats;
PlayerInterface playerInterface;
Player player;

/******************************** INIT ********************************/
void CMyGame::OnInitialize()
{
	//Basic reset
	HideMouse();

	LocationNumber = locationTextMover =  0;
	spawnLevel2 = spawnLevel3 = false;
	gameStarted = isGameWon = GameWonConditon = isPlayerDead = false;

	cutsceneCounter = 0;
	startScreenSelection = NEWGAME;
	deathScreenTimer = 0;

	//menu Music
	menuMusic.Play("menu.wav",44);
	menuMusic.Volume(0.5);
	
	//main init of all classes
	player.init(GetWidth());
	playerstats.init(GetHeight(), GetWidth());
	playerInterface.init(GetWidth(), GetHeight());
	mapGen.init(GetHeight(), GetWidth());

	//Enemies lvl 1 init
	GameLevel1Spawn();
	
	//init sprites for this class only
	initSpritesHandler();
}

/******************************** UPDATE ********************************/
void CMyGame::OnUpdate()
{	
	if (IsMenuMode() || IsGameOver() || GameWonConditon || !gameStarted || currentMenuState == STARTCUTSCENE) 
	{
		star->Update(GetTime());
		return; //prevent update while in menu and cutscenes
	}
	else 
	{
		//***** Player and Map Update
		player.OnUpdate(GetTime(), AllEnemies, IsKeyDown(SDLK_d), IsKeyDown(SDLK_a), mapGen, isPlayerDead);
		mapGen.OnUpdate(GetTime(), player.player, GetHeight());
		
		//***** BossMusic
		if (player.player.GetX() > 23000 && !finalMusic) 
		{
			BgMusic.Play("FinalBoss.wav");
			finalMusic = true;
		}

		//***** Enemies
		int i = -1;
		for (auto enemy : AllEnemies) {
			i++;
			if (enemy->DeathTimer) 
			{
				if (enemy->enemyType == BOSS1) GameWonConditon = true; // if BOSS1 killed->game won
				AllEnemies.erase(AllEnemies.begin() + i); //if regular enemie dead -> delete;
				delete enemy;
			}
			else enemy->OnUpdate(GetTime(), player, true, mapGen);	
		}

		if (isPlayerDead) GameOver();
	}
}

/******************************** DRAW ********************************/
void CMyGame::OnDraw(CGraphics* g)
{
	//***** IF CUTSCENES
	if (currentMenuState == STARTCUTSCENE)
	{
		if(cutsceneCounter == 0) cutscene1->Draw(g);
		if (cutsceneCounter == 1) cutscene2->Draw(g);
		return;
	}

	//***** IF GAME WON OR LOSE
	if (IsGameOver() || GameWonConditon) {
		BgMusic.Stop(); 
		gameStarted = false;
		float resetTimer = GameWonConditon ? 20500 : 2000;

		if (GameWonConditon) gameWon->Draw(g);
		else gameOver->Draw(g);

		if (deathScreenTimer == 0) //eg first time
		{
			if(GameWonConditon) victorySound.Play("victory.wav", 10);
			deathScreenTimer = GetTime() + resetTimer;
		}

		if (deathScreenTimer < GetTime())
		{
			victorySound.Stop();
			OnInitialize();
			ChangeMode(MODE_MENU);
			currentMenuState = MENU;
		}
		return; 
	}

	//***** IF MENU MODE OR CHARSTATS
	if (IsMenuMode()) 
	{
		if (currentMenuState == MENU)  menuHandler(g);
		if (currentMenuState == CHARSTATS)
			playerstats.OnDraw(g, GetHeight(), player.str, player.dex, player.con, player.intellect, player.Skillpoint, player);
	}
	//***** IF IN GAME!
	else 
	{
		mapGen.OnDraw(g);
		player.OnDraw(g);
		for (auto enemy : AllEnemies) enemy->OnDraw(g);
		
		//reset world Scroll to zero
		g->SetScrollPos(0, 0);

		//Location Name Printing
		if (LocationNumber == 0 && player.player.GetX() > 300)	PrintLocationName(g, "Wood Near Red Wolf Clan", 1);
		if (LocationNumber == 1 && player.player.GetX() > 17000) 	PrintLocationName(g, "Red Wolf Clan Vilage", 2);
		if (LocationNumber == 2 && player.player.GetX() > 23000) 	PrintLocationName(g, "Leader Lair", 3);
		if (player.player.GetX() < 17000 && LocationNumber != 1 && LocationNumber != 0)  LocationNumber = 0;

		//Enemies Spawn by Lvl(should be done in init probably)
		if (player.player.GetX() > 7000 && !spawnLevel2)  GameLevel2Spawn();
		if (player.player.GetX() > 14000 && !spawnLevel3) GameLevel3Spawn();
	
		playerInterface.OnDraw(player, g, mapGen.scrollOffset, GetWidth(), GetHeight());
		*g << font(22) << color(CColor::LightBlue()) << xy(0,0) << GetTime();
	}
}


//************************************* INIT SPRITES *************************************
void CMyGame::initSpritesHandler()
{
//gameOverScreen
	delete gameOver;
	gameOver = new CSprite();
	gameOver->LoadImage("gameOver.jpg");
	gameOver->SetImage("gameOver.jpg");
	gameOver->SetSize(GetWidth(), GetHeight());
	gameOver->SetPosition(GetWidth() / 2, GetHeight() / 2);

	//gameWon
	delete gameWon;
	gameWon = new CSprite();
	gameWon->LoadImage("cutscene3.jpg");
	gameWon->SetImage("cutscene3.jpg");
	gameWon->SetSize(GetWidth(), GetHeight());
	gameWon->SetPosition(GetWidth() / 2, GetHeight() / 2);

	//menu
	delete startScreen;
	startScreen = new CSprite();
	startScreen->LoadImage("menuBar.jpg");
	startScreen->SetImage("menuBar.jpg");
	startScreen->SetSize(GetWidth(), GetHeight());
	startScreen->SetPosition(GetWidth() / 2, GetHeight() / 2);

	delete star;
	star = new CSprite();
	star->LoadImage("star.png");
	star->SetImage("star.png");
	star->SetOmega(130);

	//location compass
	delete LocationCompass;
	LocationCompass = new CSprite();
	LocationCompass->ClearImage();
	LocationCompass->LoadImage("LocationCompass.png");
	LocationCompass->SetImage("LocationCompass.png");
	LocationCompass->SetSize(60, 60);
	LocationCompass->SetPosition(GetWidth() / 2 - 35, GetHeight());

	delete cutscene1;
	cutscene1 = new CSprite();
	cutscene1->LoadImage("cutscene1.jpg");
	cutscene1->SetImage("cutscene1.jpg");
	cutscene1->SetSize(GetWidth(), GetHeight());
	cutscene1->SetPosition(GetWidth() / 2, GetHeight() / 2);

	delete cutscene2;
	cutscene2 = new CSprite();
	cutscene2->LoadImage("cutscene2.jpg");
	cutscene2->SetImage("cutscene2.jpg");
	cutscene2->SetSize(GetWidth(), GetHeight());
	cutscene2->SetPosition(GetWidth() / 2, GetHeight() / 2);
}




//************************************* ALL ENEMIES *************************************

void CMyGame::enemyCreator(int enemyList[][7])
{
	std::vector<Enemy*> localAllEnemies; // to make proper enemies init
	for (int i = 0; i < 10; i++) {
		localAllEnemies.push_back(new Enemy());
	}

	int i = 0;
	for (auto enemy : localAllEnemies)
	{
		enemy->init(enemyList[i][0], enemyList[i][1], enemyList[i][2], enemyList[i][3], enemyList[i][4], enemyList[i][5], enemyList[i][6]);
		i++;
		AllEnemies.push_back(enemy);
	}

	//delete localAllEnemies to save memory
}


void CMyGame::GameLevel1Spawn()
{
	// MAP1 Enemies
	int enemyListMap1[10][7] = 
	{
		//x pos,y pos, typeOfEnemy, con, str, dex, int
		{ 1200,600,NINJAGIRLMELEE, 7, 5, 5, 5 },
		{ 1760,600, WARIOR, 7, 5, 5, 5},
		{ 2000,600, NINJAGIRLMELEE, 7, 5, 5, 5 },
		{ 2200,600, NINJAGIRL, 5, 5, 5, 3 },
		{ 2800,600, WARIOR, 7, 5, 5, 5 },
		{ 3100,600, WARIOR, 7, 5, 5, 5},
		{ 3600,600, NINJAGIRLMELEE, 7, 5, 5, 5 },
		{ 4200,600, NINJAGIRL, 5, 5, 5, 5 },
		{ 5500,600 , NINJAGIRLMELEE, 7, 5, 5, 5},
		{ 6500,600 , NINJAGIRLKUNAI, 5, 5, 5, 3}
	};

	enemyCreator(enemyListMap1);
}

void CMyGame::GameLevel2Spawn()
{
	spawnLevel2 = true;
	// MAP2 Enemies
	int enemyListMap2[10][7] =
	{
		//x pos,y pos, typeOfEnemy, con, str, dex, int
		{ 8600,600,WARIOR, 13, 10, 10, 10 },
		{ 8960,600, NINJAGIRL, 7, 10, 10, 8},
		{ 9300,600, WARIOR, 13, 10, 10, 10 },
		{ 11200,600, NINJAGIRLMELEE, 13, 10, 10, 10},
		{ 12220,600, WARIOR, 13, 10, 10, 10},
		{ 13900,600, NINJAGIRL,  7, 10, 10, 8},
		{ 14100,600, NINJAGIRLMELEE, 13, 10, 10, 10 },
		{ 14500,600, DOG, 13, 10, 10, 10 },
		{ 15200,600 , NINJAGIRLKUNAI,7, 10, 10, 8},
		{ 15800,600 , NINJAGIRL,13, 10, 10, 10}
	};
	
	 enemyCreator(enemyListMap2);
}

void CMyGame::GameLevel3Spawn()
{

	spawnLevel3 = true;
	int enemyListMap3[10][7] =
	{	//x pos,y pos, typeOfEnemy, con, str, dex, int
		{ 17500, 600 , DOG, 12, 15, 15, 15},
		{ 17960, 600 , NINJAGIRL, 10, 15, 15, 10},
		{ 18300, 600 , NINJAGIRLMELEE, 18, 15, 15, 15},
		{ 18600, 600 , WARIOR, 18, 15, 15, 15},
		{ 18900, 600 , DOG, 12, 15, 15, 15},
		{ 19000, 600 , WARIOR, 18, 15, 15, 15},
		{ 19500, 600 , DOG, 12, 15, 15, 15},
		{ 20000, 600 , NINJAGIRLMELEE, 15, 15, 15, 15},
		{ 21500, 600 , NINJAGIRLKUNAI, 10, 15, 15, 10 },
		{ 25667, 600, BOSS1, 30, 30, 30, 30}
	};
	
	enemyCreator(enemyListMap3);
}

//************************************* PRINT LOCATION NAMES *************************************
void CMyGame::PrintLocationName(CGraphics* g, char* locationName, int newMaplevel)
{
	locationTextMover += 1;
	*g << font(52) << color(CColor::Red()) << xy(GetWidth() * 0.4, GetHeight() * 0.7 + locationTextMover / 2) << locationName;
	LocationCompass->SetPosition(GetWidth() * 0.4 - 30, GetHeight() * 0.7 + 20 + locationTextMover / 2);
	LocationCompass->Draw(g);
	if (locationTextMover > 150) {
		locationTextMover = 0;
		LocationNumber = newMaplevel;
	}
}

//*************************************KEYBOARD EVENTS  *************************************

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	//*********Player BTN controler*********
	if (gameStarted) player.OnKeyDown(sym, mod, unicode, IsMenuMode(), playerInterface);
	player.footSteps.Stop();
	
	//********** CUTSCENES HANDLER **********
	if (currentMenuState == STARTCUTSCENE && sym == SDLK_SPACE)
	{
		cutsceneCounter += 1;
		if (cutsceneCounter >= 2) {
			currentMenuState = INGAME;

			CutScene.Stop();
			BgMusic.Play("main.wav", 20);
			BgMusic.Volume(0.1);
			gameStarted = true;
		} 
	}

	//********** SKIP WIN SCREEn - go back to menu **********
	if (GameWonConditon && (sym == SDLK_SPACE))
	{
		OnInitialize();
		ChangeMode(MODE_MENU);
		currentMenuState = MENU;
		victorySound.Stop();
	}

	//********** MENU NAVIGATION**********
	if (IsMenuMode() && ((sym == SDLK_s) || (sym == SDLK_DOWN)) && !showControllImg)
	{
		startScreenSelection++;
		if (startScreenSelection > 3) startScreenSelection = gameStarted ? 0 : 1; //Change Sequence if game started
	}

	if (IsMenuMode() && ((sym == SDLK_w) || (sym == SDLK_UP)) && !showControllImg)
	{
		startScreenSelection--;
		int range = gameStarted ? 0 : 1;
		if (startScreenSelection < range) startScreenSelection = 3;
	}

	//MAIN MENU
	if (IsMenuMode() && (sym == 13) && currentMenuState != CHARSTATS) //enter
	{
		if (startScreenSelection == NEWGAME    )
		{
			for (auto enemy : AllEnemies) { delete enemy;}
			AllEnemies.clear();

			OnInitialize();
			StartGame();

			BgMusic.Stop();
			menuMusic.Stop();

			CutScene.Play("CutScene.wav", 5);
			CutScene.Volume(0.1);

			startScreenSelection = CONTINUE;
			currentMenuState = STARTCUTSCENE;
		}

		if (startScreenSelection == CONTINUE && gameStarted && currentMenuState != STARTCUTSCENE) {
			ChangeMode(MODE_GAME);
			currentMenuState = INGAME;
		}

		if (startScreenSelection == CONTROLS) {
			showControllImg = true;
			startScreenSelection = BACK;
		}

		else if (startScreenSelection == BACK && showControllImg) 
		{
			showControllImg = false;
			startScreenSelection = CONTROLS; 
		}
		if (startScreenSelection == EXIT) StopGame();
	}

	//ESC MENU
	if (sym == SDLK_ESCAPE && !IsGameOver() && currentMenuState != STARTCUTSCENE)
	{ 
		if (gameStarted && IsMenuMode() && !showControllImg) 
		{
			ChangeMode(MODE_GAME);
			currentMenuState = INGAME;
		}
		else 
		{
			
			if (showControllImg)
			{
				showControllImg = false;
				startScreenSelection = CONTROLS;
			}
			else 
			{
				ChangeMode(MODE_MENU);
				currentMenuState = MENU;
			}
		}
	}

	//C - Char INFO
	if (gameStarted &&  currentMenuState != MENU && sym == SDLK_c && !IsGameOver() && currentMenuState != STARTCUTSCENE)
	{
		if (gameStarted && IsMenuMode()) 
		{
			ChangeMode(MODE_GAME);
			HideMouse();
		}
		else 
		{
			player.setDefaultStandAnimation();
			ShowMouse();
			ChangeMode(MODE_MENU);
			playerstats.charStatsScreen->SetY(GetHeight() * 2);
			currentMenuState = CHARSTATS;
		}
	}
}


void CMyGame::OnLButtonUp(Uint16 x,Uint16 y) 
{
	playerstats.OnLButtonUp(player, player.Skillpoint, GetMouseCoords());
}

void CMyGame::menuHandler(CGraphics* g)
{
	startScreen->Draw(g);
	if (showControllImg)
	{
		*g << font(42) << color(CColor::LightGray()) << xy(400, 550) << "Movement";
		*g << font(25) << color(CColor::White()) << xy(400, 520) << "S - Move Left";
		*g << font(25) << color(CColor::White()) << xy(400, 490) << "D - Move Right";

		*g << font(42) << color(CColor::LightGray()) << xy(400, 430) << "Actions";
		*g << font(25) << color(CColor::White()) << xy(400, 400) << "SPACE - Jump";
		*g << font(25) << color(CColor::White()) << xy(400, 375) << "F - Slide";
		*g << font(25) << color(CColor::White()) << xy(400, 345) << "X - Pick Up An Item";
		*g << font(25) << color(CColor::White()) << xy(400, 315) << "C - Open Stats";

		*g << font(42) << color(CColor::LightGray()) << xy(400, 255) << "Fighting";
		*g << font(25) << color(CColor::White()) << xy(400, 225) << "Q - Firebolt";
		*g << font(25) << color(CColor::White()) << xy(400, 195) << "E - Kunai";
		*g << font(25) << color(CColor::White()) << xy(400, 165) << "R - Katana Attack";

		*g << font(52) << color(startScreenSelection == BACK ? CColor::White() : CColor::LightGray()) << xy(600, 100) << "BACK";
	}
	else
	{
		if (gameStarted)
		{
			*g << font(52) << color(startScreenSelection == CONTINUE ? CColor::White() : CColor::LightGray()) << xy(600, 450) << "CONTINUE";
			if (startScreenSelection == CONTINUE) star->SetPosition(550, 470);

			*g << font(52) << color(startScreenSelection == NEWGAME ? CColor::White() : CColor::LightGray()) << xy(600, 350) << "NEW GAME";
			if (startScreenSelection == NEWGAME) star->SetPosition(550, 370);
		}
		else
		{
			if (startScreenSelection == NEWGAME) star->SetPosition(550, 470);
			*g << font(52) << color(startScreenSelection == NEWGAME ? CColor::White() : CColor::LightGray()) << xy(600, 450) << "NEW GAME";
			*g << font(52) << color(CColor::DarkGray()) << xy(600, 350) << "CONTINUE";
		}

		*g << font(52) << color(startScreenSelection == CONTROLS ? CColor::White() : CColor::LightGray()) << xy(600, 250) << "CONTROLS";
		if (startScreenSelection == CONTROLS) star->SetPosition(550, 270);

		*g << font(52) << color(startScreenSelection == EXIT ? CColor::White() : CColor::LightGray()) << xy(600, 150) << "EXIT";
		if (startScreenSelection == EXIT) star->SetPosition(550, 170);

		star->Draw(g);
	}
}