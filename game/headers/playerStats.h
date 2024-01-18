#pragma once
#include "headers/Player.h"
class Player;

class playerStats
{
public:
	//temp solution coordinats of btns

	void init(float screenHeight, float screenWidth);
	void OnUpdate();
	void OnDraw(CGraphics* g, float screenHeight, Player& player);
	void StatsPosition(CGraphics* g, int& stat, float textY, float btnY, int& Skillpoint, char* statName);

	void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	void LevelUpBtnLogic(int& stat, float btnY, int& Skillpoint, CVector mouseCoordinats);
	void OnLButtonUp(Player& player, int& Skillpoint, CVector mouseCoordinats);
	double strCoor, dexCoor, conCoor, intCoor;

	CSprite* charStatsScreen;
	CSprite* levelUpBtn;


private:

};

 