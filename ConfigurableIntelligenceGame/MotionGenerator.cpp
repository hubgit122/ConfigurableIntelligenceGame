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
		OperationStatusStack operationStatusStack;			//操作路径

		operationStatusStack.push(CIGRuleConfig::BEGIN);
		result = generateRecursively(logMotionStack, operationStatusStack, guiInput);
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
bool CIG::MotionGenerator::generateRecursively( MotionStack& logMotionStack, OperationStatusStack& operationStatusStack, bool guiInput /*= false*/ )
{
	//根据输入选择当前的操作.
	if (guiInput)		//通过GUI输入下一步的着法
	{
		bool result;

		PointOrVector dist;
		UINT msg;

		GUI::drawBoard(&chessboard, &logMotionStack);
		GUI::getInput(dist, msg);

		switch (msg)
		{
			case CIG_END:
				if (chessboard.onChangeTurn())
				{
					chessboard.undoChangeTurn();
					actionStack.push(logMotionStack);
					return true;			//已经在actionStack里了.
				}
				else
				{
					return false;
				}

				break;

			case CIG_UNDO:
				return false;
				break;

			case CIG_POINT:
			{
				MotionStack runningMotionStack;

				CIGRuleConfig::OPERATIONS op = operationStatusStack.top();
				int i = 0;

				for (; CIGRuleConfig::operationGraph[op][i] != CIGRuleConfig::NOMORE; ++i)
				{
					operationStatusStack.push(CIGRuleConfig::operationGraph[op][i]);
				}

				Motion motemp;

				//在tempMotionStack里生成所有动作, 如果有END, 保存在actionStack里
				MotionStack tempMotionStack = runningMotionStack;
				OperationStatusStack tempStatusStack = operationStatusStack;
				int j = i;

				for (; j > 0; --j)
				{
					generateMotionsForOneStatus(tempStatusStack, logMotionStack, tempMotionStack, guiInput);			
					//在tempMotionStack和tempStatusStack中保存了下一步所有的可能结果.
					tempStatusStack.popNoReturn();
				}

				int count = 0;

				for (int j = 0; j < tempMotionStack.size; ++j)
				{
					if (tempMotionStack[j].distination == dist )		//将鼠标点击位置和走法中的目的位置进行比对, 记录匹配的个数
					{
						motemp = tempMotionStack[j];
						count++;
					}
				}

				if (count == 0)
				{
					GUI::inform("当前状态已经无法走棋, 自动回溯到上一状态. ");
					return false;
				}
				else if (count > 1)
				{
					//让玩家自己选择
					//optemp =			;
				}

				for (; i > 0; --i)
				{
					generateMotionsForOneStatus(operationStatusStack, logMotionStack, runningMotionStack, guiInput);

					while (runningMotionStack.size > 0)
					{
						if (runningMotionStack.top() == motemp)
						{
							Motion& nowMotion = motemp;
							logMotionStack.push(nowMotion);
							chessboard.onMotionIntent(nowMotion);

							result = generateRecursively(logMotionStack, operationStatusStack, guiInput);

							chessboard.undoMotion(nowMotion,runningMotionStack);
							logMotionStack.popNoReturn();
							if (!result)
								//说明这一步失败了, 或者当前玩家反悔了. 应该返回上一层重新搜索. 应该回退所有log栈并恢复棋盘, 而其余可以不问. 
							{
								CIGRuleConfig::OPERATIONS op = operationStatusStack.popThenGet();
								while (!generateRecursively(logMotionStack, operationStatusStack, guiInput));
								operationStatusStack.push(op);
							}
						}

						runningMotionStack.popNoReturn();
					}

					operationStatusStack.popNoReturn();
				}
			}

			break;

			default:
				break;
		}

		return result;
	}
	else		//!guiInput
	{
		MotionStack runningMotionStack;

		CIGRuleConfig::OPERATIONS op = operationStatusStack.top();
		int i = 0;

		for (; CIGRuleConfig::operationGraph[op][i] != CIGRuleConfig::NOMORE; ++i)
		{
			operationStatusStack.push(CIGRuleConfig::operationGraph[op][i]);
		}

		for (; i > 0; --i)
		{
			generateMotionsForOneStatus(operationStatusStack, logMotionStack, runningMotionStack);

			while (runningMotionStack.size > 0)
			{
				Motion& nowOperation = runningMotionStack.top();
				logMotionStack.push(nowOperation);
				chessboard.onMotionIntent(nowOperation);

				generateRecursively(logMotionStack, operationStatusStack);

				chessboard.undoMotion(nowOperation,runningMotionStack);
				logMotionStack.popNoReturn();

				runningMotionStack.popNoReturn();
			}

			operationStatusStack.popNoReturn();
		}

		return true;
	}
}

void CIG::MotionGenerator::generateMotionsForOneStatus( OperationStatusStack& operationStatusStack, MotionStack& logMotionStack, MotionStack& runningMotionStack, bool guiInput /*= false */ )
{
	CIGRuleConfig::OPERATIONS s = operationStatusStack.top();							// TO-DO 直接使用s作为参数更好.

	switch (s)
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

					testAndSave(s, NULL, dist, runningMotionStack);
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
					testAndSave(s, c, c->coordinate, runningMotionStack);
				}
			}
			else
			{
				Chessman* c = const_cast<Chessman*> (&(cg.at(logMotionStack.top().chessmanIndex.index)));

				testAndSave(s, c, c->coordinate, runningMotionStack);
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

					testAndSave(s, c, dist, runningMotionStack);
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

