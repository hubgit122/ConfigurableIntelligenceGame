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
						//evaluations[k] += CIGRuleConfig::EVALUATIONS[k][players[k].ownedChessmans.top().chessmanType][i][j];
					}
				}
			}
		}
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
	CIG::Chessman* CIG::Chessboard::operator []( PointOrVector p ) const
	{
		ChessmanIndex& ci = chessmanLocationBoard.content[(p[1]<<CIGRuleConfig::INI_BOARD_WIDTH_LOG2) + p[0]];
		return (ci.player == -1) ? NULL : &(players[ci.player].ownedChessmans[ci.index]);
	}

	int CIG::Chessboard::getEvaluation( CIGRuleConfig::PLAYER_NAMES p ) const
	{
		// TO-DO 还要加入胜负, 包括平局, 判负
		int resualt = evaluations[p];

		for (int i=0;i<CIGRuleConfig::PLAYER_NUM;++i)
		{
			if (i!=nowTurn)
			{
				resualt-=evaluations[i];
			}
		}

		/*if ()
		{
		}*/

		return resualt;
	}

	int CIG::Chessboard::getEvaluation() const
	{
		return getEvaluation(nowTurn);
	}

	bool CIG::Chessboard::beyondBoardRange( PointOrVector& p )
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

	Chessman* Chessboard::onAddIntent(PointOrVector p)
	{
		Chessman* c = players[nowTurn].ownedChessmans.add(Chessman(CIGRuleConfig::CHESS, p,nowTurn, players[nowTurn].ownedChessmans.size, CIGRuleConfig::OFF_BOARD, CIGRuleConfig::ALL));
		pickedChessmanByIndex.add(c->chessmanLocation);
		return c;
	}

	//穿脱原理下, 操作就是这么简单
	void Chessboard::undoAdd()
	{
		pickedChessmanByIndex.deleteAtNoReturn(pickedChessmanByIndex.size-1);
		players[nowTurn].ownedChessmans.popNoReturn();
	}

	bool CIG::Chessboard::onPutIntent( Chessman* c, PointOrVector p )
	{
		if (beyondBoardRange(p))
		{
			return false;
		}
		else if ((*this)[p] != NULL)
		{
			return false;
		}
		else if ((pickedChessmanByIndex.size>0)&&(pickedChessmanByIndex[0] == c->chessmanLocation) && (c->onPutIntent(p)))
		{
			chessmanLocationBoard[p] = c->chessmanLocation;
			pickedChessmanByIndex.deleteAtNoReturn(0);
			//evaluations[nowTurn] += CIGRuleConfig::EVALUATIONS[nowTurn][c->chessmanType][p.x[1]][p.x[0]];
			return true;
		}

		return false;
	}

	void CIG::Chessboard::undoPut(Chessman* c, PointOrVector previousP)
	{
		//evaluations[nowTurn] -= CIGRuleConfig::EVALUATIONS[nowTurn][c->chessmanType][c->coordinate[1]][c->coordinate[0]];
		chessmanLocationBoard[c->coordinate].clear();
		c->undoPut(previousP);
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

	bool CIG::Chessboard::onActionIntent(Action& action)
	{
		bool result = true;

		for (int i = 0; i < action.size; i++)
		{
			result  &= onOperationIntent(action[i]);
		}

		result &= (this->pickedChessmanByIndex.size == 0);
		return result;
	}

	bool Chessboard::onOperationIntent( Operation& op )
	{
		switch (op.operation)
		{
		case CIGRuleConfig::ADD:
			if (onAddIntent(op.distination))
			{
				return true;
			}
			break;
		case CIGRuleConfig::PICK:
			return false;

			break;

		case CIGRuleConfig::PUT_CHESS:
			if(!onPutIntent(&(this->players[op.chessmanIndex.player].ownedChessmans[op.chessmanIndex.index]),op.distination))
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

	bool CIG::Chessboard::canMakeWholeAction(Action& action)
	{
		Chessboard cb(*this);
		bool result = cb.onWholeActionIntent(action);
		return result;
	}

	void Chessboard::undoAction( Action& action )
	{
		for (int i = action.size-1; i >=0; --i)				////应该倒着恢复!!!
		{
			undoOperation(action[i]);
		}
	}

	// TO-DO这个函数还没有写就拿去调试了, 当然不会有好结果! 
	void Chessboard::undoOperation( Operation& operation )
	{
		switch (operation.operation)
		{
		case CIGRuleConfig::ADD:
			undoAdd();
			break;

		case CIGRuleConfig::PICK:
			undoPick(&(this->players[operation.chessmanIndex.player].ownedChessmans[operation.chessmanIndex.index]));
			break;

		case CIGRuleConfig::PUT_CHESS:
			undoPut(&(this->players[operation.chessmanIndex.player].ownedChessmans[operation.chessmanIndex.index]),operation.savedCoodinate);
			break;

		case CIGRuleConfig::CAPTURE:
			undoCaptured(&(this->players[operation.chessmanIndex.player].ownedChessmans[operation.chessmanIndex.index]));
			break;

		default:
			break;
		}
	}

	bool Chessboard::onWholeActionIntent( Action& action )
	{
		return onActionIntent(action)&&onChangeTurn();
	}

	void Chessboard::undoWholeAction( Action& action )
	{
		undoChangeTurn();				//穿脱原理
		undoAction(action);
	}

	bool CIG::Chessboard::onPickIntent( PointOrVector p )
	{
		return onPickIntent((*this)[p]);
	}

	void CIG::Chessboard::undoPick(PointOrVector p)
	{
		undoPick((*this)[p]);
	}
	void CIG::Chessboard::undoPick(Chessman* c )
	{
	}

	void CIG::Chessboard::undoPromotion(Chessman* c, CIGRuleConfig::CHESSMAN_TYPES t)
	{
		//TODO
	}
	void CIG::Chessboard::undoPromotion(PointOrVector p, CIGRuleConfig::CHESSMAN_TYPES t)
	{
		//TODO
	}

	bool CIG::Chessboard::onCaptureIntent( Chessman* c, PointOrVector p )
	{
		return false;
	}

	bool CIG::Chessboard::onPromotionIntent( Chessman* c, CIGRuleConfig::CHESSMAN_TYPES t )
	{
		return false;
	}

	bool CIG::Chessboard::onPickIntent( Chessman* c )
	{
		return false;
	}

	bool CIG::Chessboard::onPromotionIntent( PointOrVector p, CIGRuleConfig::CHESSMAN_TYPES t )
	{
		return onPromotionIntent((*this)[p], t);
	}

	void Chessboard::undoCaptured( Chessman* c )
	{
	}

}