//#include "stdafx.h"
#include "ChessBoard.h"
#include "Player.h"
#include "Array.h"
#include "Chessman.h"

CIG::ChessBoard::ChessBoard() : nowRound(0), nowTurn((CIGRuleConfig::PLAYER_NAMES)0), pickedChessman("PickedChessman"), currentBannedMotions("currentBannedMotions")
{
	players[0] = new Player(CIGRuleConfig::HUMAN);
	players[1] = new Player(CIGRuleConfig::COMPUTER);			// TO-DO  应该是智能引擎实例而不是player类实例. 
	memset(mChessmanBoard,0,sizeof(Chessman*)<<(CIGRuleConfig::INI_BOARD_WIDTH_LOG2+CIGRuleConfig::INI_BOARD_HEIGHT_LOG2));			//如果指针不清空初始值根本不是0 
	
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
					players[k]->ownedChessmans.push_back(
						// const string& str, CIGRuleConfig::CHESSMAN_TYPES t, const PointOrVector& c, CIGRuleConfig::PLAYER_NAMES p, CIGRuleConfig::CHESSMAN_STATUS s, CIGRuleConfig::VISIBILITIES v
						Chessman(/*string("test"), */t, PointOrVector(j, i), (CIGRuleConfig::PLAYER_NAMES)k, CIGRuleConfig::ON_BOARD, CIGRuleConfig::ALL)			// TO-DO
					);
					Chessman* temp = &(players[k]->ownedChessmans.back());
					mChessmanBoard[i][j] = temp;
					evaluations[k] += CIGRuleConfig::EVALUATIONS[k][temp->chessmanType][i][j];
				}
			}
		}
	}
}

CIG::ChessBoard::ChessBoard( const ChessBoard& cb ):
	currentBannedMotions(cb.currentBannedMotions),
	pickedChessman(cb.pickedChessman.entityName),			//注意只用了名字初始化. 
	nowRound(cb.nowRound), nowTurn(cb.nowTurn)
{
	memset(mChessmanBoard,0,sizeof(Chessman*)<<(CIGRuleConfig::INI_BOARD_WIDTH_LOG2+CIGRuleConfig::INI_BOARD_HEIGHT_LOG2));			//如果指针不清空初始值根本不是0 
	for (int p = 0; p < CIGRuleConfig::PLAYER_NUM; ++p)
	{
		players[p] = new Player(*(cb.players[p]));

		evaluations[p] += cb.evaluations[p];
		
		for(int i= (int)players[p]->ownedChessmans.size()-1; i>=0; --i)
		{
			this->mChessmanBoard[players[p]->ownedChessmans[i].coordinate.x[1]][players[p]->ownedChessmans[i].coordinate.x[0]] = &players[p]->ownedChessmans[i];
		}	
	}
	for (int i=cb.pickedChessman.size-1 ;i>=0;  --i)
	{
		pickedChessman.add((*this)[cb.pickedChessman[i]->coordinate]);
	}
	memcpy(this->evaluations, cb.evaluations, sizeof(int)* CIGRuleConfig::PLAYER_NUM);
}

void CIG::ChessBoard::operator=( const ChessBoard& cb )
{
	//这个函数没写完

	ChessBoard temp(cb);
	//this->nowRound = cb.nowRound;
	//this->nowTurn = cb.nowTurn;

	//memcpy(players,cb.players,sizeof(Player*)*CIGRuleConfig::PLAYER_NUM);
	//memcpy(evaluations,cb.evaluations,sizeof(int)*CIGRuleConfig::PLAYER_NUM);
	//currentBannedMotions
	memcpy(this,&temp,sizeof(ChessBoard));				// TO-DO 调试是否正确.
}

bool CIG::ChessBoard::beyondBoardRange( PointOrVector& p )
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

bool CIG::ChessBoard::onSelfHalfOfBoard( PointOrVector& p )
{
	return !beyondBoardRange(p) && ((nowTurn == 1) ? (p.x[1] & 8) : !(p.x[1] & 8));
}

bool CIG::ChessBoard::onPickIntent( PointOrVector p )
{
	return onPickIntent((*this)[p], p);
}

bool CIG::ChessBoard::onPickIntent( Chessman* c , PointOrVector p)
{
	if ((c->belongsToWhom == nowTurn) && (pickedChessman.size == 0) && (c->onPickIntent()))
	{
		pickedChessman.add(c);
		mChessmanBoard[p.x[1]][p.x[0]] = NULL;
		evaluations[nowTurn] -= CIGRuleConfig::EVALUATIONS[nowTurn][c->chessmanType][p.x[1]][p.x[0]];
		return true;
	}

	return false;
}

