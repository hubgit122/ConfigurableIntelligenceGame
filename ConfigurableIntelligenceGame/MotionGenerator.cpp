#include "stdafx.h"
#include "MotionGenerator.h"
#include "Array.h"
#include "Chessboard.h"
#include "Player.h"
#include "Chessman.h"

void CIG::MotionGenerator::generateMotionsAndBoards()
{
	if (chessboard.loose[chessboard.nowTurn])
	{
		return;								//如果已经输了, 就不产生走法, 走法栈是空的. 
	}

	const Stack<Chessman, CIGRuleConfig::INI_CHESSMAN_GROUP_SIZE, 0>& cg = chessboard.players[chessboard.nowTurn].ownedChessmans;

	Action logOperationStack;
	StatusStack statusStack;
	
	statusStack.push(CIGRuleConfig::BEGIN_CHESS);
	generateForPresentPlayer(logOperationStack, statusStack);
	statusStack.popNoReturn();

	for (unsigned i = 0; i < cg.size; ++i)
	{
		Chessman* c = const_cast<Chessman*> (&(cg.at(i)));
		statusStack.push(CIGRuleConfig::BEGIN_CHESS);
		generateForOneChessman(c, logOperationStack, statusStack);
		statusStack.popNoReturn();
	}
}

CIG::MotionGenerator::MotionGenerator(Chessboard& cb)
	: chessboard(cb), actionStack() {}

void CIG::MotionGenerator::generateForPresentPlayer( Action& logMotionStack, StatusStack& statusStack )
{
	Action runningOperationStack;

	CIGRuleConfig::CHESS_OPERATIONS op = statusStack.top();
	int i = 0;

	for (; CIGRuleConfig::chessOperationGraph[op][i] != CIGRuleConfig::NOMORE_CHESS_OPERATION; ++i)
	{
		statusStack.push(CIGRuleConfig::chessOperationGraph[op][i]);
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

	CIGRuleConfig::CHESS_OPERATIONS op = statusStack.top();
	int i = 0;

	for (; CIGRuleConfig::chessOperationGraph[op][i] != CIGRuleConfig::NOMORE_CHESS_OPERATION; ++i)
	{
		statusStack.push(CIGRuleConfig::chessOperationGraph[op][i]);
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
	CIGRuleConfig::CHESS_OPERATIONS s = statusStack.top();							// TO-DO 直接使用s作为参数更好. 

	switch (s)
	{
		case CIGRuleConfig::BEGIN_CHESS:
			break;

		//case CIGRuleConfig::PICK:
		//	if (chessboard.onPickIntent(c))
		//	{
		//		chessboard.undoPick(c);
		//		Operation optemp(c->chessmanLocation, CIGRuleConfig::PICK,c->coordinate);
		//		runningOperationStack.push(optemp);
		//	}

		//	break;
		case CIGRuleConfig::ADD:
			for (int i=0; i< CIGRuleConfig::INI_BOARD_WIDTH_LOG2;++i)
			{
				for (int j=0;j< CIGRuleConfig::INI_BOARD_HEIGHT_LOG2;++j)
				{
					PointOrVector dist =  PointOrVector(i,j);
					if (chessboard[dist])
					{
						continue;
					}

					testAndSave(s, c, dist, runningOperationStack);
				}
			}
			break;

		case CIGRuleConfig::PUT_CHESS:
		//case CIGRuleConfig::CAPTURE:
			//if (logOperationStack.top().operation==CIGRuleConfig::CAPTURE)
			//{
			//	PointOrVector p(c->coordinate);
			//	if (chessboard.onPutIntent(c, logOperationStack.top().distination))
			//	{
			//		chessboard.undoPut(c,p);
			//		Operation optemp(c->chessmanLocation, s, logOperationStack.top().distination, p);
			//		runningOperationStack.push(optemp);
			//	}
			//	break;
			//}

			switch (c->chessmanType)				//生成象棋走法, 改变棋子的坐标.
			{
			case CIGRuleConfig::CHESS:
				testAndSave(s, c, c->, runningOperationStack);
				break;
			}

		case CIGRuleConfig::END_CHESS:
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

bool CIG::MotionGenerator::testAndSave( CIGRuleConfig::CHESS_OPERATIONS s, Chessman* c, PointOrVector dist, Action &runningOperationStack )
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

