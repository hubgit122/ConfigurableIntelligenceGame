
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
            static void makeBestMove(Chessboard *cb, void *op);
            static void setTimeOut(unsigned long t);
            static void setLimitDepth(int d);

        private:
            static int alphaBetaSearch(int alpha, int beta, int depth);
            //HashTable<Motion> historyList;
            static int limitDepth;
            static int rootDepth;
            static unsigned long beginTime;
            static unsigned long timeout;
            static Chessboard *pChessboard;
            static Move bestMove;
    };
}

#endif /*__GRAPHSEARCHENGINE_H_*/

