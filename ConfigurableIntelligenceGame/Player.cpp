#include "stdafx.h"
#include "GUI.h"
#include "Player.h"
#include "CConfigurableIntelligenceGameView.h"
#include "utilities.h"

namespace CIG
{
	Player::Player( const Player& p, Chessboard* cb)
		: makeBestAction(p.makeBestAction), chessboard(cb), NAME(p.NAME), ownedChessmans(p.ownedChessmans)
	{
	}

	Player::Player(CIGRuleConfig::PLAYER_NAMES p, void (*makeBestAction_)(Chessboard*cb, void*)  , Chessboard* cb)
		: chessboard(cb), NAME(p), ownedChessmans(),makeBestAction(makeBestAction_)
	{
	}

	Player::Player(): chessboard(NULL), NAME((CIGRuleConfig::PLAYER_NAMES) - 1), ownedChessmans(), makeBestAction(GUI::askForAction)
	{
	}

	Player::~Player()
	{
	}

	void Player::operator=( const Player& p )
	{
		makeBestAction = p.makeBestAction;
		chessboard = p.chessboard;
		ownedChessmans = p.ownedChessmans;
		NAME = p.NAME;
	}
}