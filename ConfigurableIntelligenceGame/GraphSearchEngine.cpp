#include "stdafx.h"
#include "GraphSearchEngine.h"
#include "ChessBoard.h"
#include "MotionGenerator.h"
#include <time.h>

namespace CIG
{
	const float GraphSearchEngine::MAX_SEARCH_TIME = 10000;		//10s

	//************************************
	// Method:    alphaBetaSearch
	// FullName:  CIG::GraphSearchEngine::alphaBetaSearch
	// Access:    private static
	// Returns:   int
	// Qualifier:
	// Parameter: int alpha
	// Parameter: int beta
	// Parameter: int depth
	// 这个函数的多人版, 应该是这样的:
	// 原理是当前玩家看有限步, 得到在别的玩家也走对自己最有利的走法时, 自己所能获得的最有利的结果.
	// 这和最大最小搜索的最大不同在于得到当前被评估玩家走这一步之后所能得到的最好分数并不是直接由前一人取相反数得到的, 而应该直接由下级搜索返回本级的可期望评估.
	//************************************
	int GraphSearchEngine::alphaBetaSearch( int alpha, int beta, int depth)
	{
		int vl, vlBest;
		MotionStack nowBestMove;
		Chessboard& nowBoard = *pChessboard;
		// 一个Alpha-Beta完全搜索分为以下几个阶段

		// 1. 到达水平线，则返回局面评价值
		if (depth == 0)
		{
			return nowBoard.getEvaluation((CIGRuleConfig::PLAYER_NAMES)((nowBoard.nowTurn - 1) < 0 ? (CIGRuleConfig::PLAYER_NUM - 1) : nowBoard.nowTurn - 1));
		}

		// 2. 初始化最佳值和最佳走法
		vlBest = -Chessboard::MATE_VALUE; // 这样可以知道，是否一个走法都没走过(杀棋)
		//bestMove.clear();           // 这样可以知道，是否搜索到了Beta走法或PV走法，以便保存到历史表

		// 3. 生成全部走法，并根据历史表排序			如果被将死, 没有棋可以走.
		MotionGenerator mg(nowBoard);
		mg.generateMoves();

		MoveStack& runningMoveStack = mg.actionStack;

		//qsort(mvs, nGenMoves, sizeof(int), CompareHistory);		//根据CompareHistory的定义, 这里由大到小排序.

		// 4. 逐一走这些走法，并进行递归
		for (int i = runningMoveStack.size - 1; i >= 0 ; --i)
		{
			MotionStack& nowMove = runningMoveStack[i];

			nowBoard.onWholeMoveIntent(nowMove, true);
			vl = alphaBetaSearch(-beta, -alpha, depth - 1);
			nowBoard.undoWholeMove(nowMove, true);

			// 5. 进行Alpha-Beta大小判断和截断
			if (vl > vlBest)      // 找到最佳值(但不能确定是Alpha、PV还是Beta走法)
			{
				vlBest = vl;        // "vlBest"就是目前要返回的最佳值，可能超出Alpha-Beta边界

				if (vl >= beta)   // 找到一个Beta走法
				{
					nowBestMove.forceCopyFrom(runningMoveStack[i]);		// TO-DO保存到历史表有什么用? 为什么是beta?
					break;            // Beta截断
				}
				else if (vl > alpha)   // 找到一个PV走法
				{
					nowBestMove.forceCopyFrom(runningMoveStack[i]);
					alpha = vl;     // 缩小Alpha-Beta边界
				}
			}
		}

		// 5. 所有走法都搜索完了，把最佳走法(不能是Alpha走法)保存到历史表，返回最佳值
		if (vlBest <= -Chessboard::WIN_VALUE )
		{
			// 如果是杀棋，就根据杀棋步数给出评价
			return nowBoard.nowRound - Chessboard::MATE_VALUE;
		}

		if (nowBestMove.size != 0)
		{
			//// 如果不是Alpha走法，就将最佳走法保存到历史表
			//Search.nHistoryTable[mvBest] += nDepth * nDepth;

			if (depth == rootDepth)
			{
				// 搜索根节点时，总是有一个最佳走法(因为全窗口搜索不会超出边界)，将这个走法保存下来
				GraphSearchEngine::bestMove.forceCopyFrom(nowBestMove);
			}
		}

		//return vlBest;						//通过vlBest选出最佳走法, 但是并不能返回这个值, 而是对应的前一玩家的评估值.
		nowBoard.onMoveIntent(nowBestMove, true);
		int preBest = nowBoard.getEvaluation((CIGRuleConfig::PLAYER_NAMES)((nowBoard.nowTurn - 1) < 0 ? (CIGRuleConfig::PLAYER_NUM - 1) : nowBoard.nowTurn - 1));
		nowBoard.undoMove(nowBestMove, true);

		return preBest;
	}

	void GraphSearchEngine::makeBestMove( Chessboard* chessboard, void* action )
	{
		int t = clock();
		bestMove.clear();
		GraphSearchEngine::pChessboard = chessboard;
		// TO-DO 加入历史表

		//for (int i=1; i<=LIMIT_DEPTH; ++i)
		//{
		rootDepth =/*i*/LIMIT_DEPTH;

		int vl = alphaBetaSearch( -Chessboard::MATE_VALUE, Chessboard::MATE_VALUE, /*i*/LIMIT_DEPTH);

		//// 搜索到杀棋，就终止搜索
		//if (vl > Chessboard::WIN_VALUE || vl < -Chessboard::WIN_VALUE)			//为了和记录深度的机制保持一致, WIN_VALUE = MATE_VALUE - 100
		//{
		//	break;
		//}

		//// 超过 MAX_SEARCH_TIME，就终止搜索
		//if (clock() - t > MAX_SEARCH_TIME)
		//{
		//	break;
		//}
		//}
		(*((MotionStack*)action)).forceCopyFrom(bestMove);
	}

	int GraphSearchEngine::rootDepth;

	Chessboard* GraphSearchEngine::pChessboard = NULL;

	const int GraphSearchEngine::LIMIT_DEPTH = 3;    // 最大的搜索深度

	CIG::MotionStack GraphSearchEngine::bestMove;
}