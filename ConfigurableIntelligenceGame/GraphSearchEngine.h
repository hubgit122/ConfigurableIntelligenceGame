
#ifndef __GRAPHSEARCHENGINE_H__
#define __GRAPHSEARCHENGINE_H__

#include "Operation.h"
#include "Stack.h"
#include "IntellegenceEngine.h"

namespace CIG
{
	class GraphSearchEngine: public IntellegenceEngine
	{
		public:
			GraphSearchEngine(CIGRuleConfig::PLAYER_NAMES p = CIGRuleConfig::PLAYER_NAMES(-1) , Chessboard* cb = NULL/*, int POWER_ = 5*/);
			GraphSearchEngine(const GraphSearchEngine& gse);
			virtual ~GraphSearchEngine();
			//void operator = (const GraphSearchEngine& ie);
			
			virtual void makeBestAction( OperationStack& op );

			static const float MAX_SEARCH_TIME;
			static const int MAX_GEN_MOVES = 128; // 最大的生成走法数
			static const int LIMIT_DEPTH = 32;    // 最大的搜索深度

		private:
			//HashTable<Motion> historyList;
			Stack<CIGRuleConfig::CHESSBOARD_STACK,Chessboard,LIMIT_DEPTH,0> searchingChessboardStack;
			Stack<CIGRuleConfig::CHESSMAN_MOTION_STACK,OperationStack,CIGRuleConfig::INT_BOARD_HISTORY_STACK_SIZE,0> searchingOperationStack;
			OperationStack bestMove;

			int alphaBetaSearch(int alpha, int beta, int depth);
	};

}

#endif /*__GRAPHSEARCHENGINE_H_*/

