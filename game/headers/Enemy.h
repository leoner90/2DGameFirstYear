#pragma once
 
class Player;
class MapGen;

class Enemy  
{
public:
	//Main Functions
	void init(int xPos, int yPos, int typeofEnemy, int con, int str, int dex, int intellect);
	void OnUpdate(Uint32 t, Player& player, bool patrol, MapGen& mapGen);
	void OnDraw(CGraphics* g);
	
	//Damage
	void EnemyGettingDamage(float damageAmount, float t, CSpriteList& DroppedPoutions);

	//mainSprite
	CSprite enemy;

	//Is enemy dead , And coundown till insstance is deleted to play animation
	bool dead;
	bool DeathTimer;
	int enemyType;

private:
	//Variables Miroring for local use
	MapGen* localMapVar;
	Player* castToPlayer;
	Uint32 CurrentTime;

	//Enemie Stats 
	float CurrentEnemyHealth, CurrentEnemyMp, CurrentEnemyEnergy;
	int str, intellect, con, dex;
	float maxEnemyHealth, maxEnemyMp, maxEnemyEnergy;

	//Hp bar
	void EnemyInterface();
	CSpriteRect* enemyHpBarRect2 = new CSpriteRect(0, 500, 70, 20, CColor::Red(), CColor::Red(), 1);

	//Sprite Lists for Hit Effect and Shots List
	CSpriteList hitEffectList;
	CSpriteList EnemyShotList;

	//Enemie Conditions
	enum AllEnemies { WARIOR, NINJAGIRL, NINJAGIRLKUNAI, DOG, NINJAGIRLMELEE, BOSS1 };
				
	enum action_state { CHASSING, PATROL };
	enum animation_state { NONE, STANDLEFT, STANDRIGHT, WALKLEFT, WALKRIGHT, InAir, FALLING, SLIDE, INATTACK, ONHIT, DEAD };
	bool inAttack, inDamage;

	//Combat
	void EnemyChasing(Player& player);
	void EnemyAttack(Player& player);
	void shotsHandler();
	bool hitTestDelay;
	int frameRatesToStop;
	float distToEnemy;

	//Base Stats
	float enemySpeed, attackDistance;
	float EnemyDirection;
	char* throwAttackImgPath;
	float EnemyRotation;
	float meleeDamage, kunaiDamage, fireboltDamage, RangeAttackDamage;

	//Sounds
	CSoundPlayer deadSound;
	
	//patroling
	int action;
	float angle = 0;
	CVector initEnemyPos; 

	//enemie position
	CVector pos;

	//Death
	void enemyDeathHandler();

	//Controller
	void EnemyController(bool patrol);

	//Collisions
	void EnemyCollision(MapGen& mapGen);

	//Animation Handler
	void EnemyAnimation(int old_animation_status);
	void setDefaultStandAnimation();
	void initAnimations();
};