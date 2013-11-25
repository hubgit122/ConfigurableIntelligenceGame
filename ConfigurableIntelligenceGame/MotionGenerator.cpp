#include "stdafx.h"
#include "MotionGenerator.h"
#include "Array.h"
#include "Chessboard.h"
#include "Player.h"
#include "Chessman.h"

void CIG::MotionGenerator::generateMotionsAndBoards()
{
	const Stack<Chessman, CIGRuleConfig::INI_CHESSMAN_GROUP_SIZE, 0>& cg = chessBoard.players[chessBoard.nowTurn].ownedChessmans;

	for (unsigned i = 0; i < cg.size; ++i)
	{
		OperationStack logOperationStack;
		ChessboardStack logChesssboardStack;
		StatusStack statusStack;
		Chessman* c = const_cast<Chessman*> (&(cg.at(i)));
		statusStack.push(CIGRuleConfig::BEGIN);
		generateForOneChessman(c, logOperationStack, logChesssboardStack, statusStack);
		statusStack.popThenGet();
	}
}

CIG::MotionGenerator::MotionGenerator(const Chessboard& cb)
	: chessBoard(cb), chessboardStack(), actionStack() {}

// ���̱Ƚϸ���:
// ����������������һ��, ��ȫ��״̬ջ�ǿ�, ȡ��ջ��Ϊ��ǰ״̬,
// �Լ�¼����ջ��Ϊ��ǰ����, ������״̬ջ����Ӧ�Ŀ��ܵ��߷��Ͷ�Ӧ�Ľ������, ���������ջ.
// ����ǰ�߷�����ջ�ǿ�, ȡ��ջ����Ԫ�����¼ջ, ��ʼ��һ�׶ε�����.			///�����ʺ��õݹ麯����, ������������ջ, ������ѭ��.
// ����ǰ���̺��߷�����ջ��, ˵����״̬û�п����߷�, ״̬ջ��ջ, ����ȡ��ջ��Ϊ��ǰ״̬.
// ��ȫ��״̬ջ��Ϊend, ��ʱջ��ȫ��ջ, ״̬ջ��ջ, ����ȡ��ջ��Ϊ��ǰ״̬.
// ��ʼ��������: statusStack.push(CIGRuleConfig::BEGIN);
void CIG::MotionGenerator::generateForOneChessman( Chessman* c , OperationStack& logOperationStack, ChessboardStack& logChessboardStack, StatusStack& statusStack)
{
	OperationStack runningOperationStack;
	ChessboardStack runningChessboardStack;

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

			logOperationStack.popNoReturn();
			logChessboardStack.popNoReturn();

			runningOperationStack.popNoReturn();
			runningChessboardStack.popNoReturn();
		}

		statusStack.popThenGet();
	}
}

