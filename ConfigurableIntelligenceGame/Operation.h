
#ifndef __CHESSMANMOTION_H__
#define __CHESSMANMOTION_H__

#include "CIGObject.h"
#include "CIGRuleConfig.h"
#include "Chessman.h"

namespace CIG
{
	class Operation
	{
		public:
			Operation();
			Operation(const Operation& op);
			Operation(ChessmanIndex ci, CIGRuleConfig::OPERATIONS op, PointOrVector dist = 0);
			virtual ~Operation();
			void operator=(const Operation& op);

			friend std::ostringstream& operator<<(std::ostringstream& oss, const Operation& op)						///不加引用符号, 就会调用拷贝构造函数, id管理得乱七八糟.
			{
				oss << "Operation::\n" << "\toperation: " << op.operation << "\n\tchessman: ";
				//oss<<(const Chessman&)(*op.chessman);				// TO-DO
				return oss;
			}

			CIGRuleConfig::OPERATIONS operation;			//操作类型
			ChessmanIndex chessmanIndex;				//棋子所属的玩家号和在玩家手中的索引号
			PointOrVector distination;								//事件的位置
	};
}

#endif /*__CHESSMANMOTION_H_*/

