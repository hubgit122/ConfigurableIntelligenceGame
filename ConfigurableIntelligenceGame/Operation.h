
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
			Operation(ChessmanLocation cl, CIGRuleConfig::OPERATIONS op, PointOrVector dist);
			virtual ~Operation();
			void operator=(const Operation& op);

			friend std::ostringstream& operator<<(std::ostringstream& oss, const Operation& op)						///不加引用符号, 就会调用拷贝构造函数, id管理得乱七八糟.
			{
				oss << "Operation::\n" << "\toperation: " << op.operation << "\n\tchessman: ";
				//oss<<(const Chessman&)(*op.chessman);				// TO-DO
				return oss;
			}
			CIGRuleConfig::OPERATIONS operation;
			ChessmanLocation chessmanLocation;
			PointOrVector distination;
	};
}

#endif /*__CHESSMANMOTION_H_*/

