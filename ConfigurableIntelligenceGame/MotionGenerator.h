
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
			virtual ~MotionGenerator() {};
			//void operator=(const MotionGenerator&mg);

			MoveStack actionStack;
			Chessboard& chessboard;				//ÿ��ChesssBoard��һ��MotionGeneratorʵ��.

			//virtual OperationStack* nextMove();
			virtual void generateMoves(bool guiInput = false);
			virtual bool generateRecursively(MotionStack& logMotionStack, CIGRuleConfig::OPERATIONS lastOperation, bool guiInput = false);
			virtual void generateMotionsForOneStatus(CIGRuleConfig::OPERATIONS lastOperation, MotionStack& logOperationStack, MotionStack& runningOperationStack, bool guiInput = false);

			bool testAndSave( CIGRuleConfig::OPERATIONS s, Chessman* c, PointOrVector dist, MotionStack& runningOperationStack );

	};
}
#endif /*__MOTIONGENERATOR_H_*/

