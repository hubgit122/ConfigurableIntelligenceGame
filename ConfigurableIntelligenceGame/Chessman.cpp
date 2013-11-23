//#include "stdafx.h"
#include "Chessman.h"

CIG::Chessman::Chessman( const Chessman& c )
	: /*CIGNamedObject(c), */chessmanType(c.chessmanType), coordinate(c.coordinate), belongsToWhom(c.belongsToWhom),
	  status(c.status)
{
	for (int i = 0 ; i < CIGRuleConfig::PLAYER_NUM; ++i)
	{
		this->visibility[i] = c.visibility[i];
	}
}

CIG::Chessman::Chessman( /*const string& str,*/ CIGRuleConfig::CHESSMAN_TYPES t, const PointOrVector& c, CIGRuleConfig::PLAYER_NAMES p, CIGRuleConfig::CHESSMAN_STATUS s, CIGRuleConfig::VISIBILITIES v )
	: /*CIGNamedObject(str), */chessmanType(t), coordinate(c), belongsToWhom(p), status(s)
{
	for (int i = 0 ; i < CIGRuleConfig::PLAYER_NUM; ++i)
	{
		this->visibility[i] = CIGRuleConfig::VISIBILITIES::ALL;
	}
}

bool CIG::Chessman::onPickIntent()
{
	if (status == CIGRuleConfig::ON_BOARD)
	{
		status = CIGRuleConfig::OFF_BOARD;
		return true;
	}
	else
	{
		return false;
	}
}

bool CIG::Chessman::onPutIntent( PointOrVector p )
{
	if (status == CIGRuleConfig::OFF_BOARD)
	{
		status = CIGRuleConfig::ON_BOARD;
		coordinate = p;
		return true;
	}
	else
	{
		return false;
	}
}

bool CIG::Chessman::onCapturedIntent()
{
	if (status == CIGRuleConfig::ON_BOARD)
	{
		status = CIGRuleConfig::CAPTURED;
		return true;
	}
	else
	{
		return false;
	}
}

bool CIG::Chessman::onPromotionIntent( CIGRuleConfig::CHESSMAN_TYPES t )
{
	chessmanType = t;
	return true;
}

bool CIG::Chessman::onCaptureIntent(Chessman* c)
{
	return true;
}

CIG::Chessman::~Chessman()
{
}
