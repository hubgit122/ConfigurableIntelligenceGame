//#include "stdafx.h"
#include "GUI.h"
#include "CIGRuleConfig.h"

namespace CIG
{
	bool CIG::CIGRuleConfig::checkConstrains()
	{
		typedef unsigned long long U64;

		if (sizeof(U64) != 8)					//检测过之后, 认为sizeof(U64)==8, 这样会化简一些除法.
		{
			ostringstream oss;
			oss << "系统字长与期望不同. \n" << "sizeof(U64) = " << sizeof(U64) << " != 8";
			GUI::inform(oss.str(), true);
			return false;
		}

		if (PLAYER_NUM != 2)
		{
			ostringstream oss;
			oss << "暂时不支持多人游戏. \n" ;
			GUI::inform(oss.str(), true);
			return false;
		}

		return true;
	}

	const float CIGRuleConfig::TIME_LIMIT = -1;
	//
	//const int CIG::CIGRuleConfig::INT_BANNED_MOTION_SIZE;
	//
	//const int CIG::CIGRuleConfig::INI_LATTICE_GROUP_SIZE;
	//
	//const int CIG::CIGRuleConfig::INI_CHESSMAN_GROUP_SIZE;

	const CIGRuleConfig::OPERATIONS CIGRuleConfig::operationGraph[NOMORE + 1][NOMORE + 1] =
	{
		{PICK, NOMORE, NOMORE, NOMORE, NOMORE, NOMORE, NOMORE, NOMORE},
		{PUT, CAPTURE, NOMORE, NOMORE, NOMORE, NOMORE, NOMORE, NOMORE},
		{END, NOMORE, NOMORE, NOMORE, NOMORE, NOMORE, NOMORE, NOMORE},
		{PUT, NOMORE, NOMORE, NOMORE, NOMORE, NOMORE, NOMORE, NOMORE},
		{NOMORE, NOMORE, NOMORE, NOMORE, NOMORE, NOMORE, NOMORE, NOMORE},
		{NOMORE, NOMORE, NOMORE, NOMORE, NOMORE, NOMORE, NOMORE, NOMORE},
		{NOMORE, NOMORE, NOMORE, NOMORE, NOMORE, NOMORE, NOMORE, NOMORE},
		{NOMORE, NOMORE, NOMORE, NOMORE, NOMORE, NOMORE, NOMORE, NOMORE},
	};

