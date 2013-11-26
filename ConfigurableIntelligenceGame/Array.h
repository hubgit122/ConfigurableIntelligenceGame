
#ifndef __ARRAY_H__
#define __ARRAY_H__
#include "utilities.h"
#include "CIGRuleConfig.h"
#include "CIGObject.h"

namespace CIG
{
	template <class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	class Array: public CIGObject
	{
		public:
			Array();
			Array(const Array& a);
			virtual ~Array();

			T* elements;
			unsigned short size;
			unsigned short capacity;

			void operator = (const Array<T, INI_DEPTH, DEPTH_INCRE>& a);

			bool contains(const T& e)const;
			Array<T, INI_DEPTH, DEPTH_INCRE>& add(const T& element);
			void addAt(short index, const T e);
			void deleteAtNoReturn(short index);
			T deleteAtThenGet(short index);
			T& at(short index)const;
			T& operator[](short index)const;

			void increaseCapacity();
			void memAlloc();
			void memRealloc();
			void clear();

			friend ostream& operator << (ostream& os, const Array<T, INI_DEPTH, DEPTH_INCRE>& o)
			{
				ostringstream oss;
				oss << o;
				os << oss.str();
				return os;
			}

			friend ostringstream& operator << (ostringstream& oss, const Array<T, INI_DEPTH, DEPTH_INCRE>& o)
			{
				oss << "Array<" << typeid(T).name() << ',' << INI_DEPTH << ',' << DEPTH_INCRE << ">:: \n\tsize = " << o.size << "\n\tcapacity = " << o.capacity << "\n\telements: \n";

				for (unsigned short i = 0; i < o.size; ++i)
				{
					oss << "\t\t";
					oss << o.elements[i] << '\n';
				}

				return oss;
			}
	};

	template <class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	void CIG::Array<T, INI_DEPTH, DEPTH_INCRE>::deleteAtNoReturn( short index )
	{
		if ((index >= this->size) || (index < -this->size))
		{
			this->informError(string("at : 范围错误\n"));
		}

		if (index < 0)			//指标可以有一点循环, 如用-1指定最后一个元素. 还不需要除法.
		{
			index += this->size;
		}

		for (unsigned short i = index; i + 1 < this->size; ++i)
		{
			this->elements[i] = this->elements[i + 1];
		}

		this->size--;
	}

	template <class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	void CIG::Array<T, INI_DEPTH, DEPTH_INCRE>::operator=( const Array<T, INI_DEPTH, DEPTH_INCRE>& a )
	{
		clear();
		size = a.size;
		capacity = a.capacity;
		memAlloc();

		//memset(elements, 0, sizeof(T)*size);						//可以清空一些未初始化的指针
		for (int i = 0; i < size; ++i)
		{
			//memcpy(&this->elements[i], &a.elements[i], sizeof(void*));		//初始化虚函数指针.

			elements[i] = a.elements[i];
		}
	}

	template <class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	bool CIG::Array<T, INI_DEPTH, DEPTH_INCRE>::contains(const T& e) const
	{
		for (int i = size - 1; i >= 0; --i)
		{
			if (this->at[i] == e)
			{
				return true;
			}
		}

		return false;
	}

	//清空所有内容.
	template <class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	void CIG::Array<T, INI_DEPTH, DEPTH_INCRE>::clear()
	{
		size = 0;
		capacity = INI_DEPTH;

		if (elements)
		{
			free(elements);
		}
	}

	template <class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	void CIG::Array<T, INI_DEPTH, DEPTH_INCRE>::memRealloc()
	{
		if ( (elements = (T*)realloc(this->elements, sizeof(T) * (this->capacity))) == NULL)
		{
			this->informError(string("内存不足, 请检查\n"));
		}
	}

	template <class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	void CIG::Array<T, INI_DEPTH, DEPTH_INCRE>::increaseCapacity()
	{
		if (this->size >= this->capacity)
		{
			if (DEPTH_INCRE < 0)
			{
				informError("达到内存最大限制! \n");
			}
			else
			{
				this->capacity += ((DEPTH_INCRE == 0) ? this->capacity : DEPTH_INCRE);
				memRealloc();
			}
		}
	}

	template <class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	void CIG::Array<T, INI_DEPTH, DEPTH_INCRE>::memAlloc()
	{
		if ( (elements = (T*)malloc(sizeof(T) * (this->capacity))) == NULL)
		{
			this->informError(string("内存不足, 请检查\n"));
		}
	}

