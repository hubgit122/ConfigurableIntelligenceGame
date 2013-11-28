
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
			Chessboard(const Chessboard& cb);		//��Ϊ����֮��������̫��, �˺���û��д��.
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

			static const int MATE_VALUE = 10000000;  // ��߷�ֵ���������ķ�ֵ
			static const int WIN_VALUE = MATE_VALUE>>1; // ������ʤ���ķ�ֵ���ޣ�������ֵ��˵���Ѿ�������ɱ����
			static const int ADVANCED_VALUE = 3;  // ����Ȩ��ֵ

			//************************************
			// Method:    onXXIntent
			// FullName:  CIG::ChessmanBoard::onXXIntent
			// Access:    virtual public
			// Returns:   bool
			// Qualifier:
			// Parameter: PointOrVector p, Chessman& c, CIGConfig::CHESSMAN_TYPES t
			// ���Բ���, �������Ƿ�����ɹ�, ������ɹ�, �򱾴ε��ò�������̲���Ӱ��, ��������ɹ�, ��ᱣ������Ľ��. 
			// Ĭ������Ϊ:
			// ��������"�������˵�����, �����Լ�������"�����ĳ���, ��������״̬, ��������ֵ, ����true;
			// ����޸Ĺ���, Ӧ�ü̳и���, ��дonXXXIntent����, �������������Լ�����������.
			// ��ȻҲ�������Լ��ĺ�������ñ���ĺ���, �ڲ������¼���Ĺ���.
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
			// ע��, Ϊ������Ч��, ���Ҿ����������ӿ�, ���Բ����κ��ж�, ����߱����ϸ�֤�ǰ��������˳����. һ�������ڲ�����.
			// �ر�ע��undoCaptureIntent�Ĳ����Ǳ�������, onCapture�Ĳ����ǳ��ӵ�����
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

			friend ostringstream& operator<<(ostringstream& oss, const Chessboard& cb)						///�������÷���, �ͻ���ÿ������캯��, id��������߰���.
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

