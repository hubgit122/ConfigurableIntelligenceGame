#include "stdafx.h"
#include "MotionGenerator.h"
#include "Array.h"
#include "Chessboard.h"
#include "Player.h"
#include "Chessman.h"

void CIG::MotionGenerator::generateMotionsAndBoards()
{
	const Stack<Chessman, CIGRuleConfig::INI_CHESSMAN_GROUP_SIZE, 0>& cg = chessboard.players[chessboard.nowTurn].ownedChessmans;

	Action logOperationStack;
	StatusStack statusStack;
	for (unsigned i = 0; i < cg.size; ++i)
	{
		Chessman* c = const_cast<Chessman*> (&(cg.at(i)));
		statusStack.push(CIGRuleConfig::BEGIN);
		generateForOneChessman(c, logOperationStack, statusStack);
		statusStack.popNoReturn();
	}
}

CIG::MotionGenerator::MotionGenerator(Chessboard& cb)
	: chessboard(cb), actionStack() {}

// 过程比较复杂:
// 对于运行中搜索的一步, 若全局状态栈非空, 取定栈顶为当前状态,
// 以记录棋盘栈顶为当前环境, 生成与状态栈顶相应的可能的走法和对应的结果棋盘, 入各自运行栈.
// 若当前走法运行栈非空, 取定栈顶的元素入记录栈, 开始下一阶段的搜索.			///这里适合用递归函数做, 借助程序运行栈, 而不是循环.
// 若当前棋盘和走法运行栈空, 说明该状态没有可用走法, 状态栈弹栈, 重新取定栈顶为当前状态.
// 若全局状态栈顶为end, 临时栈入全局栈, 状态栈弹栈, 重新取定栈顶为当前状态.
// 初始条件配置: statusStack.push(CIGRuleConfig::BEGIN);
void CIG::MotionGenerator::generateForOneChessman( Chessman* c , Action& logOperationStack, StatusStack& statusStack)
{
	Action runningOperationStack;

	CIGRuleConfig::OPERATIONS op = statusStack.top();
	int i = 0;

	for (; CIGRuleConfig::operationGraph[op][i] != CIGRuleConfig::NOMORE; ++i)
	{
		statusStack.push(CIGRuleConfig::operationGraph[op][i]);
	}

	for (; i > 0; --i)
	{
		generateForOneOp(c, statusStack, logOperationStack, runningOperationStack);

		while (runningOperationStack.size > 0)
		{
			Operation& nowOperation = runningOperationStack.top();
			logOperationStack.push(nowOperation);
			chessboard.onOperationIntent(nowOperation);

			generateForOneChessman(c, logOperationStack, statusStack);

			chessboard.undoOperation(nowOperation);
			logOperationStack.popNoReturn();
			
			runningOperationStack.popNoReturn();
		}

		statusStack.popNoReturn();
	}
}

