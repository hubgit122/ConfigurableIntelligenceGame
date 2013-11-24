#include "Operation.h"




CIG::Operation::Operation() : chessmanLocation(), operation(CIGRuleConfig::NOMORE), extra(0)
{

}

CIG::Operation::Operation( ChessmanLocation cl, CIGRuleConfig::OPERATIONS op, DWORD extra_ /*= 0*/ ) : chessmanLocation(cl), operation(op), extra(extra_)
{
}

CIG::Operation::Operation( const Operation& op ):chessmanLocation(op.chessmanLocation), operation(op.operation), extra(op.extra)
{
}

CIG::Operation::~Operation()
{

}

void CIG::Operation::operator=( const Operation&op )
{
	memcpy(this,&op,sizeof(Operation));
}

void CIG::Operation::valify()
{

}
