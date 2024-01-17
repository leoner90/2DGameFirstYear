#pragma once

//forward declaration
class Enemy;
class MapGen;
class PlayerInterface;

class Player
{
public:
	//Main Functions
	void init(float screenWidth);
	void OnUpdate(long t, std::vector<Enemy*> AllEnemies, bool SDLK_d, bool SDLK_a, MapGen& mapgen, bool& gameOver);
	void OnDraw(CGraphics* g);
	void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode, bool IsMenuMode,  PlayerInterface& playerInterface);

	void PlayerGettingDamage(float damageAmount);

	//Leveling
	void PlayerGettingExp(float expAmount);
	void DrawExpInfo(CGraphics* g);
	float lvlTextMover;

	//player stats
	float CurrentPlayerHealth, CurrentPlayerMp, CurrentPlayerEnergy;
	float maxPlayerHealth, maxPlayerMp, maxPlayerEnergy;
	int str, intellect, con, dex;
	float meleeDamage, kunaiDamage, fireboltDamage;

	//basic
	void setDefaultStandAnimation();
	float PlayerDirection;
	float slideInitPos;
	CSprite player;
 
	//level
	float levelsRange[9] = { 500,1500,3000,5000,8000,12000,17000,23000,30000};
	bool maxLvl, isLvlup;
	float currentExp, tillNextLvlExp, expGained;
	int playerLVL , Skillpoint;

	float msgTimer;

	CSpriteList shotList;
	CSpriteList DroppedPoutions;

	//animation
	enum animation_state {OCCUPIED ,PICKUP, NONE, STANDLEFT, STANDRIGHT, WALKLEFT, WALKRIGHT, InAir, FALLING, SLIDE, ATTACK, REMOTEATTACK, INDAMAGE, DEAD};
	int numberOfFramesToStop;

	//inventory
	int HpPoutionCount, MpPoutionCount, EnergyPoutionCount, ammoCount;
	enum ammoType {  HpPoutin, MpPoution, EnergyPoution , kunai, firebolt};
	CSoundPlayer   footSteps;
	bool inDamage, inAtack;
private:
	//mirroring
	std::vector<Enemy*> AllEnemiesLocal;
	float ScreenWidth;
	int inslide;
	MapGen* localMapVar;

	//player control and speed
	float hwalkSpeed = 295;
	float vwalkSpeed = 100;
	float jumpHeight;

	//combat
	
	Uint32 CurrentTime;
	CSpriteList hitEffectList;

	//collision
	CVector pos;
	float lowerlimit;
	float upperlimit;
	float leftlimit;
	bool TpToLastBoss;

	//sounds
	CSoundPlayer hitSound, deadSound, katanaSound, remoteAttack, levelUp, fireboltSound,slideSound , poutionDrink, noAmmo, pickUpSound;

	void playerControl(bool SDLK_d, bool SDLK_a, MapGen& mapgen);
	void playerCollision(float velocity, MapGen& mapgen);
	void playerPickUpItem();
	void shotHandler();
};