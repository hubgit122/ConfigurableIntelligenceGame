#include "stdafx.h"
#include "MotionGenerator.h"
#include "Array.h"
#include "Chessboard.h"
#include "Player.h"
#include "Chessman.h"
#include "ChessmanLocation.h"

void CIG::MotionGenerator::generateActions()
{
	if (chessboard.loose[chessboard.nowTurn]||chessboard.win[chessboard.nowTurn])
	{
		return;								//����Ѿ����˻�Ӯ��, �Ͳ������߷�, �߷�ջ�ǿյ�. 
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

		case CIGRuleConfig::PICK:
			{
				const Stack<Chessman, CIGRuleConfig::INI_CHESSMAN_GROUP_SIZE, 0>& cg = chessboard.players[chessboard.nowTurn].ownedChessmans;

				if (logOperationStack.size==0)
				{
					for (unsigned i = 0; i < cg.size; ++i)
					{
						Chessman* c = const_cast<Chessman*> (&(cg.at(i)));
						testAndSave(s, c, c->coordinate,runningOperationStack);
					}
				}
				else
				{
					Chessman* c = const_cast<Chessman*> (&(cg.at(logOperationStack.top().chessmanIndex.index)));
					
					testAndSave(s, c, c->coordinate,runningOperationStack);
				}
			}
			break;

		case CIGRuleConfig::PUT:
			{
				ChessmanIndex& cl = chessboard.pickedChessmanByIndex[-1];
				Chessman* c = &chessboard.players[cl.player].ownedChessmans[cl.index];

				if(logOperationStack.size>=3)
				{
					PointOrVector diff = (logOperationStack[-3].distination-logOperationStack[-2].distination);
					if (abs(diff[0]|diff[1])==1)			//����
					{
						return;
					}
				}

				for (int i=GUI::LINE_DIRECTION.size()-1; i>=0; --i)
				{
					for (int j=-1; j<2; j+=2)
					{
						PointOrVector& delt = j*GUI::LINE_DIRECTION.at(i);
						Chessman* oneStep = chessboard[c->coordinate+delt];
						Chessman* twoStep = chessboard[c->coordinate+2*delt];
						PointOrVector dist;

						if (logOperationStack.size==1)			//��δȷ���Ƿ�����
						{
							if (oneStep&&twoStep)
							{
								continue;
							}
							else if (oneStep)
							{
								dist = c->coordinate+2*delt;
							}
							else
							{
								dist = c->coordinate+delt;
							}
						}
						else														//�Ѿ�ȷ��Ϊ����
						{
							if (oneStep&&!twoStep)
							{
								dist = c->coordinate + 2*delt;
							}
							else
							{
								continue;
							}

							bool flag = false;
							for (int j=logOperationStack.size-1;j>=0;--j)
							{
								if (logOperationStack[j].operation==CIGRuleConfig::PICK&&logOperationStack[j].distination==dist)
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

						testAndSave(s, c, dist, runningOperationStack);
					}
				}
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
	switch (s)
	{
	case CIG::CIGRuleConfig::BEGIN:
		break;
	case CIG::CIGRuleConfig::ADD:
		{
			Operation optemp(ChessmanIndex(),CIGRuleConfig::ADD,dist);
			runningOperationStack.push(optemp);
			return true;
		}
		break;
	case CIG::CIGRuleConfig::PICK:
		{
			if (chessboard.onPickIntent(c))
			{
				chessboard.undoPick(c,c->coordinate);
				Operation optemp(c->chessmanLocation, CIGRuleConfig::PICK,c->coordinate);
				runningOperationStack.push(optemp);
			}
		}
		break;
	case CIG::CIGRuleConfig::PUT:
		{
			PointOrVector preP(c->coordinate);
			if (chessboard.onPutIntent(c, dist))
			{
				chessboard.undoPut(c);
				c->coordinate = preP;
				Operation optemp(c->chessmanLocation, s, dist);
				runningOperationStack.push(optemp);

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
				Operation optemp(temp->chessmanLocation, s, dist);
				runningOperationStack.push(optemp);

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

