#include "stdafx.h"
#include "Chessboard.h"
#include "Player.h"
#include "Array.h"
#include "Chessman.h"
#include "ChessmanIndex.h"
#include "GraphSearchEngine.h"


namespace CIG
{
	const PointOrVector Chessboard::DISTAINATION[CIGRuleConfig::PLAYER_NUM] = 
	{
		PointOrVector(4,0),
		PointOrVector(0,4),
		PointOrVector(4,12),
		PointOrVector(12,16),
		PointOrVector(16,12),
		PointOrVector(12,4),
	};
	// 按配置初始化很少执行, 所以可以速度慢一点.
	CIG::Chessboard::Chessboard() : nowRound(0), nowTurn((CIGRuleConfig::PLAYER_NAMES)0), pickedChessmanByIndex(), currentBannedMotions(),chessmanIndexBoard()
	{
		players[0] = Player(CIGRuleConfig::A, GUI::askForMove, this);												//测试. 
		players[1] = Player(CIGRuleConfig::B, GraphSearchEngine::makeBestMove, this);
		players[2] = Player(CIGRuleConfig::C, GUI::askForMove, this);
		players[3] = Player(CIGRuleConfig::D, GraphSearchEngine::makeBestMove, this);
		players[4] = Player(CIGRuleConfig::E, GraphSearchEngine::makeBestMove, this);
		players[5] = Player(CIGRuleConfig::F, GraphSearchEngine::makeBestMove, this);			// TO-DO  应该是智能引擎实例而不是player类实例.

		memset(loose, 0, sizeof(bool)*CIGRuleConfig::PLAYER_NUM);
		memset(win, 0, sizeof(bool)*CIGRuleConfig::PLAYER_NUM);

		for (int k = 0; k < CIGRuleConfig::PLAYER_NUM; ++k)
		{
			evaluations[k] = 0;

			for (int i = 0; i < (1 << CIGRuleConfig::INI_BOARD_HEIGHT_LOG2); ++i)
			{
				for (int j = 0 ; j < (1 << CIGRuleConfig::INI_BOARD_WIDTH_LOG2); ++j)
				{
					CIGRuleConfig::CHESSMAN_TYPES t = CIGRuleConfig::INI_BOARD[k][i][j];

					if ((t != CIGRuleConfig::NOCHESSMAN)&&(!beyondBoardRange(PointOrVector(j,i))))
					{
						Chessman c = Chessman(t, PointOrVector(j, i), (CIGRuleConfig::PLAYER_NAMES)k, players[k].ownedChessmans.size, CIGRuleConfig::ON_BOARD, CIGRuleConfig::ALL);
						players[k].ownedChessmans.push(c);

						chessmanIndexBoard[PointOrVector(j,i)] = ChessmanIndex((CIGRuleConfig::PLAYER_NAMES)k, players[k].ownedChessmans.size - 1);
					}
				}
			}
		}
		refreshEvaluations();
	}

	CIG::Chessboard::Chessboard( const Chessboard& cb )
		: nowRound(cb.nowRound), 
		nowTurn(cb.nowTurn),
		chessmanIndexBoard(cb.chessmanIndexBoard)
	{
		currentBannedMotions.forceCopyFrom(cb.currentBannedMotions);
		pickedChessmanByIndex.forceCopyFrom(cb.pickedChessmanByIndex);

		memcpy(loose, cb.loose, sizeof(bool)*CIGRuleConfig::PLAYER_NUM);
		memcpy(win, cb.win, sizeof(bool)*CIGRuleConfig::PLAYER_NUM);
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

		this->chessmanIndexBoard = cb.chessmanIndexBoard;
		memcpy(win, cb.win, sizeof(bool)*CIGRuleConfig::PLAYER_NUM);
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
		ChessmanIndex& ci = chessmanIndexBoard.content[(p[1]<<CIGRuleConfig::INI_BOARD_WIDTH_LOG2) + p[0]];
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
		pickedChessmanByIndex.add(c->chessmanIndex);
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
		if ((*this)[p] != NULL)			//ffff也会返回false
		{
			return false;
		}
		else if ((pickedChessmanByIndex.size>0)&&(pickedChessmanByIndex[0] == c->chessmanIndex))
		{
			if (refreshEvaluations)
			{
				PointOrVector srcVector = DISTAINATION[nowTurn]-c->coordinate;
				PointOrVector distVector = DISTAINATION[nowTurn]-p;
				int srcDistance = ((srcVector[0]^srcVector[1])>=0)? max(abs(srcVector[0]), abs(srcVector[1]) ) : (abs(srcVector[0])+abs(srcVector[1]));
				int distDistance = ((distVector[0]^distVector[1])>=0)? max(abs(distVector[0]), abs(distVector[1]) ) : (abs(distVector[0])+abs(distVector[1]));

				evaluations[nowTurn] += srcDistance*srcDistance - distDistance*distDistance;
				win[nowTurn] = (evaluations[nowTurn] == 1920);			//调试输出得出的结果. 
			}
			chessmanIndexBoard[p] = c->chessmanIndex;
			if(!c->onPutIntent(p))
			{
				GUI::inform("走法错误, 请重新设计. ");
			}
			pickedChessmanByIndex.deleteAtNoReturn(0);
			return true;
		}

		return false;
	}

