
#ifndef __CHESSMAN_H__
#define __CHESSMAN_H__

#include "utilities.h"
#include "CIGRuleConfig.h"
#include "ChessmanLocation.h"
//#include "CIGNamedObject.h"

namespace CIG
{
	class Chessman//: public CIGNamedObject<CIGRuleConfig::CLASS_TYPES::CHESSMAN>
	{
		private:
			Chessman();
		public:
			virtual ~Chessman();
			Chessman(const Chessman& c);
			Chessman(/*const string& str,*/ CIGRuleConfig::CHESSMAN_TYPES t, const PointOrVector& c, CIGRuleConfig::PLAYER_NAMES p, int index, CIGRuleConfig::CHESSMAN_STATUS s, CIGRuleConfig::VISIBILITIES v);

			CIGRuleConfig::CHESSMAN_TYPES chessmanType;
			struct PointOrVector coordinate;
			ChessmanLocation chessmanLocation;
			CIGRuleConfig::CHESSMAN_STATUS status;
			CIGRuleConfig::VISIBILITIES visibility[CIGRuleConfig::PLAYER_NUM];
			//static const HBITMAP bitMap[CIGRuleConfig::CHESSMAN_NUM];
			//ChessmanGroup& chessmanGroup;
			
			friend std::ostringstream& operator<<(std::ostringstream& oss, const Chessman& c)						///不加引用符号, 就会调用拷贝构造函数, id管理得乱七八糟.
			{
				oss << "Chessman::\n" <<"\tchessmanType: "<<c.chessmanType<<"\n\tcoordinate: ";
				oss<<c.coordinate<<"\nchessmanLocation.player: "<<c.chessmanLocation.player<<'\n';
				//oss<<(const CIGNamedObject<CIGRuleConfig::CLASS_TYPES::CHESSMAN>&)c;
				return oss;
			}
			//************************************
			// Method:    onXXIntent
			// FullName:  CIG::Chessman::onIntent
			// Access:    virtual public
			// Returns:   bool
			// Qualifier:
			// Parameter: CIGConfig::Operation op, xx
			// 查询操作否可用, 并更新棋子状态.
			// 虚函数, 默认一切操作均可通过检查. 若有特殊需要, 应该重新定义.
			//************************************
			virtual bool onPickIntent();
			virtual bool onPutIntent(PointOrVector p);
			virtual bool onCapturedIntent();
			virtual bool onCaptureIntent(Chessman* c);
			virtual bool onPromotionIntent(CIGRuleConfig::CHESSMAN_TYPES t);
	};
}

#endif /*__CHESSMAN_H_*/