void CIG::MotionGenerator::generateForOneOp( Chessman* c, StatusStack& statusStack, Action& logOperationStack, Action& runningOperationStack )
{
	CIGRuleConfig::OPERATIONS s = statusStack.top();							// TO-DO 直接使用s作为参数更好. 

	switch (s)
	{
		case CIGRuleConfig::BEGIN:
			break;

		case CIGRuleConfig::PICK:
			if (chessboard.onPickIntent(c))
			{
				chessboard.undoPick(c);
				Operation optemp(c->chessmanLocation, CIGRuleConfig::PICK,c->coordinate);
				runningOperationStack.push(optemp);
			}

			break;

		case CIGRuleConfig::PUT:
		case CIGRuleConfig::CAPTURE:
			if (logOperationStack.top().operation==CIGRuleConfig::CAPTURE)
			{
				PointOrVector p(c->coordinate);
				if (chessboard.onPutIntent(c, logOperationStack.top().distination))
				{
					chessboard.undoPut(c,p);
					Operation optemp(c->chessmanLocation, s, logOperationStack.top().distination, p);
					runningOperationStack.push(optemp);
				}
				break;
			}

			switch (c->chessmanType)				//生成象棋走法, 改变棋子的坐标.
			{
				case CIGRuleConfig::KING:
					for (int i = 0; i < 4; ++i)
					{
						PointOrVector dist = c->coordinate + PointOrVector( ((i & 1) == 0) ? ((i & 2) ? -1 : 1) : 0, (i & 1) ? ((i & 2) ? -1 : 1) : 0 );
						PointOrVector offset = dist - PointOrVector(7, (c->chessmanLocation.player == CIGRuleConfig::COMPUTER) ? 4 : 11);

						if (abs(offset.x[0]) & (-2) || abs(offset.x[1]) & (-2))
						{
							continue;
						}

						testAndSave(s, c, dist, runningOperationStack);
					}

					break;

				case CIGRuleConfig::ADVISOR:
					for (int i = 0; i < 4; ++i)
					{
						PointOrVector dist = c->coordinate + PointOrVector( (i & 1) ? -1 : 1, (i & 2) ? -1 : 1 );
						PointOrVector offset = dist - PointOrVector(7, (c->chessmanLocation.player == CIGRuleConfig::COMPUTER) ? 4 : 11);

						if (abs(offset[0]) & (-2) || abs(offset[1]) & (-2))
						{
							continue;
						}

						testAndSave(s, c, dist, runningOperationStack);
					}

					break;

				case CIGRuleConfig::ELEPHANT:
					for (int i = 0; i < 4; ++i)
					{
						PointOrVector dist = c->coordinate + PointOrVector( (i & 1) ? -2 : 2, (i & 2) ? -2 : 2 );
						PointOrVector eye = c->coordinate + PointOrVector( (i & 1) ? -1 : 1, (i & 2) ? -1 : 1 );

						if (!chessboard.onSelfHalfOfBoard(dist) || chessboard[eye] != NULL)
						{
							continue;
						}

						testAndSave(s, c, dist, runningOperationStack);
					}

					break;

				case CIGRuleConfig::HORSE:
					for (int i = 0; i < 8; ++i)
					{
						PointOrVector dist = c->coordinate + PointOrVector( ((i & 1) ? -1 : 1) << ((i & 4) ? 1 : 0), ((i & 2) ? -1 : 1) << ((i & 4) ? 0 : 1) );
						PointOrVector eye = c->coordinate + PointOrVector( (i & 4) ? ((i & 1) ? -1 : 1) : 0, (i & 4) ? 0 : ((i & 2) ? -1 : 1) );

						if (chessboard.beyondBoardRange(dist) || chessboard[eye] != NULL)
						{
							continue;
						}

						testAndSave(s, c, dist, runningOperationStack);
					}

					break;

				case CIGRuleConfig::CHAROIT:
					for (int i = 0; i < 4; ++i)
					{
						for (int j = 1;; ++j)
						{
							PointOrVector dist =  c->coordinate + PointOrVector( ((i & 1) == 0) ? ((i & 2) ? -j : j) : 0, (i & 1) ? ((i & 2) ? -j : j) : 0 );

							if (chessboard.beyondBoardRange(dist))
							{
								break;
							}
							else if ((chessboard[dist]) && (chessboard[dist]->chessmanLocation.player == c->chessmanLocation.player))
							{
								break;
							}
							else
							{
								if (s == CIGRuleConfig::CAPTURE)
								{
									if(testAndSave(s,c,dist,runningOperationStack))
									{
										break;
									}
								}
								else
								{
									if(!testAndSave(s,c,dist,runningOperationStack))
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
							PointOrVector dist =  c->coordinate + PointOrVector( ((i & 1) == 0) ? ((i & 2) ? -j : j) : 0, (i & 1) ? ((i & 2) ? -j : j) : 0 );

							if (chessboard.beyondBoardRange(dist))
							{
								break;
							}
							else if (chessboard[dist] == NULL)			//在棋盘上, 没有棋子
							{
								if ((s == CIGRuleConfig::PUT) && (!haveEmplacement))				//没有 "炮架" 
								{
									testAndSave(s,c,dist,runningOperationStack);
								}
							}
							else if (haveEmplacement)							//有棋子, 有炮架
							{
								testAndSave(s,c,dist,runningOperationStack);

								break;
							}
							else   															  //有棋子, 没炮架
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
					if (chessboard.onSelfHalfOfBoard(c->coordinate))
					{
						PointOrVector dist = c->coordinate + PointOrVector(0, (c->chessmanLocation.player == CIGRuleConfig::COMPUTER) ? 1 : -1);
						testAndSave(s, c, dist, runningOperationStack);
					}
					else
					{
						for (int i = -1; i < 2; i += 1)
						{
							PointOrVector dist = c->coordinate + PointOrVector( i, (c->chessmanLocation.player==CIGRuleConfig::COMPUTER)?(!i):(-!i) );
							
							testAndSave(s, c, dist, runningOperationStack);
						}
					}
				}
				break;

				default:
					break;
			}

			break;

		case CIGRuleConfig::END:
			if (chessboard.onChangeTurn())
			{
				chessboard.undoChangeTurn();
				actionStack.push(logOperationStack);
			}

			break;

		default:
			break;
	}
}

bool CIG::MotionGenerator::testAndSave( CIGRuleConfig::OPERATIONS s, Chessman* c, PointOrVector dist, Action &runningOperationStack )
{
	if (s == CIGRuleConfig::CAPTURE)
	{
		Chessman* temp = chessboard[dist];
		if (chessboard.onCaptureIntent(c, dist))
		{
			chessboard.undoCaptured(temp);
			Operation optemp(temp->chessmanLocation, s, dist);
			runningOperationStack.push(optemp);

			return true;
		}
	}
	else
	{
		PointOrVector p(c->coordinate);
		if (chessboard.onPutIntent(c, dist))
		{
			chessboard.undoPut(c,p);
			Operation optemp(c->chessmanLocation, s, dist, p);
			runningOperationStack.push(optemp);

			return true;
		}
	}	
	return false;
}

