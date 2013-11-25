
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

			T* elements;
			unsigned short size;
			unsigned short capacity;

			void operator = (const Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>& a);

			bool contains(const T& e)const;
			Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>& add(const T& element);
			void addAt(short index, const T e);
			T deleteAt(short index);
			T& at(short index)const;
			T& operator[](short index)const;

			void increaseCapacity();
			void memAlloc();
			void memRealloc();
			void clear();

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

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	void CIG::Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::operator=( const Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>& a )
	{
		clear();
		size = a.size;
		capacity = a.capacity;
		memAlloc();

		//memset(elements, 0, sizeof(T)*size);						//�������һЩδ��ʼ����ָ��
		for (int i = 0; i < size; ++i)
		{
			//memcpy(&this->elements[i], &a.elements[i], sizeof(void*));		//��ʼ���麯��ָ��.

			elements[i] = a.elements[i];
		}
	}

	template <CIG::CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	bool CIG::Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::contains(const T& e) const
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

	//�����������.
	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	void CIG::Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::clear()
	{
		size = 0;
		capacity = INI_DEPTH;

		if (elements)
		{
			free(elements);
		}

		memAlloc();
	}

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	void CIG::Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::memRealloc()
	{
		if ( (elements = (T*)realloc(this->elements, sizeof(T) * (this->capacity))) == NULL)
		{
			this->informError(string("�ڴ治��, ����\n"));
		}
	}

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	void CIG::Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::increaseCapacity()
	{
		if (this->size >= this->capacity)
		{
			if (DEPTH_INCRE < 0)
			{
				informError("�ﵽ�ڴ��������! \n");
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
		if ( (elements = (T*)malloc(sizeof(T) * (this->capacity))) == NULL)
		{
			this->informError(string("�ڴ治��, ����\n"));
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
			this->informError(string("at : ��Χ����\n"));
		}

		if (index < 0)
		{
			index += this->size;
		}

		return this->elements[index];
	}

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	T CIG::Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::deleteAt( short index )
	{
		if ((index >= this->size) || (index < -this->size))
		{
			this->informError(string("at : ��Χ����\n"));
		}

		if (index < 0)			//ָ�������һ��ѭ��, ����-1ָ�����һ��Ԫ��. ������Ҫ����.
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

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	void CIG::Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::addAt( short index, const T e )
	{
		if ((index >= this->size) || (index < -this->size))
		{
			this->informError(string("at : ��Χ����\n"));
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

		memset(&elements[size], 0, sizeof(T));						//�������һЩδ��ʼ����ָ��
		memcpy(&this->elements[index], &e, sizeof(void*));		//��ʼ���麯��ָ��.
		this->elements[index] = e;										//������ֵ
		this->size++;
	}

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>& CIG::Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::add( const T& element )
	{
		// �Ժ�Ҫд������ģ�����, �ܻ��ҵ�. C++�޷�д����һ������������.
		// ���翽��, ���е���operator=�Ϳ������캯����private��, ���������캯���ֲ��ܶԻ�������ʹ��,
		// ֱ���ڴ濽����ǳ����, ���ҿ�����Ϊʹ�ø����ָ�뵫�Ǹ���û�����������, ����4�ֽڵĲ��.
		// ���⻹Ҫ����һ��������, ����ĳЩ���Ͳ�����������ͬ������ʵ������, ����Щ��Ŀ���������ʱҪ��һ��Ҫ�м�¼.

		this->increaseCapacity();
		memset(&elements[size], 0, sizeof(T));
		memcpy(&elements[size], &element, sizeof(void*));
		this->elements[this->size++] = element;											//��α�֤�ٶȺ���ȷ��, ��T��ĸ�ֵ�������.
		//memcpy(this->elements + this->size++, &element, sizeof(T));			//ǳ����������. ����ָ�����.
		return *this;
	}

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	CIG::Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::~Array()
	{
		if (elements)
		{
			free(this->elements);
		}

		elements = NULL;
	}

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	CIG::Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::Array( const Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>& a ): CIGNamedObject(a)
		, size(a.size), capacity(max<unsigned short>(a.capacity, INI_DEPTH))
	{
#ifdef DEBUG
		cout << "Array copy" << endl;
#endif // DEBUG
		memAlloc();

		//memcpy(elements, a.elements, sizeof(T)*a.size);					//�ƻ���װ�����, Ч���ٸ�Ҳ������.
		for (int i = 0; i < size; ++i)
		{
			memcpy(&this->elements[i], &a.elements[i], sizeof(void*));
			elements[i] = a.elements[i];
		}
	}

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	CIG::Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::Array(const string& str): CIGNamedObject<TYPE_ID>(str), size(0), capacity(INI_DEPTH), elements(NULL)
	{
#ifdef DEBUG
		cout << "Array create" << endl;
#endif // DEBUG
		memAlloc();
	}

	class Chessman;
	typedef Array<CIGRuleConfig::CHESSMAN_GROUP, Chessman, CIGRuleConfig::INI_CHESSMAN_GROUP_SIZE, 0> ChessmanGroup;
}
#endif /*__ARRAY_H_*/

// ���ݶ���ʱ, һ��Ҫ����һ��, ��string
// ����๹�캯�����ݲ�������ͨ����Ա��ʼ����, ��ʽ��:                �๹�캯����(�����б�):������(��������б���һ����), ��(ֵ),����