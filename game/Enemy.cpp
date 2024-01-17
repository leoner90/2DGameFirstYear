#include "stdafx.h"
#include "headers/Enemy.h"
#include "headers/Player.h"
#include "headers/mapGen.h"
 
void Enemy::init(int xPos, int yPos, int typeofEnemy,int conInit, int strInit, int dexInit, int intellectInit)
{
	//Local Var. miroring
	enemyType = AllEnemies(typeofEnemy);
	initEnemyPos = { float(xPos), float(yPos) };

	//reset shots and hit effects
	EnemyShotList.delete_all();
	hitEffectList.delete_all();

	//reset Basic
	dead = DeathTimer = inDamage = inAttack = false;
	
	//Enemy attack distance and speed should be before initAnimations();
	attackDistance = 50;
	enemySpeed = 100;

	//Enemy Stats
	con = conInit;
	str = strInit;
	dex = dexInit;
	intellect = intellectInit;
	CurrentEnemyHealth = maxEnemyHealth = 100 * con;
	CurrentEnemyMp = maxEnemyMp = 10 * intellect;
	CurrentEnemyEnergy = maxEnemyEnergy = 10.0f * dex;

	//Damage
	meleeDamage = str * 10;
	kunaiDamage = dex * 20;
	fireboltDamage = intellect * 15;

	//Animations and images
	initAnimations();

	//default Statuses
	enemy.SetStatus(STANDLEFT);
	enemy.SetPosition(initEnemyPos);
	enemy.SetAnimation("standright", 2);
	enemy.SetSpeed(0);
	EnemyDirection = 90;
}

//**************************** UPDATE ****************************
void Enemy::OnUpdate(Uint32 t, Player& player, bool patrol, MapGen& mapGen)
{
	castToPlayer = &player;

	//Return if player is to far Away
	if (Distance(enemy.GetPosition(), castToPlayer->player.GetPosition()) > 1600) {
		enemy.SetY(600); // just in case , hold enemies in the air
		return;
	}
	CurrentTime = t;

	if (CurrentEnemyHealth <= 0) {
		if (enemy.GetCurrentAnimationFrame() == frameRatesToStop && sizeof(EnemyShotList) / sizeof(CSprite) == 0)
		{
			enemyHpBarRect2->Delete();
			enemy.Delete();
			DeathTimer = true;
		}
		shotsHandler();
		EnemyShotList.delete_if(deleted);
		enemy.Update(t);
		return;
	}
	
	localMapVar = &mapGen;
	int old_animation_status = enemy.GetStatus();
	
	EnemyController(patrol);
	EnemyCollision(mapGen);
	EnemyChasing(player);
	EnemyAnimation(old_animation_status);
	EnemyInterface();

	//Hit vfx
	for (CSprite* hitEffect : hitEffectList) hitEffect->Update(t);
	hitEffectList.delete_if(deleted);

	//Enemy Shots
	shotsHandler();
	EnemyShotList.delete_if(deleted);

	pos = enemy.GetPos();
	enemy.Update(t);
}


//**************************** DRAW ****************************
void Enemy::OnDraw(CGraphics* g)
{
 
	if (Distance(enemy.GetPosition(), castToPlayer->player.GetPosition()) > 1600) return;

	for (CSprite* hitEffect : hitEffectList) hitEffect->Draw(g);
	if(enemy.GetY() > 0) enemy.Draw(g);
	if(CurrentEnemyHealth > 0) enemyHpBarRect2->Draw(g);
	
	for (CSprite* shot : EnemyShotList) shot->Draw(g);
}

