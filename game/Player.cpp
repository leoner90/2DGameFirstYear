#include "stdafx.h"
#include "headers/Player.h"
#include "headers/mapGen.h"
#include "headers/enemy.h"
#include "headers/playerInterface.h"
#include <iostream>
void Player::init(float screenWidth)
{
	//Stats
	con = str = dex = intellect = 10;
	CurrentPlayerHealth = maxPlayerHealth = 11100 * con;
	CurrentPlayerMp = maxPlayerMp = 1110 * intellect;
	CurrentPlayerEnergy = maxPlayerEnergy = 10.0f * dex;
	
	//Damage 
	meleeDamage = str * 10;
	kunaiDamage = dex * 20;
	fireboltDamage = intellect * 1115;

	//Combat
	hitEffectList.delete_all();
	DroppedPoutions.delete_all();
	shotList.delete_all();
	inDamage = inAtack = isLvlup = false;

	//init Inventory 
	MpPoutionCount = EnergyPoutionCount = 3;
	HpPoutionCount = ammoCount = 5;

	//Leveling 
	playerLVL = 0;
	tillNextLvlExp = levelsRange[playerLVL];
	Skillpoint = 0;
	currentExp = 0;
	msgTimer = 0;
	maxLvl = false;
	lvlTextMover = 0;

	//Walk Limits
	lowerlimit = 0;
	upperlimit = 200;
	leftlimit = 30;

	//Other 
	ScreenWidth = screenWidth;
	TpToLastBoss = false;

	//********* SPRITES **********
	delete playerSprite;
	playerSprite = new CSprite();
	//movement
	playerSprite->AddImage("playerMainIdle.png", "standright", 10, 1, 0, 0, 10, 0, CColor::Black());
	playerSprite->AddImage("playerRunRight.png", "walkright", 10, 1, 0, 0, 9, 0);
	playerSprite->AddImage("playerMainIdleLeft.png", "standleft", 10, 1, 0, 0, 9, 0, CColor::Black());
	playerSprite->AddImage("playerRunLeft.png", "walkleft", 10, 1, 0, 0, 9, 0, CColor::Black());
	playerSprite->SetAnimation("standright", 8);

	//actions
	playerSprite->AddImage("playerMainJump.png", "jump", 10, 1, 0, 0, 9, 0, CColor::Black());
	playerSprite->AddImage("playerJumpLeft.png", "playerJumpLeft", 10, 1, 0, 0, 9, 0, CColor::Black());
	playerSprite->AddImage("playerSlide.png", "slide", 2, 1, 0, 0, 1, 0, CColor::Black());
	playerSprite->AddImage("slideleft.png", "slideleft", 2, 1, 0, 0, 1, 0, CColor::Black());

	//attack
	playerSprite->AddImage("katana.png", "katanaAttack", 10, 1, 0, 0, 9, 0, CColor::Black());
	playerSprite->AddImage("katanaLeft.png", "katanaLeft", 10, 1, 0, 0, 9, 0, CColor::Black());
	playerSprite->AddImage("throwAttack.png", "throwAttack", 10, 1, 0, 0, 10, 0, CColor::Black());
	playerSprite->AddImage("throwAttackLeft.png", "throwAttackLeft", 10, 1, 0, 0, 10, 0, CColor::Black());

	//status
	playerSprite->AddImage("inDamageRight.png", "inDamageRight", 2, 1, 0, 0, 1, 0, CColor::Black());
	playerSprite->AddImage("inDamageLeft.png", "inDamageLeft", 2, 1, 1, 0, 0, 0, CColor::Black());
	playerSprite->AddImage("throwAttackLeft.png", "inDamageLeft", 10, 1, 0, 0, 6, 0, CColor::Black());
	playerSprite->AddImage("pickUpRight.png", "pickUpRight", 6, 1, 0, 0, 5, 0, CColor::Black());
	playerSprite->AddImage("pickUpLeft.png", "pickUpLeft", 6, 1, 5, 0, 0, 0, CColor::Black());

	//death
	playerSprite->AddImage("deadRight.png", "deadRight", 10, 1, 0, 0, 10, 0, CColor::Black());
	playerSprite->AddImage("deadLeft.png", "deadLeft", 10, 1, 9, 0, 0, 0, CColor::Black());

	//Init Positions
	playerSprite->SetPosition(60, 330);
	playerSprite->SetSpeed(0);
	playerSprite->SetRotation(0);
	PlayerDirection = 90;

	playerSprite->SetStatus(STANDRIGHT);
}

 
void Player::OnUpdate(long t, std::vector<Enemy*> AllEnemies, bool SDLK_d, bool SDLK_a, MapGen& mapgen, bool& gameOver)
{
	//mirroring to use in diferenc methods without passing parameters every time
	AllEnemiesLocal = AllEnemies;
	CurrentTime = t;
	localMapVar = &mapgen;

	//for Traps
	if (playerSprite->GetY() < 70 && playerSprite->GetStatus() != DEAD) {
		playerSprite->SetVelocity(0, 0);
		if (PlayerDirection == -90) playerSprite->SetAnimation("deadLeft", 6);
		else playerSprite->SetAnimation("deadRight", 6);
		playerSprite->SetStatus(DEAD);
		deadSound.Play("playerDeath.wav");
		CurrentPlayerHealth = 0;
	}

	//DEAD ANIMATION AND DELAY FOR GAMEOVER
	if (CurrentPlayerHealth <= 0) {
		if (playerSprite->GetCurrentAnimationFrame() == 8 ) gameOver = true;
		playerSprite->Update(t);
		footSteps.Stop();
		return;
	}

	//Player Controler
	playerControl(SDLK_d, SDLK_a, mapgen);

	//Tp To Last Boss
	if (playerSprite->GetX() > 21950 && playerSprite->GetX() < 24000) {
		playerSprite->SetPosition(24753, 500);
		leftlimit = 24667;
	}

	//Poutions Drop
	for (CSprite* poutions : DroppedPoutions) poutions->Update(t);
	DroppedPoutions.delete_if(deleted);

	//Shots
	shotHandler();
	shotList.delete_if(deleted);

	//Hit Effect animation
	for (CSprite* hitEffect : hitEffectList) hitEffect->Update(t);
	hitEffectList.delete_if(deleted);

	pos = playerSprite->GetPos();
	playerSprite->Update(t);
}

