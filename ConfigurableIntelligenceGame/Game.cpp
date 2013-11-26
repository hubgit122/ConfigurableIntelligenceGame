#include "stdafx.h"
#include "Game.h"

namespace CIG
{
	Game::~Game()
	{
	}

	Game::Game():
		history(),
		chessBoard()
	{
	}

	void Game::operator=( const Game& g )
	{
		history = g.history;
		chessBoard = g.chessBoard;
	}
}
