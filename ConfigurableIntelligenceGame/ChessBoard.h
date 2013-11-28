
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
#include "ChessmanLocationBoard.h"

namespace CIG
{
	class Chessboard
	{
		public:
			Chessboard();
			Chessboard(const Chessboard& cb);		//因为数据之间的相关性太大, 此函数没有写完.
			virtual ~Chessboard(){};
			void operator=(const Chessboard& cb);

			/*Array<CIGRuleConfig::PLAYERS, Player, CIGRuleConfig::INI_BOARD_WIDTH_LOG2, 0>*/
			Player players[CIGRuleConfig::PLAYER_NUM];			// TO-DO
			unsigned int nowRound;
			CIGRuleConfig::PLAYER_NAMES nowTurn;
			int evaluations[CIGRuleConfig::PLAYER_NUM];
			Array<Operation, CIGRuleConfig::INT_BANNED_MOTION_SIZE, 0> currentBannedMotions;
			Array<ChessmanLocation , CIGRuleConfig::INI_CHESSMAN_GROUP_SIZE, 0> pickedChessmanByLocation;
			ChessmanLocationBoard chessmanLocationBoard;

			static const int MATE_VALUE = 10000000;  // 最高分值，即将死的分值
			static const int WIN_VALUE = MATE_VALUE>>1; // 搜索出胜负的分值界限，超出此值就说明已经搜索出杀棋了
			static const int ADVANCED_VALUE = 3;  // 先行权分值

			//************************************
			// Method:    onXXIntent
			// FullName:  CIG::ChessmanBoard::onXXIntent
			// Access:    virtual public
			// Returns:   bool
			// Qualifier:
			// Parameter: PointOrVector p, Chessman& c, CIGConfig::CHESSMAN_TYPES t
			// 尝试操作, 并返回是否操作成功, 如果不成功, 则本次调用不会对棋盘产生影响, 如果操作成功, 则会保存操作的结果. 
			// 默认配置为:
			// 对于满足"不动别人的棋子, 不吃自己的棋子"条件的尝试, 调整棋盘状态, 更新评估值, 返回true;
			// 如果修改规则, 应该继承该类, 重写onXXXIntent函数, 并在其中配置自己的评估规则.
			// 当然也可以在自己的函数里调用本类的函数, 在并配置新加入的规则.
			//************************************
			virtual bool onPickIntent(PointOrVector p);
			//virtual bool onPickIntent(Chessman* c , PointOrVector p);
			virtual bool onPickIntent(Chessman* c );
			virtual bool onPutIntent(Chessman* c, PointOrVector p);
			//virtual bool onPutIntent(Chessman* c);
			virtual bool onCaptureIntent(Chessman* c, PointOrVector p);
			virtual bool onPromotionIntent(Chessman* c, CIGRuleConfig::CHESSMAN_TYPES t);
			virtual bool onPromotionIntent(PointOrVector p, CIGRuleConfig::CHESSMAN_TYPES t);
			virtual bool onActionIntent(Action& action);
			virtual bool onWholeActionIntent(Action& action);
			virtual bool onOperationIntent(Operation& operation);
			virtual bool canMakeAction(Action& action);
			virtual bool onChangeTurn();

			//************************************
			// Method:    undoXX
			// FullName:  CIG::Chessboard::undoXX
			// Access:    virtual private 
			// Returns:   void
			// Qualifier:
			// Parameter: PointOrVector p
			// 注意, 为了运行效率, 并且尽量减少外界接口, 所以不做任何判断, 编程者必须严格保证是按照走棋的顺序撤销. 一定是类内部调用.
			// 特别注意undoCaptureIntent的参数是被吃棋子, onCapture的参数是吃子的棋子
			//************************************
			virtual void undoPick(Chessman* c );
			virtual void undoPick(PointOrVector p);
			//virtual void undoPick(Chessman* c , PointOrVector p);
			virtual void undoPut(Chessman* c, PointOrVector p);
			virtual void undoCaptured(Chessman* c);
			//virtual void undoCapture(Chessman* c, PointOrVector p);
			virtual void undoPromotion(Chessman* c, CIGRuleConfig::CHESSMAN_TYPES t);
			virtual void undoPromotion(PointOrVector p, CIGRuleConfig::CHESSMAN_TYPES t);
			virtual void undoAction(Action& action);
			virtual void undoWholeAction(Action& action);
			virtual void undoOperation(Operation& operation);
			virtual void undoChangeTurn();

		public:
			virtual int getEvaluation(CIGRuleConfig::PLAYER_NAMES p)const;
			virtual int getEvaluation()const;

			Chessman* operator[](PointOrVector p)const;
			bool beyondBoardRange( PointOrVector& p );
			bool onSelfHalfOfBoard( PointOrVector& p );

			friend ostringstream& operator<<(ostringstream& oss, const Chessboard& cb)						///不加引用符号, 就会调用拷贝构造函数, id管理得乱七八糟.
			{
				oss << "Chessboard::\n" << "\tnowTurn: " << cb.nowTurn << "\n\tnowRound: " << cb.nowRound << "\n{\n";

				for (int i = 0; i < (1 << CIGRuleConfig::INI_BOARD_WIDTH_LOG2); ++i)
				{
					for (int j = 0; j < (1 << CIGRuleConfig::INI_BOARD_HEIGHT_LOG2); ++j)
					{
						oss << "[ " << i << " , " << j << " ]";
						//oss<< cb.mChessmanBoard[j][i];			// TO-DO
					}
				}

				oss << '}\n';
				return oss;
			}
			//static Array<CIGRuleConfig::Points, PointOrVector, CIGRuleConfig::INT_MARKED_POINTS_SIZE, 0> markedPoints;
			//static const Array<CIGRuleConfig::Lines, Line, CIGRuleConfig::INT_MARKED_POINTS_SIZE, 0> additionalLines;
			//static const bool CHESSMANES_ON_CROSS_NOT_IN_HOLES;
			//static const HBITMAP background;
	};
}

#endif /*__TOTALCHESSBOARD_H_*/

