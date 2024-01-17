#include "stdafx.h"
#include "headers/MyGame.h"

CMyGame game;
CGameApp app;

int main(int argc, char* argv[])
{
	app.DebugMemoryLeaks();
	app.OpenWindow(1366, 768, "NINJA GAME");
	//app.OpenFullScreen(1366, 768, 32);
	//app.OpenConsole();
	app.SetClearColor(CColor::Black());
	app.Run(&game);
	return(0);
}