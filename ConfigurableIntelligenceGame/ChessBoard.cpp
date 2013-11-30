#include "stdafx.h"
#include "Chessboard.h"
#include "Player.h"
#include "Array.h"
#include "Chessman.h"
#include "ChessmanLocation.h"
#include "GraphSearchEngine.h"


namespace CIG
{
	// 按配置初始化很少执行, 所以可以速度慢一点.
	CIG::Chessboard::Chessboard() : nowRound(0), nowTurn((CIGRuleConfig::PLAYER_NAMES)0), pickedChessmanByIndex(), currentBannedMotions(),chessmanLocationBoard()
	{
		players[0] = Player(CIGRuleConfig::HUMAN, GraphSearchEngine::makeBestAction, this);
		players[1] = Player(CIGRuleConfig::COMPUTER, GraphSearchEngine::makeBestAction, this);			// TO-DO  应该是智能引擎实例而不是player类实例.

		memset(loose, 0, sizeof(bool)*CIGRuleConfig::PLAYER_NUM);

		for (int k = 0; k < CIGRuleConfig::PLAYER_NUM; ++k)
		{
			evaluations[k] = 0;

			for (int i = 0; i < (1 << CIGRuleConfig::INI_BOARD_HEIGHT_LOG2); ++i)
			{
				for (int j = 0 ; j < (1 << CIGRuleConfig::INI_BOARD_WIDTH_LOG2); ++j)
				{
					CIGRuleConfig::CHESSMAN_TYPES t = CIGRuleConfig::INI_BOARD[k][i][j];

					if (t != CIGRuleConfig::NOCHESSMAN)
					{
						Chessman c = Chessman(t, PointOrVector(j, i), (CIGRuleConfig::PLAYER_NAMES)k, players[k].ownedChessmans.size, CIGRuleConfig::ON_BOARD, CIGRuleConfig::ALL);
						players[k].ownedChessmans.push(c);

						chessmanLocationBoard[PointOrVector(j,i)] = ChessmanIndex((CIGRuleConfig::PLAYER_NAMES)k, players[k].ownedChessmans.size - 1);
					}
				}
			}
		}
		calEvaluations();
	}

	CIG::Chessboard::Chessboard( const Chessboard& cb )
		: nowRound(cb.nowRound), 
		nowTurn(cb.nowTurn),
		chessmanLocationBoard(cb.chessmanLocationBoard)
	{
		currentBannedMotions.forceCopyFrom(cb.currentBannedMotions);
		pickedChessmanByIndex.forceCopyFrom(cb.pickedChessmanByIndex);

		memcpy(loose, cb.loose, sizeof(bool)*CIGRuleConfig::PLAYER_NUM);
		memcpy(this->evaluations, cb.evaluations, sizeof(evaluations));

		for (int i = 0; i < CIGRuleConfig::PLAYER_NUM; ++i)
		{
			players[i] = cb.players[i];
			players[i].chessboard = this;
		}
	}

	void CIG::Chessboard::operator=( const Chessboard& cb )
	{
		currentBannedMotions.forceCopyFrom(cb.currentBannedMotions);
		pickedChessmanByIndex.forceCopyFrom(cb.pickedChessmanByIndex);
		nowRound = cb.nowRound;
		nowTurn = cb.nowTurn;

		this->chessmanLocationBoard = cb.chessmanLocationBoard;
		memcpy(loose, cb.loose, sizeof(bool)*CIGRuleConfig::PLAYER_NUM);
		memcpy(this->evaluations, cb.evaluations, sizeof(evaluations));

		for (int i = 0; i < CIGRuleConfig::PLAYER_NUM; ++i)
		{
			players[i] = cb.players[i];
			players[i].chessboard = this;
		}
	}
	
	//************************************
	// Method:    operator[]
	// FullName:  CIG::Chessboard::operator[]
	// Access:    public
	// Returns:   Chessman*
	// Qualifier: const
	// Parameter: PointOrVector p
	// 注意内部调用, 默认已经做过范围检查.
	//************************************
	CIG::Chessman* CIG::Chessboard::operator []( PointOrVector p ) const			// TO-DO 这里逻辑可以优化一下, 看看怎么检测比较好
	{
		if (beyondBoardRange(p))
		{
			return (Chessman*)(void*)-1;
		}
		ChessmanIndex& ci = chessmanLocationBoard.content[(p[1]<<CIGRuleConfig::INI_BOARD_WIDTH_LOG2) + p[0]];
		return (ci.player == -1) ? NULL : &(players[ci.player].ownedChessmans[ci.index]);
	}

