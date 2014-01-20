
#ifndef __GRAPHSEARCHENGINE_H__
#define __GRAPHSEARCHENGINE_H__

#include "Motion.h"
#include "Stack.h"
#include "IntellegenceEngine.h"

namespace CIG
{
	class GraphSearchEngine
	{
		public:
			static void makeBestMove(Chessboard*cb, void* op);

			static const float MAX_SEARCH_TIME;
			static const int LIMIT_DEPTH;

		private:
			static int alphaBetaSearch(int alpha, int beta, int depth);
			//HashTable<Motion> historyList;
			static int rootDepth;
			static Chessboard* pChessboard;
			static Move bestMove;
	};
}

#endif /*__GRAPHSEARCHENGINE_H_*/

