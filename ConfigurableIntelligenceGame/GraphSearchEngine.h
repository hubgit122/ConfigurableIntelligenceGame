
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
			GraphSearchEngine();
			virtual ~GraphSearchEngine();

			//HashTable<Motion> historyList;
			Stack<Motion> currentSearchedMotionStack;
			int MAX_DEPTH;
			Operation bestMove;

			int alphaBetaSearch(int alpha, int beta, int depth);
	};
}

#endif /*__GRAPHSEARCHENGINE_H_*/

