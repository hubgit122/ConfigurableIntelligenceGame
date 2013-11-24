
#ifndef __CHESSGROUP_H__
#define __CHESSGROUP_H__

#include "Array.h"
#include "CIGObject.h"
#include "Chessman.h"

namespace CIG
{
	class ChessmanGroup: public Array<CIGRuleConfig::CHESSMAN_GROUP, Chessman, CIGRuleConfig::INI_CHESSMAN_GROUP_SIZE, 0>
	{
		private:
			ChessmanGroup();
		public:
			ChessmanGroup(const ChessmanGroup& c);
			ChessmanGroup(const string& str);
			virtual ~ChessmanGroup();
			void operator = (const ChessmanGroup& cg);
	};
}

#endif /*__CHESSGROUP_H_*/

