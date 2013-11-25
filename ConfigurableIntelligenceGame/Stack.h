
#ifndef __STACK_H__
#define __STACK_H__

#include "Array.h"
#include "CIGRuleConfig.h"

namespace CIG
{
	class Chessboard;
	class Operation;
	template <class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	class Stack: public Array< T, INI_DEPTH, DEPTH_INCRE>
	{
		public:
			Stack();
			Stack(const Stack& s);
			virtual ~Stack();

			void push(const T& E);
			T popThenGet();
			void popNoReturn();
			T& top()const;
			friend ostream& operator << (ostream& os, const Stack<T, INI_DEPTH, DEPTH_INCRE>& o)
			{
				ostringstream oss;
				oss << o;
				os << oss.str();
				return os;
			}

			friend ostringstream& operator << (ostringstream& oss, const Stack<T, INI_DEPTH, DEPTH_INCRE>& o)
			{
				oss << "Stack<" << typeid(T).name() << ',' << INI_DEPTH << ',' << DEPTH_INCRE << ">::\n";
				oss << (const Array<T, INI_DEPTH, DEPTH_INCRE>&)o;
				return oss;
			}
	};

	template <class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	CIG::Stack<T, INI_DEPTH, DEPTH_INCRE>::~Stack()
	{
	}

	template <class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	T& CIG::Stack<T, INI_DEPTH, DEPTH_INCRE>::top() const
	{
		return this->at(this->size - 1);					//size-1 这个错误犯了两次了.
	}

	template <class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	T CIG::Stack<T, INI_DEPTH, DEPTH_INCRE>::popThenGet()
	{
		return Array<T, INI_DEPTH, DEPTH_INCRE>::deleteAtThenGet(this->size - 1);
	}

	template <class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	void CIG::Stack<T, INI_DEPTH, DEPTH_INCRE>::popNoReturn()
	{
		Array<T, INI_DEPTH, DEPTH_INCRE>::deleteAtNoReturn(this->size - 1);
	}

	template <class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	void CIG::Stack<T, INI_DEPTH, DEPTH_INCRE>::push( const T& E )
	{
		Array<T, INI_DEPTH, DEPTH_INCRE>::add(E);
	}

	template <class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	Stack<T, INI_DEPTH, DEPTH_INCRE>::Stack(){}

	template <class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	CIG::Stack<T, INI_DEPTH, DEPTH_INCRE>::Stack( const Stack& s ): Array(s) {}

	typedef Stack<Operation, CIGRuleConfig::INT_BOARD_HISTORY_STACK_SIZE, 0> OperationStack;
	typedef Stack<Chessboard, CIGRuleConfig::INT_BOARD_HISTORY_STACK_SIZE, 0> ChessboardStack;
	typedef	 Stack<CIGRuleConfig::OPERATIONS, CIGRuleConfig::INT_BOARD_HISTORY_STACK_SIZE, 0> StatusStack;
	typedef Stack<OperationStack, CIGRuleConfig::INT_BOARD_HISTORY_STACK_SIZE, 0> ActionStack;
}

#endif /*__STACK_H_*/