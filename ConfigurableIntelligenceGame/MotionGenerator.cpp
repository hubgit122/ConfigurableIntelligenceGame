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
		return;								//����Ѿ����˻�Ӯ��, �Ͳ������߷�, �߷�ջ�ǿյ�.
	}

	bool result;

	do
	{
		GUI::drawBoard();
		if (guiInput)
		{
			GUI::inform("�����壡");
		}

		MotionStack logMotionStack;							//�ŷ�·��
		OperationStatusStack operationStatusStack;			//����·��

		operationStatusStack.push(CIGRuleConfig::BEGIN);
		result = generateRecursively(logMotionStack, operationStatusStack, guiInput);
	}
	while (!result);
}

CIG::MotionGenerator::MotionGenerator(Chessboard& cb)
	: chessboard(cb), actionStack() {}

// ���̱Ƚϸ���:
// ����������������һ��, ��ȫ��״̬ջ�ǿ�, ȡ��ջ��Ϊ��ǰ״̬,
// �Լ�¼����ջ��Ϊ��ǰ����, ������״̬ջ����Ӧ�Ŀ��ܵ��߷��Ͷ�Ӧ�Ľ������, ���������ջ.
// ����ǰ�߷�����ջ�ǿ�, ȡ��ջ����Ԫ�����¼ջ, ��ʼ��һ�׶ε�����.			///�����ʺ��õݹ麯����, ������������ջ, ������ѭ��.
// ����ǰ���̺��߷�����ջ��, ˵����״̬û�п����߷�, ״̬ջ��ջ, ����ȡ��ջ��Ϊ��ǰ״̬.
// ��ȫ��״̬ջ��Ϊend, ��ʱջ��ȫ��ջ, ״̬ջ��ջ, ����ȡ��ջ��Ϊ��ǰ״̬.
// ��ʼ��������: operationStatusStack.push(CIGRuleConfig::BEGIN);
// ����ֵ: �����ŷ���ȡ��(��GUI����ʱ)-false, ����-true, ��ʵ���߷��޹�. 
bool CIG::MotionGenerator::generateRecursively( MotionStack& logMotionStack, OperationStatusStack& operationStatusStack, bool guiInput /*= false*/ )
{
	//��������ѡ��ǰ�Ĳ���.
	if (guiInput)		//ͨ��GUI������һ�����ŷ�
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
					return true;			//�Ѿ���actionStack����.
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

				//��tempMotionStack���������ж���, �����END, ������actionStack��
				MotionStack tempMotionStack = runningMotionStack;
				OperationStatusStack tempStatusStack = operationStatusStack;
				int j = i;

				for (; j > 0; --j)
				{
					generateMotionsForOneStatus(tempStatusStack, logMotionStack, tempMotionStack, guiInput);			
					//��tempMotionStack��tempStatusStack�б�������һ�����еĿ��ܽ��.
					tempStatusStack.popNoReturn();
				}

				int count = 0;

				for (int j = 0; j < tempMotionStack.size; ++j)
				{
					if (tempMotionStack[j].distination == dist )		//�������λ�ú��߷��е�Ŀ��λ�ý��бȶ�, ��¼ƥ��ĸ���
					{
						motemp = tempMotionStack[j];
						count++;
					}
				}

				if (count == 0)
				{
					GUI::inform("��ǰ״̬�Ѿ��޷�����, �Զ����ݵ���һ״̬. ");
					return false;
				}
				else if (count > 1)
				{
					//������Լ�ѡ��
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
								//˵����һ��ʧ����, ���ߵ�ǰ��ҷ�����. Ӧ�÷�����һ����������. Ӧ�û�������logջ���ָ�����, ��������Բ���. 
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
	CIGRuleConfig::OPERATIONS s = operationStatusStack.top();							// TO-DO ֱ��ʹ��s��Ϊ��������.

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

				if (abs(diff[0] | diff[1]) == 1)			//����
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

					if (logMotionStack.size == 1)			//��δȷ���Ƿ�����
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
					else														//�Ѿ�ȷ��Ϊ����
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

