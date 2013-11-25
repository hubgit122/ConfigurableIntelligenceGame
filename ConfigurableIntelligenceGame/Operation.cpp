#include "stdafx.h"
#include "Operation.h"




CIG::Operation::Operation() : chessmanLocation(), operation(CIGRuleConfig::NOMORE), distination(0)
{

}

CIG::Operation::Operation( ChessmanLocation cl, CIGRuleConfig::OPERATIONS op, PointOrVector dist /*= 0*/ ) : chessmanLocation(cl), operation(op), distination(dist)
{
}

CIG::Operation::Operation( const Operation& op ): chessmanLocation(op.chessmanLocation), operation(op.operation), distination(op.distination)
{
}

CIG::Operation::~Operation()
{

}

void CIG::Operation::operator=( const Operation& op )
{
	memcpy(this, &op, sizeof(Operation));
}