void Player::OnDraw(CGraphics* g)
{
	if (playerSprite->GetStatus() == DEAD)
	{
		playerSprite->Draw(g);
		footSteps.Stop();
		return;
	}

	for (CSprite* shot : shotList) shot->Draw(g);
	for (CSprite* hitEffect : hitEffectList) hitEffect->Draw(g);
	for (CSprite* poutions : DroppedPoutions) poutions->Draw(g);
	playerSprite->Draw(g);
	DrawExpInfo(g);
}


//********************* PLAYER SHOTS HANDLER *********************
void Player::shotHandler()
{
	for (CSprite* shot : shotList)
	{
		shot->Update(CurrentTime);

		if (shot->HitTest(localMapVar->bgBottom) || shot->HitTest(localMapVar->bgBottom2) || shot->HitTest(localMapVar->bgBottom3))
			shot->Delete();

		for (auto enemy : AllEnemiesLocal) 
		{
			if (enemy->dead) continue;
			if (shot->HitTest(enemy->enemySprite))
			{
				shot->Delete();
				float damage = 0;
				if (shot->GetStatus() == kunai) damage = kunaiDamage;
				if (shot->GetStatus() == firebolt) damage = fireboltDamage;
				enemy->EnemyGettingDamage(damage, CurrentTime, DroppedPoutions);
		
			}
		}
	}
}

void Player::setDefaultStandAnimation()
{
	if (PlayerDirection == 90) playerSprite->SetStatus(STANDRIGHT) ;
	if (PlayerDirection == -90) playerSprite->SetStatus(STANDLEFT);
	playerSprite->SetVelocity(0, 0);
}

