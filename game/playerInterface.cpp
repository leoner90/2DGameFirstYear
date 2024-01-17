#include "stdafx.h"
#include "headers/playerInterface.h"
#include "headers/Player.h"

CSpriteRect hpBarRect(0, 500, 100, 100, CColor::Red(), CColor::Red(), 1);
CSpriteRect EnergyBarRect(0, 500, 100, 100, CColor::DarkYellow(), CColor::DarkYellow(), 1);
CSpriteRect ManaBarRect(0, 500, 100, 100, CColor::Blue(), CColor::Blue(), 1);

void PlayerInterface::playerEnergyBarUpdate(CSprite& ddEnergyBarRect, float& CurrentEnergy, float maxEnergy, float InitRectSize, float CustomeOffset, float RegenRate, float initWidth, float scrollOffset)
{
	float StaminaInPercent = CurrentEnergy / maxEnergy; // 1%
	float rectSize = InitRectSize * StaminaInPercent;
	if (CurrentEnergy < maxEnergy) CurrentEnergy += RegenRate;
	ddEnergyBarRect.SetSize(rectSize, initWidth);
	ddEnergyBarRect.SetX( CustomeOffset + ((rectSize - InitRectSize) / 2));
}

void PlayerInterface::init(int screenWidth, int screenHeight)
{
	localScreenHeight = screenHeight;
	localScreenWidth = screenWidth;

	// poutions Locations 
	fullHpPos = { 260, 37 };
	fullMpPos = { 345, 37 };
	fullEnergyPos = { 430, 37 };
	kunaiPos = { 515, 40 };

	//delete sprite on init/ restart
	delete controllPanel;
	delete fullHpPoution;
	delete fullMpPoution;
	delete fullEnergyPoution;
	delete emptyHpPoution;
	delete emptyMpPoution;
	delete emptyEnergyPoution;
	delete kunai;
	delete energyBar;

	//create new Sprites
	energyBar = new CSprite();
	controllPanel = new CSprite();
	fullHpPoution = new CSprite();
	fullMpPoution = new CSprite();
	fullEnergyPoution = new CSprite();
	emptyHpPoution = new CSprite();
	emptyMpPoution = new CSprite();
	emptyEnergyPoution = new CSprite();
	kunai = new CSprite();

	SpriteInits();

	//ENERGY BARS
	energyBar->LoadImage("hpBar.png", "hpBar", CColor::Black());
	energyBar->SetImage("hpBar", false);
	energyBar->SetSize(500, 75);

	hpBarRect.SetY(screenHeight - 33);
	hpBarRect.SetSize(410, 45);

	EnergyBarRect.SetY(screenHeight - 60);
	EnergyBarRect.SetSize(165, 25);

	ManaBarRect.SetY(screenHeight - 72);
	ManaBarRect.SetSize(165, 28);
}

void PlayerInterface::OnUpdate(Player& player, float scrollOffset, int screenWidth, int screenHeight)
{
	energyBar->SetPosition(270, screenHeight - 50);
	controllPanel->SetPosition( screenWidth / 2, 40);

	//ENERGY
	playerEnergyBarUpdate(hpBarRect, player.CurrentPlayerHealth, player.maxPlayerHealth, 410, 302.5, 0.f, 45, scrollOffset);
	playerEnergyBarUpdate(EnergyBarRect, player.CurrentPlayerEnergy, player.maxPlayerEnergy, 165, 210, 0.01f * player.dex, 25, scrollOffset);
	playerEnergyBarUpdate(ManaBarRect, player.CurrentPlayerMp, player.maxPlayerMp, 165, 170, 0.f, 25, scrollOffset);
}

void PlayerInterface::OnDraw(Player& player, CGraphics* g, float scrollOffset, int screenWidth, int screenHeight)
{
	OnUpdate( player, scrollOffset,screenWidth,  screenHeight);
	hpBarRect.Draw(g);
	EnergyBarRect.Draw(g);
	ManaBarRect.Draw(g);
	energyBar->Draw(g);
	controllPanel->Draw(g);

	//Poutions
	if (player.HpPoutionCount > 0) fullHpPoution->Draw(g);
	else  emptyHpPoution->Draw(g);

	if (player.MpPoutionCount > 0) fullMpPoution->Draw(g);
	else emptyMpPoution->Draw(g);

	if (player.EnergyPoutionCount > 0) fullEnergyPoution->Draw(g);
	else emptyEnergyPoution->Draw(g);
	kunai->Draw(g);

	*g << font(22) << color(CColor::White()) << xy(fullHpPos.GetX() - 10, fullHpPos.GetY() - 15) << "x" << player.HpPoutionCount;
	*g << font(22) << color(CColor::White()) << xy(fullMpPos.GetX() - 10, fullMpPos.GetY() - 15) << "x" << player.MpPoutionCount;
	*g << font(22) << color(CColor::White()) << xy(fullEnergyPos.GetX() - 10, fullEnergyPos.GetY() - 15) << "x" << player.EnergyPoutionCount;
	*g << font(22) << color(CColor::White()) << xy(kunaiPos.GetX()  , kunaiPos.GetY() - 25) << "x" << player.ammoCount;
}


void PlayerInterface::SpriteInits()
{
	//Control Panel
	controllPanel->LoadImage("skillBarTest.png", "skillBar");
	controllPanel->SetImage("skillBar", false);
	controllPanel->SetSize(localScreenWidth, 120);

	//inventory
	emptyHpPoution->LoadImage("emptyBottle.png", "emptyBottle", CColor::Black());
	emptyHpPoution->SetImage("emptyBottle", false);
	emptyHpPoution->SetSize(60, 60);
	emptyHpPoution->SetPosition(fullHpPos);

	emptyMpPoution->LoadImage("emptyBottle.png", "emptyBottle", CColor::Black());
	emptyMpPoution->SetImage("emptyBottle", false);
	emptyMpPoution->SetSize(60, 60);
	emptyMpPoution->SetPosition(fullMpPos);

	emptyEnergyPoution->LoadImage("emptyBottle.png", "emptyBottle", CColor::Black());
	emptyEnergyPoution->SetImage("emptyBottle", false);
	emptyEnergyPoution->SetSize(60, 60);
	emptyEnergyPoution->SetPosition(fullEnergyPos);

	fullHpPoution->LoadImage("HpBottle.png", "HpBottle", CColor::Black());
	fullHpPoution->SetImage("HpBottle", false);
	fullHpPoution->SetSize(60, 60);
	fullHpPoution->SetPosition(fullHpPos);

	fullMpPoution->LoadImage("MpBottle.png", "MpBottle", CColor::Black());
	fullMpPoution->SetImage("MpBottle", false);
	fullMpPoution->SetSize(60, 60);
	fullMpPoution->SetPosition(fullMpPos);

	fullEnergyPoution->LoadImage("energyBottle.png", "energyBottle", CColor::Black());
	fullEnergyPoution->SetImage("energyBottle", false);
	fullEnergyPoution->SetSize(60, 60);
	fullEnergyPoution->SetPosition(fullEnergyPos);

	kunai->LoadImage("kunaiForBar.png", "kunai");
	kunai->SetImage("kunai", true);
	kunai->SetSize(60, 40);
	kunai->SetRotation(0);
	kunai->SetPosition(kunaiPos);
}
