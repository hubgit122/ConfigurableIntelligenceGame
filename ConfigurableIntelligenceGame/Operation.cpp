#include "stdafx.h"
#include "Operation.h"


CIG::Operation::Operation() : chessmanIndex(), operation(CIGRuleConfig::NOMORE_CHESS_OPERATION), distination(0), savedCoodinate(0)
{

}

CIG::Operation::Operation( ChessmanIndex ci, CIGRuleConfig::CHESS_OPERATIONS op, PointOrVector dist /*= 0*/ , PointOrVector pre /*= 0*/) 
	: chessmanIndex(ci), operation(op), distination(dist), savedCoodinate(pre)
{
}

CIG::Operation::Operation( const Operation& op ): chessmanIndex(op.chessmanIndex), operation(op.operation), distination(op.distination), savedCoodinate(op.savedCoodinate)
{
}

CIG::Operation::~Operation()
{

}

void CIG::Operation::operator=( const Operation& op )
{
	memcpy(this, &op, sizeof(Operation));
}