void Enemy::EnemyChasing(Player& player)
{
	if (inDamage) return;

	distToEnemy = Distance(enemy.GetPosition(), player.player.GetPosition());
	CVector DisplVector = player.player.GetPosition() - enemy.GetPosition();
	float direction = Dot(player.player.GetPosition(), DisplVector);
	if (distToEnemy < 450 && distToEnemy > attackDistance && enemyType != NINJAGIRL && enemyType != NINJAGIRLKUNAI && player.CurrentPlayerHealth > 0)
	{
		if (direction <= 0)
		{
			action = 1;
			enemy.SetVelocity(-enemySpeed, 0);
			enemy.SetStatus(WALKLEFT);
			EnemyDirection = -90;
		}
		else
		{
			action = 1;
			EnemyDirection = 90;
			enemy.SetVelocity(enemySpeed, 0);
			enemy.SetStatus(WALKRIGHT);
		}
	}
	else if (distToEnemy <= attackDistance && direction >= 0     && player.CurrentPlayerHealth > 0)
	{
		EnemyDirection = 90;
		action = 1;
		EnemyAttack(player);
	}
	else if (distToEnemy <= attackDistance &&  direction < 0    && player.CurrentPlayerHealth > 0)
	{
		EnemyDirection = -90;
		action = 1;
		EnemyAttack( player);
	}
 
	else {

		//RETURN TO PATROL POSITION
		if (enemy.GetX() <= initEnemyPos.GetX() - 20 && action == 1)
		{
			EnemyDirection = 90;
			enemy.SetStatus(WALKRIGHT);
			enemy.SetVelocity(200, 0);
		}
		else if (enemy.GetX() >= initEnemyPos.GetX()  && action == 1 )
		{
			EnemyDirection = -90;
			enemy.SetStatus(WALKLEFT);
			enemy.SetVelocity(-200, 0);
		}
		else {
			//START TO PATROL
			action = 2;
		}
	}
}

void Enemy::EnemyController(bool patrol)
{
	if (enemy.GetCurrentAnimationFrame() == frameRatesToStop && inDamage == true) {
		setDefaultStandAnimation();
		inDamage = false;
		//inAttack = false;
	}

	if (inDamage) return;

	if (enemy.GetCurrentAnimationFrame() == frameRatesToStop && inAttack == true) {
		setDefaultStandAnimation();
		inAttack = false;
		hitTestDelay = false;

	}

	//Attack Player
	if (inAttack && !hitTestDelay) {
		if (enemy.HitTest(&castToPlayer->player)) {
			if (castToPlayer->player.GetStatus() != castToPlayer->SLIDE) {
				castToPlayer->PlayerGettingDamage(meleeDamage);
				hitTestDelay = true;
			}
		}
		
	}

	//patroll
	if(action == 2 && patrol )
	{ 
		angle += 0.01;
	
		if (cos(angle) > 0) {
			EnemyDirection = -90;
			if (cos(angle) >= 0.5)
			{
				enemy.SetVelocity(0, 0);
				enemy.SetStatus(STANDLEFT);
			}
			else {
				enemy.SetVelocity(-100, 0);
				enemy.SetStatus(WALKLEFT);
			}
		}
		else
		{
			EnemyDirection = 90;
			if (cos(angle) <= -0.5)
			{
				enemy.SetStatus(STANDRIGHT);
				enemy.SetVelocity(0, 0);
			}
			else {
				enemy.SetVelocity(100, 0);
				enemy.SetStatus(WALKRIGHT);
			}
		}
	}
}

void Enemy::EnemyCollision(MapGen& mapGen)
{
	if (inDamage) return;
	float dist = Distance(castToPlayer->player.GetPosition(), enemy.GetPosition());
	// to avoid drop enemies
	if (dist < 600 && enemy.GetY() <= 0) EnemyGettingDamage(maxEnemyHealth,CurrentTime, castToPlayer->DroppedPoutions);

	if (enemy.HitTest(&castToPlayer->player) && enemy.GetXVelocity() > 0) enemy.SetPosition(pos);
	
	//Bottom Map
	if (enemy.HitTest(localMapVar->bgBottom) || enemy.HitTest(localMapVar->bgBottom2) || enemy.HitTest(localMapVar->bgBottom3))
	{
		if (!inAttack) 
		{
			enemy.SetVelocity(0, 0);
			setDefaultStandAnimation();
		}
		
		enemy.SetPosition(pos + CVector(0, 25));

		// To HIGHT -> set player posY + 25 and checks is it still hitt test , if so ->to higth to walk
		if (enemy.HitTest(localMapVar->bgBottom) || enemy.HitTest(localMapVar->bgBottom2) || enemy.HitTest(localMapVar->bgBottom3))
			enemy.SetPosition(pos);
		//if not to higth set prev. pos Y + 5 , to walk UP
		else enemy.SetPosition(pos + CVector(0, 5));	 
	}
	else
	{
		enemy.SetY(enemy.GetY() - 10);
		if (enemy.HitTest(localMapVar->bgBottom) || enemy.HitTest(localMapVar->bgBottom2) || enemy.HitTest(localMapVar->bgBottom3))
		{
			enemy.SetY(enemy.GetY() + 10);
		}
	}
}