	const CIGRuleConfig::CHESSMAN_TYPES CIGRuleConfig::INI_BOARD[PLAYER_NUM][1 << INI_BOARD_HEIGHT_LOG2][1 << INI_BOARD_WIDTH_LOG2] =
	{
		{
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,							},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,							},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,							},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN, NOCHESSMAN, NOCHESSMAN, NOCHESSMAN, NOCHESSMAN, NOCHESSMAN, NOCHESSMAN, NOCHESSMAN, NOCHESSMAN, NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,				},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,							},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN, NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN, NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,						},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN, NOCHESSMAN,  NOCHESSMAN, NOCHESSMAN,  NOCHESSMAN, NOCHESSMAN,  NOCHESSMAN, NOCHESSMAN,  NOCHESSMAN, NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,					},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,							},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,							},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN, PAWN,  NOCHESSMAN, PAWN,  NOCHESSMAN, PAWN,  NOCHESSMAN, PAWN,  NOCHESSMAN, PAWN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,					},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN, CANNON,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN, CANNON,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,						},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,							},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN, CHAROIT, HORSE, ELEPHANT,  ADVISOR,  KING,  ADVISOR, ELEPHANT, HORSE, CHAROIT,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,					},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,							},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,							},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN							},
		},
		{
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,							},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,							},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,							},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN, CHAROIT, HORSE, ELEPHANT,  ADVISOR,  KING,  ADVISOR, ELEPHANT, HORSE, CHAROIT,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,					},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN, NOCHESSMAN, NOCHESSMAN, NOCHESSMAN, NOCHESSMAN, NOCHESSMAN, NOCHESSMAN, NOCHESSMAN, NOCHESSMAN, NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,				},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN, CANNON,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN, CANNON,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,						},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN, PAWN,  NOCHESSMAN, PAWN,  NOCHESSMAN, PAWN,  NOCHESSMAN, PAWN,  NOCHESSMAN, PAWN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,					},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,							},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN, NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN, NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,						},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN, NOCHESSMAN,  NOCHESSMAN, NOCHESSMAN,  NOCHESSMAN, NOCHESSMAN,  NOCHESSMAN, NOCHESSMAN,  NOCHESSMAN, NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,					},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,							},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,							},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,							},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,							},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,							},
			{NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN,  NOCHESSMAN							},
		}
	};

	const bool CIGRuleConfig::BOARD_RANGE[1 << INI_BOARD_HEIGHT_LOG2][1 << INI_BOARD_WIDTH_LOG2] =
	{
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, },
		{0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, },
		{0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, },
		{0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, },
		{0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, },
		{0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, },
		{0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, },
		{0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, },
		{0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, },
		{0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
	};

	const int CIGRuleConfig::EVALUATIONS[PLAYER_NUM][NUM_OF_CHESSMAN_TYPE][1 << INI_BOARD_HEIGHT_LOG2][1 << INI_BOARD_WIDTH_LOG2] =
	{
		{
			{
				// 帅(将)
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  2,  2,  2,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0, 11, 15, 11,  0,  0,  0,  0,  0,  0, 0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}
			},
			{
				// 仕(士)
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0, 20,  0, 20,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0, 23,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0, 20,  0, 20,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}
			},
			{
				// 相(象)
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,				},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,				},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,				},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,				},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,				},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,				},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,				},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,				},
				{0,  0,  0,  0,  0, 20,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0,			 },
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,				},
				{0,  0,  0, 18,  0,  0,  0, 23,  0,  0,  0, 18,  0,  0,  0,  0,			 },
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,				},
				{0,  0,  0,  0,  0, 20,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0,			 },
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,				},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,				},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0				},
			},
			{
				// 马
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,										},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,										},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,										},
				{0,  0,  0, 90, 90, 90, 96, 90, 96, 90, 90, 90,  0,  0,  0,  0,							},
				{0,  0,  0, 90, 96, 103, 97, 94, 97, 103, 96, 90,  0,  0,  0,  0,						},
				{0,  0,  0, 92, 98, 99, 103, 99, 103, 99, 98, 92,  0,  0,  0,  0,						},
				{0,  0,  0, 93, 108, 100, 107, 100, 107, 100, 108, 93,  0,  0,  0,  0,				},
				{0,  0,  0, 90, 100, 99, 103, 104, 103, 99, 100, 90,  0,  0,  0,  0,					},
				{0,  0,  0, 90, 98, 101, 102, 103, 102, 101, 98, 90,  0,  0,  0,  0,					 },
				{0,  0,  0, 92, 94, 98, 95, 98, 95, 98, 94, 92,  0,  0,  0,  0,							},
				{0,  0,  0, 93, 92, 94, 95, 92, 95, 94, 92, 93,  0,  0,  0,  0,							 },
				{0,  0,  0, 85, 90, 92, 93, 78, 93, 92, 90, 85,  0,  0,  0,  0,							},
				{0,  0,  0, 88, 85, 90, 88, 90, 88, 90, 85, 88,  0,  0,  0,  0,							 },
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,										},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,										},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0										},
			},
			{
				// 车
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,											},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,											},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,											},
				{0,  0,  0, 206, 208, 207, 213, 214, 213, 207, 208, 206,  0,  0,  0,  0,				  },
				{0,  0,  0, 206, 212, 209, 216, 233, 216, 209, 212, 206,  0,  0,  0,  0,				  },
				{0,  0,  0, 206, 208, 207, 214, 216, 214, 207, 208, 206,  0,  0,  0,  0,				  },
				{0,  0,  0, 206, 213, 213, 216, 216, 216, 213, 213, 206,  0,  0,  0,  0,				  },
				{0,  0,  0, 208, 211, 211, 214, 215, 214, 211, 211, 208,  0,  0,  0,  0,				  },
				{0,  0,  0, 208, 212, 212, 214, 215, 214, 212, 212, 208,  0,  0,  0,  0,				   },
				{0,  0,  0, 204, 209, 204, 212, 214, 212, 204, 209, 204,  0,  0,  0,  0,				  },
				{0,  0,  0, 198, 208, 204, 212, 212, 212, 204, 208, 198,  0,  0,  0,  0,				   },
				{0,  0,  0, 200, 208, 206, 212, 200, 212, 206, 208, 200,  0,  0,  0,  0,				  },
				{0,  0,  0, 194, 206, 204, 212, 200, 212, 204, 206, 194,  0,  0,  0,  0,				   },
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,											},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,											},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0											},
			},
			{
				// 炮
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,											},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,											},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,											},
				{0,  0,  0, 100, 100, 96, 91, 90, 91, 96, 100, 100,  0,  0,  0,  0,						   },
				{0,  0,  0, 98, 98, 96, 92, 89, 92, 96, 98, 98,  0,  0,  0,  0,								   },
				{0,  0,  0, 97, 97, 96, 91, 92, 91, 96, 97, 97,  0,  0,  0,  0,								   },
				{0,  0,  0, 96, 99, 99, 98, 100, 98, 99, 99, 96,  0,  0,  0,  0,							  },
				{0,  0,  0, 96, 96, 96, 96, 100, 96, 96, 96, 96,  0,  0,  0,  0,							  },
				{0,  0,  0, 95, 96, 99, 96, 100, 96, 99, 96, 95,  0,  0,  0,  0,							   },
				{0,  0,  0, 96, 96, 96, 96, 96, 96, 96, 96, 96,  0,  0,  0,  0,								   },
				{0,  0,  0, 97, 96, 100, 99, 101, 99, 100, 96, 97,  0,  0,  0,  0,							 },
				{0,  0,  0, 96, 97, 98, 98, 98, 98, 98, 97, 96,  0,  0,  0,  0,								   },
				{0,  0,  0, 96, 96, 97, 99, 99, 99, 97, 96, 96,  0,  0,  0,  0,								    },
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,											},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,											},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0											},
			},
			{
				// 兵(卒)
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,												},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,												},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,												},
				{0,  0,  0,  9,  9,  9, 11, 13, 11,  9,  9,  9,  0,  0,  0,  0,											 },
				{0,  0,  0, 19, 24, 34, 42, 44, 42, 34, 24, 19,  0,  0,  0,  0,									   },
				{0,  0,  0, 19, 24, 32, 37, 37, 37, 32, 24, 19,  0,  0,  0,  0,									   },
				{0,  0,  0, 19, 23, 27, 29, 30, 29, 27, 23, 19,  0,  0,  0,  0,									   },
				{0,  0,  0, 14, 18, 20, 27, 29, 27, 20, 18, 14,  0,  0,  0,  0,									   },
				{0,  0,  0,  7,  0, 13,  0, 16,  0, 13,  0,  7,  0,  0,  0,  0,											  },
				{0,  0,  0,  7,  0,  7,  0, 15,  0,  7,  0,  7,  0,  0,  0,  0,											   },
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,												 },
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,												},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,												 },
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,												},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,												},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0												},
			}
		},
		{
			{
				// 帅(将)
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0, 11, 15, 11,  0,  0,  0,  0,  0,  0, 0},
				{0,  0,  0,  0,  0,  0,  2,  2,  2,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}
			},
			{
				// 仕(士)
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0, 20,  0, 20,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0, 23,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0, 20,  0, 20,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}
			},
			{
				// 相(象)
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,				},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,				},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,				},
				{0,  0,  0,  0,  0, 20,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0,			 },
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,				},
				{0,  0,  0, 18,  0,  0,  0, 23,  0,  0,  0, 18,  0,  0,  0,  0,			 },
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,				},
				{0,  0,  0,  0,  0, 20,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0,			 },
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,				},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,				},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,				},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,				},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,				},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,				},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,				},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0				},
			},
			{
				// 马
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,										},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,										},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,										},
				{0,  0,  0, 88, 85, 90, 88, 90, 88, 90, 85, 88,  0,  0,  0,  0,							 },
				{0,  0,  0, 85, 90, 92, 93, 78, 93, 92, 90, 85,  0,  0,  0,  0,							},
				{0,  0,  0, 93, 92, 94, 95, 92, 95, 94, 92, 93,  0,  0,  0,  0,							 },
				{0,  0,  0, 92, 94, 98, 95, 98, 95, 98, 94, 92,  0,  0,  0,  0,							},
				{0,  0,  0, 90, 98, 101, 102, 103, 102, 101, 98, 90,  0,  0,  0,  0,					 },
				{0,  0,  0, 90, 100, 99, 103, 104, 103, 99, 100, 90,  0,  0,  0,  0,					},
				{0,  0,  0, 93, 108, 100, 107, 100, 107, 100, 108, 93,  0,  0,  0,  0,				},
				{0,  0,  0, 92, 98, 99, 103, 99, 103, 99, 98, 92,  0,  0,  0,  0,						},
				{0,  0,  0, 90, 96, 103, 97, 94, 97, 103, 96, 90,  0,  0,  0,  0,						},
				{0,  0,  0, 90, 90, 90, 96, 90, 96, 90, 90, 90,  0,  0,  0,  0,							},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,										},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,										},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0										},
			},
			{
				// 车
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,											},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,											},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,											},
				{0,  0,  0, 194, 206, 204, 212, 200, 212, 204, 206, 194,  0,  0,  0,  0,				   },
				{0,  0,  0, 200, 208, 206, 212, 200, 212, 206, 208, 200,  0,  0,  0,  0,				  },
				{0,  0,  0, 198, 208, 204, 212, 212, 212, 204, 208, 198,  0,  0,  0,  0,				   },
				{0,  0,  0, 204, 209, 204, 212, 214, 212, 204, 209, 204,  0,  0,  0,  0,				  },
				{0,  0,  0, 208, 212, 212, 214, 215, 214, 212, 212, 208,  0,  0,  0,  0,				   },
				{0,  0,  0, 208, 211, 211, 214, 215, 214, 211, 211, 208,  0,  0,  0,  0,				  },
				{0,  0,  0, 206, 213, 213, 216, 216, 216, 213, 213, 206,  0,  0,  0,  0,				  },
				{0,  0,  0, 206, 208, 207, 214, 216, 214, 207, 208, 206,  0,  0,  0,  0,				  },
				{0,  0,  0, 206, 212, 209, 216, 233, 216, 209, 212, 206,  0,  0,  0,  0,				  },
				{0,  0,  0, 206, 208, 207, 213, 214, 213, 207, 208, 206,  0,  0,  0,  0,				  },
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,											},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,											},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0											},
			},
			{
				// 炮
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,											},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,											},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,											},
				{0,  0,  0, 96, 96, 97, 99, 99, 99, 97, 96, 96,  0,  0,  0,  0,								    },
				{0,  0,  0, 96, 97, 98, 98, 98, 98, 98, 97, 96,  0,  0,  0,  0,								   },
				{0,  0,  0, 97, 96, 100, 99, 101, 99, 100, 96, 97,  0,  0,  0,  0,							 },
				{0,  0,  0, 96, 96, 96, 96, 96, 96, 96, 96, 96,  0,  0,  0,  0,								   },
				{0,  0,  0, 95, 96, 99, 96, 100, 96, 99, 96, 95,  0,  0,  0,  0,							   },
				{0,  0,  0, 96, 96, 96, 96, 100, 96, 96, 96, 96,  0,  0,  0,  0,							  },
				{0,  0,  0, 96, 99, 99, 98, 100, 98, 99, 99, 96,  0,  0,  0,  0,							  },
				{0,  0,  0, 97, 97, 96, 91, 92, 91, 96, 97, 97,  0,  0,  0,  0,								   },
				{0,  0,  0, 98, 98, 96, 92, 89, 92, 96, 98, 98,  0,  0,  0,  0,								   },
				{0,  0,  0, 100, 100, 96, 91, 90, 91, 96, 100, 100,  0,  0,  0,  0,						   },
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,											},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,											},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0											},
			},
			{
				// 兵(卒)
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,												},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,												},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,												},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,												 },
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,												},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,												 },
				{0,  0,  0,  7,  0,  7,  0, 15,  0,  7,  0,  7,  0,  0,  0,  0,											   },
				{0,  0,  0,  7,  0, 13,  0, 16,  0, 13,  0,  7,  0,  0,  0,  0,											  },
				{0,  0,  0, 14, 18, 20, 27, 29, 27, 20, 18, 14,  0,  0,  0,  0,									   },
				{0,  0,  0, 19, 23, 27, 29, 30, 29, 27, 23, 19,  0,  0,  0,  0,									   },
				{0,  0,  0, 19, 24, 32, 37, 37, 37, 32, 24, 19,  0,  0,  0,  0,									   },
				{0,  0,  0, 19, 24, 34, 42, 44, 42, 34, 24, 19,  0,  0,  0,  0,									   },
				{0,  0,  0,  9,  9,  9, 11, 13, 11,  9,  9,  9,  0,  0,  0,  0,											 },
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,												},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,												},
				{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0												},
			}
		}
	};

	int CIGRuleConfig::getChessmanNum()
	{
		int result = 0;

		for (int i = 0; i < (1 << INI_BOARD_WIDTH_LOG2); ++i)
		{
			for (int j = 0; j < (1 << INI_BOARD_HEIGHT_LOG2); ++j)
			{
				result += INI_BOARD[0][j][i] != CHESSMAN_TYPES::NOCHESSMAN;
			}
		}

		return result;
	}

	bool CIGRuleConfig::CHESSMAN_IN_LATTICE = false;

	int CIGRuleConfig::INI_CHESSMAN_NUM_OF_ONE_PLAYER = CIGRuleConfig::getChessmanNum();

	/*const Enum CIGConfig::CLASS_TYPES[] =
	{
		Enum(0, "PlayerArray"),
		Enum(1, "ChessmanBoard"),
		Enum(2, "BitBoard"),
		Enum(3, "TotalBoard")
	};

	const Enum CIGConfig::PLAYERS[] =
	{
		Enum(0, "ssq"),
		Enum(1, "computer")
	};

	const Enum CIGConfig::CHESSMAN_TYPES[] =
	{
		Enum(0, "ALL"),
		Enum(1, ""),
		Enum(2, "King"),
		Enum(3, "")
	};*/
}
