#include "stdafx.h"
#include "headers/mapGen.h"

void MapGen::init(float screenHeight, int screenWidth)
{
	//local variables
	localScreenHeight = screenHeight;

	//is player passed certain Threshold , true if moves forward , false if moves backward
	Pass4k = Pass7k = Pass10k = Pass13k = Pass16k = Pass19k = Pass22k =  false;

	//clean variables for Restart
	delete BgScreen;
	delete BgScreen2;
	delete BgScreen3;
	delete bgBottom;
	delete bgBottom2;
	delete bgBottom3;
	 
	//main Bg
	BgScreen = new CSprite();
	BgScreen2 = new CSprite();
	BgScreen3 = new CSprite();
 
	//Bg bottom (used for hit test, Prepare your CPU!! )
	bgBottom = new CSprite();
	bgBottom2 = new CSprite();
	bgBottom3 = new CSprite();

	//limits and map scrolling
	leftScreenLimit = 300;
	rightScreenLimit = 21000;
	scrollOffset = 0;

	//map image sizes
	BgImageSize = 2759;
	BgImagePos1 = BgImageSize / 2;
	BgImagePos2 = BgImageSize + BgImageSize / 2;
	BgImagePos3 = 2 * BgImageSize + BgImageSize / 2;
	TpToLastBoss = false;
	
	MapConstructor(*BgScreen, "MainBgtest.jpg", { BgImagePos1, screenHeight / 2 + 130 }, *bgBottom, "bgBottomMain1.png", { BgImagePos1, 156 });
	MapConstructor(*BgScreen2, "MainBgtest.jpg", { BgImagePos2, screenHeight / 2 + 130 }, *bgBottom2, "bgBottomMain2.png", { BgImagePos2, 156 });
	MapConstructor(*BgScreen3, "MainBgtest.jpg", { BgImagePos3, screenHeight / 2 + 130 }, *bgBottom3, "bgBottomMain3.png", { BgImagePos3, 156 });
}

//Map images loader (bg and bottom img)
void MapGen::MapConstructor(CSprite& mainBg, char* mainImgPath ,CVector bgimgPos, CSprite& bottom, char* bottomimgPath, CVector bottomimgPos)
{
	mainBg.LoadImage(mainImgPath, CColor::Black());
	mainBg.SetImage(mainImgPath);
	mainBg.SetPosition(bgimgPos);

	bottom.LoadImage(bottomimgPath);
	bottom.SetImage(bottomimgPath);
	bottom.SetPosition(bottomimgPos);
	bottom.SetColorKey(CColor::Black());
	bottom.ClearColorKey();
}

//Map changing if player moves Forward
void MapGen::ForwardMapChanger( char* bgImgName , char* bottomImgName, CSprite** mainBg, CSprite** bottom, int passThresholdPx, bool &ThresholdCondition, float BgImagePosX, float BgHeigth , float bottomHeigth)
{
	if (localPlayerXpos > passThresholdPx && !ThresholdCondition)
	{
		delete *mainBg;
		*mainBg = new CSprite();
		delete *bottom;
		*bottom = new CSprite();

		ThresholdCondition = true;
		MapConstructor(**mainBg, bgImgName, { BgImagePosX, localScreenHeight / 2 + BgHeigth }, **bottom, bottomImgName, { BgImagePosX, bottomHeigth });
	}
} 

//Map changing if player moves back
void MapGen::OnWayBackMapChanger(char* bgImgName, char* bottomImgName, CSprite** mainBg, CSprite** bottom, int passThresholdPx, bool& ThresholdCondition, float BgImagePosX, float BgHeigth, float bottomHeigth)
{
	if (localPlayerXpos < passThresholdPx && ThresholdCondition)
	{
		delete* mainBg;
		*mainBg = new CSprite();
		delete* bottom;
		*bottom = new CSprite();

		ThresholdCondition = false;
		MapConstructor(**mainBg, bgImgName, { BgImagePosX, localScreenHeight / 2 + 130 }, **bottom, bottomImgName, { BgImagePosX, 156 });
	}
}


