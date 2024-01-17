#pragma once
 
class MapGen 
{
public:
	//Map Scrolling
	int scrollOffset;
	float leftScreenLimit, rightScreenLimit;

	//Base Functions
	void init(float screenHeight, int screenWidth);
	void OnUpdate(long t, CSprite& player, float screenHeight);
	void OnDraw(CGraphics* g);

	//Map generator	
	CSprite* bgBottom, * bgBottom2, * bgBottom3;

private:
	//localVar 
	float localScreenHeight;
	float localPlayerXpos;

	//********* MAP GENERATOR **********
	//Map generator changes position of bgBottoms & BgScreens
	void ForwardMapChanger(char* bgImgName, char* bottomImgName, CSprite** mainBg, CSprite** bottom, int passThresholdPx, bool& ThresholdCondition, float BgImagePosX, float addHeigth, float addWidth);
	void OnWayBackMapChanger(char* bgImgName, char* bottomImgName, CSprite** mainBg, CSprite** bottom, int passThresholdPx, bool& ThresholdCondition, float BgImagePosX, float addHeigth, float addWidth);

	//img loader
	void MapConstructor(CSprite& mainBg, char* mainImgPath, CVector bgimgPos, CSprite& bottom, char* bottomimgPath, CVector bottomimgPos);
	//hold status where player is 
	bool Pass4k, Pass7k, Pass10k, Pass13k, Pass16k, Pass19k, Pass22k, TpToLastBoss;

	//bg position , size 
	float BgImagePos1, BgImagePos2, BgImagePos3;
	float BgImageSize;


	CSprite* BgScreen, * BgScreen2, * BgScreen3;
};