void Player::playerControl( bool SDLK_d, bool SDLK_a , MapGen& mapgen)
{
	int old_animation_status = playerSprite->GetStatus();
	if (playerSprite->GetCurrentAnimationFrame() == 1 && inDamage == true)
	{
		setDefaultStandAnimation();
		inDamage = false;
	}
	if (inDamage) return;
	playerCollision(playerSprite->GetXVelocity(), mapgen);

	//attack cancel
	if (playerSprite->GetCurrentAnimationFrame() == numberOfFramesToStop && (playerSprite->GetStatus() == ATTACK || playerSprite->GetStatus() == PICKUP)) {
		setDefaultStandAnimation();
		inAtack = false;
	}

	if (playerSprite->GetStatus() == PICKUP) return;
 

	if (inAtack) {
		for (auto enemy : AllEnemiesLocal)
		{
			if (enemy->dead) return;
			if (enemy->enemySprite->HitTest(playerSprite))
			{
				enemy->EnemyGettingDamage(meleeDamage, CurrentTime, DroppedPoutions);
				inAtack = false;
			}
		}
	}

	//jump
	if (playerSprite->GetY() >= jumpHeight && playerSprite->GetStatus() == InAir)
	{
		playerSprite->SetStatus(FALLING);
		playerSprite->SetVelocity(playerSprite->GetVelocity() + CVector(0, -850));
	}

	if (playerSprite->GetStatus() == SLIDE &&  (slideInitPos + 150 <= playerSprite->GetX() || slideInitPos - 150 >= playerSprite->GetX()))
	{
		setDefaultStandAnimation();
	}

	if (playerSprite->GetStatus() != PICKUP && playerSprite->GetStatus() != InAir && playerSprite->GetStatus() != FALLING && playerSprite->GetStatus() != SLIDE && playerSprite->GetStatus() != ATTACK)
	{
		playerSprite->SetMotion(0, 0);

		if (playerSprite->GetStatus() == WALKLEFT) { playerSprite->SetStatus(STANDLEFT); PlayerDirection = -90;}
		if (playerSprite->GetStatus() == WALKRIGHT) { playerSprite->SetStatus(STANDRIGHT); PlayerDirection = 90;}

		// setting the speeds and animation status according to the keyboard controls
		if (SDLK_d) { playerSprite->SetXVelocity(hwalkSpeed); playerSprite->SetStatus(WALKRIGHT); PlayerDirection = 90; }
		if (SDLK_a) { playerSprite->SetXVelocity(-hwalkSpeed); playerSprite->SetStatus(WALKLEFT); PlayerDirection = -90; }

		//ANIMATION
		if (playerSprite->GetStatus() != old_animation_status)
		{
			footSteps.Stop();
			if (playerSprite->GetStatus() == WALKLEFT)
			{
				footSteps.Play("footsteps.wav",99); 
				footSteps.Volume(0.4);
				playerSprite->SetAnimation("walkleft", 10);
				
			}
			if (playerSprite->GetStatus() == WALKRIGHT)
			{
				footSteps.Play("footsteps.wav", 99);
				footSteps.Volume(0.4);
				playerSprite->SetAnimation("walkright", 10);
			}
			if (playerSprite->GetStatus() == STANDRIGHT) playerSprite->SetAnimation("standright", 6);
			if (playerSprite->GetStatus() == STANDLEFT) playerSprite->SetAnimation("standleft", 6);
		}
	}

}
 