void CIG::MotionGenerator::generateForOneOp( Chessman* c, StatusStack& statusStack, ChessboardStack& logChessboardStack, OperationStack& logOperationStack, ChessboardStack& runningChessboardStack, OperationStack& runningOperationStack )
{
	Chessboard chessBoard = logChessboardStack.size == 0 ? this->chessBoard : logChessboardStack.top();
	CIGRuleConfig::OPERATIONS s = statusStack.top();

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
				Operation optemp(chess->chessmanLocation, CIGRuleConfig::PICK);
				runningChessboardStack.push(chessBoard);
				runningOperationStack.push(optemp);
			}

			break;

		case CIGRuleConfig::PUT:
		case CIGRuleConfig::CAPTURE:
			if (logOperationStack.top().operation==CIGRuleConfig::CAPTURE)
			{
				if (chessBoard.onPutIntent(chess, logOperationStack.top().distination))
				{
					Operation optemp(chess->chessmanLocation, s, logOperationStack.top().distination);
					runningChessboardStack.push(chessBoard);
					runningOperationStack.push(optemp);
				}
				break;
			}

			switch (chess->chessmanType)				//���������߷�, �ı����ӵ�����.
			{
				case CIGRuleConfig::KING:
					for (int i = 0; i < 4; ++i)
					{
						Chessboard tempBoard(chessBoard);
						Chessman* tempChess = &(tempBoard.players[chess->chessmanLocation.player].ownedChessmans[chess->chessmanLocation.index]);

						PointOrVector dist = chess->coordinate + PointOrVector( ((i & 1) == 0) ? ((i & 2) ? -1 : 1) : 0, (i & 1) ? ((i & 2) ? -1 : 1) : 0 );
						PointOrVector offset = dist - PointOrVector(7, (chess->chessmanLocation.player == CIGRuleConfig::COMPUTER) ? 4 : 11);

						if (abs(offset.x[0]) & (-2) || abs(offset.x[1]) & (-2))
						{
							continue;
						}

						if (s == CIGRuleConfig::CAPTURE)
						{
							result = tempBoard.onCaptureIntent(tempChess, dist);
						}
						else
						{
							result = tempBoard.onPutIntent(tempChess, dist);
						}

						if (result)
						{
							Operation optemp(chess->chessmanLocation, s, dist);
							runningChessboardStack.push(tempBoard);
							runningOperationStack.push(optemp);
						}
					}

					break;

				case CIGRuleConfig::ADVISOR:
					for (int i = 0; i < 4; ++i)
					{
						Chessboard tempBoard(chessBoard);
						Chessman* tempChess = &(tempBoard.players[chess->chessmanLocation.player].ownedChessmans[chess->chessmanLocation.index]);
						
						PointOrVector dist = tempChess->coordinate + PointOrVector( (i & 1) ? -1 : 1, (i & 2) ? -1 : 1 );
						PointOrVector offset = dist - PointOrVector(7, (tempChess->chessmanLocation.player == CIGRuleConfig::COMPUTER) ? 4 : 11);

						if (abs(offset.x[0]) & (-2) || abs(offset.x[1]) & (-2))
						{
							continue;
						}

						if (s == CIGRuleConfig::CAPTURE)
						{
							result = tempBoard.onCaptureIntent(tempChess, dist);
						}
						else
						{
							result = tempBoard.onPutIntent(tempChess, dist);
						}

						if (result)
						{
							Operation optemp(chess->chessmanLocation, s, dist);
							runningChessboardStack.push(tempBoard);
							runningOperationStack.push(optemp);
						}
					}

					break;

				case CIGRuleConfig::ELEPHANT:
					for (int i = 0; i < 4; ++i)
					{
						Chessboard tempBoard(chessBoard);
						Chessman* tempChess = &(tempBoard.players[chess->chessmanLocation.player].ownedChessmans[chess->chessmanLocation.index]);

						PointOrVector dist = tempChess->coordinate + PointOrVector( (i & 1) ? -2 : 2, (i & 2) ? -2 : 2 );
						PointOrVector eye = tempChess->coordinate + PointOrVector( (i & 1) ? -1 : 1, (i & 2) ? -1 : 1 );

						if (!tempBoard.onSelfHalfOfBoard(dist) || tempBoard[eye] != NULL)
						{
							continue;
						}

						if (s == CIGRuleConfig::CAPTURE)
						{
							result = tempBoard.onCaptureIntent(tempChess, dist);
						}
						else
						{
							result = tempBoard.onPutIntent(tempChess, dist);
						}

						if (result)
						{
							Operation optemp(chess->chessmanLocation, s, dist);
							runningChessboardStack.push(tempBoard);
							runningOperationStack.push(optemp);
						}
					}

					break;

				case CIGRuleConfig::HORSE:
					for (int i = 0; i < 8; ++i)
					{
						Chessboard tempBoard(chessBoard);
						Chessman* tempChess = &(tempBoard.players[chess->chessmanLocation.player].ownedChessmans[chess->chessmanLocation.index]);

						PointOrVector dist = tempChess->coordinate + PointOrVector( ((i & 1) ? -1 : 1) << ((i & 4) ? 1 : 0), ((i & 2) ? -1 : 1) << ((i & 4) ? 0 : 1) );
						PointOrVector eye = tempChess->coordinate + PointOrVector( (i & 4) ? ((i & 1) ? -1 : 1) : 0, (i & 4) ? 0 : ((i & 2) ? -1 : 1) );

						if (tempBoard.beyondBoardRange(dist) || tempBoard[eye] != NULL)
						{
							continue;
						}

						if (s == CIGRuleConfig::CAPTURE)
						{
							result = tempBoard.onCaptureIntent(tempChess, dist);
						}
						else
						{
							result = tempBoard.onPutIntent(tempChess, dist);
						}

						if (result)
						{
							Operation optemp(chess->chessmanLocation, s, dist);
							runningChessboardStack.push(tempBoard);
							runningOperationStack.push(optemp);
						}
					}

					break;

				case CIGRuleConfig::CHAROIT:
					for (int i = 0; i < 4; ++i)
					{
						for (int j = 1;; ++j)
						{
							Chessboard tempBoard(chessBoard);
							Chessman* tempChess = &(tempBoard.players[chess->chessmanLocation.player].ownedChessmans[chess->chessmanLocation.index]);

							PointOrVector dist =  chess->coordinate + PointOrVector( ((i & 1) == 0) ? ((i & 2) ? -j : j) : 0, (i & 1) ? ((i & 2) ? -j : j) : 0 );

							if (tempBoard.beyondBoardRange(dist))
							{
								break;
							}
							else /*if (tempBoard[dist] == NULL)
							{
								continue;
							}
							else */if ((tempBoard[dist]) && (tempBoard[dist]->chessmanLocation.player == tempChess->chessmanLocation.player))
							{
								break;
							}
							else
							{
								if (s == CIGRuleConfig::CAPTURE)
								{
									result = tempBoard.onCaptureIntent(tempChess, dist);
									
									if (result)
									{
										Operation optemp(chess->chessmanLocation, s, dist);
										runningChessboardStack.push(tempBoard);
										runningOperationStack.push(optemp);
										break;
									}
								}
								else
								{
									result = tempBoard.onPutIntent(tempChess, dist);
									if (result)
									{
										Operation optemp(chess->chessmanLocation, s, dist);
										runningChessboardStack.push(tempBoard);
										runningOperationStack.push(optemp);
									}
									else
									{
										break;
									}
								}
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
							Chessboard tempBoard(chessBoard);
							Chessman* tempChess = &(tempBoard.players[chess->chessmanLocation.player].ownedChessmans[chess->chessmanLocation.index]);

							PointOrVector dist =  chess->coordinate + PointOrVector( ((i & 1) == 0) ? ((i & 2) ? -j : j) : 0, (i & 1) ? ((i & 2) ? -j : j) : 0 );

							if (tempBoard.beyondBoardRange(dist))
							{
								break;
							}
							else if (tempBoard[dist] == NULL)
							{
								if ((s == CIGRuleConfig::PUT) && (!haveEmplacement))
								{
									result = tempBoard.onPutIntent(tempChess, dist);

									if (result)
									{
										Operation optemp(tempChess->chessmanLocation, s, dist);
										runningChessboardStack.push(tempBoard);
										runningOperationStack.push(optemp);
									}
								}
							}
							else if (haveEmplacement)
							{

								result = tempBoard.onCaptureIntent(tempChess, dist);

								if (result)
								{
									Operation optemp(tempChess->chessmanLocation, s, dist);
									runningChessboardStack.push(tempBoard);
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
					if (chessBoard.onSelfHalfOfBoard(chess->coordinate))
					{
						PointOrVector dist = chess->coordinate + PointOrVector(0, (chess->chessmanLocation.player == CIGRuleConfig::COMPUTER) ? 1 : -1);

						if (s == CIGRuleConfig::CAPTURE)
						{
							result = chessBoard.onCaptureIntent(chess, dist);
						}
						else
						{
							result = chessBoard.onPutIntent(chess, dist);
						}

						if (result)
						{
							Operation optemp(chess->chessmanLocation, s, dist);
							runningChessboardStack.push(chessBoard);
							runningOperationStack.push(optemp);
						}
					}
					else
					{
						for (int i = -1; i < 2; i += 1)
						{
							Chessboard tempBoard(chessBoard);
							Chessman* tempChess = &(tempBoard.players[chess->chessmanLocation.player].ownedChessmans[chess->chessmanLocation.index]);

							PointOrVector dist = tempChess->coordinate + PointOrVector( i, (tempChess->chessmanLocation.player==CIGRuleConfig::COMPUTER)?(!i):(-!i) );
							
							if (s == CIGRuleConfig::CAPTURE)
							{
								result = tempBoard.onCaptureIntent(tempChess, dist);
							}
							else
							{
								result = tempBoard.onPutIntent(tempChess, dist);
							}

							if (result)
							{
								Operation optemp(chess->chessmanLocation, s, dist);
								runningChessboardStack.push(tempBoard);
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
			if (logChessboardStack.top().onChangeTurn())
			{
				chessboardStack.push(logChessboardStack.top());
				actionStack.push(logOperationStack);
			}

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

