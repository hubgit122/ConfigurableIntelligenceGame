
#ifndef __CHESSMANMOTION_H__
#define __CHESSMANMOTION_H__

#include "CIGObject.h"
#include "CIGRuleConfig.h"
#include "Chessman.h"

namespace CIG
{
	class Operation
	{
			typedef unsigned long DWORD;
		public:
			Operation();
			Operation(const Operation& op);
			Operation(ChessmanLocation cl, CIGRuleConfig::OPERATIONS op, DWORD extra_ = 0);
			virtual ~Operation();

			friend std::ostringstream& operator<<(std::ostringstream& oss, const Operation& op)						///�������÷���, �ͻ���ÿ������캯��, id��������߰���.
			{
				oss << "Operation::\n"<<"\toperation: "<<op.operation <<"\n\tchessman: ";
				//oss<<(const Chessman&)(*op.chessman);				// TO-DO
				return oss;
			}

			CIGRuleConfig::OPERATIONS operation;
			ChessmanLocation chessmanLocation;
			DWORD extra;
	};
}

#endif /*__CHESSMANMOTION_H_*/