void MapGen::OnUpdate(long t, CSprite& player, float screenHeight  )
{
	localPlayerXpos = player.GetX();
	//4k image change
	ForwardMapChanger("MainBgtest.jpg"  , "bgBottomMain4.png", &BgScreen, &bgBottom, 4000, Pass4k , BgImageSize * 3 + BgImagePos1, 130 , 156 );
	OnWayBackMapChanger("MainBgtest.jpg", "bgBottomMain1.png", &BgScreen, &bgBottom, 4000, Pass4k, BgImageSize / 2, 130, 156);

	//7k image change
	ForwardMapChanger("MainBgtest.jpg", "bgBottomMain5.png", &BgScreen2, &bgBottom2, 7000, Pass7k, BgImageSize * 4 + BgImagePos1, 130, 156);
	OnWayBackMapChanger("MainBgtest.jpg", "bgBottomMain2.png", &BgScreen2, &bgBottom2, 7000, Pass7k, BgImageSize  + BgImagePos1, 130, 156);

	//10k image change
	ForwardMapChanger("MainBgtest.jpg", "bgBottomMain6.png", &BgScreen3, &bgBottom3, 10000, Pass10k, BgImageSize * 5 + BgImagePos1, 130, 276);
	OnWayBackMapChanger("MainBgtest.jpg", "bgBottomMain3.png", &BgScreen3, &bgBottom3, 10000, Pass10k, BgImageSize * 2+ BgImagePos1, 130, 156);

	//13k image change
	ForwardMapChanger("townBg1.jpg", "bgBottomMain7.png", &BgScreen, &bgBottom, 13000, Pass13k, BgImageSize * 6 + BgImagePos1, 206, 276);
	OnWayBackMapChanger("MainBgtest.jpg", "bgBottomMain4.png", &BgScreen, &bgBottom, 13000, Pass13k, BgImageSize * 3 + BgImagePos1, 130, 156);

	//16k image change
	ForwardMapChanger("townBg2.jpg", "bgBottomMain8.png", &BgScreen2, &bgBottom2, 16000, Pass16k, BgImageSize * 7 + BgImagePos1, 206, 276);
	OnWayBackMapChanger("MainBgtest.jpg", "bgBottomMain5.png", &BgScreen2, &bgBottom2, 16000, Pass16k, BgImageSize * 4 + BgImagePos1, 130, 156);
	
	//finall boss
	if (localPlayerXpos > 21950 && player.GetX() < 24000 && !TpToLastBoss)
	{
		ForwardMapChanger("final1.jpg", "finalBottom.jpg", &BgScreen3, &bgBottom3, 21950, TpToLastBoss, 25500, 130, 116);
		bgBottom3->SetSize(1920, 30);
		rightScreenLimit = 25130;
		leftScreenLimit = 120;
	}	 
}

void MapGen::OnDraw(CGraphics* g)
{
	//map Scrolling
	if (localPlayerXpos < leftScreenLimit)
	{
		scrollOffset = 0;
		g->SetScrollPos(0, 0);
	}
	else if (localPlayerXpos >= leftScreenLimit && localPlayerXpos <= rightScreenLimit)
	{
		scrollOffset = leftScreenLimit - localPlayerXpos;
		g->SetScrollPos(leftScreenLimit - localPlayerXpos, 0);
	}
	else if (localPlayerXpos > rightScreenLimit - leftScreenLimit - 800)
	{
		scrollOffset = leftScreenLimit - rightScreenLimit;
		g->SetScrollPos(leftScreenLimit - rightScreenLimit, 0);
	}
 
	//Map Drawing
	BgScreen->Draw(g);
	BgScreen2->Draw(g);
	BgScreen3->Draw(g);

	bgBottom->Draw(g);
	bgBottom2->Draw(g);
	bgBottom3->Draw(g);
}