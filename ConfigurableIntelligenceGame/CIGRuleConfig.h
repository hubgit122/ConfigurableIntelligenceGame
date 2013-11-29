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
				CHESS,										//五子棋只有一种棋子
				CHESSMAN_TYPE_NUM,			//总数
			};

			enum CHESS_OPERATIONS
			{
				BEGIN_CHESS,
				PICK,						//拿起
				PUT_CHESS,				//走棋
				CAPTURE,					//吃子
				PROMOTION,			//升变
				DECOVER,				//掀开
				END_CHESS,							//作为走法生成器返回的提示语: 一条搜索路径结束, 请保存结果并回退搜索.
				NOMORE_CHESS_OPERATION					//作为走法生成器返回的提示语: 无更多操作, 请回退搜索.
			};

			enum PLAYER_OPERATIONS
			{
				BEGIN_PLAYER,
				ADD,						//增加一个棋子
				PUT_PLAYER,
				END_PLAYER,
				NOMORE_PLAYER_OPERATIONS
			};

			static const CHESS_OPERATIONS chessOperationGraph[NOMORE_CHESS_OPERATION][NOMORE_CHESS_OPERATION + 1];
			static const PLAYER_OPERATIONS playerOperationGraph[NOMORE_PLAYER_OPERATIONS][NOMORE_PLAYER_OPERATIONS+1];

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
			//static const int EVALUATIONS[PLAYER_NUM][CHESSMAN_TYPE_NUM][1 << INI_BOARD_HEIGHT_LOG2][1 << INI_BOARD_WIDTH_LOG2];
	};
}

#endif /*__CIGMANAGER_H_*/

