#include "stdafx.h"
#include "headers/playerStats.h"
#include "headers/Player.h"

void playerStats::init(float screenHeight, float screenWidth)
{
	delete charStatsScreen;
	delete levelUpBtn;
	charStatsScreen = new CSprite();
	levelUpBtn = new CSprite();

	charStatsScreen->LoadImage("CharStats.jpg");
	charStatsScreen->SetImage("CharStats.jpg");
	charStatsScreen->SetSize(screenWidth, screenHeight);
	charStatsScreen->SetPosition(screenWidth / 2, screenHeight / 2);

	levelUpBtn->LoadImage("addSkillBtn.png");
	levelUpBtn->SetImage("addSkillBtn.png");
	levelUpBtn->SetPosition(screenWidth / 3, screenHeight / 2);
}

void playerStats::OnUpdate()
{

}

void playerStats::OnDraw(CGraphics* g, float screenHeight, int& str, int& dex, int& con, int& intellect, int& Skillpoint, Player &player)
{
	charStatsScreen->Draw(g);

	if (charStatsScreen->GetY() > screenHeight / 2 + 40) charStatsScreen->SetY(charStatsScreen->GetY() - 50);
	else
	{
		strCoor = screenHeight - (screenHeight * 0.36);
		dexCoor = screenHeight - (screenHeight * 0.45);
		conCoor = screenHeight - (screenHeight * 0.54);
		intCoor = screenHeight - (screenHeight * 0.63);
		 

		StatsPosition(g, player.str, strCoor - 10, strCoor, player.Skillpoint,"STR");
		StatsPosition(g, player.dex, dexCoor - 10, dexCoor, player.Skillpoint, "DEX");
		StatsPosition(g, player.con, conCoor - 15, conCoor, player.Skillpoint, "CON");
		StatsPosition(g, player.intellect, intCoor - 15, intCoor, player.Skillpoint, "INT");

		//DAMAGE DRAW
		*g << font("AFontPTSerif.ttf", 16)  << color(CColor::White()) << xy(levelUpBtn->GetX() +450 , strCoor) << "MELEE DAMAGE:  "<< player.meleeDamage;
		
		*g << font("AFontPTSerif.ttf", 16) << color(CColor::White()) << xy(levelUpBtn->GetX() + 450, dexCoor+30) << "KUNAI DAMAGE:  " << player.kunaiDamage;
		*g << font("AFontPTSerif.ttf", 16) << color(CColor::White()) << xy(levelUpBtn->GetX() + 450, dexCoor + 10) << "MAX. STAMINA  " << player.maxPlayerEnergy;
		*g << font("AFontPTSerif.ttf", 16) << color(CColor::White()) << xy(levelUpBtn->GetX() + 450, dexCoor - 10) << "CURRENT STAMINA  " << int(player.CurrentPlayerEnergy);
		*g << font("AFontPTSerif.ttf", 16) << color(CColor::White()) << xy(levelUpBtn->GetX() + 450, dexCoor - 30) << "STAMINA REGEN PER SEC  " <<  0.01f * player.dex;


		*g << font("AFontPTSerif.ttf", 16) << color(CColor::White()) << xy(levelUpBtn->GetX() + 450, intCoor) << "FIREBOLT DAMAGE:  " << player.fireboltDamage;
		*g << font("AFontPTSerif.ttf", 16) << color(CColor::White()) << xy(levelUpBtn->GetX() + 450, intCoor - 20) << "MAX. MANA POINTS:  " << player.maxPlayerMp;
		*g << font("AFontPTSerif.ttf", 16) << color(CColor::White()) << xy(levelUpBtn->GetX() + 450, intCoor - 40) << "CURRENT MANA POINTS:  " << player.CurrentPlayerMp;

		*g << font("AFontPTSerif.ttf", 16) << color(CColor::White()) << xy(levelUpBtn->GetX() + 450, conCoor - 10) << "MAX. HEALTH:  " << player.maxPlayerHealth;
		*g << font("AFontPTSerif.ttf", 16) << color(CColor::White()) << xy(levelUpBtn->GetX() + 450, conCoor -30) << "CURRENT HEALTH:  " << player.CurrentPlayerHealth;

		//SKILL POINTS AND NEXT LVL
		*g << font("AFontPTSerif.ttf", 22) << color(CColor::White()) << xy(levelUpBtn->GetX() - 150, 160) << "LEVEL " << player.playerLVL;
		*g << font("AFontPTSerif.ttf", 22) << color(CColor::White()) << xy(levelUpBtn->GetX() + 350, 160) << "CURRENT EXP " << player.currentExp;
		

		if(player.maxLvl)
			*g << font("AFontPTSerif.ttf", 22) << color(CColor::White()) << xy(levelUpBtn->GetX() + 350, 120) << " MAX LVL REACHED ";
		else
			*g << font("AFontPTSerif.ttf", 22) << color(CColor::White()) << xy(levelUpBtn->GetX() + 350, 120) << "NEXT LVL EXP " << player.tillNextLvlExp;
		
		*g << font("AFontPTSerif.ttf", 22) << color(CColor::White()) << xy(levelUpBtn->GetX() - 150, 120) << "SKILLPOINTS " << player.Skillpoint;
	}
}


void playerStats::StatsPosition(CGraphics* g, int& stat, float textY, float btnY,int & Skillpoint, char* statName)
{
 
	*g << font(42) << color(CColor::White()) << xy(levelUpBtn->GetX() - 180, textY) << statName;
	*g << font(33) << color(CColor::White()) << xy(levelUpBtn->GetX() - 100, textY) << stat;

	if (Skillpoint > 0) {
		levelUpBtn->SetPosition(levelUpBtn->GetX(), btnY);
		levelUpBtn->Draw(g);
	}
}
 



void playerStats::LevelUpBtnLogic(int& stat, float btnY, int& Skillpoint, CVector mouseCoordinats )
{
	if (Skillpoint > 0 &&
		mouseCoordinats.GetX() <= levelUpBtn->GetX() + 20 &&
		mouseCoordinats.GetX() >= levelUpBtn->GetX() - 20 &&
		mouseCoordinats.GetY() <= btnY + 20 &&
		mouseCoordinats.GetY() >= btnY - 20)
	{
		stat += 1;
		Skillpoint -= 1;
	}
}



void playerStats::OnLButtonUp(Player& player,int& Skillpoint, CVector mouseCoordinats)
{
	LevelUpBtnLogic(player.str, strCoor, Skillpoint, mouseCoordinats);
	LevelUpBtnLogic(player.dex, dexCoor, Skillpoint, mouseCoordinats);
	LevelUpBtnLogic(player.con, conCoor, Skillpoint, mouseCoordinats);
	LevelUpBtnLogic(player.intellect, intCoor, Skillpoint, mouseCoordinats);
	
	//STATS UPDATE  (TO MOVE ON ACTUAL LVL UPDATE;
	player.maxPlayerHealth = 100 * player.con;
	player.maxPlayerMp = 10 * player.intellect;
	player.maxPlayerEnergy = 10.0f * player.dex;
	player.meleeDamage = player.str * 10;
	player.kunaiDamage = player.dex * 20;
	player.fireboltDamage = player.intellect * 15;

}

void playerStats::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{

}