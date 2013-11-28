
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
			static void makeBestAction(Chessboard*cb, void* op);

			static const float MAX_SEARCH_TIME;
			static const int LIMIT_DEPTH;

		private:
			static int alphaBetaSearch(int alpha, int beta, int depth);
			//HashTable<Motion> historyList;
			static Chessboard* pChessboard;
			static Stack<Action,CIGRuleConfig::INT_BOARD_HISTORY_STACK_SIZE,0> searchingOperationStack;
			static Action bestAction;
	};
}

#endif /*__GRAPHSEARCHENGINE_H_*/

