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
	CIG::Chessboard::Chessboard() : nowRound(0), nowTurn((CIGRuleConfig::PLAYER_NAMES)0), pickedChessmanByLocation(), currentBannedMotions(),chessmanLocationBoard()
	{
		players[0] = Player(CIGRuleConfig::HUMAN, GraphSearchEngine::makeBestAction, this);
		players[1] = Player(CIGRuleConfig::COMPUTER, GraphSearchEngine::makeBestAction, this);			// TO-DO  应该是智能引擎实例而不是player类实例.

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

						chessmanLocationBoard[PointOrVector(j,i)] = ChessmanLocation((CIGRuleConfig::PLAYER_NAMES)k, players[k].ownedChessmans.size - 1);
						evaluations[k] += CIGRuleConfig::EVALUATIONS[k][players[k].ownedChessmans.top().chessmanType][i][j];
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
		pickedChessmanByLocation.forceCopyFrom(cb.pickedChessmanByLocation);

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
		pickedChessmanByLocation.forceCopyFrom(cb.pickedChessmanByLocation);
		nowRound = cb.nowRound;
		nowTurn = cb.nowTurn;

		this->chessmanLocationBoard = cb.chessmanLocationBoard;
		memcpy(this->evaluations, cb.evaluations, sizeof(evaluations));

		for (int i = 0; i < CIGRuleConfig::PLAYER_NUM; ++i)
		{
			players[i] = cb.players[i];
			players[i].chessboard = this;
		}
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

	bool CIG::Chessboard::onSelfHalfOfBoard( PointOrVector& p )
	{
		return !beyondBoardRange(p) && ((nowTurn == CIGRuleConfig::COMPUTER) ? !(p.x[1] & 8) : (p.x[1] & 8));
	}

	bool CIG::Chessboard::onPickIntent( PointOrVector p )
	{
		return onPickIntent((*this)[p], p);
	}

	void CIG::Chessboard::undoPick(PointOrVector p)
	{
		undoPick((*this)[p],p);
	}
	void CIG::Chessboard::undoPick(Chessman* c )
	{
		undoPick(c,c->coordinate);
	}
	void CIG::Chessboard::undoPick(Chessman* c , PointOrVector p)
	{
		c->undoPick();
		pickedChessmanByLocation.clear();
		chessmanLocationBoard[p] = c->chessmanLocation;
		evaluations[nowTurn] += CIGRuleConfig::EVALUATIONS[nowTurn][c->chessmanType][p.x[1]][p.x[0]];
	}
	void CIG::Chessboard::undoPut(Chessman* c)
	{
		undoPut(c,c->coordinate);
	}

	void CIG::Chessboard::undoPut(Chessman* c, PointOrVector p)
	{
		c->undoPut();
		pickedChessmanByLocation.add(c->chessmanLocation);
		evaluations[nowTurn] -= CIGRuleConfig::EVALUATIONS[nowTurn][c->chessmanType][p.x[1]][p.x[0]];
	}

	void CIG::Chessboard::undoCapture(Chessman* c, PointOrVector p)
	{
		c->undoCapture();
		chessmanLocationBoard[p] = c->chessmanLocation;
		evaluations[c->chessmanLocation.player] += (c->chessmanType==CIGRuleConfig::KING)?MATE_VALUE:(CIGRuleConfig::EVALUATIONS[c->chessmanLocation.player][c->chessmanType][p.x[1]][p.x[0]]);
	}

	void CIG::Chessboard::undoPromotion(Chessman* c, CIGRuleConfig::CHESSMAN_TYPES t)
	{
		//TODO
	}
	void CIG::Chessboard::undoPromotion(PointOrVector p, CIGRuleConfig::CHESSMAN_TYPES t)
	{
		//TODO
	}

	bool CIG::Chessboard::onPickIntent( Chessman* c , PointOrVector p)
	{
		if ((c->chessmanLocation.player == nowTurn) && (pickedChessmanByLocation.size == 0) && (c->onPickIntent()))
		{
			pickedChessmanByLocation.add(c->chessmanLocation);
			chessmanLocationBoard[p].clear();
			evaluations[nowTurn] -= CIGRuleConfig::EVALUATIONS[nowTurn][c->chessmanType][p.x[1]][p.x[0]];
			return true;
		}

		return false;
	}

	bool CIG::Chessboard::onPickIntent( Chessman* c )
	{
		return onPickIntent(c, c->coordinate);
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
		else if ((pickedChessmanByLocation.size>0)&&(pickedChessmanByLocation[0] == c->chessmanLocation) && (c->onPutIntent(p)))
		{
			chessmanLocationBoard[p] = c->chessmanLocation;
			evaluations[nowTurn] += CIGRuleConfig::EVALUATIONS[nowTurn][c->chessmanType][p.x[1]][p.x[0]];
			pickedChessmanByLocation.deleteAtThenGet(0);
			return true;
		}

		return false;
	}

	bool CIG::Chessboard::onPutIntent( Chessman* c )
	{
		return onPutIntent(c, c->coordinate);
	}

	bool CIG::Chessboard::onCaptureIntent( Chessman* c, PointOrVector p )
	{
		if (beyondBoardRange(p))
		{
			return false;
		}
		else
		{
			Chessman* testC = (*this)[p];

			if ((testC == NULL) || (testC->chessmanLocation.player == nowTurn))
			{
				return false;
			}
			else if (pickedChessmanByLocation[0] == c->chessmanLocation)
			{
				if (testC->onCapturedIntent())
				{
					c->onCaptureIntent(testC);
					chessmanLocationBoard[p].clear();
					evaluations[testC->chessmanLocation.player] -= (testC->chessmanType==CIGRuleConfig::KING)?MATE_VALUE:(CIGRuleConfig::EVALUATIONS[testC->chessmanLocation.player][testC->chessmanType][p.x[1]][p.x[0]]);
					return true;
				}
				else
				{
					return false;
				}
			}
		}

		return false;
	}

	bool CIG::Chessboard::onPromotionIntent( Chessman* c, CIGRuleConfig::CHESSMAN_TYPES t )
	{
		if ((c->chessmanLocation.player == nowTurn) && c->onPromotionIntent(t))
		{
			evaluations[nowTurn] += (CIGRuleConfig::EVALUATIONS[nowTurn][t][c->coordinate.x[1]][c->coordinate.x[0]] - CIGRuleConfig::EVALUATIONS[nowTurn][c->chessmanType][c->coordinate.x[1]][c->coordinate.x[0]]);
			return true;
		}

		return false;
	}

	bool CIG::Chessboard::onPromotionIntent( PointOrVector p, CIGRuleConfig::CHESSMAN_TYPES t )
	{
		return onPromotionIntent((*this)[p], t);
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
		ChessmanLocation& cl = chessmanLocationBoard.content[(p[1]<<CIGRuleConfig::INI_BOARD_WIDTH_LOG2) + p[0]];
		return (cl.player == -1) ? NULL : &(players[cl.player].ownedChessmans[cl.index]);
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

	bool CIG::Chessboard::onChangeTurn()
	{
		nowTurn = (CIGRuleConfig::PLAYER_NAMES)((nowTurn + 1) % (CIGRuleConfig::PLAYER_NUM));
		return true;
	}

	bool CIG::Chessboard::makeAction(OperationStack& action)
	{
		bool result = true;

		for (int i = 0; i < action.size; i++)
		{
			//BEGIN,
			//PICK,						//拿起
			//PUT,							//走棋
			//CAPTURE,					//吃子
			//PROMOTION,			//升变
			//DECOVER,				//掀开
			//END,							//作为走法生成器返回的提示语: 一条搜索路径结束, 请保存结果并回退搜索.
			//NOMORE					//作为走法生成器返回的提示语: 无更多操作, 请回退搜索.

			switch (action[i].operation)
			{
				case CIGRuleConfig::PICK:
					if (!onPickIntent(&(this->players[action[i].chessmanLocation.player].ownedChessmans[action[i].chessmanLocation.index])))
					{
						return false;
					}

					break;

				case CIGRuleConfig::PUT:
					if(!onPutIntent(&(this->players[action[i].chessmanLocation.player].ownedChessmans[action[i].chessmanLocation.index]),action[i].distination))
					{
						return false;
					}

					break;

				case CIGRuleConfig::CAPTURE:
					if(!onCaptureIntent(&(this->players[action[i].chessmanLocation.player].ownedChessmans[action[i].chessmanLocation.index]),action[i].distination))
					{
						return false;
					}

					break;

				default:
					return false;
					break;
			}
		}

		result &= (this->pickedChessmanByLocation.size == 0);
		return result;
	}

	bool CIG::Chessboard::canMakeAction(OperationStack& action)
	{
		Chessboard cb(*this);
		bool result = cb.makeAction(action);
		return result;
	}

	void Chessboard::undoChangeTurn()
	{

	}

}