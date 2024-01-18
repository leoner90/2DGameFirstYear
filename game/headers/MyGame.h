#pragma once
class Enemy;
class Player;
class MapGen;
class playerStats;
class PlayerInterface;

class CMyGame : public CGame
{
public:
	CMyGame();
private:
	 
	//Enemies
	std::vector<Enemy*> AllEnemies;
	enum AllEnemiesTypes { WARIOR, NINJAGIRL, NINJAGIRLKUNAI, DOG, NINJAGIRLMELEE, BOSS1 }; //should be same as in Enemy , TO REDO!! :))

	//Game Conditions
	bool GameWonConditon;
	bool gameStarted;
	bool isGameWon;
	bool isPlayerDead;

	// how long to show dead / Win screen img  
	float deathScreenTimer; // saves current time + resetTimer
	float resetTimer; // actual waiting time

	//Player Class
	Player* player;

	//Map Class
	MapGen* mapGen;

	//playerStats Class
	playerStats* playerstats;

	//PlayerInterface Class
	PlayerInterface* playerInterface;

	//Sprites todo NEW pointers
	void initSpritesHandler();
	CSprite* gameOver, *gameWon, *star, *LocationCompass;
	CSprite* startScreen, *cutscene1, *cutscene2, *cutscene3;

	//Menu / Cutscenes / Char stats
	void menuHandler(CGraphics* g);
	int startScreenSelection;
	bool showControllImg;
	int currentMenuState;
	enum game_menu_state { MENU, CHARSTATS, STARTCUTSCENE, INGAME };
	enum menuScreenSelection { CONTINUE, NEWGAME, CONTROLS, EXIT, BACK };

	//CutScenes
	int cutsceneCounter = 0;
 
	//Spawn Enemies at certain point
	void enemyCreator(int enemyList[][7]);
	void GameLevel1Spawn();
	void GameLevel2Spawn();
	void GameLevel3Spawn();
	bool spawnLevel2, spawnLevel3;

	//print locationname at certain point
	void PrintLocationName(CGraphics* g, char* locationName, int newMaplevel);
	float locationTextMover;
	int LocationNumber;

	//Sounds
	bool finalMusic; //boss Music , if teleported to boss -> true
	CSoundPlayer BgMusic;
	CSoundPlayer  menuMusic;
	CSoundPlayer  CutScene;
	CSoundPlayer  victorySound;

	//main GFC fun
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);
	virtual void OnInitialize();
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnLButtonUp(Uint16 x, Uint16 y);
};
