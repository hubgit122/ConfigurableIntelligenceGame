#include "stdafx.h"
#include "Operation.h"


CIG::Operation::Operation() : chessmanIndex(), operation(CIGRuleConfig::NOMORE), distination(0)
{

}

CIG::Operation::Operation( ChessmanIndex ci, CIGRuleConfig::OPERATIONS op, PointOrVector dist /*= 0*/ ) 
	: chessmanIndex(ci), operation(op), distination(dist)
{
}

CIG::Operation::Operation( const Operation& op )
	: chessmanIndex(op.chessmanIndex), operation(op.operation), distination(op.distination)
{
}

CIG::Operation::~Operation()
{

}

void CIG::Operation::operator=( const Operation& op )
{
	memcpy(this, &op, sizeof(Operation));
}

bool CIG::Operation::operator==( const Operation& op ) const
{
	return (this->chessmanIndex.player==op.chessmanIndex.player)&&(this->chessmanIndex.index==op.chessmanIndex.index)&&(this->distination==op.distination)&&(this->operation==op.operation);
}
