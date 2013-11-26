
#include "ChessmanLocationBoard.h"

namespace CIG
{
	void CIG::ChessmanLocationBoard::operator=( const ChessmanLocationBoard& clb )
	{
		if (!content)
		{
			if ((content = (ChessmanLocation*) malloc(sizeof(ChessmanLocation)<<CIGRuleConfig::INI_BOARD_HEIGHT_LOG2<<CIGRuleConfig::INI_BOARD_WIDTH_LOG2))==0)
			{
				GUI::inform("在生成ChessmanLocaionBoard时内存不足. ",true);
			}
		}
		memcpy(content, clb.content, sizeof(ChessmanLocation)<<CIGRuleConfig::INI_BOARD_HEIGHT_LOG2<<CIGRuleConfig::INI_BOARD_WIDTH_LOG2);
	}

	ChessmanLocationBoard::ChessmanLocationBoard()
	{
		if ((content = (ChessmanLocation*) malloc(sizeof(ChessmanLocation)<<CIGRuleConfig::INI_BOARD_HEIGHT_LOG2<<CIGRuleConfig::INI_BOARD_WIDTH_LOG2))==0)
		{
			GUI::inform("在生成ChessmanLocaionBoard时内存不足. ",true);
		}
		memset(content,-1,sizeof(ChessmanLocation)<<CIGRuleConfig::INI_BOARD_HEIGHT_LOG2<<CIGRuleConfig::INI_BOARD_WIDTH_LOG2);
	}

	ChessmanLocationBoard::ChessmanLocationBoard( const ChessmanLocationBoard& clb )
	{
		if ((content = (ChessmanLocation*) malloc(sizeof(ChessmanLocation)<<CIGRuleConfig::INI_BOARD_HEIGHT_LOG2<<CIGRuleConfig::INI_BOARD_WIDTH_LOG2))==0)
		{
			GUI::inform("在生成ChessmanLocaionBoard时内存不足. ",true);
		}
		memcpy(content, clb.content, sizeof(ChessmanLocation)<<CIGRuleConfig::INI_BOARD_HEIGHT_LOG2<<CIGRuleConfig::INI_BOARD_WIDTH_LOG2);
	}

	ChessmanLocationBoard::~ChessmanLocationBoard()
	{
		if (content)
		{
			free(content);
		}
	}

	CIG::ChessmanLocation& ChessmanLocationBoard::operator[]( PointOrVector p )
	{
		return content[ (p[1]<<CIGRuleConfig::INI_BOARD_WIDTH_LOG2)+p[0] ];
	}
}


