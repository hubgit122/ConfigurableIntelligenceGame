#include "stdafx.h"
#include "MotionGenerator.h"
#include "Array.h"
#include "Chessboard.h"
#include "Player.h"
#include "Chessman.h"
#include "ChessmanIndex.h"

void CIG::MotionGenerator::generateMoves( bool guiInput )
{
	if (chessboard.loose[chessboard.nowTurn] || chessboard.win[chessboard.nowTurn])
	{
		return;								//如果已经输了或赢了, 就不产生走法, 走法栈是空的.
	}

	bool result;

	do
	{
		GUI::drawBoard();
		if (guiInput)
		{
			GUI::inform("请走棋！");
		}

		MotionStack logMotionStack;							//着法路径

		result = generateRecursively(logMotionStack, CIGRuleConfig::BEGIN, guiInput);
	}
	while (!result);
}

CIG::MotionGenerator::MotionGenerator(Chessboard& cb)
	: chessboard(cb), actionStack() {}

// 过程比较复杂:
// 对于运行中搜索的一步, 若全局状态栈非空, 取定栈顶为当前状态,
// 以记录棋盘栈顶为当前环境, 生成与状态栈顶相应的可能的走法和对应的结果棋盘, 入各自运行栈.
// 若当前走法运行栈非空, 取定栈顶的元素入记录栈, 开始下一阶段的搜索.			///这里适合用递归函数做, 借助程序运行栈, 而不是循环.
// 若当前棋盘和走法运行栈空, 说明该状态没有可用走法, 状态栈弹栈, 重新取定栈顶为当前状态.
// 若全局状态栈顶为end, 临时栈入全局栈, 状态栈弹栈, 重新取定栈顶为当前状态.
// 初始条件配置: operationStatusStack.push(CIGRuleConfig::BEGIN);
// 返回值: 后续着法被取消(在GUI操作时)-false, 否则-true, 跟实际走法无关. 
bool CIG::MotionGenerator::generateRecursively( MotionStack& logMotionStack, CIGRuleConfig::OPERATIONS lastOperation, bool guiInput /*= false*/ )
{
	MotionStack runningMotionStack;

	OperationStatusStack runningOperationStack;
	int i = 0;
	//得到当前操作的孩子节点存到operationStatusStack
	for (; CIGRuleConfig::operationGraph[lastOperation][i] != CIGRuleConfig::NOMORE; ++i)
	{
		runningOperationStack.push(CIGRuleConfig::operationGraph[lastOperation][i]);
	}

	for (; i > 0; --i)
	{
		generateMotionsForOneStatus(runningOperationStack.top(), logMotionStack, runningMotionStack);

		while (runningMotionStack.size > 0) 
		{
			Motion& nowMotion = runningMotionStack.top();
			logMotionStack.push(nowMotion);
			chessboard.onMotionIntent(nowMotion);

			generateRecursively(logMotionStack, runningOperationStack.top());

			chessboard.undoMotion(nowMotion,logMotionStack);
			logMotionStack.popNoReturn();

			runningMotionStack.popNoReturn();
		}

		runningOperationStack.popNoReturn();
	}

	return true;
}

