
#ifndef __CIGNAMEDOBJECT_H__
#define __CIGNAMEDOBJECT_H__

#include "CIGObject.h"
#include "utilities.h"
/*************************************************************************
  有名字的部件, 管理id和个体名.
  **************************************************************************/
namespace CIG
{
	template <CIGRuleConfig::CLASS_TYPES TYPE_ID>
	class CIGNamedObject: public CIGObject<TYPE_ID>
	{
		private:
			CIGNamedObject();				//不允许不命名
		public:
			CIGNamedObject(const string& str);
			CIGNamedObject(const CIGNamedObject<TYPE_ID>& o);
			virtual ~CIGNamedObject();
			void operator = (const CIGNamedObject& o);

			static unsigned short entityNumOfType;
			static stack<unsigned short> deprecatedIDs;

			unsigned short id;
			string entityName;


			unsigned short genID();

			friend ostream& operator << (ostream& os, const CIGNamedObject<TYPE_ID>& o)
			{
				ostringstream oss;
				oss << o;
				os << oss.str();
				return os;
			}

			friend ostringstream& operator<<(ostringstream& oss, const CIGNamedObject<TYPE_ID>& o)
			{
				oss << "CIGNamedObject<" << TYPE_ID/*CIGConfig::CLASS_TYPES[TYPE_ID]*/ << ">::\n\tentityNumOfType: " << o.entityNumOfType << "\n\tid: " << o.id << "\n\tentityName: " << o.entityName << endl;
				oss << (CIGObject<TYPE_ID>&)o;															///C++中向上造型不调用拷贝构造函数的方式是转换成引用.
				return oss;
			}
	};

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID>
	void CIG::CIGNamedObject<TYPE_ID>::operator=( const CIGNamedObject& o )
	{
		memcpy(this,&o,sizeof(CIG::CIGNamedObject<TYPE_ID>));
	}

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID>
	stack<unsigned short> CIG::CIGNamedObject<TYPE_ID>::deprecatedIDs = stack<unsigned short>();

	template<CIGRuleConfig::CLASS_TYPES TYPE_ID>
	unsigned short CIG::CIGNamedObject<TYPE_ID>::entityNumOfType = 0;

	template<CIGRuleConfig::CLASS_TYPES TYPE_ID>
	CIG::CIGNamedObject<TYPE_ID>::~CIGNamedObject()
	{
		deprecatedIDs.push(id);
		entityNumOfType--;
	}

	template<CIGRuleConfig::CLASS_TYPES TYPE_ID>
	unsigned short CIG::CIGNamedObject<TYPE_ID>::genID()
	{
		if (deprecatedIDs.size() > 0)
		{
			unsigned short top = deprecatedIDs.top();
			deprecatedIDs.pop();
			entityNumOfType++;
			return top;
		}
		else
		{
			return entityNumOfType++;
		}
	}

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID>
	CIGNamedObject<TYPE_ID>::CIGNamedObject(const string& str): entityName(str), id(genID())
	{
#ifdef DEBUG
		cout << "CIGNamedObject create" << endl;
#endif // DEBUG
	}

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID>
	CIGNamedObject<TYPE_ID>::CIGNamedObject( const CIGNamedObject<TYPE_ID>& o )
		: id(o.id), entityName(o.entityName)
		//: id(this->genID()), entityName(string("copy_of_") + o.entityName)
	{
		entityNumOfType++;
#ifdef DEBUG
		cout << "CIGNamedObject copy" << endl;
#endif // DEBUG
	}
}
#endif /*__CIGNAMEDOBJECT_H_*/