void Enemy::EnemyAnimation(int old_animation_status)
{
	if (enemy.GetStatus() != old_animation_status)
	{
		if (enemy.GetStatus() == WALKLEFT) { enemy.SetAnimation("walkleft", 10);  }
		if (enemy.GetStatus() == WALKRIGHT) { enemy.SetAnimation("walkright", 10);  }
		if (enemy.GetStatus() == STANDRIGHT) { enemy.SetAnimation("standright", 2);  }
		if (enemy.GetStatus() == STANDLEFT) { enemy.SetAnimation("standleft", 2);   }
	}
}

void Enemy::EnemyInterface()
{
	float baseHpBarWidth = 100;
	if (enemyType == BOSS1) baseHpBarWidth = 350;

	enemyHpBarRect2->SetX(enemy.GetX());
	enemyHpBarRect2->SetY(enemy.GetY() + enemy.GetSize().GetY()  - 10);
	float hpBarSize = baseHpBarWidth * (CurrentEnemyHealth / maxEnemyHealth);
	if (hpBarSize < 0) hpBarSize = 0;
	enemyHpBarRect2->SetSize(hpBarSize, 10);
	enemyHpBarRect2->Update(CurrentTime);
}

void Enemy::EnemyAttack(Player& player)
{
	if (enemy.GetStatus() != INATTACK && inAttack == false)
	{
		enemy.SetStatus(INATTACK);
		enemy.SetVelocity(0, 0);
		inAttack = true;

		// NINJAGIRL = remote Attack
		if (enemyType != NINJAGIRL && enemyType != NINJAGIRLKUNAI)
		{
			if (enemyType == DOG) frameRatesToStop = 2;
			else frameRatesToStop = 9;
			if (EnemyDirection == 90) enemy.SetAnimation("katanaAttack", frameRatesToStop);
			else enemy.SetAnimation("katanaLeft", frameRatesToStop);
			
		}

		else {
			hitTestDelay = true; // to avoid melee damage
			if (EnemyDirection == 90) enemy.SetAnimation("throwAttack", 7);
			else if (EnemyDirection == -90) enemy.SetAnimation("throwAttackLeft", 7);
			frameRatesToStop = 7;
			char * imgpath = throwAttackImgPath;
			CSprite* newShot = new CSprite(enemy.GetX(), enemy.GetY(), imgpath, CurrentTime);
			newShot->SetDirection(EnemyDirection);

			int shotRotation = 0;
			if (EnemyDirection == -90) shotRotation = 180;
			newShot->SetRotation(shotRotation);

		
			newShot->SetSpeed(500 + enemy.GetSpeed());
			newShot->Die(1000);
			EnemyShotList.push_back(newShot);
		}	
	}
}


void Enemy::shotsHandler()
{
	for (CSprite* shot : EnemyShotList)
	{
		//if (shot->GetY() < 0 || shot->GetY() > 800 || shot->GetX() < 0 || shot->GetX() > enemy.GetX() + 1000) shot->Delete();
		if (shot->HitTest(localMapVar->bgBottom) || shot->HitTest(localMapVar->bgBottom2) || shot->HitTest(localMapVar->bgBottom3))
			shot->Delete();

		if (shot->HitTest(&castToPlayer->player) && castToPlayer->player.GetStatus() != castToPlayer->SLIDE)
		{
			shot->Delete();
			castToPlayer->PlayerGettingDamage(fireboltDamage);
		}
		shot->Update(CurrentTime);
	}
}

void Enemy::setDefaultStandAnimation()
{
	if (EnemyDirection == 90) 	enemy.SetStatus(STANDRIGHT);
	if (EnemyDirection == -90) enemy.SetStatus(STANDLEFT);
	enemy.SetVelocity(0, 0);
}