void Player::playerCollision(float velocity, MapGen& mapGen)
{
	//******* START/END OF THE MAP COLLISION ************
	if (playerSprite->GetX() < leftlimit) {
		playerSprite->SetPosition(pos + CVector(10, 0));
	}

	if (playerSprite->GetX() > mapGen.rightScreenLimit + 1214 )
	{
		playerSprite->SetPosition(pos - CVector(10, 0));
	}

	//******* ENEMIES COLLISION ************
	for (auto enemy : AllEnemiesLocal)
	{
		if (playerSprite->HitTest(enemy->enemySprite) && !enemy->dead) {
			if(playerSprite->GetStatus() == SLIDE) setDefaultStandAnimation();
			// if jump over head + move 5px away to avoid stack in the enemie sprite
		 
			CVector DisplVector = playerSprite->GetPosition() - enemy->enemySprite->GetPosition();
			float direction = Dot(playerSprite->GetPosition(), DisplVector);

			
			CVector maring = CVector(0, 0);;
			playerSprite->SetPosition(playerSprite->GetPosition() - CVector(0, 5));

			//if (playerSprite->HitTest(localMapVar->bgBottom) || playerSprite->HitTest(localMapVar->bgBottom2) || playerSprite->HitTest(localMapVar->bgBottom3))
		   // int x = 1;

			//else maring =    CVector(10, 0);

			playerSprite->SetPosition(playerSprite->GetPosition() + CVector(0, 5));
 
			if(direction > 0) playerSprite->SetPosition(pos + maring);
			else playerSprite->SetPosition(pos -maring);
		}
	}


	//******* BOTTOM IMG COLLISION ************
	
	CSprite* BottomHitTest; 
	BottomHitTest = new CSprite();
	float cooficent = playerSprite->GetX() / 2759;
	if (cooficent <= 1 || (cooficent > 3 && cooficent <= 4) || (cooficent> 6 && cooficent <= 7)) BottomHitTest = localMapVar->bgBottom;
	else if (cooficent <= 2 || (cooficent > 4 && cooficent <= 5) || (cooficent > 7 && cooficent <= 8)) BottomHitTest = localMapVar->bgBottom2;
	else  BottomHitTest = localMapVar->bgBottom3;

	if (playerSprite->HitTest(BottomHitTest))
	{
		std::cout << "bgBottomHit Test";
		//if in attack do not set default animation
		if (playerSprite->GetStatus() != ATTACK) setDefaultStandAnimation();

		playerSprite->SetPosition(playerSprite->GetPosition() + CVector(15, 45));

		// set player posY + 25 and checks is it still hitt test , if so ->to higth to walk
		if (playerSprite->HitTest(BottomHitTest))
			playerSprite->SetPosition(pos);
		//if not to higth set prev. pos Y + 5 , to walk UP

		else playerSprite->SetPosition(pos + CVector(0, 5));
	}
	else
	{
		//do not check if in Actions
		if ( playerSprite->GetStatus() == InAir || playerSprite->GetStatus() == ATTACK || playerSprite->GetStatus() == SLIDE || playerSprite->GetStatus() == PICKUP)
			return;
		
		//checks if player Y pos -5 no hit test with bottom img -> player dropes down by 5px/frame,if hit test stays on same spot
		playerSprite->SetPosition(playerSprite->GetPosition() - CVector(0, 5));
		if (playerSprite->HitTest(BottomHitTest))
		{
			std::cout << "bggoingDown Test";
			playerSprite->SetPosition(playerSprite->GetPosition() + CVector(0, 5));
		}
	}
}

void Player::playerPickUpItem()
{
	for (CSprite* poution : DroppedPoutions)
	{
		if (playerSprite->HitTest(poution))
		{
			pickUpSound.Play("pickup.wav");
			pickUpSound.Volume(0.4);

			poution->Delete();
			switch (poution->GetState()) 
			{
				case HpPoutin: HpPoutionCount += 1; break;
				case MpPoution: MpPoutionCount += 1; break;
				case EnergyPoution: EnergyPoutionCount += 1; break;
				case kunai: ammoCount += 1; break;
				default: break;
			}
		}
	}
	playerSprite->SetStatus(PICKUP);
	playerSprite->SetVelocity(0, 0);
	if (PlayerDirection == -90) playerSprite->SetAnimation("pickUpLeft", 10);
	else playerSprite->SetAnimation("pickUpRight", 10);
	numberOfFramesToStop = 4;
}


void Player::PlayerGettingDamage(float damageAmount)
{
	if (CurrentPlayerHealth <= 0) return;
	
	CSprite* hitEffect = new CSprite(playerSprite->GetX(), playerSprite->GetY(), 0, 0, CurrentTime);
	hitEffect->AddImage("vfxHit.png", "hitEffect", 4, 3, 0, 0, 3, 2, CColor::Black());
	hitEffect->SetAnimation("hitEffect", 8);
	hitEffect->Die(300);
	hitEffectList.push_back(hitEffect);
	
	if (CurrentPlayerHealth > 0) hitSound.Play("hitSound.wav");
	hitSound.Volume(0.3);
	playerSprite->SetStatus(INDAMAGE);
	playerSprite->SetVelocity(0, 0);

	inDamage = true;
	CurrentPlayerHealth -= damageAmount;

	if (CurrentPlayerHealth <= 0) 
	{
		if (PlayerDirection == -90) playerSprite->SetAnimation("deadLeft", 6);
		else playerSprite->SetAnimation("deadRight", 6);
		playerSprite->SetStatus(DEAD);
		deadSound.Play("playerDeath.wav");
	}
	else {
		if (PlayerDirection == -90) playerSprite->SetAnimation("inDamageLeft", 4);
		else playerSprite->SetAnimation("inDamageRight", 4);
	}
}

