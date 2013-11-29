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
		return;								//����Ѿ�����, �Ͳ������߷�, �߷�ջ�ǿյ�. 
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

// ���̱Ƚϸ���:
// ����������������һ��, ��ȫ��״̬ջ�ǿ�, ȡ��ջ��Ϊ��ǰ״̬,
// �Լ�¼����ջ��Ϊ��ǰ����, ������״̬ջ����Ӧ�Ŀ��ܵ��߷��Ͷ�Ӧ�Ľ������, ���������ջ.
// ����ǰ�߷�����ջ�ǿ�, ȡ��ջ����Ԫ�����¼ջ, ��ʼ��һ�׶ε�����.			///�����ʺ��õݹ麯����, ������������ջ, ������ѭ��.
// ����ǰ���̺��߷�����ջ��, ˵����״̬û�п����߷�, ״̬ջ��ջ, ����ȡ��ջ��Ϊ��ǰ״̬.
// ��ȫ��״̬ջ��Ϊend, ��ʱջ��ȫ��ջ, ״̬ջ��ջ, ����ȡ��ջ��Ϊ��ǰ״̬.
// ��ʼ��������: statusStack.push(CIGRuleConfig::BEGIN);
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
	CIGRuleConfig::CHESS_OPERATIONS s = statusStack.top();							// TO-DO ֱ��ʹ��s��Ϊ��������. 

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

			switch (c->chessmanType)				//���������߷�, �ı����ӵ�����.
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

