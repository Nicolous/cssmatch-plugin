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
#include "../messages/I18nManager.h"
#include "MatchManager.h"
#include "DisabledMatchState.h"
#include "BreakMatchState.h"
#include "WarmupMatchState.h"

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
	I18nManager * i18n = plugin->getI18nManager();

	// Subscribe to the needed game events
	listener->addCallback("player_death",&SetMatchState::player_death);
	listener->addCallback("round_start",&SetMatchState::round_start);
	listener->addCallback("round_end",&SetMatchState::round_end);

	infos->roundNumber = -2; // a negative round number causes a game restart (see round_start)

	RecipientFilter recipients;
	recipients.addAllPlayers();

	try
	{
		map<string,string> parameters;
		parameters["$current"] = toString(infos->setNumber);
		parameters["$total"] = plugin->getConVar("cssmatch_sets")->GetString();
		i18n->i18nChatSay(recipients,"match_start_manche",parameters);
	}
	catch(const ServerPluginException & e)
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
	ValveInterfaces * interfaces = plugin->getInterfaces();
	MatchManager * match = plugin->getMatch();
	MatchInfo * infos = match->getInfos();
	I18nManager * i18n = plugin->getI18nManager();

	plugin->queueCommand("plugin_print\n");

	// Update the score history of each player
	list<ClanMember *> * playerlist = plugin->getPlayerlist();
	list<ClanMember *>::iterator itPlayer = playerlist->begin();
	list<ClanMember *>::iterator invalidPlayer = playerlist->end();
	while(itPlayer != invalidPlayer)
	{
		PlayerStats * currentStats = (*itPlayer)->getCurrentStats();
		PlayerStats * lastSetStats = (*itPlayer)->getLastSetStats();

		lastSetStats->deaths = currentStats->deaths;
		lastSetStats->kills = currentStats->kills;

		itPlayer++;
	}

	// Update the score history of each clan
	MatchClan * clanT = match->getClan(T_TEAM);
	ClanStats * currentStatsClanT = clanT->getStats();
	ClanStats * lastSetStatsClanT = clanT->getLastSetStats();
	lastSetStatsClanT->scoreT = currentStatsClanT->scoreT;

	MatchClan * clanCT = match->getClan(CT_TEAM);
	ClanStats * currentStatsClanCT = clanCT->getStats();
	ClanStats * lastSetStatsClanCT = clanCT->getLastSetStats();
	lastSetStatsClanCT->scoreCT = currentStatsClanCT->scoreCT;

	try
	{
		if (infos->setNumber < plugin->getConVar("cssmatch_sets")->GetInt())
		{
			// There is at least one other set to play
			MatchLignup * lignup = match->getLignup();

			RecipientFilter recipients;
			recipients.addAllPlayers();
			map<string,string> parameters;

			parameters["$current"] = toString(infos->setNumber);

			ClanStats * statsClan1 = lignup->clan1.getStats();
			parameters["$team1"] = *lignup->clan1.getName();
			parameters["$score1"] = toString(statsClan1->scoreCT + statsClan1->scoreT);

			ClanStats * statsClan2 = lignup->clan2.getStats();
			parameters["$team2"] = *lignup->clan2.getName();
			parameters["$score2"] = toString(statsClan2->scoreCT + statsClan2->scoreT);

			i18n->i18nPopupSay(recipients,"match_end_manche_popup",6,parameters);
			i18n->i18nConsoleSay(recipients,"match_end_manche_popup",parameters);

			// Do a time break (if any) before starting the next state
			BaseMatchState * nextState = this;
			if ((plugin->getConVar("cssmatch_warmup_time")->GetInt() > 0) && infos->warmup)
			{
				nextState = WarmupMatchState::getInstance();
			}

			int breakDuration = plugin->getConVar("cssmatch_end_set")->GetInt();
			if (breakDuration > 0)
			{
				BreakMatchState::doBreak(breakDuration,nextState);
			}
			else
			{
				match->setMatchState(nextState);
			}
			
			// One more set prepared
			infos->setNumber++;

			// Swap every players
			plugin->addTimer(new SwapTimer(interfaces->gpGlobals->curtime + (float)breakDuration));
		}
		else
		{
			// End of the match
			match->stop();
		}
	}
	catch(const ServerPluginException & e)
	{
		printException(e,__FILE__,__LINE__);
		match->stop();
	}
}

void SetMatchState::player_death(IGameEvent * event)
{
	// Update the score [history] of the involved players

	ServerPlugin * plugin = ServerPlugin::getInstance();

	list<ClanMember *> * playerlist = plugin->getPlayerlist();
	list<ClanMember *>::iterator itPlayer = playerlist->begin();
	list<ClanMember *>::iterator invalidPlayer = playerlist->end();

	int userid = event->GetInt("userid");
	list<ClanMember *>::iterator itVictim = 
		find_if(itPlayer,invalidPlayer,PlayerHavingUserid(userid));
	if (itVictim != invalidPlayer)
	{
		PlayerStats * currentStats = (*itVictim)->getCurrentStats();
		currentStats->deaths++;
	}

	int attacker = event->GetInt("attacker");
	if (attacker != userid)
	{
		list<ClanMember *>::iterator itAttacker = 
			find_if(itPlayer,invalidPlayer,PlayerHavingUserid(attacker));
		if (itAttacker != invalidPlayer)
		{
			PlayerStats * currentStats = (*itAttacker)->getCurrentStats();
			if ((*itVictim)->getMyTeam() != (*itAttacker)->getMyTeam())
				currentStats->kills++;
			else
				currentStats->kills--;
		}
	}
}

void SetMatchState::round_start(IGameEvent * event)
{
	// Update the score history of each player, do the restart and announce the begin of a new round

	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	I18nManager * i18n = plugin->getI18nManager();

	MatchInfo * infos = match->getInfos();

	MatchLignup * lignup = match->getLignup();
	ClanStats * statsClan1 = lignup->clan1.getStats();
	ClanStats * statsClan2 = lignup->clan2.getStats();

	RecipientFilter recipients;
	recipients.addAllPlayers();
	map<string,string> parameters;

	list<ClanMember *> * playerlist = plugin->getPlayerlist();
	list<ClanMember *>::iterator itPlayer = playerlist->begin();
	list<ClanMember *>::iterator invalidPlayer = playerlist->end();
	while(itPlayer != invalidPlayer)
	{
		PlayerStats * currentStats = (*itPlayer)->getCurrentStats();
		PlayerStats * lastRoundStats = (*itPlayer)->getLastRoundStats();

		lastRoundStats->deaths = currentStats->deaths;
		lastRoundStats->kills = currentStats->kills;

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
		catch(const ServerPluginException & e)
		{
			printException(e,__FILE__,__LINE__);
		}
		parameters["$team1"] = *lignup->clan1.getName();
		parameters["$score1"] = toString(statsClan1->scoreCT + statsClan1->scoreT);
		parameters["$team2"] = *lignup->clan2.getName();
		parameters["$score2"] = toString(statsClan2->scoreCT + statsClan2->scoreT);
		plugin->addTimer(
			new TimerI18nPopupSay(0.5f,recipients,"match_round_popup",5,parameters));
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

SwapTimer::SwapTimer(float date) : BaseTimer(date)
{
}

void SwapTimer::execute()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();

	list<ClanMember *> * playerlist = plugin->getPlayerlist();
	list<ClanMember *>::iterator itPlayer = playerlist->begin();
	list<ClanMember *>::iterator invalidPlayer = playerlist->end();

	while(itPlayer != invalidPlayer)
	{
		(*itPlayer)->swap();
		itPlayer++;
	}
}

