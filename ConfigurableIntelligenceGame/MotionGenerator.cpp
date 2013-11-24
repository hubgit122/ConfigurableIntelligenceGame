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

// ���̱Ƚϸ���:
// ����������������һ��, ��ȫ��״̬ջ�ǿ�, ȡ��ջ��Ϊ��ǰ״̬,
// �Լ�¼����ջ��Ϊ��ǰ����, ������״̬ջ����Ӧ�Ŀ��ܵ��߷��Ͷ�Ӧ�Ľ������, ���������ջ.
// ����ǰ�߷�����ջ�ǿ�, ȡ��ջ����Ԫ�����¼ջ, ��ʼ��һ�׶ε�����.			///�����ʺ��õݹ麯����, ������������ջ, ������ѭ��.
// ����ǰ���̺��߷�����ջ��, ˵����״̬û�п����߷�, ״̬ջ��ջ, ����ȡ��ջ��Ϊ��ǰ״̬.
// ��ȫ��״̬ջ��Ϊend, ��ʱջ��ȫ��ջ, ״̬ջ��ջ, ����ȡ��ջ��Ϊ��ǰ״̬.
// ��ʼ��������: statusStack.push(CIGRuleConfig::BEGIN);
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
		switch (chess->chessmanType)				//���������߷�, �ı����ӵ�����.
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

