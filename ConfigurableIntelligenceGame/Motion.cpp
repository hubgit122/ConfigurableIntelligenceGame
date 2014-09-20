#include "stdafx.h"
#include "Motion.h"


CIG::Motion::Motion() : chessmanIndex(), operation(CIGRuleConfig::NOMORE), distination(0)
{

}

CIG::Motion::Motion( ChessmanIndex ci, CIGRuleConfig::OPERATIONS op, PointOrVector dist /*= 0*/, PointOrVector pre /*= 0*/ )
	: chessmanIndex(ci), operation(op), distination(dist), savedCoodinate(pre)
{
}

CIG::Motion::Motion( const Motion& op )
	: chessmanIndex(op.chessmanIndex), operation(op.operation), distination(op.distination), savedCoodinate(op.savedCoodinate)
{
}

CIG::Motion::~Motion()
{

}

void CIG::Motion::operator=( const Motion& op )
{
	memcpy(this, &op, sizeof(Motion));
}

bool CIG::Motion::operator==( const Motion& op ) const
{
	return (this->chessmanIndex.player == op.chessmanIndex.player) && (this->chessmanIndex.index == op.chessmanIndex.index) && (this->distination == op.distination) && (this->operation == op.operation) && (this->savedCoodinate == this->savedCoodinate);
}
