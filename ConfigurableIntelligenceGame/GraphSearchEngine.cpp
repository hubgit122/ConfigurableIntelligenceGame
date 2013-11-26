#include "stdafx.h"
#include "GraphSearchEngine.h"
#include "ChessBoard.h"
#include "MotionGenerator.h"
#include <time.h>

namespace CIG
{
	const float GraphSearchEngine::MAX_SEARCH_TIME = 10000;		//10s

	int GraphSearchEngine::alphaBetaSearch( int alpha, int beta, int depth )
	{
		int vl, vlBest;
		OperationStack bestMove;
		Chessboard& nowBoard = searchingChessboardStack.top();
		// 一个Alpha-Beta完全搜索分为以下几个阶段

		// 1. 到达水平线，则返回局面评价值
		if (depth == 0)
		{
			return nowBoard.getEvaluation();
		}

		// 2. 初始化最佳值和最佳走法
		vlBest = -Chessboard::MATE_VALUE; // 这样可以知道，是否一个走法都没走过(杀棋)
		//bestMove.clear();           // 这样可以知道，是否搜索到了Beta走法或PV走法，以便保存到历史表

		// 3. 生成全部走法，并根据历史表排序			如果被将死, 没有棋可以走.
		MotionGenerator mg(nowBoard);
		mg.generateMotionsAndBoards();

		ChessboardStack& runningChessboardStack= mg.chessboardStack;
		ActionStack& runningActionStack = mg.actionStack;

		//qsort(mvs, nGenMoves, sizeof(int), CompareHistory);		//根据CompareHistory的定义, 这里由大到小排序.

		// 4. 逐一走这些走法，并进行递归
		for (int i = runningChessboardStack.size -1; i >=0 ; --i)
		{
			searchingChessboardStack.push(runningChessboardStack[i]);
			vl = -alphaBetaSearch(-beta, -alpha, depth - 1);

			// 5. 进行Alpha-Beta大小判断和截断
			if (vl > vlBest)      // 找到最佳值(但不能确定是Alpha、PV还是Beta走法)
			{
				vlBest = vl;        // "vlBest"就是目前要返回的最佳值，可能超出Alpha-Beta边界

				if (vl >= beta)   // 找到一个Beta走法
				{
					bestMove = runningActionStack[i];  // Beta走法要保存到历史表		// TO-DO保存到历史表有什么用? 为什么是beta?
					break;            // Beta截断
				}
				else if (vl > alpha)   // 找到一个PV走法
				{
					bestMove = runningActionStack[i];  // PV走法要保存到历史表
					alpha = vl;     // 缩小Alpha-Beta边界
				}
			}

			searchingChessboardStack.popNoReturn();
		}

		// 5. 所有走法都搜索完了，把最佳走法(不能是Alpha走法)保存到历史表，返回最佳值
		if (vlBest == -Chessboard::MATE_VALUE)
		{
			// 如果是杀棋，就根据杀棋步数给出评价
			return searchingChessboardStack.size - Chessboard::MATE_VALUE;
		}

		if (bestMove.size != 0)
		{
			//// 如果不是Alpha走法，就将最佳走法保存到历史表
			//Search.nHistoryTable[mvBest] += nDepth * nDepth;

			if (searchingChessboardStack.size == 1)
			{
				// 搜索根节点时，总是有一个最佳走法(因为全窗口搜索不会超出边界)，将这个走法保存下来
				GraphSearchEngine::bestMove = bestMove;
			}
		}

		return vlBest;
	}

	void GraphSearchEngine::makeBestAction( Chessboard*chessboard, void* op )
	{
		int t = clock();
		searchingOperationStack.clear();
		searchingChessboardStack.clear();
		searchingChessboardStack.push(*chessboard);
		// TO-DO 加入历史表

		for (int i=1; i<LIMIT_DEPTH; ++i)
		{
			int vl = alphaBetaSearch(-Chessboard::MATE_VALUE, Chessboard::MATE_VALUE, i);

			// 搜索到杀棋，就终止搜索
			if (vl > Chessboard::WIN_VALUE || vl < -Chessboard::WIN_VALUE)			//为了和记录深度的机制保持一致, WIN_VALUE = MATE_VALUE - 100
			{
				break;
			}

			// 超过 MAX_SEARCH_TIME，就终止搜索
			/*if (clock() - t > MAX_SEARCH_TIME)
			{
				break;
			}*/
		}
		*(OperationStack*)op = bestMove;
	}

	const int GraphSearchEngine::LIMIT_DEPTH = 16;    // 最大的搜索深度

	Stack<OperationStack,CIGRuleConfig::INT_BOARD_HISTORY_STACK_SIZE,0> GraphSearchEngine::searchingOperationStack;
	Stack<Chessboard,CIGRuleConfig::INT_BOARD_HISTORY_STACK_SIZE,0> GraphSearchEngine::searchingChessboardStack;
	CIG::OperationStack GraphSearchEngine::bestMove;
}

