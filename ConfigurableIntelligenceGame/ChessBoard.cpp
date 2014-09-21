#include "stdafx.h"
#include "Chessboard.h"
#include "Player.h"
#include "Array.h"
#include "Chessman.h"
#include "ChessmanIndex.h"
#include "GraphSearchEngine.h"

namespace CIG
{
	// 按配置初始化很少执行, 所以可以速度慢一点.
	CIG::Chessboard::Chessboard() : nowRound(0), nowTurn((CIGRuleConfig::PLAYER_NAMES)0), pickedChessmanByIndex(), currentBannedMotions(), chessmanIndexBoard()
	{
		players[0] = Player(CIGRuleConfig::HUMAN, GUI::askForMove, this);
		players[1] = Player(CIGRuleConfig::COMPUTER, GraphSearchEngine::makeBestMove, this);

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

					if ((t != CIGRuleConfig::NOCHESSMAN) && (!beyondBoardRange(PointOrVector(j, i))))
					{
						Chessman c = Chessman(t, PointOrVector(j, i), (CIGRuleConfig::PLAYER_NAMES)k, players[k].ownedChessmans.size, CIGRuleConfig::ON_BOARD, CIGRuleConfig::ALL);
						players[k].ownedChessmans.push(c);

						chessmanIndexBoard[PointOrVector(j, i)] = ChessmanIndex((CIGRuleConfig::PLAYER_NAMES)k, players[k].ownedChessmans.size - 1);
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
			return (Chessman*)(void*) - 1;
		}

		ChessmanIndex& ci = chessmanIndexBoard.content[(p[1] << CIGRuleConfig::INI_BOARD_WIDTH_LOG2) + p[0]];
		return (ci.player == -1) ? NULL : &(players[ci.player].ownedChessmans[ci.index]);
	}

	int CIG::Chessboard::getEvaluation( CIGRuleConfig::PLAYER_NAMES p ) const
	{
		//可以加入胜负, 包括平局, 判负
		int result = 0;

		for (int i = 0; i < CIGRuleConfig::PLAYER_NUM; ++i)
		{
			if (i != p)
			{
				result -= evaluations[i];
			}
		}

		result = evaluations[p] + (result) / (CIGRuleConfig::PLAYER_NUM - 1);

		return result;
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

	bool Chessboard::onAddIntent(PointOrVector p /*= PointOrVector(-1,-1)*/, bool refreshEvaluations /*= false*/)
	{
		Chessman* c = players[nowTurn].ownedChessmans.add(Chessman(CIGRuleConfig::CHESS, p, nowTurn, players[nowTurn].ownedChessmans.size, CIGRuleConfig::OFF_BOARD, CIGRuleConfig::ALL));
        return onPutIntent(c, p, refreshEvaluations);
	}

	//穿脱原理下, 操作就是这么简单
    void Chessboard::undoAdd(PointOrVector p, bool refreshEvaluations)
	{
        undoPut(&players[chessmanIndexBoard[p].player].ownedChessmans[chessmanIndexBoard[p].index], PointOrVector(), refreshEvaluations);
		//pickedChessmanByIndex.deleteAtNoReturn(pickedChessmanByIndex.size - 1);
		players[nowTurn].ownedChessmans.popNoReturn();
	}

	bool CIG::Chessboard::onPutIntent( Chessman* c, PointOrVector p , bool refreshEvaluations)
	{
		if ((*this)[p] != NULL)			//beyondBoard时返回-1(ffff), 所以也会返回false
		{
			return false;
		}
		else /*if ((pickedChessmanByIndex.size > 0) && (pickedChessmanByIndex[0] == c->chessmanIndex))*/
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
                stack<Chessman*> toBerefreshed;

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
						else if (testc->chessmanIndex.player!=nowTurn)
						{
							enemyOnTheEnd[d] = true;
                            toBerefreshed.push(testc);
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

                while (toBerefreshed.size())
                {

                }
			}

			chessmanIndexBoard[p] = c->chessmanIndex;

			assert(c->onPutIntent(p));    //如果不满足条件, 说明走法设计有问题.

			//pickedChessmanByIndex.deleteAtNoReturn(0);
			return true;
		}

		return false;
	}