void CIG::MotionGenerator::generateMotionsForOneStatus( CIGRuleConfig::OPERATIONS op, MotionStack& logMotionStack, MotionStack& runningMotionStack, bool guiInput /*= false*/ )
{
	switch (op)
	{
		case CIGRuleConfig::BEGIN:
			break;

		case CIGRuleConfig::ADD:
			for (int i = 0; i < (1 << CIGRuleConfig::INI_BOARD_WIDTH_LOG2) ; ++i)
			{
				for (int j = 0; j < (1 << CIGRuleConfig::INI_BOARD_HEIGHT_LOG2); ++j)
				{
					PointOrVector dist =  PointOrVector(i, j);

					if (chessboard[dist] || !CIGRuleConfig::BOARD_RANGE[j][i])
					{
						continue;
					}

					testAndSave(op, NULL, dist, runningMotionStack);
				}
			}

			break;

		case CIGRuleConfig::PICK:
		{
			const Stack<Chessman, CIGRuleConfig::INI_CHESSMAN_GROUP_SIZE, 0>& cg = chessboard.players[chessboard.nowTurn].ownedChessmans;

			if (logMotionStack.size == 0)
			{
				for (unsigned i = 0; i < cg.size; ++i)
				{
					Chessman* c = const_cast<Chessman*> (&(cg.at(i)));
					testAndSave(op, c, c->coordinate, runningMotionStack);
				}
			}
			else
			{
				Chessman* c = const_cast<Chessman*> (&(cg.at(logMotionStack.top().chessmanIndex.index)));

				testAndSave(op, c, c->coordinate, runningMotionStack);
			}
		}
		break;

		case CIGRuleConfig::PUT:
		{
			ChessmanIndex& cl = chessboard.pickedChessmanByIndex[-1];
			Chessman* c = &chessboard.players[cl.player].ownedChessmans[cl.index];

			if(logMotionStack.size >= 3)
			{
				PointOrVector diff = (logMotionStack[-3].distination - logMotionStack[-2].distination);

				if (abs(diff[0] | diff[1]) == 1)			//单跳
				{
					return;
				}
			}

			for (int i = GUI::LINE_DIRECTION.size() - 1; i >= 0; --i)
			{
				for (int j = -1; j < 2; j += 2)
				{
					PointOrVector& delt = j * GUI::LINE_DIRECTION.at(i);
					Chessman* oneStep = chessboard[c->coordinate + delt];
					Chessman* twoStep = chessboard[c->coordinate + 2 * delt];
					PointOrVector dist;

					if (logMotionStack.size == 1)			//还未确定是否连跳
					{
						if (oneStep && twoStep)
						{
							continue;
						}
						else if (oneStep)
						{
							dist = c->coordinate + 2 * delt;
						}
						else
						{
							dist = c->coordinate + delt;
						}
					}
					else														//已经确定为连跳
					{
						if (oneStep && !twoStep)
						{
							dist = c->coordinate + 2 * delt;
						}
						else
						{
							continue;
						}

						bool flag = false;

						for (int j = logMotionStack.size - 1; j >= 0; --j)
						{
							if (logMotionStack[j].operation == CIGRuleConfig::PICK && logMotionStack[j].distination == dist)
							{
								flag = true;
								break;
							}
						}

						if (flag)
						{
							continue;
						}
					}

					testAndSave(op, c, dist, runningMotionStack);
				}
			}
		}
		break;

		case CIGRuleConfig::END:
			if (!guiInput)
			{
				if (chessboard.onChangeTurn())
				{
					chessboard.undoChangeTurn();
					actionStack.push(logMotionStack);
				}
			}

			break;

		default:
			break;
	}
}

bool CIG::MotionGenerator::testAndSave( CIGRuleConfig::OPERATIONS s, Chessman* c, PointOrVector dist, MotionStack& runningMotionStack )
{
	switch (s)
	{
		case CIG::CIGRuleConfig::BEGIN:
			break;

		case CIG::CIGRuleConfig::ADD:
		{
			Motion optemp(ChessmanIndex(), CIGRuleConfig::ADD, dist);
			runningMotionStack.push(optemp);
			return true;
		}
		break;

		case CIG::CIGRuleConfig::PICK:
		{
			if (chessboard.onPickIntent(c))
			{
				chessboard.undoPick(c, c->coordinate);
				Motion optemp(c->chessmanIndex, CIGRuleConfig::PICK, c->coordinate);
				runningMotionStack.push(optemp);
			}
		}
		break;

		case CIG::CIGRuleConfig::PUT:
		{
			PointOrVector preP(c->coordinate);

			if (chessboard.onPutIntent(c, dist))
			{
				chessboard.undoPut(c,preP);
				Motion optemp(c->chessmanIndex, s, dist);
				runningMotionStack.push(optemp);

				return true;
			}
		}
		break;

		case CIG::CIGRuleConfig::CAPTURE:
		{
			Chessman* temp = chessboard[dist];

			if (chessboard.onCaptureIntent(c, dist))
			{
				chessboard.undoCaptured(temp);
				Motion optemp(temp->chessmanIndex, s, dist);
				runningMotionStack.push(optemp);

				return true;
			}
		}
		break;

		case CIG::CIGRuleConfig::PROMOTION:
			break;

		case CIG::CIGRuleConfig::DECOVER:
			break;

		case CIG::CIGRuleConfig::END:
			break;

		default:
			break;
	}

	return false;
}

