
#ifndef __GAME_H__
#define __GAME_H__
#include "stdafx.h"
#include "CIGRuleConfig.h"
#include "Stack.h"
#include "CIGNamedObject.h"
#include "ChessBoard.h"

namespace CIG
{
	class Game: public CIGNamedObject<CIGRuleConfig::GAME_CLASS>
	{
		public:
			Game();
			virtual ~Game();
			Chessboard chessBoard;
			CIG::Stack<CIGRuleConfig::CHESSBOARD_STACK, Chessboard, CIGRuleConfig::INT_BOARD_HISTORY_STACK_SIZE, 0> history;

		public:
			static void deleteInstance( Game* instance );
			static void endThread();
			static void restartThread();
	};
}

#endif /*__GAME_H_*/