	void CIG::Chessboard::undoPut(Chessman* c, PointOrVector previousP, bool refreshEvaluations /*= false*/)
    {
        chessmanIndexBoard[c->coordinate].clear();
        c->undoPut();
        //pickedChessmanByIndex.add(c->chessmanIndex);

        if (refreshEvaluations)
        {
            static const int DIRECTION_NUM = 8;
            static const PointOrVector direction[DIRECTION_NUM] =
            {
                PointOrVector(1, 0),
                PointOrVector(1, 1),
                PointOrVector(0, 1),
                PointOrVector(-1, 1),
                PointOrVector(-1, 0),
                PointOrVector(-1, -1),
                PointOrVector(0, -1),
                PointOrVector(1, -1),
            };
            bool enemyOnTheEnd[DIRECTION_NUM];
            unsigned char numOfChessman[DIRECTION_NUM];
            unsigned char totalnumOfChessman[DIRECTION_NUM >> 1];

            for (int d = 0; d < DIRECTION_NUM; ++d)
            {
                enemyOnTheEnd[d] = false;
                numOfChessman[d] = 0;
                PointOrVector testPoint = c->coordinate;

                for (Chessman* testc = (*this)[testPoint += direction[d]]; testc; testc = (*this)[testPoint += direction[d]])
                {
                    if (testc == (void*)-1)
                    {
                        enemyOnTheEnd[d] = true;
                        break;
                    }
                    else if (!testc)
                    {
                        enemyOnTheEnd[d] = false;
                        break;
                    }
                    else if (testc->chessmanIndex.player != nowTurn)
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

            for (int d = 0; d<(DIRECTION_NUM >> 1); ++d)
            {
                totalnumOfChessman[d] = numOfChessman[d] + numOfChessman[d + 4] + 1;
                loose[1 - nowTurn] = false;			//特别注意这里, 没输, 搜索时才会调用undo, 所以清空looose. 
                evaluations[nowTurn] += GRADES[enemyOnTheEnd[d] + 0][numOfChessman[d]];
                evaluations[nowTurn] += GRADES[enemyOnTheEnd[d + 4] + 0][numOfChessman[d + 4]];
                evaluations[nowTurn] -= GRADES[enemyOnTheEnd[d] + enemyOnTheEnd[d + 4]][totalnumOfChessman[d]>5 ? 5 : totalnumOfChessman[d]];
            }
        }
		c->coordinate = previousP;
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

	bool Chessboard::onMotionIntent( Motion& op , bool refreshEvaluations)
	{
		switch (op.operation)
		{
			case CIGRuleConfig::BEGIN:
				return true;
			
			case CIGRuleConfig::ADD:
				return onAddIntent(op.distination , refreshEvaluations);
				break;

			case CIGRuleConfig::PICK:
				return onPickIntent(&(this->players[nowTurn].ownedChessmans[op.chessmanIndex.index]), refreshEvaluations);
				break;

			case CIGRuleConfig::PUT:
				return onPutIntent(&(this->players[nowTurn].ownedChessmans[op.chessmanIndex.index]), op.distination, refreshEvaluations);
				break;

			case CIGRuleConfig::CAPTURE:
				return onCaptureIntent(&(this->players[pickedChessmanByIndex[0].player].ownedChessmans[pickedChessmanByIndex[0].index]), op.distination, refreshEvaluations);
				break;

			case CIGRuleConfig::PROMOTION:
				return false;
				break;

			default:
				return false;
		}

		return false;
	}

	bool CIG::Chessboard::canMakeWholeMove(Move& move, bool refreshEvaluations)
	{
		Chessboard cb(*this);
		bool result = cb.onWholeMoveIntent(move, refreshEvaluations);
		return result;
	}

	void Chessboard::undoMove( Move& move , bool refreshEvaluations)
	{
		for (int i = move.size - 1; i >= 0; --i)
		{
			undoMotion(move[i], refreshEvaluations);
		}
	}

	void Chessboard::undoMotion( Motion& operation , bool refreshEvaluations)
	{
		switch (operation.operation)
		{
			case CIGRuleConfig::ADD:
				undoAdd(operation.distination, refreshEvaluations);
				break;

			case CIGRuleConfig::PICK:
				undoPick(&(this->players[operation.chessmanIndex.player].ownedChessmans[operation.chessmanIndex.index]), operation.distination, refreshEvaluations);
				break;

			case CIGRuleConfig::PUT:
				undoPut(&(this->players[operation.chessmanIndex.player].ownedChessmans[operation.chessmanIndex.index]), operation.savedCoodinate, refreshEvaluations);
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
		return onMoveIntent(move, refreshEvaluations) && onChangeTurn();
	}

	void Chessboard::undoWholeMove( Move& move , bool refreshEvaluations)
	{
		undoChangeTurn();
		undoMove(move, refreshEvaluations);
	}

	bool CIG::Chessboard::onPickIntent( PointOrVector p , bool refreshEvaluations)
	{
		return onPickIntent((*this)[p], refreshEvaluations);
	}

	void CIG::Chessboard::undoPick(Chessman* c , PointOrVector p, bool refreshEvaluations)
	{
		if (refreshEvaluations)
		{
			// TO-DO

		}

		pickedChessmanByIndex.deleteAtNoReturn(pickedChessmanByIndex.size - 1);
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
		if (beyondBoardRange(p))
		{
			return false;
		}
		else
		{
			Chessman* testC = (*this)[p];

			if ((testC == NULL) || (testC->chessmanIndex.player == nowTurn))
			{
				return false;
			}
			else if (pickedChessmanByIndex[0] == c->chessmanIndex)
			{
				if (testC->onCapturedIntent())
				{
					c->onCaptureIntent(testC);
					chessmanIndexBoard[p].clear();
					//loose[testC->chessmanIndex.player] = (testC->chessmanType == CIGRuleConfig::KING);

					if (refreshEvaluations)
					{
						//evaluations[testC->chessmanIndex.player] -= CIGRuleConfig::EVALUATIONS[testC->chessmanIndex.player][testC->chessmanType][p.x[1]][p.x[0]];
					}

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

	bool CIG::Chessboard::onPromotionIntent( Chessman* c, CIGRuleConfig::CHESSMAN_TYPES t , bool refreshEvaluations)
	{
		return false;
	}

	bool CIG::Chessboard::onPickIntent( Chessman* c , bool refreshEvaluations)
	{
		if ((c->chessmanIndex.player == nowTurn) && (pickedChessmanByIndex.size == 0) && (c->onPickIntent()))
		{
			PointOrVector& p = c->coordinate;
			pickedChessmanByIndex.add(c->chessmanIndex);
			chessmanIndexBoard[c->coordinate].clear();

			if (refreshEvaluations)
			{
				//evaluations[nowTurn] -= CIGRuleConfig::EVALUATIONS[nowTurn][c->chessmanType][p.x[1]][p.x[0]];
			}

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
		c->undoCaptured();
		PointOrVector p(c->coordinate);
		chessmanIndexBoard[p] = c->chessmanIndex;

		//if (c->chessmanType == CIGRuleConfig::KING)
		//{
		//	loose[c->chessmanIndex.player] = false;
		//}

		if (refreshEvaluations)
		{
			// TO-DO
		}
	}

	void Chessboard::refreshEvaluations()
	{
		// TO-DO
	}

	bool Chessboard::gameOver()
	{
		// TO-DO
		for (int i = 0; i < CIGRuleConfig::PLAYER_NUM; ++i)
		{
			if (loose[i])
			{
				return true;
			}
		}

		return false;
	}

    const int Chessboard::GRADES[3][6] = {
        { 0, 30, 100, MATE_VALUE >> 7, MATE_VALUE >> 4, MATE_VALUE },
        { 0, 10, 50, 200, MATE_VALUE >> 7, MATE_VALUE },
        { 0, 0, 0, 0, 0, MATE_VALUE },
    };
}