	void CIG::Chessboard::undoPut(Chessman* c , bool refreshEvaluations)
	{
		chessmanIndexBoard[c->coordinate].clear();
		c->undoPut();
		pickedChessmanByIndex.add(c->chessmanIndex);
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

	bool CIG::Chessboard::onMoveIntent(Move& move, bool refreshEvaluations)
	{
		bool result = true;

		for (int i = 0; i < move.size; i++)
		{
			result  &= onMotionIntent(move[i], refreshEvaluations);
		}

		result &= (this->pickedChessmanByIndex.size == 0);
		return result;
	}

	bool Chessboard::onMotionIntent( Motion& motion , bool refreshEvaluations)
	{
		switch (motion.operation)
		{
		case CIGRuleConfig::ADD:
			if (!onAddIntent(motion.distination ,refreshEvaluations))
			{
				return false;
			}
			break;

		case CIGRuleConfig::PICK:
			if (!onPickIntent(&(this->players[motion.chessmanIndex.player].ownedChessmans[motion.chessmanIndex.index]),refreshEvaluations))
			{
				return false;
			}

			break;

		case CIGRuleConfig::PUT:
			if(!onPutIntent(&(this->players[motion.chessmanIndex.player].ownedChessmans[motion.chessmanIndex.index]),motion.distination, refreshEvaluations))
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

	bool CIG::Chessboard::canMakeWholeMove(Move& move, bool refreshEvaluations)
	{
		Chessboard cb(*this);
		bool result = cb.onWholeMoveIntent(move, refreshEvaluations);
		return result;
	}
	void Chessboard::undoMove( Move& move , bool refreshEvaluations)
	{
		for (int i = move.size-1; i >=0; --i)				////应该倒着恢复!!!
		{
			undoMotion(move[i], refreshEvaluations);
		}
	}

	// TO-DO这个函数还没有写就拿去调试了, 当然不会有好结果! 
	void Chessboard::undoMotion( Motion& operation , bool refreshEvaluations)
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

	bool Chessboard::onWholeMoveIntent( Move& move , bool refreshEvaluations)
	{
		return onMoveIntent(move, refreshEvaluations)&&onChangeTurn();
	}

	void Chessboard::undoWholeMove( Move& move , bool refreshEvaluations)
	{
		undoChangeTurn();				//穿脱原理
		undoMove(move,refreshEvaluations);
	}

	bool CIG::Chessboard::onPickIntent( PointOrVector p , bool refreshEvaluations)
	{
		return onPickIntent((*this)[p], refreshEvaluations);
	}

	void CIG::Chessboard::undoPick(Chessman* c , PointOrVector p, bool refreshEvaluations)
	{
		if (refreshEvaluations)
		{
			win[nowTurn] = false;

			PointOrVector srcVector = DISTAINATION[nowTurn]-c->coordinate;
			PointOrVector distVector = DISTAINATION[nowTurn]-p;
			int srcDistance = ((srcVector[0]^srcVector[1])>=0)? max(abs(srcVector[0]), abs(srcVector[1]) ) : (abs(srcVector[0])+abs(srcVector[1]));
			int distDistance = ((distVector[0]^distVector[1])>=0)? max(abs(distVector[0]), abs(distVector[1]) ) : (abs(distVector[0])+abs(distVector[1]));

			evaluations[nowTurn] += srcDistance*srcDistance - distDistance*distDistance;
		}
		pickedChessmanByIndex.deleteAtNoReturn(pickedChessmanByIndex.size-1);
		chessmanIndexBoard[p] = c->chessmanIndex;
		c->coordinate = p;
		c->undoPick();
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
		if ((c->chessmanIndex.player == nowTurn) && (pickedChessmanByIndex.size == 0) && (c->onPickIntent()))
		{
			pickedChessmanByIndex.add(c->chessmanIndex);
			chessmanIndexBoard[c->coordinate].clear();
			
			return true;
		}

		return false;
	}

	bool CIG::Chessboard::onPromotionIntent( PointOrVector p, CIGRuleConfig::CHESSMAN_TYPES t , bool refreshEvaluations)
	{
		return onPromotionIntent((*this)[p], t, refreshEvaluations);
	}

	void Chessboard::undoCaptured( Chessman* c , bool refreshEvaluations)
	{
	}

	void Chessboard::refreshEvaluations()
	{

	}

	bool Chessboard::gameOver()
	{
		return false;
	}
}