#ifndef __UTILITIES_H__
#define __UTILITIES_H__
/*************************************************************************
  �����ļ�
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
 ���Կ���
 **************************************************************************/
//#define VERBOS
//#define DEBUG_INTEGER
//#define  DEBUG_ARRAY
//#define DEBUG_CHESSBOARD
//#define DEBUG_REF
//#define DEBUG_MESSAGE
#define DEBUG_GENERATOR

/*************************************************************************
	������ͽṹ��
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

	float operator*(const PointOrVector_Float& p)
	{
		return x[0] * p.x[0] + x[1] * p.x[1];
	}

	void operator = (const PointOrVector_Float& p)
	{
		memcpy(x, p.x, sizeof(float) * 2);
	}

	friend PointOrVector_Float operator*(int lambda, const PointOrVector_Float& p)
	{
		return (float)lambda * p;
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

	friend ostringstream& operator<<(ostringstream& oss, const PointOrVector_Float& p)						///�������÷���, �ͻ���ÿ������캯��, id���������߰���.
	{
		oss << "( " << p.x[0] << " , " << p.x[1] << " ) \n";
		return oss;
	}
};

struct PointOrVector
{
	short x[2];

	PointOrVector()
	{
		memset(x, 0, sizeof(unsigned short) * 2);
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

	short& operator[](bool index)
	{
		return x[index];
	}

	PointOrVector operator- (const PointOrVector& p)
	{
		PointOrVector temp = *this;

		//*(unsigned*)temp.x -= *(unsigned*)(p.x);				//�漰����λʱ��������ʧ��. 
		temp.x[0]-=p.x[0];
		temp.x[1]-=p.x[1];
		return temp;
	}

	PointOrVector operator+ (const PointOrVector& p)
	{
		PointOrVector temp = *this;

		temp.x[0]+=p.x[0];
		temp.x[1]+=p.x[1];

		//*(unsigned*)temp.x += *(unsigned*)(p.x);			//�ڵ�λ�и�����ʱ�����ּӷ�ʧ��. 
		return temp;
	}

	friend ostringstream& operator<<(ostringstream& oss, const PointOrVector& p)						///�������÷���, �ͻ���ÿ������캯��, id���������߰���.
	{
		oss << "( " << p.x[0] << " , " << p.x[1] << " ) \n";
		return oss;
	}
};

struct Line
{
	PointOrVector p0, p1;
	Line(int x0, int y0, int x1, int y1): p0(x0, y0), p1(x1, y1)
	{
	}
};

int roundInt( float f );

#endif // UTILITIES



/*************************************************************************
 ѧϰ�ʼ�
**************************************************************************/
// 1. reallocʧ�ܵ�ʱ�򣬷���NULL
//	2. reallocʧ�ܵ�ʱ��ԭ�����ڴ治�ı䣬�����ͷ�Ҳ�����ƶ�
//	3. ����ԭ�����ڴ���滹���㹻��ʣ���ڴ�Ļ���realloc���ڴ�=ԭ�����ڴ�+ʣ���ڴ�,realloc���Ƿ���ԭ���ڴ�ĵ�ַ; ����ԭ�����ڴ����û���㹻��ʣ���ڴ�Ļ���realloc�������µ��ڴ棬Ȼ���ԭ�����ڴ����ݿ��������ڴ��ԭ�����ڴ潫��free��,realloc�������ڴ�ĵ�ַ
//	4. ���sizeΪ0��Ч����ͬ��free()��������Ҫע�����ֻ��ָ�뱾�������ͷţ�����Զ�άָ��**a����a����reallocʱֻ���ͷ�һά��ʹ��ʱ�����ڴ�й¶��
//	5. ���ݸ�realloc��ָ���������ǰͨ��malloc(), calloc(), ��realloc()�����
//	6.���ݸ�realloc��ָ�����Ϊ�գ���ͬ��malloc��
//
// static ��Ա�ķ��ʷ�ʽ:   ������::��Ա, ��java�� ������.��Ա ��һ��
// ������char*, const char*, string���� ��ģ�����, C++��ģ�岻��java֧�ֵĺ�. ��Ȼjava��֧�ֻ�������, ���ǿ��Դ���װ����.