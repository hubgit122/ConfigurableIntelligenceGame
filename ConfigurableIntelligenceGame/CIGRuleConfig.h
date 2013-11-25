#ifndef __CIGMANAGER_H__
#define __CIGMANAGER_H__
using namespace std;
#include "utilities.h"

namespace CIG
{
	class CIGRuleConfig
	{
			/*************************************************************************
				���ϵͳ����
			**************************************************************************/
		public:
			static bool checkConstrains();


			/*************************************************************************
				��ʼ������
			**************************************************************************/
		private:
			static int getChessmanNum();

			/*************************************************************************
				����ȫ������id������
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
				PLAYER_NUM,				//������������ɫ�ǲ���ȷ��, ��������Ҫ�Ľ�.
			};

			enum CHESSMAN_TYPES
			{
				NOCHESSMAN = -1,
				KING,								//��
				ADVISOR,							//ʿ
				ELEPHANT,						//��
				HORSE,								//��
				CHAROIT,							//��
				CANNON,						//��
				PAWN,								//��
				CHESSMAN_TYPE_NUM,			//����
			};

			enum OPERATIONS
			{
				BEGIN,
				PICK,						//����
				PUT,							//����
				CAPTURE,					//����
				PROMOTION,			//����
				DECOVER,				//�ƿ�
				END,							//��Ϊ�߷����������ص���ʾ��: һ������·������, �뱣��������������.
				NOMORE					//��Ϊ�߷����������ص���ʾ��: �޸������, ���������.
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
