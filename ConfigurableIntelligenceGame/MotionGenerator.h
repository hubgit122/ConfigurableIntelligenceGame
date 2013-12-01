
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
			virtual void generateActions();
			virtual void generateRecursively(Action& logMotionStack, StatusStack& statusStack);
			virtual void generateOperationsForOneStatus(StatusStack& statusStack, Action& logOperationStack, Action& runningOperationStack );

			bool testAndSave( CIGRuleConfig::OPERATIONS s, Chessman* c, PointOrVector dist, Action &runningOperationStack );

	};
}
#endif /*__MOTIONGENERATOR_H_*/