/********************************* LEVELING **********************************************/
void Player::PlayerGettingExp(float expAmount)
{
	if (maxLvl) return;
	expGained += expAmount;
	currentExp += expAmount;
	lvlTextMover = 0;
	if ( currentExp >= levelsRange[playerLVL])
	{
		playerLVL += 1;

		if (playerLVL >= sizeof(levelsRange) / sizeof(float)) 
		{
			maxLvl = true;
		}
		tillNextLvlExp = levelsRange[playerLVL];
		levelUp.Play("levelUp.wav");
		levelUp.Volume(0.2);
		Skillpoint += 5;
		isLvlup = true;

		//if more then one lvl per enemy
		if (maxLvl) return;
		if (currentExp > levelsRange[playerLVL + 1]) PlayerGettingExp(0);
	}
}

//Draw Exp information
void Player::DrawExpInfo(CGraphics* g)
{
	if (expGained > 0)
	{
		lvlTextMover += 0.5;
		if (lvlTextMover < 40)
		{
			if (isLvlup) *g << font(28) << color(CColor::Red()) << xy(playerSprite->GetX(), playerSprite->GetY() + 65  + lvlTextMover) << "LEVEL UP";
			*g << font(22) << color(CColor::LightBlue()) << xy(playerSprite->GetX() - 20, playerSprite->GetY() + 30 + lvlTextMover) << "+ " << expGained << " exp ";
		}
		else 
		{
			expGained = 0;
			lvlTextMover = 0;
			isLvlup = false;
		}
	}
	 *g << font(22) << color(CColor::LightBlue()) << xy(playerSprite->GetX() - 20, playerSprite->GetY() + 30 + lvlTextMover) << playerSprite->GetX() ;
}



/********************************* KEYBOARD **********************************************/

