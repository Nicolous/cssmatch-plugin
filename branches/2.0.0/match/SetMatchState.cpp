/* 
 * Copyright 2008, 2009 Nicolas Maingot
 * 
 * This file is part of CSSMatch.
 * 
 * CSSMatch is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * CSSMatch is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with CSSMatch; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Portions of this code are also Copyright © 1996-2005 Valve Corporation, All rights reserved
 */

#include "SetMatchState.h"

#include "../common/common.h"
#include "../plugin/ServerPlugin.h"
#include "../player/Player.h"
#include "../player/ClanMember.h"
#include "MatchManager.h"
#include "../messages/I18nManager.h"

#include "igameevents.h"

#include <algorithm>

using namespace cssmatch;

using std::string;
using std::list;
using std::map;
using std::find_if;

SetMatchState::SetMatchState()
{
	listener = new EventListener<SetMatchState>(this);
}

SetMatchState::~SetMatchState()
{
	delete listener;
}

void SetMatchState::startState()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	MatchInfo * infos = match->getInfos();
	I18nManager * i18n = plugin->get18nManager();

	// Subscribe to the needed game events
	listener->addCallback("player_death",&SetMatchState::player_death);
	listener->addCallback("round_start",&SetMatchState::round_start);
	listener->addCallback("round_end",&SetMatchState::round_end);

	infos->roundNumber = -2; // a negative round number causes a game restart (see round_start)

	list<ClanMember *> * playerlist = plugin->getPlayerlist();
	RecipientFilter recipients;
	for_each(playerlist->begin(),playerlist->end(),PlayerToRecipient(&recipients));

	try
	{
		map<string,string> parameters;
		parameters["$current"] = toString(infos->setNumber);
		parameters["$total"] = plugin->getConVar("cssmatch_sets")->GetString();
		i18n->i18nChatSay(recipients,"match_start_manche",0,parameters);
	}
	catch(const BaseConvarsAccessorException & e)
	{
		printException(e,__FILE__,__LINE__);
	}

	i18n->i18nChatSay(recipients,"match_restarts");

	plugin->queueCommand("mp_restartgame 2\n");
}

void SetMatchState::endState()
{
	listener->removeCallbacks();
}

void SetMatchState::endSet()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();

	plugin->queueCommand("plugin_print\n");

	// TODO: ...
}

void SetMatchState::player_death(IGameEvent * event)
{
	// Update the score [history] of the involved players

	ServerPlugin * plugin = ServerPlugin::getInstance();

	list<ClanMember *> * playerlist = plugin->getPlayerlist();
	list<ClanMember *>::iterator itPlayer = playerlist->begin();
	list<ClanMember *>::iterator invalidPlayer = playerlist->end();

	list<ClanMember *>::iterator itVictim = 
		find_if(itPlayer,invalidPlayer,PlayerHavingUserid(event->GetInt("userid")));
	if (itVictim != invalidPlayer)
	{
		list<StatsRound> * roundStats = (*itVictim)->getStatsRound();
		if (roundStats->begin() != roundStats->end())
		{
			roundStats->back().getPlayerStats()->deaths++;
		}
	}

	list<ClanMember *>::iterator itAttacker = 
		find_if(itPlayer,invalidPlayer,PlayerHavingUserid(event->GetInt("attacker")));
	if (itAttacker != invalidPlayer)
	{
		list<StatsRound> * roundStats = (*itAttacker)->getStatsRound();
		if (roundStats->begin() != roundStats->end())
		{
			if ((*itVictim)->getMyTeam() != (*itAttacker)->getMyTeam())
				roundStats->back().getPlayerStats()->kills++;
			else
				roundStats->back().getPlayerStats()->kills--;
		}
	}
}

void SetMatchState::round_start(IGameEvent * event)
{
	// Update the score history of each player, do the restart and announce the begin of a new round

	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	I18nManager * i18n = plugin->get18nManager();

	MatchInfo * infos = match->getInfos();

	MatchLignup * lignup = match->getLignup();
	ClanStats * statsClan1 = lignup->clan1.getStats();
	ClanStats * statsClan2 = lignup->clan2.getStats();

	list<ClanMember *> * playerlist = plugin->getPlayerlist();
	RecipientFilter recipients;
	map<string,string> parameters;

	for_each(playerlist->begin(),playerlist->end(),PlayerToRecipient(&recipients));

	list<ClanMember *>::iterator itPlayer = playerlist->begin();
	list<ClanMember *>::iterator invalidPlayer = playerlist->end();
	while(itPlayer != invalidPlayer)
	{
		list<StatsRound> * plStatsRound = (*itPlayer)->getStatsRound();

		PlayerStats plStats;
		if (plStatsRound->size() > 0)
			plStats = *plStatsRound->back().getPlayerStats();

		StatsRound stats(infos->roundNumber,plStats);
		plStatsRound->push_back(stats);

		itPlayer++;
	}

	switch(infos->roundNumber++)
	{
	case -2:
		plugin->queueCommand("mp_restartgame 1\n");
		break;
	case -1:
		plugin->queueCommand("mp_restartgame 2\n");
		break;
	default:
		parameters["$current"] = toString(infos->roundNumber);
		try
		{
			parameters["$total"] = plugin->getConVar("cssmatch_rounds")->GetString();
		}
		catch(const BaseConvarsAccessorException & e)
		{
			printException(e,__FILE__,__LINE__);
		}
		parameters["$team1"] = *lignup->clan1.getName();
		parameters["$score1"] = toString(statsClan1->scoreCT + statsClan1->scoreT);
		parameters["$team2"] = *lignup->clan2.getName();
		parameters["$score2"] = toString(statsClan2->scoreCT + statsClan2->scoreT);
		plugin->addTimer(
			new TimerI18nPopupSay(i18n,0.5f,recipients,"match_round_popup",5,OPTION_ALL,parameters));
	}
}

void SetMatchState::round_end(IGameEvent * event)
{
	// Update the score of each clan, then end the [round] set if scheduled number of rounds is reached

	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	MatchInfo * infos = match->getInfos();

	try
	{
		TeamCode winnerTeam = (TeamCode)event->GetInt("winner");
		MatchClan * winner = match->getClan(winnerTeam);
		switch(winnerTeam)
		{
		case T_TEAM:
			winner->getStats()->scoreT++;
			break;
		case CT_TEAM:
			winner->getStats()->scoreCT++;
			break;
		}

		if (infos->roundNumber >= plugin->getConVar("cssmatch_rounds")->GetInt())
			endSet();
	}
	catch(const MatchManagerException & e)
	{
		// printException(e,__FILE__,__LINE__); // round draw
	}
}