void Enemy::initAnimations() 
{
	if (enemyType == WARIOR)
	{
		//walk / iddle animation
		enemy.AddImage("WariorIdleRight.png", "standright", 10, 1, 0, 0, 9, 0, CColor::White());
		enemy.AddImage("WariorRunRight.png", "walkright", 10, 1, 0, 0, 9, 0, CColor::White());
		enemy.AddImage("WariorIdleLeft.png", "standleft", 10, 1, 0, 0, 9, 0, CColor::White());
		enemy.AddImage("WariorRunLeft.png", "walkleft", 10, 1, 9, 0, 0, 0, CColor::White());
		//in damage 
		enemy.AddImage("wariorInDamageRight.png", "inDamageRight", 2, 1, 0, 0, 1, 0, CColor::White());
		enemy.AddImage("wariorInDamageLeft.png", "inDamageLeft", 2, 1, 1, 0, 0, 0, CColor::White());

		//attack
		enemy.AddImage("wariorAttackRight.png", "katanaAttack", 10, 1, 0, 0, 9, 0, CColor::White());
		enemy.AddImage("wariorAttackLeft.png", "katanaLeft", 10, 1, 9, 0, 0, 0, CColor::White());

		//death
		enemy.AddImage("wariorDeadLeft.png", "deadLeft", 10, 1, 9, 0, 0, 0, CColor::White());
		enemy.AddImage("wariorDeadRight.png", "deadRight", 10, 1, 0, 0, 9, 0, CColor::White());
	}

	if (enemyType == BOSS1)
	{
		//walk / iddle animation
		enemy.AddImage("narutoIddleRight.png", "standright", 6, 1, 0, 0, 5, 0, CColor::Black());
		enemy.AddImage("narutoRunRight.png", "walkright", 10, 1, 0, 0, 9, 0, CColor::Black());
		enemy.AddImage("narutoIddleLeft.png", "standleft", 6, 1, 5, 0, 0, 0, CColor::Black());
		enemy.AddImage("narutoRunLeft.png", "walkleft", 10, 1, 0, 0, 9, 0, CColor::Black());

		//in damage 
		enemy.AddImage("narutoDamageRight.png", "inDamageRight", 2, 1, 0, 0, 1, 0, CColor::Black());
		enemy.AddImage("narutoDamageLeft.png", "inDamageLeft", 2, 1, 1, 0, 0, 0, CColor::Black());

		//death
		enemy.AddImage("narutoDeadLeft.png", "deadLeft", 4, 1, 3, 0, 0, 0, CColor::Black());
		enemy.AddImage("narutoDeadRight.png", "deadRight", 4, 1, 0, 0, 3, 0, CColor::Black());

		//melee attack
		enemy.AddImage("narutoAttackRight.png", "katanaAttack", 10, 1, 0, 0, 9, 0, CColor::Black());
		enemy.AddImage("narutoAttackLeft.png", "katanaLeft", 10, 1, 9, 0, 0, 0, CColor::Black());
		enemySpeed = 270;
		attackDistance = 40;

	}

	else if (enemyType == DOG) {
		attackDistance = 70;
		//walk / iddle animation
		enemy.AddImage("dogIdle.png", "standright", 3, 1, 0, 0, 2, 0, CColor::Black());
		enemy.AddImage("dogWalkRight.png", "walkright", 8, 1, 6, 0, 0, 0, CColor::Black());
		enemy.AddImage("dogIdleLeft.png", "standleft", 3, 1, 2, 0, 0, 0, CColor::Black());
		enemy.AddImage("dogWalkLeft.png", "walkleft", 8, 1, 1, 0, 7, 0, CColor::Black());

		//indamage
		enemy.AddImage("dogInDamageLeft.png", "inDamageLeft", 2, 1, 0, 0, 1, 0, CColor::Black());
		enemy.AddImage("dogInDamageRight.png", "inDamageRight", 2, 1, 0, 0, 1, 0, CColor::Black());

		//Melee Attack Animation
		enemy.AddImage("dogAttackRight.png", "katanaAttack", 3, 1, 2, 0, 0, 0, CColor::Black());
		enemy.AddImage("dogAttackLeft.png", "katanaLeft", 3, 1, 0, 0, 2, 0, CColor::Black());

		//Death Animation
		enemy.AddImage("dogDeathLeft.png", "deadLeft", 4, 1, 0, 0, 3, 0, CColor::Black());
		enemy.AddImage("dogDeathRight.png", "deadRight", 4, 1, 3, 0, 0, 0, CColor::Black());
	}
	else if (enemyType == NINJAGIRL || enemyType == NINJAGIRLMELEE || enemyType == NINJAGIRLKUNAI) {
		if (enemyType == NINJAGIRL || enemyType == NINJAGIRLKUNAI) attackDistance = 450;
		//walk / iddle animation
		enemy.AddImage("EnemyGirlIdleRight.png", "standright", 10, 1, 0, 0, 9, 0, CColor::Black());
		enemy.AddImage("EnemyGirlRunRight.png", "walkright", 10, 1, 0, 0, 9, 0, CColor::Black());
		enemy.AddImage("EnemyGirlIdleLeft.png", "standleft", 10, 1, 0, 0, 9, 0, CColor::Black());
		enemy.AddImage("EnemyGirlRunLeft.png", "walkleft", 10, 1, 9, 0, 0, 0, CColor::Black());

		//Melee Attack Animation
		enemy.AddImage("EnemyGirlMelleAttackRight.png", "katanaAttack", 10, 1, 0, 0, 9, 0, CColor::Black());
		enemy.AddImage("EnemyGirlMelleAttackLeft.png", "katanaLeft", 10, 1, 9, 0, 0, 0, CColor::Black());

		//Remote Attack
		enemy.AddImage("EnemyGirlIThrowAttackRight.png", "throwAttack", 8, 1, 0, 0, 7, 0, CColor::Black());
		enemy.AddImage("EnemyGirlIThrowAttackLeft.png", "throwAttackLeft", 8, 1, 7, 0, 0, 0, CColor::Black());

		//Death Animation
		enemy.AddImage("EnemyGirllDeadLeft.png", "deadLeft", 9, 1, 8, 0, 0, 0, CColor::Black());
		enemy.AddImage("EnemyGirllDeadRight.png", "deadRight", 9, 1, 0, 0, 8, 0, CColor::Black());

		//InDamage Animation
		enemy.AddImage("EGirrllInDamageLeft.png", "inDamageLeft", 2, 1, 1, 0, 0, 0, CColor::Black());
		enemy.AddImage("EGirrllInDamageRight.png", "inDamageRight", 2, 1, 0, 0, 1, 0, CColor::Black());
	}

	if (enemyType == NINJAGIRL)
	{
		throwAttackImgPath = "firebolt2.png";
		RangeAttackDamage = fireboltDamage;
	}
	else
	{
		throwAttackImgPath = "kunai.png";
		RangeAttackDamage = kunaiDamage;
	}
}

