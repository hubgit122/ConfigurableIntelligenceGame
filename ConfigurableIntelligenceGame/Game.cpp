#include "Game.h"

namespace CIG
{
	Game::~Game()
	{
	}

	Game::Game():
		history("HistoryBoard"),
		chessBoard(),
		CIGNamedObject("GameInstance")
	{
	}

	void Game::deleteInstance( Game* instance )
	{
		if (instance)
		{
			delete instance;
		}
	}

	/*UINT Game::runThread( LPVOID pParam )
	{
		Game::instance->mainLoop();
		}*/

	//void CIG::Game::mainLoop()
	//{
	//	chessBoard = Chessboard();
	//	history.clear();

	//	do
	//	{
	//		opeartionOfThisTurn.clear();
	//		chessBoard.players[chessBoard.nowTurn].operate();
	//		switchTurn.Lock();

	//		if (chessBoard.canMakeAction())
	//		{
	//			history.push(chessBoard);
	//			chessBoard.makeAction();
	//		}
	//		else
	//		{
	//			GUI::inform("走法不正确, 请重新走. \n", false);
	//			continue;
	//		}

	//		GUI::drawBoard();
	//	}
	//	while (chessBoard.onChangeTurn());
	//}

}