	template <class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	T& CIG::Array<T, INI_DEPTH, DEPTH_INCRE>::operator []( short index ) const
	{
		return this->at(index);
	}

	template <class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	T& CIG::Array<T, INI_DEPTH, DEPTH_INCRE>::at( short index ) const
	{
		if ((index >= this->size) || (index < -this->size))
		{
			this->informError(string("at : 范围错误\n"));
		}

		if (index < 0)
		{
			index += this->size;
		}

		return this->elements[index];
	}

	template <class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	T CIG::Array<T, INI_DEPTH, DEPTH_INCRE>::deleteAtThenGet( short index )
	{
		if ((index >= this->size) || (index < -this->size))
		{
			this->informError(string("at : 范围错误\n"));
		}

		if (index < 0)			//指标可以有一点循环, 如用-1指定最后一个元素. 还不需要除法.
		{
			index += this->size;
		}

		T temp = this->elements[index];

		for (unsigned short i = index; i + 1 < this->size; ++i)
		{
			this->elements[i] = this->elements[i + 1];
		}

		this->size--;

		return temp;
	}

	template <class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	void CIG::Array<T, INI_DEPTH, DEPTH_INCRE>::addAt( short index, const T e )
	{
		if ((index >= this->size) || (index < -this->size))
		{
			this->informError(string("at : 范围错误\n"));
		}

		if (index < 0)
		{
			index += this->size;
		}

		this->increaseCapacity();

		for (unsigned short i = this->size - 1; i >= index; --i)
		{
			this->elements[i + 1] = this->elements[i];
		}

		//注意, 只有自己或者父类有虚函数, 而且是单继承结构的类型, 才能安全地使用这个模板数组. 
		memset(&elements[size], 0, sizeof(T));						//可以清空一些未初始化的指针
		memcpy(&this->elements[index], &e, sizeof(void*));		//初始化虚函数指针.
		this->elements[index] = e;										//正常赋值
		this->size++;
	}

	template <class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	Array<T, INI_DEPTH, DEPTH_INCRE>& CIG::Array<T, INI_DEPTH, DEPTH_INCRE>::add( const T& element )
	{
		// 以后不要写这样的模板代码, 很混乱的. C++无法写出来一个完美的容器.
		// 比如拷贝, 用有的类operator=和拷贝构造函数是private的, 而拷贝构造函数又不能对基本类型使用,
		// 直接内存拷贝是浅拷贝, 而且可能因为使用父类的指针但是父类没有虚表子类有, 导致4字节的差别.
		// 另外还要考虑一致性问题, 比如某些类型不允许属性相同的两个实例存在, 而有些类的拷贝和析构时要求一定要有记录.

		this->increaseCapacity();
		memset(&elements[size], 0, sizeof(T));
		memcpy(&elements[size], &element, sizeof(void*));
		this->elements[this->size++] = element;											//如何保证速度和正确性, 由T类的赋值运算决定.
		//memcpy(this->elements + this->size++, &element, sizeof(T));			//浅拷贝不可以. 各种指针错误.
		return *this;
	}

	template <class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	CIG::Array<T, INI_DEPTH, DEPTH_INCRE>::~Array()
	{
		if (elements)
		{
			free(this->elements);
		}

		elements = NULL;
	}

	template <class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	CIG::Array<T, INI_DEPTH, DEPTH_INCRE>::Array( const Array<T, INI_DEPTH, DEPTH_INCRE>& a )
		: size(a.size), capacity(max<unsigned short>(a.capacity, INI_DEPTH))
	{
		memAlloc();

		for (int i = 0; i < size; ++i)
		{
			memcpy(&this->elements[i], &a.elements[i], sizeof(void*));
			elements[i] = a.elements[i];
		}
	}

	template <class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	CIG::Array<T, INI_DEPTH, DEPTH_INCRE>::Array()
		:size(0), capacity(INI_DEPTH), elements(NULL)
	{
		memAlloc();
	}

	class Chessman;
	typedef Array<Chessman, CIGRuleConfig::INI_CHESSMAN_GROUP_SIZE, 0> ChessmanGroup;
}
#endif /*__ARRAY_H_*/

// 传递对象时, 一定要拷贝一份, 如string
// 向基类构造函数传递参数可以通过成员初始化表, 格式如:                类构造函数名(参数列表):基类名(本类参数列表的一部分), 域(值),……