#include "stdafx.h"
#include "MotionGenerator.h"
#include "Array.h"
#include "Chessboard.h"
#include "Player.h"
#include "Chessman.h"
#include "ChessmanLocation.h"

void CIG::MotionGenerator::generateActions( bool guiInput )
{
	if (chessboard.loose[chessboard.nowTurn]||chessboard.win[chessboard.nowTurn])
	{
		return;								//����Ѿ����˻�Ӯ��, �Ͳ������߷�, �߷�ջ�ǿյ�. 
	}

	Action logMotionStack;
	StatusStack statusStack;

	statusStack.push(CIGRuleConfig::BEGIN);
	generateRecursively(logMotionStack, statusStack, guiInput);
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
void CIG::MotionGenerator::generateRecursively( Action& logMotionStack, StatusStack& statusStack, bool guiInput /*= false*/ )
{
	Action runningMotionStack;

	CIGRuleConfig::OPERATIONS op = statusStack.top();
	int i = 0;

	for (; CIGRuleConfig::operationGraph[op][i] != CIGRuleConfig::NOMORE; ++i)
	{
		statusStack.push(CIGRuleConfig::operationGraph[op][i]);
	}

	for (; i > 0; --i)
	{
		generateOperationsForOneStatus(statusStack, logMotionStack, runningMotionStack);

		if (guiInput)
		{
			PointOrVector dist;
			Operation optemp;
			if (runningMotionStack.size==0)				//END
			{
				statusStack.popNoReturn();
				continue;
			}
			else if (runningMotionStack.size==1)		//Ĭ�ϲ����Զ����
			{
				optemp = runningMotionStack.top();
			}
			else
			{
				GUI::getPoint(dist);				//������Ҫ�����βŻ�����Ӧ. 
				for (int i=0; i<runningMotionStack.size; ++i)
				{
					if (runningMotionStack[i].distination == dist )
					{
						optemp = runningMotionStack[i];
						break;
					}
				}
			}

			if (optemp==Operation())			//��״̬��û���ҵ�ƥ���λ�õĲ���
			{
				for (--i; (i > 0)&&(optemp==Operation()); --i)
				{
					statusStack.popNoReturn();
					generateOperationsForOneStatus(statusStack, logMotionStack, runningMotionStack);
					for (int j=0; j<runningMotionStack.size; ++j)
					{
						if (runningMotionStack[j].distination == dist )
						{
							optemp = runningMotionStack[j];
							break;
						}
					}
				}
			}

			if (optemp==Operation())			//����״̬����ƥ���λ��
			{
				//˵�������д���, �Զ��˳�ѭ��������һ��
			}
			else
			{
				logMotionStack.push(optemp);
				chessboard.onOperationIntent(optemp);
				GUI::drawBoard(&chessboard);

				generateRecursively(logMotionStack, statusStack, guiInput);			//������ȷ, ������һ��

				chessboard.undoOperation(optemp);
				logMotionStack.popNoReturn();
				runningMotionStack.clear();
				i=0;			//ʹѭ���˳�
			}
		} 
		else					//����GUI�����߷�
		{
			while (runningMotionStack.size > 0)
			{
				Operation& nowOperation = runningMotionStack.top();
				logMotionStack.push(nowOperation);
				chessboard.onOperationIntent(nowOperation);
	
				generateRecursively(logMotionStack, statusStack);
	
				chessboard.undoOperation(nowOperation);
				logMotionStack.popNoReturn();
				
				runningMotionStack.popNoReturn();
			}
		}

		statusStack.popNoReturn();
	}
}

void CIG::MotionGenerator::generateOperationsForOneStatus(StatusStack& statusStack, Action& logMotionStack, Action& runningMotionStack )
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
					
					testAndSave(s, NULL, dist, runningMotionStack);
				}
			}
			break;

		case CIGRuleConfig::PICK:
			{
				const Stack<Chessman, CIGRuleConfig::INI_CHESSMAN_GROUP_SIZE, 0>& cg = chessboard.players[chessboard.nowTurn].ownedChessmans;

				if (logMotionStack.size==0)
				{
					for (unsigned i = 0; i < cg.size; ++i)
					{
						Chessman* c = const_cast<Chessman*> (&(cg.at(i)));
						testAndSave(s, c, c->coordinate,runningMotionStack);
					}
				}
				else
				{
					Chessman* c = const_cast<Chessman*> (&(cg.at(logMotionStack.top().chessmanIndex.index)));
					
					testAndSave(s, c, c->coordinate,runningMotionStack);
				}
			}
			break;

		case CIGRuleConfig::PUT:
			{
				ChessmanIndex& cl = chessboard.pickedChessmanByIndex[-1];
				Chessman* c = &chessboard.players[cl.player].ownedChessmans[cl.index];

				if(logMotionStack.size>=3)
				{
					PointOrVector diff = (logMotionStack[-3].distination-logMotionStack[-2].distination);
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

						if (logMotionStack.size==1)			//��δȷ���Ƿ�����
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
							for (int j=logMotionStack.size-1;j>=0;--j)
							{
								if (logMotionStack[j].operation==CIGRuleConfig::PICK&&logMotionStack[j].distination==dist)
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
			if (chessboard.onChangeTurn())
			{
				chessboard.undoChangeTurn();
				actionStack.push(logMotionStack);
			}

			break;

		default:
			break;
	}
}

bool CIG::MotionGenerator::testAndSave( CIGRuleConfig::OPERATIONS s, Chessman* c, PointOrVector dist, Action &runningMotionStack )
{
	switch (s)
	{
	case CIG::CIGRuleConfig::BEGIN:
		break;
	case CIG::CIGRuleConfig::ADD:
		{
			Operation optemp(ChessmanIndex(),CIGRuleConfig::ADD,dist);
			runningMotionStack.push(optemp);
			return true;
		}
		break;
	case CIG::CIGRuleConfig::PICK:
		{
			if (chessboard.onPickIntent(c))
			{
				chessboard.undoPick(c,c->coordinate);
				Operation optemp(c->chessmanIndex, CIGRuleConfig::PICK,c->coordinate);
				runningMotionStack.push(optemp);
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
				Operation optemp(c->chessmanIndex, s, dist);
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
				Operation optemp(temp->chessmanIndex, s, dist);
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

