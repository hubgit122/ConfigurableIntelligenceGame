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
		// һ��Alpha-Beta��ȫ������Ϊ���¼����׶�

		// 1. ����ˮƽ�ߣ��򷵻ؾ�������ֵ
		if (depth == 0)
		{
			return nowBoard.getEvaluation();
		}

		// 2. ��ʼ�����ֵ������߷�
		vlBest = -Chessboard::MATE_VALUE; // ��������֪�����Ƿ�һ���߷���û�߹�(ɱ��)
		//bestMove.clear();           // ��������֪�����Ƿ���������Beta�߷���PV�߷����Ա㱣�浽��ʷ��

		// 3. ����ȫ���߷�����������ʷ������			���������, û���������.
		MotionGenerator mg(nowBoard);
		mg.generateMotionsAndBoards();

		ChessboardStack& runningChessboardStack= mg.chessboardStack;
		ActionStack& runningActionStack = mg.actionStack;

		//qsort(mvs, nGenMoves, sizeof(int), CompareHistory);		//����CompareHistory�Ķ���, �����ɴ�С����.

		// 4. ��һ����Щ�߷��������еݹ�
		for (int i = runningChessboardStack.size -1; i >=0 ; --i)
		{
			searchingChessboardStack.push(runningChessboardStack[i]);
			vl = -alphaBetaSearch(-beta, -alpha, depth - 1);

			// 5. ����Alpha-Beta��С�жϺͽض�
			if (vl > vlBest)      // �ҵ����ֵ(������ȷ����Alpha��PV����Beta�߷�)
			{
				vlBest = vl;        // "vlBest"����ĿǰҪ���ص����ֵ�����ܳ���Alpha-Beta�߽�

				if (vl >= beta)   // �ҵ�һ��Beta�߷�
				{
					bestMove = runningActionStack[i];  // Beta�߷�Ҫ���浽��ʷ��		// TO-DO���浽��ʷ����ʲô��? Ϊʲô��beta?
					break;            // Beta�ض�
				}
				else if (vl > alpha)   // �ҵ�һ��PV�߷�
				{
					bestMove = runningActionStack[i];  // PV�߷�Ҫ���浽��ʷ��
					alpha = vl;     // ��СAlpha-Beta�߽�
				}
			}

			searchingChessboardStack.popNoReturn();
		}

		// 5. �����߷����������ˣ�������߷�(������Alpha�߷�)���浽��ʷ���������ֵ
		if (vlBest == -Chessboard::MATE_VALUE)
		{
			// �����ɱ�壬�͸���ɱ�岽����������
			return searchingChessboardStack.size - Chessboard::MATE_VALUE;
		}

		if (bestMove.size != 0)
		{
			//// �������Alpha�߷����ͽ�����߷����浽��ʷ��
			//Search.nHistoryTable[mvBest] += nDepth * nDepth;

			if (searchingChessboardStack.size == 1)
			{
				// �������ڵ�ʱ��������һ������߷�(��Ϊȫ�����������ᳬ���߽�)��������߷���������
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
		// TO-DO ������ʷ��

		for (int i=1; i<LIMIT_DEPTH; ++i)
		{
			int vl = alphaBetaSearch(-Chessboard::MATE_VALUE, Chessboard::MATE_VALUE, i);

			// ������ɱ�壬����ֹ����
			if (vl > Chessboard::WIN_VALUE || vl < -Chessboard::WIN_VALUE)			//Ϊ�˺ͼ�¼��ȵĻ��Ʊ���һ��, WIN_VALUE = MATE_VALUE - 100
			{
				break;
			}

			// ���� MAX_SEARCH_TIME������ֹ����
			/*if (clock() - t > MAX_SEARCH_TIME)
			{
				break;
			}*/
		}
		*(OperationStack*)op = bestMove;
	}

	const int GraphSearchEngine::LIMIT_DEPTH = 16;    // �����������

	Stack<OperationStack,CIGRuleConfig::INT_BOARD_HISTORY_STACK_SIZE,0> GraphSearchEngine::searchingOperationStack;
	Stack<Chessboard,CIGRuleConfig::INT_BOARD_HISTORY_STACK_SIZE,0> GraphSearchEngine::searchingChessboardStack;
	CIG::OperationStack GraphSearchEngine::bestMove;
}

