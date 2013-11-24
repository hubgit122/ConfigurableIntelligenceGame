//#include "stdafx.h"
#include "ChessmanGroup.h"


CIG::ChessmanGroup::ChessmanGroup( const string& str ) : Array(str)
{

}

CIG::ChessmanGroup::ChessmanGroup( const ChessmanGroup& c ) : Array(c)
{

}

CIG::ChessmanGroup::~ChessmanGroup()
{

}

void CIG::ChessmanGroup::operator=( const ChessmanGroup& cg )
{
	(Array*)this
}
