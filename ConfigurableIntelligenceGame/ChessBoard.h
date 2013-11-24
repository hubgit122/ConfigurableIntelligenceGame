
#ifndef __TOTALCHESSBOARD_H__
#define __TOTALCHESSBOARD_H__

#include "Operation.h"
#include "Array.h"
#include "Stack.h"
#include "CIGRuleConfig.h"
#include "Chessman.h" 
#include "utilities.h"
#include "ChessmanLocation.h"
#include "Player.h"

namespace CIG
{
	class Chessboard
	{
		public:
			Chessboard();
			Chessboard(const Chessboard& cb);		//因为数据之间的相关性太大, 此函数没有写完. 
			virtual ~Chessboard();
			void operator=(const Chessboard& cb);

			/*Array<CIGRuleConfig::PLAYERS, Player, CIGRuleConfig::INI_BOARD_WIDTH_LOG2, 0>*/ 
			Player players[CIGRuleConfig::PLAYER_NUM];
			unsigned int nowRound;
			CIGRuleConfig::PLAYER_NAMES nowTurn;
			int evaluations[CIGRuleConfig::PLAYER_NUM];
			Array<CIGRuleConfig::CHESSMAN_OPERATIONS, Operation, CIGRuleConfig::INT_BANNED_MOTION_SIZE, 0> currentBannedMotions;
			Array<CIGRuleConfig::CHESSMAN, ChessmanLocation , CIGRuleConfig::INI_CHESSMAN_GROUP_SIZE, 0> pickedChessmanByLocation;
			ChessmanLocation chessmanLocationBoard[1 << CIGRuleConfig::INI_BOARD_HEIGHT_LOG2][1 << CIGRuleConfig::INI_BOARD_WIDTH_LOG2];
			//Array<CIGRuleConfig::Players, Player*, 10, 0>winners("Winners");
			//Array<CIGRuleConfig::Players, Player*, 10, 0>losers("Losers");

			//************************************
			// Method:    onXXIntent
			// FullName:  CIG::ChessmanBoard::onXXIntent
			// Access:    virtual public
			// Returns:   bool
			// Qualifier:
			// Parameter: PointOrVector p, Chessman& c, CIGConfig::CHESSMAN_TYPES t
			// 返回是否操作成功, 默认配置为:
			// 对于满足"不动别人的棋子, 不吃自己的棋子"条件的尝试, 调整棋盘状态, 更新评估值, 返回true;
			// 如果修改规则, 应该继承该类, 重写onXXXIntent函数, 并在其中配置自己的评估规则.
			// 当然也可以在自己的函数里调用本类的函数, 在并配置新加入的规则.
			//************************************
			virtual bool onPickIntent(PointOrVector p);
			virtual bool onPickIntent(Chessman* c , PointOrVector p);
			virtual bool onPickIntent(Chessman* c );
			virtual bool onPutIntent(Chessman* c, PointOrVector p);
			virtual bool onPutIntent(Chessman* c);
			virtual bool onCaptureIntent(Chessman* c, PointOrVector p);

			Chessman* operator[](PointOrVector p)const;
			bool beyondBoardRange( PointOrVector& p );
			bool onSelfHalfOfBoard( PointOrVector& p );

			virtual bool onPromotionIntent(Chessman* c, CIGRuleConfig::CHESSMAN_TYPES t);
			virtual bool onPromotionIntent(PointOrVector p, CIGRuleConfig::CHESSMAN_TYPES t);
			virtual int getEvaluation(CIGRuleConfig::PLAYER_NAMES p)const;
			virtual int getEvaluation()const;
			virtual bool onChangeTurn();
			bool makeAction(OperationStack& action);
			bool canMakeAction(OperationStack& action);
			//virtual void undoCapture(PointOrVector p);
			//virtual void undoCapture(Chessman& c);

			friend ostringstream& operator<<(ostringstream& oss, const Chessboard& cb)						///不加引用符号, 就会调用拷贝构造函数, id管理得乱七八糟.
			{
				oss << "Chessboard::\n" <<"\tnowTurn: "<<cb.nowTurn<<"\n\tnowRound: "<<cb.nowRound<<"\n{\n";
				for (int i=0; i<(1<<CIGRuleConfig::INI_BOARD_WIDTH_LOG2);++i)
				{
					for (int j=0;j<(1<<CIGRuleConfig::INI_BOARD_HEIGHT_LOG2);++j)
					{
						oss<<"[ "<<i<<" , "<<j<<" ]";
						//oss<< cb.mChessmanBoard[j][i];			// TO-DO
					}
				}
				oss<<'}\n';
				return oss;
			}
			void valify();
			//static Array<CIGRuleConfig::Points, PointOrVector, CIGRuleConfig::INT_MARKED_POINTS_SIZE, 0> markedPoints;
			//static const Array<CIGRuleConfig::Lines, Line, CIGRuleConfig::INT_MARKED_POINTS_SIZE, 0> additionalLines;
			//static const bool CHESSMANES_ON_CROSS_NOT_IN_HOLES;
			//static const HBITMAP background;
	};
}

#endif /*__TOTALCHESSBOARD_H_*/

