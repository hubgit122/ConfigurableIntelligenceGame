
#ifndef __STACK_H__
#define __STACK_H__

#include "Array.h"
#include "CIGNamedObject.h"
#include "CIGRuleConfig.h"

namespace CIG
{
	class Chessboard;
	class Operation;
	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	class Stack: public Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>
	{
		private:
			Stack();
		public:
			Stack(const string& str);
			Stack(const Stack& s);
			virtual ~Stack();

			void push(const T& E);
			T pop();
			T& top()const;
			friend ostream& operator << (ostream& os, const Stack<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>& o)
			{
				ostringstream oss;
				oss << o;
				os << oss.str();
				return os;
			}

			friend ostringstream& operator << (ostringstream& oss, const Stack<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>& o)
			{
				oss << "Stack<" << TYPE_ID/*CIGConfig::CLASS_TYPES[TYPE_ID]*/ << ',' << typeid(T).name() << ',' << INI_DEPTH << ',' << DEPTH_INCRE << ">::\n";
				oss << (const Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>&)o;
				return oss;
			}
	};

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	CIG::Stack<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::~Stack()
	{
	}

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	T& CIG::Stack<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::top() const
	{
		return this->at(this->size - 1);					//size-1 这个错误犯了两次了.
	}

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	T CIG::Stack<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::pop()
	{
		return Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::deleteAt(this->size - 1);
	}

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	void CIG::Stack<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::push( const T& E )
	{
		Array<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::add(E);
	}

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	Stack<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::Stack( const string& str ) : Array(str) {}

	template <CIGRuleConfig::CLASS_TYPES TYPE_ID, class T, unsigned short INI_DEPTH, unsigned short DEPTH_INCRE>
	CIG::Stack<TYPE_ID, T, INI_DEPTH, DEPTH_INCRE>::Stack( const Stack& s ): Array(s) {}

	typedef Stack<CIGRuleConfig::CLASS_TYPES::OPERATION_STACK, Operation, CIGRuleConfig::INT_BOARD_HISTORY_STACK_SIZE, 0> OperationStack;
	typedef Stack<CIGRuleConfig::CLASS_TYPES::CHESSBOARD_STACK, Chessboard, CIGRuleConfig::INT_BOARD_HISTORY_STACK_SIZE, 0> ChessboardStack;
	typedef	 Stack<CIGRuleConfig::CLASS_TYPES::MOTION_STATUS, CIGRuleConfig::OPERATIONS, CIGRuleConfig::INT_BOARD_HISTORY_STACK_SIZE, 0> StatusStack;
}

#endif /*__STACK_H_*/