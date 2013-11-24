
#ifndef __MOTIONGENERATOR_H__
#define __MOTIONGENERATOR_H__
#include "Stack.h"
#include "CIGRuleConfig.h"
#include "Chessboard.h"

namespace CIG
{
	class MotionGenerator
	{
		private:
			MotionGenerator();
		public:
			MotionGenerator(const Chessboard& cb);
			virtual ~MotionGenerator();
			
			Stack<CIGRuleConfig::CHESSMAN_MOTION_STACK, OperationStack, CIGRuleConfig::INT_BOARD_HISTORY_STACK_SIZE, 0> chessmanActionStack;
			ChessboardStack chessboardStack;
			const Chessboard& chessBoard;				//ÿ��ChesssBoard��һ��MotionGeneratorʵ��.

			virtual void generateMotionsAndBoards();
			virtual void generateForOneChessman( Chessman* c, OperationStack& logMotionStack, ChessboardStack& logChessboardStack, StatusStack& statusStack);

			virtual void generateForOneOp( Chessman* c, StatusStack& statusStack, ChessboardStack& logChessboardStack, OperationStack& logOperationStack, ChessboardStack& runningChessboardStack, OperationStack& runningOperationStack );

	};
}
#endif /*__MOTIONGENERATOR_H_*/