bool CIG::ChessBoard::onPickIntent( Chessman* c )
{
	return onPickIntent(c, c->coordinate);
}

bool CIG::ChessBoard::onPutIntent( Chessman* c, PointOrVector p )
{
	if (beyondBoardRange(p))
	{
		return false;
	}
	else if ((*this)[p] != NULL)
	{
		return false;
	}
	else if ((pickedChessman[0] == c) && (c->onPutIntent(p)))
	{
		mChessmanBoard[p.x[1]][p.x[0]] = c;
		evaluations[nowTurn] += CIGRuleConfig::EVALUATIONS[nowTurn][c->chessmanType][p.x[1]][p.x[0]];
		pickedChessman.deleteAt(0);
		return true;
	}

	return false;
}

bool CIG::ChessBoard::onPutIntent( Chessman* c )
{
	return onPutIntent(c, c->coordinate);
}

bool CIG::ChessBoard::onCaptureIntent( Chessman* c, PointOrVector p )
{
	if (beyondBoardRange(p))
	{
		return false;
	}
	else
	{
		Chessman* testC = (*this)[p];

		if ((testC == NULL) || (testC->belongsToWhom == nowTurn))
		{
			return false;
		}
		else if (pickedChessman[0] == c)
		{
			if (testC->onCapturedIntent())
			{
				c->onCaptureIntent(testC);
				mChessmanBoard[p.x[1]][p.x[0]] = NULL;
				evaluations[testC->belongsToWhom] -= CIGRuleConfig::EVALUATIONS[testC->belongsToWhom][testC->chessmanType][p.x[1]][p.x[0]];
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

bool CIG::ChessBoard::onPromotionIntent( Chessman* c, CIGRuleConfig::CHESSMAN_TYPES t )
{
	if ((c->belongsToWhom == nowTurn) && c->onPromotionIntent(t))
	{
		evaluations[nowTurn] += (CIGRuleConfig::EVALUATIONS[nowTurn][t][c->coordinate.x[1]][c->coordinate.x[0]] - CIGRuleConfig::EVALUATIONS[nowTurn][c->chessmanType][c->coordinate.x[1]][c->coordinate.x[0]]);
		return true;
	}

	return false;
}

bool CIG::ChessBoard::onPromotionIntent( PointOrVector p, CIGRuleConfig::CHESSMAN_TYPES t )
{
	return onPromotionIntent((*this)[p], t);
}

//************************************
// Method:    operator[]
// FullName:  CIG::ChessBoard::operator[]
// Access:    public
// Returns:   Chessman*
// Qualifier: const
// Parameter: PointOrVector p
// 注意内部调用, 默认已经做过范围检查.
//************************************
CIG::Chessman* CIG::ChessBoard::operator []( PointOrVector p ) const
{
	return mChessmanBoard[p.x[1]][p.x[0]];
}

int CIG::ChessBoard::getEvaluation( CIGRuleConfig::PLAYER_NAMES p ) const
{
	// TO-DO 还要加入胜负, 平局, 判负
	return evaluations[p] - evaluations[1 - p];
}

int CIG::ChessBoard::getEvaluation() const
{
	return getEvaluation(nowTurn);
}

bool CIG::ChessBoard::onChangeTurn()
{
	nowTurn = (CIGRuleConfig::PLAYER_NAMES)((nowTurn + 1) % (CIGRuleConfig::PLAYER_NUM));
	return true;
}

bool CIG::ChessBoard::makeAction(OperationStack& action)
{
	bool result = true;

	for (int i = 0; i < action.size-1; i++)
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
				if (!onPickIntent(action[i].chessman))
				{
					return false;
				}
				break;

			case CIGRuleConfig::PUT:
				if(this->onPutIntent(action[i].chessman))
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

	result &= (this->pickedChessman.size == 0);
	return result;
}

bool CIG::ChessBoard::canMakeAction(OperationStack& action)
{
	ChessBoard cb(*this);

	return cb.makeAction(action);
}

CIG::ChessBoard::~ChessBoard()
{
	for (int i = 0; i<CIGRuleConfig::PLAYER_NUM; ++i)
	{
		delete players[i];
	}
}


//void ChessBoard::undoCapture( PointOrVector p )
//{
//	undoCapture(*(*this)[p]);
//}

//void ChessBoard::undoCapture( Chessman& c )
//{
//	c.undoCapture();
//	evaluations[c.belongsToWhom] += CIGRuleConfig::EVALUATIONS[c.belongsToWhom][c.chessmanType][c.coordinate.x[1]][c.coordinate.x[0]];
//}