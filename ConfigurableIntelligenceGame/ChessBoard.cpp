//#include "stdafx.h"
#include "Chessboard.h"
#include "Player.h"
#include "Array.h"
#include "Chessman.h"
#include "ChessmanLocation.h"

namespace CIG
{
	// 按配置初始化很少执行, 所以可以速度慢一点.
	CIG::Chessboard::Chessboard() : nowRound(0), nowTurn((CIGRuleConfig::PLAYER_NAMES)0), pickedChessmanByLocation("PickedChessman"), currentBannedMotions("currentBannedMotions")
	{
		players[0] = Player(CIGRuleConfig::HUMAN, this);
		players[1] = Player(CIGRuleConfig::COMPUTER, this);			// TO-DO  应该是智能引擎实例而不是player类实例.
		memset(chessmanLocationBoard, -1, sizeof(ChessmanLocation) << (CIGRuleConfig::INI_BOARD_WIDTH_LOG2 + CIGRuleConfig::INI_BOARD_HEIGHT_LOG2));			//如果指针不清空初始值根本不是0

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
						Chessman c = Chessman(/*string("test"), */t, PointOrVector(j, i), (CIGRuleConfig::PLAYER_NAMES)k, players[k].ownedChessmans.size, CIGRuleConfig::ON_BOARD, CIGRuleConfig::ALL);
						players[k].ownedChessmans.push(c
													   // const string& str, CIGRuleConfig::CHESSMAN_TYPES t, const PointOrVector& c, CIGRuleConfig::PLAYER_NAMES p, CIGRuleConfig::CHESSMAN_STATUS s, CIGRuleConfig::VISIBILITIES v
													   //Chessman(/*string("test"), */t, PointOrVector(j, i), (CIGRuleConfig::PLAYER_NAMES)k, players[k].ownedChessmans.size, CIGRuleConfig::ON_BOARD, CIGRuleConfig::ALL)			// TO-DO
													  );

						chessmanLocationBoard[i][j] = ChessmanLocation((CIGRuleConfig::PLAYER_NAMES)k, players[k].ownedChessmans.size - 1);
						evaluations[k] += CIGRuleConfig::EVALUATIONS[k][players[k].ownedChessmans.top().chessmanType][i][j];
					}
				}
			}
		}
	}

	CIG::Chessboard::Chessboard( const Chessboard& cb ):
		currentBannedMotions(cb.currentBannedMotions),
		pickedChessmanByLocation(cb.pickedChessmanByLocation),
		nowRound(cb.nowRound), nowTurn(cb.nowTurn)
	{
		//一定不要破坏底层封装, 这样极大地影响了系统的可移植性. 应该调用成员的响应函数, 如何操作数据由成员决定.
		memcpy(this->chessmanLocationBoard, cb.chessmanLocationBoard, sizeof(ChessmanLocation) << (CIGRuleConfig::INI_BOARD_WIDTH_LOG2 + CIGRuleConfig::INI_BOARD_HEIGHT_LOG2));
		memcpy(this->evaluations, cb.evaluations, sizeof(evaluations));

		for (int i = 0; i < CIGRuleConfig::PLAYER_NUM; ++i)
		{
			players[i] = cb.players[i];
			players[i].chessboard = this;
		}
	}

	void CIG::Chessboard::operator=( const Chessboard& cb )
	{
		//memcpy(this,&cb,4);
		currentBannedMotions = cb.currentBannedMotions;
		pickedChessmanByLocation = cb.pickedChessmanByLocation;
		nowRound = cb.nowRound;
		nowTurn = cb.nowTurn;

		memcpy(this->chessmanLocationBoard, cb.chessmanLocationBoard, sizeof(ChessmanLocation) << (CIGRuleConfig::INI_BOARD_WIDTH_LOG2 + CIGRuleConfig::INI_BOARD_HEIGHT_LOG2));
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

	bool CIG::Chessboard::onPickIntent( Chessman* c , PointOrVector p)
	{
		if ((c->chessmanLocation.player == nowTurn) && (pickedChessmanByLocation.size == 0) && (c->onPickIntent()))
		{
			pickedChessmanByLocation.add(c->chessmanLocation);
			chessmanLocationBoard[p.x[1]][p.x[0]].clear();
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
			chessmanLocationBoard[p.x[1]][p.x[0]] = c->chessmanLocation;
			evaluations[nowTurn] += CIGRuleConfig::EVALUATIONS[nowTurn][c->chessmanType][p.x[1]][p.x[0]];
			pickedChessmanByLocation.deleteAt(0);
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
					chessmanLocationBoard[p.x[1]][p.x[0]].clear();
					evaluations[testC->chessmanLocation.player] -= CIGRuleConfig::EVALUATIONS[testC->chessmanLocation.player][testC->chessmanType][p.x[1]][p.x[0]];
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
		ChessmanLocation cl = chessmanLocationBoard[p.x[1]][p.x[0]];
		Chessman* pc = &(players[cl.player].ownedChessmans[cl.index]);
		return (cl.player == -1) ? NULL : pc;
	}

	int CIG::Chessboard::getEvaluation( CIGRuleConfig::PLAYER_NAMES p ) const
	{
		// TO-DO 还要加入胜负, 包括平局, 判负
		return evaluations[p] - evaluations[1 - p];
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

		for (int i = 0; i < action.size - 1; i++)
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
					if(this->onPutIntent(&(this->players[action[i].chessmanLocation.player].ownedChessmans[action[i].chessmanLocation.index])))
					{
						return false;
					}

					break;

				case CIGRuleConfig::CAPTURE:
					//result&=this->onCaptureIntent(op[i].chessman);		// TO-DO
					break;

				default:
					return false;
					break;
			}

			return true;
		}

		result &= (this->pickedChessmanByLocation.size == 0);
		return result;
	}

	bool CIG::Chessboard::canMakeAction(OperationStack& action)
	{
		Chessboard cb(*this);

		return cb.makeAction(action);
	}

	CIG::Chessboard::~Chessboard()
	{
	}

}


//void Chessboard::undoCapture( PointOrVector p )
//{
//	undoCapture(*(*this)[p]);
//}

//void Chessboard::undoCapture( Chessman& c )
//{
//	c.undoCapture();
//	evaluations[c.belongsToWhom] += CIGRuleConfig::EVALUATIONS[c.belongsToWhom][c.chessmanType][c.coordinate.x[1]][c.coordinate.x[0]];
//}