#ifndef __CIGMANAGER_H__
#define __CIGMANAGER_H__
using namespace std;
#include "utilities.h"

namespace CIG
{
	class CIGRuleConfig
	{
			/*************************************************************************
				检查系统环境
			**************************************************************************/
		public:
			static bool checkConstrains();


			/*************************************************************************
				初始化参数
			**************************************************************************/
		private:
			static int getChessmanNum();

			/*************************************************************************
				管理全局类型id及名字
			**************************************************************************/
		public:
			enum CLASS_TYPES
			{
				GAME_CLASS,
				PLAYER,
				PLAYERS,
				CHESSMAN_OPERATIONS,
				OPERATION_STACK,
				CHESSMAN_MOTION_STACK,
				MOTION_STATUS,
				POINTS,
				LINES,
				CHESSMAN,
				CHESSMAN_MOTION,
				CHESSMAN_GROUP,
				CHESSBOARD_STACK,
				LATTICE_GROUP
			};

			enum PLAYER_NAMES
			{
				HUMAN,
				COMPUTER,
				PLAYER_NUM,				//用种类来定颜色是不正确的, 调试完了要改进.
			};

			enum CHESSMAN_TYPES
			{
				NOCHESSMAN = -1,
				KING,								//将
				ADVISOR,							//士
				ELEPHANT,						//象
				HORSE,								//马
				CHAROIT,							//车
				CANNON,						//炮
				PAWN,								//兵
				CHESSMAN_TYPE_NUM,			//总数
			};

			enum OPERATIONS
			{
				BEGIN,
				PICK,						//拿起
				PUT,							//走棋
				CAPTURE,					//吃子
				PROMOTION,			//升变
				DECOVER,				//掀开
				END,							//作为走法生成器返回的提示语: 一条搜索路径结束, 请保存结果并回退搜索.
				NOMORE					//作为走法生成器返回的提示语: 无更多操作, 请回退搜索.
			};

			static const OPERATIONS operationGraph[NOMORE + 1][NOMORE + 1];

			enum CHESSMAN_STATUS
			{
				ON_BOARD,
				OFF_BOARD,
				CAPTURED,
			};

			enum VISIBILITIES
			{
				ALL,
				NONE,
				SELF,
				ALLY,
			};


			static int INI_CHESSMAN_NUM_OF_ONE_PLAYER;
			static bool CHESSMAN_IN_LATTICE;
			//static const int MAX_SEARCH_DEPTH = 1024;
			static const int INI_CHESSMAN_GROUP_SIZE = 64;
			static const int INI_LATTICE_GROUP_SIZE = 64;
			static const int INT_BANNED_MOTION_SIZE = 16;
			static const int INT_MARKED_POINTS_SIZE = 32;
			static const int INT_BOARD_HISTORY_STACK_SIZE = 32;
			static const int INI_BOARD_WIDTH_LOG2 = 4;
			static const int INI_BOARD_HEIGHT_LOG2 = 4;
			static const int ROUND_LIMIT = -1;
			static const float TIME_LIMIT;

			static const bool BOARD_RANGE[1 << INI_BOARD_HEIGHT_LOG2][1 << INI_BOARD_WIDTH_LOG2];
			static const CHESSMAN_TYPES INI_BOARD[PLAYER_NUM][1 << INI_BOARD_HEIGHT_LOG2][1 << INI_BOARD_WIDTH_LOG2];
			static const int EVALUATIONS[PLAYER_NUM][CHESSMAN_TYPE_NUM][1 << INI_BOARD_HEIGHT_LOG2][1 << INI_BOARD_WIDTH_LOG2];
	};
}

#endif /*__CIGMANAGER_H_*/

