//#include "stdafx.h"
#include "MotionGenerator.h"
#include "ChessmanGroup.h"
#include "ChessBoard.h"
#include "Player.h"
#include "Chessman.h"

void CIG::MotionGenerator::generateForOneOp( Chessman* c, StatusStack& statusStack, ChessBoardStack& logChessBoardStack, OperationStack& logOperationStack, ChessBoardStack& runningChessboardStack, OperationStack& runningOperationStack )
{
	ChessBoard chessBoard = logChessBoardStack.top();
	CIGRuleConfig::OPERATIONS s = statusStack.top();
	Operation op;

	c = chessBoard[c->coordinate];
	bool result = false;

	switch (s)
	{
		case CIGRuleConfig::BEGIN:
			break;

		case CIGRuleConfig::PICK:
			result = chessBoard.onPickIntent(c);

			if (result)
			{
				op.operation = CIGRuleConfig::PICK;
				op.chessman = c;
				runningChessboardStack.push(chessBoard);
				runningOperationStack.push(op);
			}

			break;

		case CIGRuleConfig::PUT:
		case CIGRuleConfig::CAPTURE:
			switch (c->chessmanType)				//���������߷�, �ı����ӵ�����.
			{
				case CIGRuleConfig::KING:
					for (int i = 0; i < 4; ++i)
					{
						PointOrVector dist = c->coordinate + PointOrVector( ((i & 1) == 0) ? ((i & 2) ? -1 : 1) : 0, (i & 1) ? ((i & 2) ? -1 : 1) : 0 );
						PointOrVector offset = dist - PointOrVector(7, (c->belongsToWhom) ? 11 : 4);

						if (abs(offset.x[0]) & (-2) || abs(offset.x[1]) & (-2))
						{
							continue;
						}

						Operation optemp(c, s);

						if (s == CIGRuleConfig::CAPTURE)
						{
							result = chessBoard.onCaptureIntent(c, dist);
						}
						else
						{
							result = chessBoard.onPutIntent(c);
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
						PointOrVector dist = c->coordinate + PointOrVector( (i & 1) ? -1 : 1, (i & 2) ? -1 : 1 );
						Operation optemp(c, s);
						PointOrVector offset = dist - PointOrVector(7, (c->belongsToWhom) ? 11 : 4);

						if (abs(offset.x[0]) & (-2) || abs(offset.x[1]) & (-2))
						{
							continue;
						}

						if (s == CIGRuleConfig::CAPTURE)
						{
							result = chessBoard.onCaptureIntent(c, dist);
						}
						else
						{
							result = chessBoard.onPutIntent(c);
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
						PointOrVector dist = c->coordinate + PointOrVector( (i & 1) ? -2 : 2, (i & 2) ? -2 : 2 );
						PointOrVector eye = c->coordinate + PointOrVector( (i & 1) ? -1 : 1, (i & 2) ? -1 : 1 );

						if (!chessBoard.onSelfHalfOfBoard(dist) || chessBoard[eye] != NULL)
						{
							continue;
						}

						Operation optemp(c, s);

						if (s == CIGRuleConfig::CAPTURE)
						{
							result = chessBoard.onCaptureIntent(c, dist);
						}
						else
						{
							result = chessBoard.onPutIntent(c);
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
						PointOrVector dist = c->coordinate + PointOrVector( ((i & 1) ? -1 : 1) << ((i & 4) ? 1 : 0), ((i & 2) ? -1 : 1) << ((i & 4) ? 0 : 1) );
						PointOrVector eye = c->coordinate + PointOrVector( (i & 4) ? ((i & 1) ? -1 : 1) : 0, (i & 4) ? 0 : ((i & 2) ? -1 : 1) );

						if (chessBoard.beyondBoardRange(dist) || chessBoard[eye] != NULL)
						{
							continue;
						}

						Operation optemp(c, s);

						if (s == CIGRuleConfig::CAPTURE)
						{
							result = chessBoard.onCaptureIntent(c, dist);
						}
						else
						{
							result = chessBoard.onPutIntent(c);
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
							PointOrVector dist =  c->coordinate + PointOrVector( ((i & 1) == 0) ? ((i & 2) ? -j : j) : 0, (i & 1) ? ((i & 2) ? -j : j) : 0 );

							if (chessBoard.beyondBoardRange(dist))
							{
								break;
							}
							else if (chessBoard[dist] == NULL)
							{
								continue;
							}
							else if (chessBoard[dist]->belongsToWhom == c->belongsToWhom)
							{
								break;
							}
							else
							{
								Operation optemp(c, s);

								if (s == CIGRuleConfig::CAPTURE)
								{
									result = chessBoard.onCaptureIntent(c, dist);
								}
								else
								{
									result = chessBoard.onPutIntent(c);
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
							PointOrVector dist =  c->coordinate + PointOrVector( ((i & 1) == 0) ? ((i & 2) ? -j : j) : 0, (i & 1) ? ((i & 2) ? -j : j) : 0 );

							if (chessBoard.beyondBoardRange(dist))
							{
								break;
							}
							else if (chessBoard[dist] == NULL)
							{
								if ((s == CIGRuleConfig::PUT) && (!haveEmplacement))
								{
									Operation optemp(c, s);

									result = chessBoard.onCaptureIntent(c, dist);

									if (result)
									{
										runningChessboardStack.push(chessBoard);
										runningOperationStack.push(optemp);
									}
								}
							}
							else if (haveEmplacement)
							{
								Operation optemp(c, s);

								result = chessBoard.onCaptureIntent(c, dist);

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
					PointOrVector dist = c->coordinate + PointOrVector(0, 1);
					Operation optemp(c, s);

					if (s == CIGRuleConfig::CAPTURE)
					{
						result = chessBoard.onCaptureIntent(c, dist);
					}
					else
					{
						result = chessBoard.onPutIntent(c,dist);
					}

					if (result)
					{
						runningChessboardStack.push(chessBoard);
						runningOperationStack.push(optemp);
					}

					if (chessBoard.onSelfHalfOfBoard(c->coordinate))
					{
						break;
					}
					else
					{
						for (int i = -1; i < 2; i += 2)
						{
							PointOrVector dist = c->coordinate + PointOrVector( i, 0 );

							result = chessBoard.onCaptureIntent(c, dist);

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

			chessboardStack.push(logChessBoardStack.top());
			chessmanActionStack.push(logOperationStack);
			break;

		default:
			break;
	}
}

void CIG::MotionGenerator::generateMotionsAndBoards()
{
	const vector<Chessman>& cg = chessBoard.players[chessBoard.nowTurn]->ownedChessmans;

	for (unsigned i = 0; i < cg.size(); ++i)
	{
		OperationStack logOperationStack("logMotionStack");
		ChessBoardStack logChesssboardStack("logChesssboardStack");
		StatusStack statusStack("StatusStack");
		Chessman* c = (Chessman*)&(cg[i]);
		statusStack.push(CIGRuleConfig::BEGIN);
		logChesssboardStack.push(chessBoard);
		generateForOneChessman(c, logOperationStack, logChesssboardStack, statusStack);
		statusStack.pop();
	}
}

CIG::MotionGenerator::MotionGenerator(ChessBoard& cb)
	: chessBoard(cb), chessboardStack("ChessboardStack"), chessmanActionStack("StackOfMotionStack") {	}

// ���̱Ƚϸ���:
// ����������������һ��, ��ȫ��״̬ջ�ǿ�, ȡ��ջ��Ϊ��ǰ״̬,
// �Լ�¼����ջ��Ϊ��ǰ����, ������״̬ջ����Ӧ�Ŀ��ܵ��߷��Ͷ�Ӧ�Ľ������, ���������ջ.
// ����ǰ�߷�����ջ�ǿ�, ȡ��ջ����Ԫ�����¼ջ, ��ʼ��һ�׶ε�����.			///�����ʺ��õݹ麯����, ������������ջ, ������ѭ��.
// ����ǰ���̺��߷�����ջ��, ˵����״̬û�п����߷�, ״̬ջ��ջ, ����ȡ��ջ��Ϊ��ǰ״̬.
// ��ȫ��״̬ջ��Ϊend, ��ʱջ��ȫ��ջ, ״̬ջ��ջ, ����ȡ��ջ��Ϊ��ǰ״̬.
// ��ʼ��������: statusStack.push(CIGRuleConfig::BEGIN);
void CIG::MotionGenerator::generateForOneChessman( Chessman* c , OperationStack& logOperationStack, ChessBoardStack& logChessBoardStack, StatusStack& statusStack)
{
	OperationStack runningOperationStack("OperationStack");
	ChessBoardStack runningChessboardStack("runningChessboardStack");

	CIGRuleConfig::OPERATIONS op = statusStack.top();
	int i = 0;

	for (; CIGRuleConfig::operationGraph[op][i] != CIGRuleConfig::NOMORE; ++i)
	{
		statusStack.push(CIGRuleConfig::operationGraph[op][i]);
	}

	for (; i > 0; --i)
	{
		generateForOneOp(c, statusStack, logChessBoardStack, logOperationStack, runningChessboardStack, runningOperationStack);

		while (runningOperationStack.size > 0)
		{
			logOperationStack.push(runningOperationStack.top());
			logChessBoardStack.push(runningChessboardStack.top());

			generateForOneChessman(c, logOperationStack, logChessBoardStack, statusStack);

			logOperationStack.pop();
			logChessBoardStack.pop();

			runningOperationStack.pop();
			runningChessboardStack.pop();
		}

		statusStack.pop();
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

