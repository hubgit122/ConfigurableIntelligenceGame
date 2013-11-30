#include "stdafx.h"
#include "MotionGenerator.h"
#include "Array.h"
#include "Chessboard.h"
#include "Player.h"
#include "Chessman.h"
#include "ChessmanLocation.h"

void CIG::MotionGenerator::generateMotionsAndBoards()
{
	if (chessboard.loose[chessboard.nowTurn])
	{
		return;								//����Ѿ�����, �Ͳ������߷�, �߷�ջ�ǿյ�. 
	}

	Action logOperationStack;
	StatusStack statusStack;

	statusStack.push(CIGRuleConfig::BEGIN);
	generateRecursively(logOperationStack, statusStack);
	statusStack.popNoReturn();
}

CIG::MotionGenerator::MotionGenerator(Chessboard& cb)
	: chessboard(cb), actionStack() {}

// ���̱Ƚϸ���:
// ����������������һ��, ��ȫ��״̬ջ�ǿ�, ȡ��ջ��Ϊ��ǰ״̬,
// �Լ�¼����ջ��Ϊ��ǰ����, ������״̬ջ����Ӧ�Ŀ��ܵ��߷��Ͷ�Ӧ�Ľ������, ���������ջ.
// ����ǰ�߷�����ջ�ǿ�, ȡ��ջ����Ԫ�����¼ջ, ��ʼ��һ�׶ε�����.			///�����ʺ��õݹ麯����, ������������ջ, ������ѭ��.
// ����ǰ���̺��߷�����ջ��, ˵����״̬û�п����߷�, ״̬ջ��ջ, ����ȡ��ջ��Ϊ��ǰ״̬.
// ��ȫ��״̬ջ��Ϊend, ��ʱջ��ȫ��ջ, ״̬ջ��ջ, ����ȡ��ջ��Ϊ��ǰ״̬.
// ��ʼ��������: statusStack.push(CIGRuleConfig::BEGIN);
void CIG::MotionGenerator::generateRecursively( Action& logOperationStack, StatusStack& statusStack )
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
		generateOperationsForOneStatus(statusStack, logOperationStack, runningOperationStack);

		while (runningOperationStack.size > 0)
		{
			Operation& nowOperation = runningOperationStack.top();
			logOperationStack.push(nowOperation);
			chessboard.onOperationIntent(nowOperation);

			generateRecursively(logOperationStack, statusStack);

			chessboard.undoOperation(nowOperation);
			logOperationStack.popNoReturn();
			
			runningOperationStack.popNoReturn();
		}

		statusStack.popNoReturn();
	}
}

void CIG::MotionGenerator::generateOperationsForOneStatus(StatusStack& statusStack, Action& logOperationStack, Action& runningOperationStack )
{
	CIGRuleConfig::OPERATIONS s = statusStack.top();							// TO-DO ֱ��ʹ��s��Ϊ��������. 

	switch (s)
	{
		case CIGRuleConfig::BEGIN:
			break;

		case CIGRuleConfig::ADD:
			for (int i=0; i< (1<<CIGRuleConfig::INI_BOARD_WIDTH_LOG2) ;++i)
			{
				for (int j=0;j< (1<<CIGRuleConfig::INI_BOARD_HEIGHT_LOG2);++j)
				{
					PointOrVector dist =  PointOrVector(i,j);
					if (chessboard[dist]||!CIGRuleConfig::BOARD_RANGE[j][i])
					{
						continue;
					}
					
					testAndSave(s, NULL, dist, runningOperationStack);
				}
			}
			break;

		case CIGRuleConfig::PUT:
			{
				ChessmanIndex& cl = chessboard.pickedChessmanByIndex[-1];
				testAndSave(s, &chessboard.players[cl.player].ownedChessmans[cl.index], logOperationStack.top().distination, runningOperationStack);
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
	else if (s == CIGRuleConfig::PUT)
	{
		PointOrVector preP(c->coordinate);
		if (chessboard.onPutIntent(c, dist))
		{
			chessboard.undoPut(c);
			Operation optemp(c->chessmanLocation, s, dist);
			runningOperationStack.push(optemp);

			return true;
		}
	}
	else if(s == CIGRuleConfig::ADD)
	{
		Operation optemp(ChessmanIndex(),CIGRuleConfig::ADD,dist);
		runningOperationStack.push(optemp);
		return true;
	}
	return false;
}

