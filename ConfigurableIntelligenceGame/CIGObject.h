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
	class CIGObject
	{
		public:
			CIGObject();
			CIGObject(const CIGObject& o);
			virtual ~CIGObject();

			virtual void informError(const string& str)const;

			friend ostream& operator << (ostream& os, const CIGObject& o)
			{
				ostringstream oss;
				oss << o;
				os << oss.str();
				return os;
			}

			friend ostringstream& operator<<(ostringstream& oss, const CIGObject& o)						///不加引用符号, 就会调用拷贝构造函数, id管理得乱七八糟.
			{
				oss << "CIGObject::\n*******************************************************************************\n";				// TO-DO 输出字符串
				return oss;
			}
	};
}
#endif /*__CIGOBJECT_H_*/

