#include "Operation.h"




CIG::Operation::Operation() : chessman(NULL), operation(CIGRuleConfig::NOMORE), extra(0)
{

}

CIG::Operation::Operation( Chessman* c, CIGRuleConfig::OPERATIONS op, DWORD extra_ /*= 0*/ ) : chessman(c), operation(op), extra(extra_)
{
}

CIG::Operation::Operation( const Operation& op ):chessman(op.chessman), operation(op.operation), extra(op.extra)
{
}

CIG::Operation::~Operation()
{

}
