
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
			
			friend std::ostringstream& operator<<(std::ostringstream& oss, const Chessman& c)						///�������÷���, �ͻ���ÿ������캯��, id��������߰���.
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
			// ��ѯ���������, ����������״̬.
			// �麯��, Ĭ��һ�в�������ͨ�����. ����������Ҫ, Ӧ�����¶���.
			//************************************
			virtual bool onPickIntent();
			virtual bool onPutIntent(PointOrVector p);
			virtual bool onCapturedIntent();
			virtual bool onCaptureIntent(Chessman* c);
			virtual bool onPromotionIntent(CIGRuleConfig::CHESSMAN_TYPES t);
	};
}

#endif /*__CHESSMAN_H_*/

