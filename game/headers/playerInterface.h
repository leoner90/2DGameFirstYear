#pragma once
#include "headers/Player.h"

class PlayerInterface
{
public:
	void init(int screenWidth, int screenHeight);
	void OnUpdate(Player& player, float scrollOffset, int screenWidth, int screenHeight);
	void OnDraw(Player& player, CGraphics* g, float scrollOffset, int screenWidth, int screenHeight);

private:
	CVector fullHpPos, fullMpPos, fullEnergyPos, kunaiPos;
	CSprite* controllPanel;
	CSprite* fullHpPoution, *fullMpPoution, *fullEnergyPoution, *emptyHpPoution, *emptyMpPoution, *emptyEnergyPoution, *kunai;
	CSprite* energyBar;

	float localScreenHeight, localScreenWidth;
	void playerEnergyBarUpdate(CSprite& ddEnergyBarRect, float& CurrentEnergy, float maxEnergy,
	float InitRectSize, float CustomeOffset, float RegenRate, float initWidth, float scrollOffset);
	void SpriteInits();
};

