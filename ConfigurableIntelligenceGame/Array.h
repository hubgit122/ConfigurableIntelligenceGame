
#ifndef __ARRAY_H__
#define __ARRAY_H__
#include "utilities.h"
#include "CIGRuleConfig.h"
#include "CIGNamedObject.h"

namespace CIG
{
	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	class Array: public CIGNamedObject<TYPE_ID>
	{
		private:
			Array();
		public:
			Array(const string& str);
			Array(const Array& a);
			virtual ~Array();

			T** elements;
			unsigned short size;
			unsigned short capacity;

			void clear();
			bool contains(const T& e)const;
			Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>& add(const T& element);
			void addAt(short index, const T& e);
			T deleteAt(short index);
			T& at(short index)const;
			T& operator[](short index)const;
			void increaseCapacity();
			void memAlloc();
			void memRealloc();

			friend ostream& operator << (ostream& os, const Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>& o)
			{
				ostringstream oss;
				oss << o;
				os << oss.str();
				return os;
			}

			friend ostringstream& operator << (ostringstream& oss, const Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>& o)
			{
				oss << "Array<" << TYPE_ID/*CIGConfig::CLASS_TYPES[TYPE_ID]*/ << ',' << typeid(T).name() << ',' << INI_DEPTH << ',' << DEPTH_INCRE << ">:: \n\tsize = " << o.size << "\n\tcapacity = " << o.capacity << "\n\telements: \n";

				for (unsigned short i = 0; i < o.size; ++i)
				{
					oss << "\t\t";
					oss << o.elements[i] << '\n';
				}

				oss << (CIGNamedObject<TYPE_ID>&)o ;
				return oss;
			}
	};

	template <CIG::CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	bool CIG::Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::contains(const T& e) const			// TO-DO 这种函数疑问很多
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

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	void CIG::Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::clear()
	{
		for (int i=0; i<size;++i)
		{
			delete this->elements[i];
		}
		size = 0;
	}

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	void CIG::Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::memRealloc()
	{
		if ( (elements = (T**)realloc(this->elements, sizeof(T*) * (this->capacity))) == NULL)
		{
			this->informError(string("内存不足, 请检查\n"));
		}
	}

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	void CIG::Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::increaseCapacity()
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

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	void CIG::Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::memAlloc()
	{
		if ( (elements = (T**)malloc(sizeof(T*) * (this->capacity))) == NULL)
		{
			this->informError(string("内存不足, 请检查\n"));
		}
	}

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	T& CIG::Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::operator []( short index ) const
	{
		return this->at(index);
	}

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	T& CIG::Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::at( short index ) const
	{
		if ((index >= this->size) || (index < -this->size))
		{
			this->informError(string("at : 范围错误\n"));
		}

		if (index < 0)
		{
			index += this->size;
		}

		return *(this->elements[index]);
	}

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	T CIG::Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::deleteAt( short index )
	{
		if ((index >= this->size) || (index < -this->size))
		{
			this->informError(string("at : 范围错误\n"));
		}

		if (index < 0)			//指标可以有一点循环, 如用-1指定最后一个元素. 还不需要除法.
		{
			index += this->size;
		}

		T temp(*(this->elements[index]));
		delete (this->elements[index]);

		for (unsigned short i = index; i < this->size; ++i)
		{
			this->elements[i] = this->elements[i + 1];
		}

		this->size--;

		return temp;
	}

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	void CIG::Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::addAt( short index, const T& e )
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

		this->elements[index] = new T(e);
		this->size++;
	}

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>& CIG::Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::add( const T& element )
	{
		// 以后不要写这样的模板代码, 很混乱的. C++无法写出来一个完美的容器. 
		// 比如拷贝, 用有的类operator=和拷贝构造函数是private的, 而拷贝构造函数又不能对基本类型使用, 
		// 直接内存拷贝是浅拷贝, 而且可能因为使用父类的指针但是父类没有虚表子类有, 导致4字节的差别. 
		// 另外还要考虑一致性问题, 比如某些类型不允许属性相同的两个实例存在, 而有些类的拷贝和析构时要求一定要有记录. 
		
		this->increaseCapacity();
		this->elements[this->size++] = new T(element);		//拷贝构造, 深拷贝. 
		//this->elements[this->size++](element);		//拷贝构造, 深拷贝. 
		//memcpy(this->elements + this->size++, &element, sizeof(T));			//浅拷贝不可以. 各种指针错误. 
		return *this;
	}

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	CIG::Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::~Array()
	{
		for (int i=0; i<this->size;++i)
		{
			delete elements[i];
		}
		free(this->elements);
	}

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	CIG::Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::Array( const Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>& a ): CIGNamedObject(a)
	{
#ifdef DEBUG
		cout << "Array copy" << endl;
#endif // DEBUG
		this->entityName =  string(a.entityName);
		size = a.size;
		this->capacity = max<unsigned short>(a.capacity, INI_DEPTH);
		memAlloc();
		for (int i = 0; i < a.size; i++)
		{
			elements[i] = new T(*a.elements[i]);
		}
	}

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	CIG::Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::Array(const string& str): CIGNamedObject<TYPE_ID>(str), size(0), capacity(INI_DEPTH)
	{
#ifdef DEBUG
		cout << "Array create" << endl;
#endif // DEBUG
		memAlloc();
	}
}
#endif /*__ARRAY_H_*/

// 传递对象时, 一定要拷贝一份, 如string
// 向基类构造函数传递参数可以通过成员初始化表, 格式如:                类构造函数名(参数列表):基类名(本类参数列表的一部分), 域(值),……