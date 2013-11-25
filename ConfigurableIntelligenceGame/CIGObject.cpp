#include "CIGObject.h"

namespace CIG
{
	CIG::CIGObject::CIGObject( const CIGObject& o )
	{
#ifdef DEBUG
		cout << "CIGObject copy" << endl;
#endif // DEBUG
	}

	void CIG::CIGObject::informError( const string& str) const
	{
		ostringstream oss;
		oss << *this;
		GUI::inform(str + oss.str(), true);		///C++静态函数成员的引用用域符号.
	}

	CIG::CIGObject::CIGObject()
	{
#ifdef DEBUG
		cout << "CIGObject create" << endl;
#endif // DEBUG
	}

	CIGObject::~CIGObject()
	{

	}
}
