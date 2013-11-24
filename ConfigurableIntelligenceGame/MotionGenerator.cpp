//#include "stdafx.h"
#include "MotionGenerator.h"
#include "Array.h"
#include "Chessboard.h"
#include "Player.h"
#include "Chessman.h"

void CIG::MotionGenerator::generateMotionsAndBoards()
{
	const Stack<CIGRuleConfig::CHESSMAN_GROUP,Chessman,CIGRuleConfig::INI_CHESSMAN_GROUP_SIZE,0>& cg = chessBoard.players[chessBoard.nowTurn].ownedChessmans;

	for (unsigned i = 0; i < cg.size; ++i)
	{
		OperationStack logOperationStack("logMotionStack");
		ChessboardStack logChesssboardStack("logChesssboardStack");
		StatusStack statusStack("StatusStack");
		Chessman* c = const_cast<Chessman*> (&(cg.at(i)));
		statusStack.push(CIGRuleConfig::BEGIN);
		generateForOneChessman(c, logOperationStack, logChesssboardStack, statusStack);
		statusStack.pop();
	}
}

CIG::MotionGenerator::MotionGenerator(const Chessboard& cb)
	: chessBoard(cb), chessboardStack("ChessboardStack"), chessmanActionStack("StackOfMotionStack") {}

// 过程比较复杂:
// 对于运行中搜索的一步, 若全局状态栈非空, 取定栈顶为当前状态,
// 以记录棋盘栈顶为当前环境, 生成与状态栈顶相应的可能的走法和对应的结果棋盘, 入各自运行栈.
// 若当前走法运行栈非空, 取定栈顶的元素入记录栈, 开始下一阶段的搜索.			///这里适合用递归函数做, 借助程序运行栈, 而不是循环.
// 若当前棋盘和走法运行栈空, 说明该状态没有可用走法, 状态栈弹栈, 重新取定栈顶为当前状态.
// 若全局状态栈顶为end, 临时栈入全局栈, 状态栈弹栈, 重新取定栈顶为当前状态.
// 初始条件配置: statusStack.push(CIGRuleConfig::BEGIN);
void CIG::MotionGenerator::generateForOneChessman( Chessman* c , OperationStack& logOperationStack, ChessboardStack& logChessboardStack, StatusStack& statusStack)
{
	OperationStack runningOperationStack("OperationStack");
	ChessboardStack runningChessboardStack("runningChessboardStack");

	CIGRuleConfig::OPERATIONS op = statusStack.top();
	int i = 0;

	for (; CIGRuleConfig::operationGraph[op][i] != CIGRuleConfig::NOMORE; ++i)
	{
		statusStack.push(CIGRuleConfig::operationGraph[op][i]);
	}

	for (; i > 0; --i)
	{
		generateForOneOp(c, statusStack, logChessboardStack, logOperationStack, runningChessboardStack, runningOperationStack);

		while (runningOperationStack.size > 0)
		{
			logOperationStack.push(runningOperationStack.top());
			logChessboardStack.push(runningChessboardStack.top());

			generateForOneChessman(c, logOperationStack, logChessboardStack, statusStack);

			logOperationStack.pop();
			logChessboardStack.pop();

			runningOperationStack.pop();
			runningChessboardStack.pop();
		}

		statusStack.pop();
	}
}

