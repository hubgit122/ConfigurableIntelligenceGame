
#ifndef __PLAYER_H__
#define __PLAYER_H__
#include "utilities.h"
#include "CIGNamedObject.h"
#include "Array.h"
#include "Stack.h"

namespace CIG
{
	class Chessboard;
	// ע��Player����ȫ������Chesssboard��, ��Ϊ���ӵ�ƫ����. ���Գ�ʼ��ʱ���û��Chesssboard����Ϣ, ҲҪ����ֵ�����Ǹ���.
	// ӵ�е�����Ҳ�ǹ���������̵Ĺ��캯����߼���ȥ��.
	class Player//: public CIGNamedObject<CIGRuleConfig::PLAYER>
	{
		public:
			Player();
			Player(CIGRuleConfig::PLAYER_NAMES p, Chessboard* cb = NULL);
			Player(const Player& p, Chessboard* cb = NULL);
			virtual ~Player();

			void operator = (const Player& p);

			CIGRuleConfig::PLAYER_NAMES NAME;
			Stack<CIGRuleConfig::CHESSMAN_GROUP, Chessman, CIGRuleConfig::INI_CHESSMAN_GROUP_SIZE, 0> ownedChessmans;
			Chessboard* chessboard;

			virtual void makeBestAction(OperationStack& op);
			//static const unsigned char color[CIGRuleConfig::PLAYER_NUM][3];
			//static const HBITMAP bitMap;
	};
}

#endif /*__PLAYER_H_*/

