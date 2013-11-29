
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
			MotionGenerator(Chessboard& cb);
			virtual ~MotionGenerator(){};
			//void operator=(const MotionGenerator&mg);

			ActionStack actionStack;
			Chessboard& chessboard;				//每个ChesssBoard有一个MotionGenerator实例.

			//virtual OperationStack* nextAction();
			virtual void generateMotionsAndBoards();
			virtual void generateForOneChessman( Chessman* c, Action& logMotionStack, StatusStack& statusStack);
			virtual void generateForPresentPlayer(Action& logMotionStack, StatusStack& statusStack);
			virtual void generateForOneOp( Chessman* c, StatusStack& statusStack, Action& logOperationStack, Action& runningOperationStack );

			bool testAndSave( CIGRuleConfig::CHESS_OPERATIONS s, Chessman* c, PointOrVector dist, Action &runningOperationStack );

	};
}
#endif /*__MOTIONGENERATOR_H_*/