	int CIG::Chessboard::getEvaluation( CIGRuleConfig::PLAYER_NAMES p ) const
	{
		// TO-DO 还要加入胜负, 包括平局, 判负
		int resualt = 0;

		for (int i=0;i<CIGRuleConfig::PLAYER_NUM;++i)
		{
			if (i!=nowTurn)
			{
				resualt-=evaluations[i];
			}
		}
		resualt = evaluations[p]+ (resualt)/(CIGRuleConfig::PLAYER_NUM-1);

		/*if ()
		{
		}*/

		return resualt;
	}

	int CIG::Chessboard::getEvaluation() const
	{
		return getEvaluation(nowTurn);
	}

	bool CIG::Chessboard::beyondBoardRange( PointOrVector& p )const
	{
		static const unsigned mask = (~(((unsigned short)1 << CIGRuleConfig::INI_BOARD_WIDTH_LOG2) - 1) << sizeof(unsigned short) * 8) | (unsigned short) - 1 & (~(((unsigned short)1 << CIGRuleConfig::INI_BOARD_HEIGHT_LOG2) - 1));

		if (mask&*((unsigned*)p.x))
		{
			return true;
		}
		else if (!CIGRuleConfig::BOARD_RANGE[p.x[1]][p.x[0]])
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	Chessman* Chessboard::onAddIntent(PointOrVector p, bool refreshEvaluations)
	{
		Chessman* c = players[nowTurn].ownedChessmans.add(Chessman(CIGRuleConfig::CHESS, p,nowTurn, players[nowTurn].ownedChessmans.size, CIGRuleConfig::OFF_BOARD, CIGRuleConfig::ALL));
		pickedChessmanByIndex.add(c->chessmanLocation);
		return c;
	}

	//穿脱原理下, 操作就是这么简单
	void Chessboard::undoAdd(bool refreshEvaluations)
	{
		pickedChessmanByIndex.deleteAtNoReturn(pickedChessmanByIndex.size-1);
		players[nowTurn].ownedChessmans.popNoReturn();
	}

	bool CIG::Chessboard::onPutIntent( Chessman* c, PointOrVector p , bool refreshEvaluations)
	{
		if ((*this)[p] != NULL)
		{
			return false;
		}
		else if ((pickedChessmanByIndex.size>0)&&(pickedChessmanByIndex[0] == c->chessmanLocation) && (c->onPutIntent(p)))
		{
			chessmanLocationBoard[p] = c->chessmanLocation;
			pickedChessmanByIndex.deleteAtNoReturn(0);
			if (refreshEvaluations)
			{
				static const int DIRECTION_NUM = 8;
				static const PointOrVector direction[DIRECTION_NUM] = 
				{
					PointOrVector(1,0),
					PointOrVector(1,1),
					PointOrVector(0,1),
					PointOrVector(-1,1),
					PointOrVector(-1,0),
					PointOrVector(-1,-1),
					PointOrVector(0,-1),
					PointOrVector(1,-1),
				};
				bool enemyOnTheEnd[DIRECTION_NUM];
				unsigned char numOfChessman[DIRECTION_NUM];
				unsigned char totalnumOfChessman[DIRECTION_NUM>>1];

				for (int d = 0;d<DIRECTION_NUM;++d)
				{
					enemyOnTheEnd[d] = false;
					numOfChessman[d] = 0;
					PointOrVector testPoint = c->coordinate;

					for (Chessman* testc = (*this)[testPoint+=direction[d]]; testc; testc = (*this)[testPoint+=direction[d]])
					{
						if (testc==(void*)-1)
						{
							enemyOnTheEnd[d] = true;
							break;
						}
						else if (!testc)
						{
							enemyOnTheEnd[d] = false;
							break;
						}
						else if (testc->chessmanLocation.player!=nowTurn)
						{
							enemyOnTheEnd[d] = true;
							break;
						}
						else
						{
							++numOfChessman[d];
						}
					}
				}

				for (int d=0;d<(DIRECTION_NUM>>1);++d)
				{
					totalnumOfChessman[d]= numOfChessman[d]+numOfChessman[d+4]+1;
					if (totalnumOfChessman[d]>=5)
					{
						loose[1-nowTurn] = true;
					}
					evaluations[nowTurn] -= GRADES[enemyOnTheEnd[d]+0][numOfChessman[d]];
					evaluations[nowTurn] -= GRADES[enemyOnTheEnd[d+4]+0][numOfChessman[d+4]];
					evaluations[nowTurn] +=GRADES[enemyOnTheEnd[d]+enemyOnTheEnd[d+4]][totalnumOfChessman[d]>5?5:totalnumOfChessman[d]];
				}
			}
			return true;
		}

		return false;
	}

	void CIG::Chessboard::undoPut(Chessman* c , bool refreshEvaluations)
	{
		if (refreshEvaluations)
		{
			static const int DIRECTION_NUM = 8;
			static const PointOrVector direction[DIRECTION_NUM] = 
			{
				PointOrVector(1,0),
				PointOrVector(1,1),
				PointOrVector(0,1),
				PointOrVector(-1,1),
				PointOrVector(-1,0),
				PointOrVector(-1,-1),
				PointOrVector(0,-1),
				PointOrVector(1,-1),
			};
			bool enemyOnTheEnd[DIRECTION_NUM];
			unsigned char numOfChessman[DIRECTION_NUM];
			unsigned char totalnumOfChessman[DIRECTION_NUM>>1];

			for (int d = 0;d<DIRECTION_NUM;++d)
			{
				enemyOnTheEnd[d] = false;
				numOfChessman[d] = 0;
				PointOrVector testPoint = c->coordinate;

				for (Chessman* testc = (*this)[testPoint+=direction[d]]; testc; testc = (*this)[testPoint+=direction[d]])
				{
					if (testc==(void*)-1)
					{
						enemyOnTheEnd[d] = true;
						break;
					}
					else if (!testc)
					{
						enemyOnTheEnd[d] = false;
						break;
					}
					else if (testc->chessmanLocation.player!=nowTurn)
					{
						enemyOnTheEnd[d] = true;
						break;
					}
					else
					{
						++numOfChessman[d];
					}
				}
			}

			for (int d=0;d<(DIRECTION_NUM>>1);++d)
			{
				totalnumOfChessman[d]= numOfChessman[d]+numOfChessman[d+4]+1;
					loose[1-nowTurn] = false;			//特别注意这里, 没输, 搜索时才会调用undo, 所以清空looose. 
				evaluations[nowTurn] += GRADES[enemyOnTheEnd[d]+0][numOfChessman[d]];
				evaluations[nowTurn] += GRADES[enemyOnTheEnd[d+4]+0][numOfChessman[d+4]];
				evaluations[nowTurn] -=GRADES[enemyOnTheEnd[d]+enemyOnTheEnd[d+4]][totalnumOfChessman[d]>5?5:totalnumOfChessman[d]];
			}
		}
		chessmanLocationBoard[c->coordinate].clear();
		c->undoPut();
		pickedChessmanByIndex.add(c->chessmanLocation);
	}

	bool CIG::Chessboard::onChangeTurn()
	{
		nowTurn = (CIGRuleConfig::PLAYER_NAMES)((nowTurn + 1) % (CIGRuleConfig::PLAYER_NUM));
		nowRound++;
		return true;
	}

	void Chessboard::undoChangeTurn()
	{
		nowTurn = (CIGRuleConfig::PLAYER_NAMES)((nowTurn - 1 + CIGRuleConfig::PLAYER_NUM) % (CIGRuleConfig::PLAYER_NUM));
		nowRound--;
	}

	bool CIG::Chessboard::onActionIntent(Action& action, bool refreshEvaluations)
	{
		bool result = true;

		for (int i = 0; i < action.size; i++)
		{
			result  &= onOperationIntent(action[i], refreshEvaluations);
		}

		result &= (this->pickedChessmanByIndex.size == 0);
		return result;
	}

	bool Chessboard::onOperationIntent( Operation& op , bool refreshEvaluations)
	{
		switch (op.operation)
		{
		case CIGRuleConfig::ADD:
			if (onAddIntent(op.distination ,refreshEvaluations))
			{
				return true;
			}
			break;
		case CIGRuleConfig::PICK:
			return false;

			break;

		case CIGRuleConfig::PUT:
			if(!onPutIntent(&(this->players[op.chessmanIndex.player].ownedChessmans[op.chessmanIndex.index]),op.distination, refreshEvaluations))
			{
				return false;
			}

			break;

		case CIGRuleConfig::CAPTURE:
				return false;
			break;

		case CIGRuleConfig::PROMOTION:
				return false;
			break;
		default:
			return false;
			break;
		}
		return true;
	}

	bool CIG::Chessboard::canMakeWholeAction(Action& action, bool refreshEvaluations)
	{
		Chessboard cb(*this);
		bool result = cb.onWholeActionIntent(action, refreshEvaluations);
		return result;
	}

	void Chessboard::undoAction( Action& action , bool refreshEvaluations)
	{
		for (int i = action.size-1; i >=0; --i)				////应该倒着恢复!!!
		{
			undoOperation(action[i], refreshEvaluations);
		}
	}

	// TO-DO这个函数还没有写就拿去调试了, 当然不会有好结果! 
	void Chessboard::undoOperation( Operation& operation , bool refreshEvaluations)
	{
		switch (operation.operation)
		{
		case CIGRuleConfig::ADD:
			undoAdd(refreshEvaluations);
			break;

		case CIGRuleConfig::PICK:
			undoPick(&(this->players[operation.chessmanIndex.player].ownedChessmans[operation.chessmanIndex.index]),operation.distination, refreshEvaluations);
			break;

		case CIGRuleConfig::PUT:
			undoPut(&(this->players[operation.chessmanIndex.player].ownedChessmans[operation.chessmanIndex.index]), refreshEvaluations);
			break;

		case CIGRuleConfig::CAPTURE:
			undoCaptured(&(this->players[operation.chessmanIndex.player].ownedChessmans[operation.chessmanIndex.index]), refreshEvaluations);
			break;

		default:
			break;
		}
	}

	bool Chessboard::onWholeActionIntent( Action& action , bool refreshEvaluations)
	{
		return onActionIntent(action, refreshEvaluations)&&onChangeTurn();
	}

	void Chessboard::undoWholeAction( Action& action , bool refreshEvaluations)
	{
		undoChangeTurn();				//穿脱原理
		undoAction(action,refreshEvaluations);
	}

	bool CIG::Chessboard::onPickIntent( PointOrVector p , bool refreshEvaluations)
	{
		return onPickIntent((*this)[p], refreshEvaluations);
	}

	void CIG::Chessboard::undoPick(Chessman* c , PointOrVector p, bool refreshEvaluations)
	{
		c->coordinate = p;
		c->undoPick();
		//evaluations
	}

	void CIG::Chessboard::undoPromotion(Chessman* c, CIGRuleConfig::CHESSMAN_TYPES t, bool refreshEvaluations)
	{
		//TODO
	}
	void CIG::Chessboard::undoPromotion(PointOrVector p, CIGRuleConfig::CHESSMAN_TYPES t, bool refreshEvaluations)
	{
		//TODO
	}

	bool CIG::Chessboard::onCaptureIntent( Chessman* c, PointOrVector p , bool refreshEvaluations)
	{
		return false;
	}

	bool CIG::Chessboard::onPromotionIntent( Chessman* c, CIGRuleConfig::CHESSMAN_TYPES t , bool refreshEvaluations)
	{
		return false;
	}

	bool CIG::Chessboard::onPickIntent( Chessman* c , bool refreshEvaluations)
	{
		return false;
	}

	bool CIG::Chessboard::onPromotionIntent( PointOrVector p, CIGRuleConfig::CHESSMAN_TYPES t , bool refreshEvaluations)
	{
		return onPromotionIntent((*this)[p], t, refreshEvaluations);
	}

	void Chessboard::undoCaptured( Chessman* c , bool refreshEvaluations)
	{
	}

	void Chessboard::calEvaluations()
	{

	}

	const int Chessboard::GRADES[3][6]= 
	{
		{0,30,100,1000,MATE_VALUE-1,MATE_VALUE},
		{0,10,50,200,1000,MATE_VALUE},
		{0,0,0,0,0,MATE_VALUE},
	};
}