#ifndef __CIGOBJECT_H__
#define __CIGOBJECT_H__
#include "utilities.h"
#include "CIGRuleConfig.h"
#include "GUI.h"
/*************************************************************************
  ����������ϵͳ�������Ļ���, ���������������ͳ�Ա�ĸ���.
  Ҫ�������ַ����������ڳ��ڸ���.
**************************************************************************/
namespace CIG
{
	template<CIGRuleConfig::CLASS_TYPES TYPE_ID>
	class CIGObject
	{
		public:
			CIGObject();
			CIGObject(const CIGObject<TYPE_ID>& o);
			virtual ~CIGObject();
			void operator = (const CIGObject& o);

			virtual void informError(const string& str)const;

			friend ostream& operator << (ostream& os, const CIGObject<TYPE_ID>& o)
			{
				ostringstream oss;
				oss << o;
				os << oss.str();
				return os;
			}

			friend ostringstream& operator<<(ostringstream& oss, const CIGObject<TYPE_ID>& o)						///�������÷���, �ͻ���ÿ������캯��, id��������߰���.
			{
				oss << "CIGObject<" << TYPE_ID/*CIGConfig::CLASS_TYPES[TYPE_ID]*/ << ">::\n*******************************************************************************\n";				// TO-DO ����ַ���
				return oss;
			}
	};

	template<CIGRuleConfig::CLASS_TYPES TYPE_ID>
	void CIG::CIGObject<TYPE_ID>::operator=( const CIGObject& o )
	{
		memcpy(this, &o, sizeof(CIG::CIGObject<TYPE_ID>));
	}

	template<CIGRuleConfig::CLASS_TYPES TYPE_ID>
	CIG::CIGObject<TYPE_ID>::CIGObject( const CIGObject<TYPE_ID>& o )
	{
#ifdef DEBUG
		cout << "CIGObject copy" << endl;
#endif // DEBUG
	}

	template<CIGRuleConfig::CLASS_TYPES TYPE_ID>
	void CIG::CIGObject<TYPE_ID>::informError( const string& str) const
	{
		ostringstream oss;
		oss << *this;
		GUI::inform(str + oss.str(), true);		///C++��̬������Ա�������������.
	}

	template<CIGRuleConfig::CLASS_TYPES TYPE_ID>
	CIG::CIGObject<TYPE_ID>::CIGObject()
	{
#ifdef DEBUG
		cout << "CIGObject create" << endl;
#endif // DEBUG
	}

	template<CIGRuleConfig::CLASS_TYPES TYPE_ID>
	CIGObject<TYPE_ID>::~CIGObject()
	{

	}

}

#endif /*__CIGOBJECT_H_*/

