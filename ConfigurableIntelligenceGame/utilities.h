#ifndef __UTILITIES_H__
#define __UTILITIES_H__
/*************************************************************************
  包含文件
**************************************************************************/
#include <string.h>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>
using namespace std;


/*************************************************************************
 调试开关
 **************************************************************************/
//#define VERBOS
//#define DEBUG_INTEGER
//#define  DEBUG_ARRAY
//#define DEBUG_CHESSBOARD
//#define DEBUG_REF
//#define DEBUG_MESSAGE 
#define DEBUG_GENERATOR

/*************************************************************************
	辅助类和结构体
	**************************************************************************/
struct PointOrVector_Float
{
	float x[2];

	PointOrVector_Float()
	{
		this->x[0] = 0;
		this->x[1] = 0;
	}

	PointOrVector_Float(float x, float y)
	{
		this->x[0] = x;
		this->x[1] = y;
	}

	float operator*(const PointOrVector_Float&p)
	{
		return x[0]*p.x[0]+x[1]*p.x[1];
	}

	void operator = (const PointOrVector_Float& p)
	{
		memcpy(x, p.x, sizeof(float)*2);
	}

	friend PointOrVector_Float operator*(int lambda, const PointOrVector_Float& p)
	{
		return (float)lambda*p;
	}

	friend PointOrVector_Float operator*(float lambda, const PointOrVector_Float& p)
	{
		PointOrVector_Float temp = p;

		temp.x[0] *= lambda;
		temp.x[1] *= lambda;

		return temp;
	}

	PointOrVector_Float operator- (const PointOrVector_Float& p)
	{
		PointOrVector_Float temp = *this;

		temp.x[0] -= p.x[0];
		temp.x[1] -= p.x[1];

		return temp;
	}

	PointOrVector_Float operator+ (const PointOrVector_Float& p)
	{
		PointOrVector_Float temp = *this;

		temp.x[0] += p.x[0];
		temp.x[1] += p.x[1];

		return temp;
	}

	friend ostringstream& operator<<(ostringstream& oss, const PointOrVector_Float& p)						///不加引用符号, 就会调用拷贝构造函数, id管理得乱七八糟.
	{
		oss << "( "<<p.x[0]<<" , "<<p.x[1]<<" ) \n";
		return oss;
	}
};

struct PointOrVector
{
	unsigned short x[2];

	PointOrVector()
	{
		memset(x,0,sizeof(unsigned short)*2);
	}

	PointOrVector(unsigned x, unsigned y)
	{
		this->x[0] = x;
		this->x[1] = y;
	}

	PointOrVector( unsigned xy )
	{
		*(unsigned*)(this->x) = xy;
	}

	unsigned short& operator[](bool index)
	{
		return x[index];
	}

	PointOrVector operator- (const PointOrVector& p)
	{
		PointOrVector temp = *this;

		*(unsigned*)temp.x -= *(unsigned*)(p.x);
		return temp;
	}

	PointOrVector operator+ (const PointOrVector& p)
	{
		PointOrVector temp = *this;

		*(unsigned*)temp.x += *(unsigned*)(p.x);
		return temp;
	}

	friend ostringstream& operator<<(ostringstream& oss, const PointOrVector& p)						///不加引用符号, 就会调用拷贝构造函数, id管理得乱七八糟.
	{
		oss << "( "<<p.x[0]<<" , "<<p.x[1]<<" ) \n";
		return oss;
	}
};

struct Line
{
	PointOrVector p0,p1;
	Line(int x0, int y0, int x1, int y1):p0(x0,y0),p1(x1,y1)
	{
	}
};

int roundInt( float f );

#endif // UTILITIES



/*************************************************************************
 学习笔记
**************************************************************************/
// 1. realloc失败的时候，返回NULL
//	2. realloc失败的时候，原来的内存不改变，不会释放也不会移动
//	3. 假如原来的内存后面还有足够多剩余内存的话，realloc的内存=原来的内存+剩余内存,realloc还是返回原来内存的地址; 假如原来的内存后面没有足够多剩余内存的话，realloc将申请新的内存，然后把原来的内存数据拷贝到新内存里，原来的内存将被free掉,realloc返回新内存的地址
//	4. 如果size为0，效果等同于free()。这里需要注意的是只对指针本身进行释放，例如对二维指针**a，对a调用realloc时只会释放一维，使用时谨防内存泄露。
//	5. 传递给realloc的指针必须是先前通过malloc(), calloc(), 或realloc()分配的
//	6.传递给realloc的指针可以为空，等同于malloc。
//
// static 成员的访问方式:   类型名::成员, 和java的 类型名.成员 不一样
// 不能用char*, const char*, string对象 作模板参数, C++的模板不如java支持的好. 虽然java不支持基本类型, 但是可以传包装类嘛.