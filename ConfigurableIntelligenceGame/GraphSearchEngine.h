
#ifndef __GRAPHSEARCHENGINE_H__
#define __GRAPHSEARCHENGINE_H__

#include "Operation.h"
#include "Stack.h"
#include "IntellegenceEngine.h"

namespace CIG
{
	class GraphSearchEngine
	{
		public:
			//GraphSearchEngine(CIGRuleConfig::PLAYER_NAMES p = CIGRuleConfig::PLAYER_NAMES(-1) , Chessboard* cb = NULL/*, int POWER_ = 5*/);
			//GraphSearchEngine(const GraphSearchEngine& gse);
			//virtual ~GraphSearchEngine();
			//void operator = (const GraphSearchEngine& ie);
			
			static void makeBestAction(Chessboard*cb, void* op);

			static const float MAX_SEARCH_TIME;
			static const int LIMIT_DEPTH;

		private:
			static int alphaBetaSearch(int alpha, int beta, int depth);
			//HashTable<Motion> historyList;
			static Stack<Chessboard,CIGRuleConfig::INT_BOARD_HISTORY_STACK_SIZE,0> searchingChessboardStack;
			static Stack<OperationStack,CIGRuleConfig::INT_BOARD_HISTORY_STACK_SIZE,0> searchingOperationStack;
			static OperationStack bestMove;
	};

}

#endif /*__GRAPHSEARCHENGINE_H_*/

