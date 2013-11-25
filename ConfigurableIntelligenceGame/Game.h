
#ifndef __GAME_H__
#define __GAME_H__
#include "stdafx.h"
#include "CIGRuleConfig.h"
#include "Stack.h"
#include "CIGObject.h"
#include "ChessBoard.h"

namespace CIG
{
	class Game: public CIGObject
	{
		public:
			Game();
			virtual ~Game();
			void operator = (const Game& g);
			Chessboard chessBoard;
			CIG::Stack<Chessboard, CIGRuleConfig::INT_BOARD_HISTORY_STACK_SIZE, 0> history;

		public:
			static void deleteInstance( Game* instance );
			static void endThread();
			static void restartThread();
	};
}

#endif /*__GAME_H_*/