void Enemy::EnemyGettingDamage(float damageAmount, float t, CSpriteList& DroppedPoutions)
{
	//hit Effect
	CSprite* hitEffect = new CSprite(enemy.GetX(), enemy.GetY(), 0, 0, CurrentTime);
	hitEffect->AddImage("vfxHit.png", "hitEffect", 4, 3, 0, 0, 3, 2, CColor::Black());
	hitEffect->SetAnimation("hitEffect", 8);
	hitEffect->Die(300);
	hitEffectList.push_back(hitEffect);


	enum DropList { HpPoutin, MpPoution, EnergyPoution, kunai };
	CurrentEnemyHealth -= damageAmount;
	inDamage = true;
	enemy.SetStatus(inDamage);
	enemy.SetVelocity(0, 0);

	if (EnemyDirection == 90) enemy.SetAnimation("inDamageRight", 4);
	else enemy.SetAnimation("inDamageLeft", 4);
	frameRatesToStop = 1;

	if (CurrentEnemyHealth <= 0 && enemy.GetStatus() != dead) {
		dead = true;
		frameRatesToStop = 8;
		if (enemyType == DOG || enemyType == BOSS1) frameRatesToStop = 3;
		if (EnemyDirection == -90) enemy.SetAnimation("deadLeft", frameRatesToStop);
		else enemy.SetAnimation("deadRight", frameRatesToStop);
		deadSound.Play("EnemyDeath.wav");
		deadSound.Volume(0.3);
		EnemyInterface();

		int randomDrop = rand() % 4;
		char* imgpath = "";
		if (randomDrop == HpPoutin)  imgpath = "HpBottle.png";
		if (randomDrop == MpPoution)  imgpath = "MpBottle.png";
		if (randomDrop == EnergyPoution)  imgpath = "energyBottle.png";
		if (randomDrop == kunai)  imgpath = "kunaiForBar.png";

		CSprite* newPoution = new CSprite(enemy.GetX(), enemy.GetY() - enemy.GetSize().GetY() + 55, imgpath, CColor::White(), t);
		newPoution->SetSize(30, 30);
		newPoution->SetStatus(randomDrop);

		castToPlayer->PlayerGettingExp(maxEnemyHealth);
		DroppedPoutions.push_back(newPoution);
	}
}

void Enemy::enemyDeathHandler()
{
	if (CurrentEnemyHealth <= 0) {
		if (enemy.GetCurrentAnimationFrame() == frameRatesToStop && sizeof(EnemyShotList) / sizeof(CSprite) == 0)
		{
			enemyHpBarRect2->Delete();
			enemy.Delete();
			DeathTimer = true;
			//delete this;	 
		}
		shotsHandler();
		EnemyShotList.delete_if(deleted);
		enemy.Update(CurrentTime);
		return;
	}
}
 