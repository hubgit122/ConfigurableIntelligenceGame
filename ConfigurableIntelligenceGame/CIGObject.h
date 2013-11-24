#ifndef __CIGOBJECT_H__
#define __CIGOBJECT_H__
#include "utilities.h"
#include "CIGRuleConfig.h"
#include "GUI.h"
/*************************************************************************
  可配置智能系统各部件的基类, 管理类型名和类型成员的个数.
  要求类名字符串生命周期长于该类.
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

			friend ostringstream& operator<<(ostringstream& oss, const CIGObject<TYPE_ID>& o)						///不加引用符号, 就会调用拷贝构造函数, id管理得乱七八糟.
			{
				oss << "CIGObject<" << TYPE_ID/*CIGConfig::CLASS_TYPES[TYPE_ID]*/ << ">::\n*******************************************************************************\n";				// TO-DO 输出字符串
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
		GUI::inform(str + oss.str(), true);		///C++静态函数成员的引用用域符号.
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