void Player::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode, bool IsMenuMode, PlayerInterface& playerInterface)
{
	if (CurrentPlayerHealth <= 0) return;
	if (inDamage) return;
	if (IsMenuMode) return;
 
	//checks that player isn't occupied
	bool status = (playerSprite->GetStatus() != InAir && playerSprite->GetStatus() != FALLING && playerSprite->GetStatus() != SLIDE && playerSprite->GetStatus() != ATTACK);

	if (playerSprite->GetStatus() == PICKUP) return;

	//player JUMP
	if (CurrentPlayerEnergy >= 15 && sym == SDLK_SPACE && status)
	{
		if (IsMenuMode) return;
		inAtack = false;
		playerSprite->SetVelocity(playerSprite->GetVelocity() + CVector(0, 450));
		if (playerSprite->GetStatus() == STANDLEFT || playerSprite->GetStatus() == WALKLEFT) playerSprite->SetAnimation("playerJumpLeft", 15);
		else playerSprite->SetAnimation("jump", 15);
		footSteps.Stop();
		playerSprite->SetStatus(InAir);
		CurrentPlayerEnergy -= 15.0f;
		jumpHeight = playerSprite->GetY() + 150;
	}

	//player SLIDE
	if (CurrentPlayerEnergy >= 10 && sym == SDLK_f && status)
	{
		slideInitPos = playerSprite->GetX();
		footSteps.Stop();
		slideSound.Play("slide.wav");
		slideSound.Volume(0.2);
		playerSprite->SetStatus(SLIDE);
		if (PlayerDirection == 90) { playerSprite->SetAnimation("slide", 9); playerSprite->SetVelocity(300, 0); }
		if (PlayerDirection == -90) { playerSprite->SetAnimation("slideleft", 9); playerSprite->SetVelocity(-300, 0); }
		CurrentPlayerEnergy -= 10.0f;
	}

	//player KATANA ATTACK
	if (CurrentPlayerEnergy >= 20 && sym == SDLK_r && status)
	{
		footSteps.Stop();
		inAtack = true;
		playerSprite->SetVelocity(0, 0);
		playerSprite->SetStatus(ATTACK);

		if (PlayerDirection == -90) playerSprite->SetAnimation("katanaLeft", 14);
		else playerSprite->SetAnimation("katanaAttack", 14);

		CurrentPlayerEnergy -= 20.0f;
		numberOfFramesToStop = 9;
		katanaSound.Play("katana.wav");
	}

	//KUNAI and FIREBOLT   
	if (status && (sym == SDLK_q || sym == SDLK_e))
	{
		if ((sym == SDLK_e && (CurrentPlayerEnergy <= 15 || ammoCount <= 0)) || (sym == SDLK_q && CurrentPlayerMp <= 15))
		{
			noAmmo.Play("noAmmo.wav");
			noAmmo.Volume(0.1);
			return;
		}
		
		if (PlayerDirection == -90) playerSprite->SetAnimation("throwAttackLeft", 6);
		else playerSprite->SetAnimation("throwAttack", 6);

		if (sym == SDLK_e) { ammoCount -= 1; CurrentPlayerEnergy -= 15; }
		else CurrentPlayerMp -= 15;

		playerSprite->SetVelocity(0, 0);
		playerSprite->SetStatus(ATTACK);

		char* imgLink = "firebolt2.png";
		if (sym == SDLK_e)imgLink = "kunai.png";
		
		CSprite* newShot = new CSprite(playerSprite->GetX(), playerSprite->GetY(), imgLink, CurrentTime);
		if (sym == SDLK_e) remoteAttack.Play("kunai.wav");
		else  remoteAttack.Play("firebolt.wav");
		remoteAttack.Volume(0.5);

		newShot->SetDirection(PlayerDirection);
		float shotRotation = 0;
		if (PlayerDirection == -90) shotRotation = 180;

		newShot->SetRotation(shotRotation);
		newShot->SetSpeed(600);
		if (sym == SDLK_e) newShot->SetState(kunai);
		else newShot->SetState(firebolt);

		newShot->Die(900);
		shotList.push_back(newShot);
		numberOfFramesToStop = 3;
	}

	//POUTION USE 
	if 
		(
			(sym == SDLK_1 && HpPoutionCount > 0 && CurrentPlayerHealth < maxPlayerHealth) ||
			(sym == SDLK_2 && MpPoutionCount > 0 && CurrentPlayerMp < maxPlayerMp) ||
			(sym == SDLK_3 && EnergyPoutionCount > 0 && CurrentPlayerEnergy < maxPlayerEnergy)
		) 
	{
		poutionDrink.Stop();
		poutionDrink.Play("poutionDrink.wav");
		poutionDrink.Volume(2);

		switch(sym) {
		case  SDLK_1: 
			HpPoutionCount -= 1;
			if (CurrentPlayerHealth + (maxPlayerHealth / 100 * 40) < maxPlayerHealth) CurrentPlayerHealth += maxPlayerHealth / 100 * 40;
			else CurrentPlayerHealth = maxPlayerHealth;
			break;
		case  SDLK_2:
			MpPoutionCount -= 1;
			if (CurrentPlayerMp + (maxPlayerMp / 100 * 50) < maxPlayerMp) CurrentPlayerMp += maxPlayerMp / 100 * 50;
			else CurrentPlayerMp = maxPlayerMp;
			break;
		case  SDLK_3:
			EnergyPoutionCount -= 1;
			if (CurrentPlayerEnergy + (maxPlayerEnergy / 100 * 50) < maxPlayerEnergy) CurrentPlayerEnergy += maxPlayerEnergy / 100 * 50;
			else CurrentPlayerEnergy = maxPlayerEnergy;
			break;
		}
	}
	else if (sym == SDLK_1 && HpPoutionCount == 0)
	{
		poutionDrink.Play("noAmmo.wav");
		poutionDrink.Volume(0.1);
	}


	//PICKUP ITEM
	if (sym == SDLK_z && status) 
	{
		playerPickUpItem();
	}
}