void CIG::MotionGenerator::generateForOneOp( Chessman* c, StatusStack& statusStack, ChessboardStack& logChessboardStack, OperationStack& logOperationStack, ChessboardStack& runningChessboardStack, OperationStack& runningOperationStack )
{
	Chessboard chessBoard = logChessboardStack.size==0?this->chessBoard:logChessboardStack.top();
	CIGRuleConfig::OPERATIONS s = statusStack.top();
	Operation op;

	Chessman* chess = &(chessBoard.players[c->chessmanLocation.player].ownedChessmans[c->chessmanLocation.index]);
	bool result = false;

	switch (s)
	{
	case CIGRuleConfig::BEGIN:
		break;

	case CIGRuleConfig::PICK:
		result = chessBoard.onPickIntent(chess);

		if (result)
		{
			op.operation = CIGRuleConfig::PICK;
			op.chessmanLocation = chess->chessmanLocation;
			runningChessboardStack.push(chessBoard);
			runningOperationStack.push(op);
		}

		break;

	case CIGRuleConfig::PUT:
	case CIGRuleConfig::CAPTURE:
		switch (chess->chessmanType)				//生成象棋走法, 改变棋子的坐标.
		{
		case CIGRuleConfig::KING:
			for (int i = 0; i < 4; ++i)
			{
				PointOrVector dist = chess->coordinate + PointOrVector( ((i & 1) == 0) ? ((i & 2) ? -1 : 1) : 0, (i & 1) ? ((i & 2) ? -1 : 1) : 0 );
				PointOrVector offset = dist - PointOrVector(7, (chess->chessmanLocation.player) ? 11 : 4);

				if (abs(offset.x[0]) & (-2) || abs(offset.x[1]) & (-2))
				{
					continue;
				}

				Operation optemp(chess->chessmanLocation, s);

				if (s == CIGRuleConfig::CAPTURE)
				{
					result = chessBoard.onCaptureIntent(chess, dist);
				}
				else
				{
					result = chessBoard.onPutIntent(chess);
				}

				if (result)
				{
					runningChessboardStack.push(chessBoard);
					runningOperationStack.push(optemp);
				}
			}

			break;

		case CIGRuleConfig::ADVISOR:
			for (int i = 0; i < 4; ++i)
			{
				PointOrVector dist = chess->coordinate + PointOrVector( (i & 1) ? -1 : 1, (i & 2) ? -1 : 1 );
				Operation optemp(chess->chessmanLocation, s);
				PointOrVector offset = dist - PointOrVector(7, (chess->chessmanLocation.player) ? 11 : 4);

				if (abs(offset.x[0]) & (-2) || abs(offset.x[1]) & (-2))
				{
					continue;
				}

				if (s == CIGRuleConfig::CAPTURE)
				{
					result = chessBoard.onCaptureIntent(chess, dist);
				}
				else
				{
					result = chessBoard.onPutIntent(chess);
				}

				if (result)
				{
					runningChessboardStack.push(chessBoard);
					runningOperationStack.push(optemp);
				}
			}

			break;

		case CIGRuleConfig::ELEPHANT:
			for (int i = 0; i < 4; ++i)
			{
				PointOrVector dist = chess->coordinate + PointOrVector( (i & 1) ? -2 : 2, (i & 2) ? -2 : 2 );
				PointOrVector eye = chess->coordinate + PointOrVector( (i & 1) ? -1 : 1, (i & 2) ? -1 : 1 );

				if (!chessBoard.onSelfHalfOfBoard(dist) || chessBoard[eye] != NULL)
				{
					continue;
				}

				Operation optemp(chess->chessmanLocation, s);

				if (s == CIGRuleConfig::CAPTURE)
				{
					result = chessBoard.onCaptureIntent(chess, dist);
				}
				else
				{
					result = chessBoard.onPutIntent(chess);
				}

				if (result)
				{
					runningChessboardStack.push(chessBoard);
					runningOperationStack.push(optemp);
				}
			}

			break;

		case CIGRuleConfig::HORSE:
			for (int i = 0; i < 8; ++i)
			{
				PointOrVector dist = chess->coordinate + PointOrVector( ((i & 1) ? -1 : 1) << ((i & 4) ? 1 : 0), ((i & 2) ? -1 : 1) << ((i & 4) ? 0 : 1) );
				PointOrVector eye = chess->coordinate + PointOrVector( (i & 4) ? ((i & 1) ? -1 : 1) : 0, (i & 4) ? 0 : ((i & 2) ? -1 : 1) );

				if (chessBoard.beyondBoardRange(dist) || chessBoard[eye] != NULL)
				{
					continue;
				}

				Operation optemp(chess->chessmanLocation, s);

				if (s == CIGRuleConfig::CAPTURE)
				{
					result = chessBoard.onCaptureIntent(chess, dist);
				}
				else
				{
					result = chessBoard.onPutIntent(chess);
				}

				if (result)
				{
					runningChessboardStack.push(chessBoard);
					runningOperationStack.push(optemp);
				}
			}

			break;

		case CIGRuleConfig::CHAROIT:
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 1;; ++j)
				{
					PointOrVector dist =  chess->coordinate + PointOrVector( ((i & 1) == 0) ? ((i & 2) ? -j : j) : 0, (i & 1) ? ((i & 2) ? -j : j) : 0 );

					if (chessBoard.beyondBoardRange(dist))
					{
						break;
					}
					else if (chessBoard[dist] == NULL)
					{
						continue;
					}
					else if (chessBoard[dist]->chessmanLocation.player == chess->chessmanLocation.player)
					{
						break;
					}
					else
					{
						Operation optemp(chess->chessmanLocation, s);

						if (s == CIGRuleConfig::CAPTURE)
						{
							result = chessBoard.onCaptureIntent(chess, dist);
						}
						else
						{
							result = chessBoard.onPutIntent(chess);
						}

						if (result)
						{
							runningChessboardStack.push(chessBoard);
							runningOperationStack.push(optemp);
						}

						break;
					}

				}
			}

			break;

		case CIGRuleConfig::CANNON:
			for (int i = 0; i < 4; ++i)
			{
				bool haveEmplacement = false;

				for (int j = 1;; ++j)
				{
					PointOrVector dist =  chess->coordinate + PointOrVector( ((i & 1) == 0) ? ((i & 2) ? -j : j) : 0, (i & 1) ? ((i & 2) ? -j : j) : 0 );

					if (chessBoard.beyondBoardRange(dist))
					{
						break;
					}
					else if (chessBoard[dist] == NULL)
					{
						if ((s == CIGRuleConfig::PUT) && (!haveEmplacement))
						{
							Operation optemp(chess->chessmanLocation, s);

							result = chessBoard.onCaptureIntent(chess, dist);

							if (result)
							{
								runningChessboardStack.push(chessBoard);
								runningOperationStack.push(optemp);
							}
						}
					}
					else if (haveEmplacement)
					{
						Operation optemp(chess->chessmanLocation, s);

						result = chessBoard.onCaptureIntent(chess, dist);

						if (result)
						{
							runningChessboardStack.push(chessBoard);
							runningOperationStack.push(optemp);
						}

						break;
					}
					else
					{
						if (s == CIGRuleConfig::CAPTURE)
						{
							haveEmplacement = true;
						}
						else
						{
							break;
						}
					}

				}
			}

			break;

		case CIGRuleConfig::PAWN:
			{
				PointOrVector dist = chess->coordinate + PointOrVector(0, 1);
				Operation optemp(chess->chessmanLocation, s);

				if (s == CIGRuleConfig::CAPTURE)
				{
					result = chessBoard.onCaptureIntent(chess, dist);
				}
				else
				{
					result = chessBoard.onPutIntent(chess,dist);
				}

				if (result)
				{
					runningChessboardStack.push(chessBoard);
					runningOperationStack.push(optemp);
				}

				if (chessBoard.onSelfHalfOfBoard(chess->coordinate))
				{
					break;
				}
				else
				{
					for (int i = -1; i < 2; i += 2)
					{
						PointOrVector dist = chess->coordinate + PointOrVector( i, 0 );

						result = chessBoard.onCaptureIntent(chess, dist);

						if (result)
						{
							runningChessboardStack.push(chessBoard);
							runningOperationStack.push(optemp);
						}
					}
				}
			}
			break;

		default:
			break;
		}

		break;

	case CIGRuleConfig::END:

		chessboardStack.push(logChessboardStack.top());
		chessmanActionStack.push(logOperationStack);
		break;

	default:
		break;
	}
}

//
//void CIG::MotionGenerator::clearLastStacks()
//{
//	// TO-DO
//}

CIG::MotionGenerator::~MotionGenerator()
{

}

