
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

			friend std::ostringstream& operator<<(std::ostringstream& oss, const Operation& op)						///�������÷���, �ͻ���ÿ������캯��, id��������߰���.
			{
				oss << "Operation::\n" << "\toperation: " << op.operation << "\n\tchessman: ";
				//oss<<(const Chessman&)(*op.chessman);				// TO-DO
				return oss;
			}

			CIGRuleConfig::OPERATIONS operation;			//��������
			ChessmanIndex chessmanIndex;				//������������Һź���������е�������
			PointOrVector distination;								//�¼���λ��
	};
}

#endif /*__CHESSMANMOTION_H_*/

