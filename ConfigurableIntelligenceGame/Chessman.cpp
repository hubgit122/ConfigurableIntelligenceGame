#include "stdafx.h"
#include "Chessman.h"

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

void CIG::Chessman::operator=( const Chessman& c )
{
	memcpy(this, &c, sizeof(Chessman));
}

void CIG::Chessman::undoPick()
{
	this->status = CIGRuleConfig::ON_BOARD;
}

void CIG::Chessman::undoPut()
{
	this->status = CIGRuleConfig::OFF_BOARD;
}

void CIG::Chessman::undoCapture()
{
	this->status = CIGRuleConfig::ON_BOARD;
}
