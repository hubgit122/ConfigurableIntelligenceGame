#include "stdafx.h"
#include "GUI.h"
#include "Player.h"
#include "CConfigurableIntelligenceGameView.h"
#include "utilities.h"

namespace CIG
{
	void Player::makeBestAction(OperationStack& op)
	{
#ifdef DEBUG_MESSAGE
		GUI::inform("假设我已经走完一步"/*"Assume that we have mede a move there. "*/);
#else
		GUI::postMessage(WM_GET_MOVE, (WPARAM)&op,0);
		GUI::moveComplete.Lock();
#endif
	}

	Player::Player( const Player& p ) : NAME(p.NAME), ownedChessmans(p.ownedChessmans)//, CIGNamedObject(p)
	{
	}

	Player::Player(CIGRuleConfig::PLAYER_NAMES p ):NAME(p)
	{
	}

	Player::~Player()
	{
	}

	//bool Player::wins()
	//{
	//	
	//}
}