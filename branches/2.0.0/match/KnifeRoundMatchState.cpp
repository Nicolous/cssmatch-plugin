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

#include "KnifeRoundMatchState.h"

#include "../plugin/ServerPlugin.h"
#include "../common/common.h"
#include "../player/Player.h"
#include "../player/ClanMember.h"
#include "../messages/I18nManager.h"
#include "WarmupMatchState.h"
#include "SetMatchState.h"
#include "MatchManager.h"

#include "BreakMatchState.h"

#include "igameevents.h"

#include <algorithm>

using namespace cssmatch;

using std::string;
using std::list;
using std::map;
using std::for_each;

KnifeRoundMatchState::KnifeRoundMatchState()
{
	listener = new EventListener<KnifeRoundMatchState>(this);
}

KnifeRoundMatchState::~KnifeRoundMatchState()
{
	delete listener;
}

void KnifeRoundMatchState::endKniferound(TeamCode winner)
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	MatchManager * match = plugin->getMatch();
	I18nManager * i18n = plugin->get18nManager();

	MatchInfo * infos = match->getInfos();
	// Save the winner of this round, it will be used into the match report
	infos->kniferoundWinner = match->getClan(winner);
		// don't care about the exception, winner is validated above

	// Global recipient list
	list<ClanMember *> * playerlist = plugin->getPlayerlist();
	RecipientFilter recipients;
	for_each(playerlist->begin(),playerlist->end(),PlayerToRecipient(&recipients));
	
	// Announce the winner
	map<string,string> parameters;
	parameters["$team"] = *infos->kniferoundWinner->getName();
	i18n->i18nChatSay(recipients,"kniferound_winner",INVALID_ENTITY_INDEX,parameters);

	// Invite the winners to choice a side
	TeamCode teamWinner = infos->kniferoundWinner->getTeamCode();
	TeamCode teamLoser = (teamWinner == T_TEAM) ? CT_TEAM : T_TEAM;
	list<ClanMember *>::iterator itPlayer = playerlist->begin();
	list<ClanMember *>::iterator invalidPlayer = playerlist->end();
	while(itPlayer != invalidPlayer)
	{
		PlayerIdentity * identity = (*itPlayer)->getIdentity();
		IPlayerInfo * pInfo = (*itPlayer)->getPlayerInfo();
		TeamCode playerTeam = (*itPlayer)->getMyTeam();
		if (playerTeam == teamWinner)
		{
			interfaces->engine->ClientCommand(identity->pEntity,"chooseteam");
		}
		else if (playerTeam == teamLoser)
		{
			if (pInfo != NULL)
			{
				if (pInfo->IsFakeClient())
					plugin->kickid(identity->userid,"CSSMatch: Spec Bot");
				else
					pInfo->ChangeTeam(SPEC_TEAM);
			}
		}
		itPlayer++;
	}

	// Prepare a break time before lauch the next match state,
	MatchStateId statePostBreak = DISABLED;
	try
	{
		if ((plugin->getConVar("cssmatch_warmup_time")->GetInt() > 0) && infos->warmup)
		{
			statePostBreak = WARMUP;
		}
		else if (plugin->getConVar("cssmatch_sets")->GetInt() > 0)
		{
			statePostBreak = SET;
		}
		// TODO: else, end of the match

		if (statePostBreak != NULL)
		{
			int breakDuration = plugin->getConVar("cssmatch_end_kniferound")->GetInt();
			if (breakDuration > 0)
			{
				match->doTimeBreak(breakDuration,statePostBreak);

				//parameters["$team"] = // already set above
				parameters["$time"] = toString(breakDuration);
				i18n->i18nChatSay(recipients,"kniferound_dead_time",INVALID_ENTITY_INDEX,parameters);
			}
			else
			{
				match->setMatchState(statePostBreak);
			}
		}
		else // Error case
		{
			list<ClanMember *> * playerlist = plugin->getPlayerlist();
			RecipientFilter recipients;
			for_each(playerlist->begin(),playerlist->end(),PlayerToRecipient(&recipients));

			i18n->i18nChatWarning(recipients,"match_config_error");
		}
	}
	catch(const BaseConvarsAccessorException & e)
	{
		printException(e,__FILE__,__LINE__);
	}
}

void KnifeRoundMatchState::startState()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	I18nManager * i18n = plugin->get18nManager();

	// Global recipient list
	list<ClanMember *> * playerlist = plugin->getPlayerlist();
	RecipientFilter recipients;
	for_each(playerlist->begin(),playerlist->end(),PlayerToRecipient(&recipients));

	// Register to the needed events
	listener->addCallback("round_start",&KnifeRoundMatchState::round_start);
	listener->addCallback("round_end",&KnifeRoundMatchState::round_end);
	listener->addCallback("item_pickup",&KnifeRoundMatchState::item_pickup);
	listener->addCallback("player_spawn",&KnifeRoundMatchState::player_spawn);
	listener->addCallback("bomb_beginplant",&KnifeRoundMatchState::bomb_beginplant);

	i18n->i18nChatSay(recipients,"kniferound_restarts");

	match->getInfos()->roundNumber = -2; // a negative round number causes a game restart (see round_start)

	// First restart
	plugin->queueCommand("mp_restartgame 2\n");
}

void KnifeRoundMatchState::endState()
{
	listener->removeCallbacks();
}

void KnifeRoundMatchState::round_start(IGameEvent * event)
{
	// Do the needed restarts then announce the begin of the knife round

	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	I18nManager * i18n = plugin->get18nManager();

	switch(match->getInfos()->roundNumber++)
	{
	case -2:
		plugin->queueCommand("mp_restartgame 1\n");
		break;
	case -1:
		plugin->queueCommand("mp_restartgame 2\n");
		break;
	default:
		// Global recipient list
		list<ClanMember *> * playerlist = plugin->getPlayerlist();
		RecipientFilter recipients;
		for_each(playerlist->begin(),playerlist->end(),PlayerToRecipient(&recipients));
		
		i18n->i18nChatSay(recipients,"kniferound_announcement");
	}
}

void KnifeRoundMatchState::item_pickup(IGameEvent * event)
{
	// Restrict all the weapons but knife

	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	string item = event->GetString("item");

	if (item != "knife")
	{
		list<ClanMember *> * playerlist = plugin->getPlayerlist();
		list<ClanMember *>::iterator invalidPlayer = playerlist->end();

		list<ClanMember *>::iterator itPlayer = 
			find_if(playerlist->begin(),invalidPlayer,PlayerHavingUserid(event->GetInt("userid")));
		if (itPlayer != invalidPlayer)
		{
			interfaces->helpers->ClientCommand((*itPlayer)->getIdentity()->pEntity,"use weapon_knife");
			// Kill any other weapon entity the player has
			(*itPlayer)->removeWeapon(WEAPON_SLOT1);
			(*itPlayer)->removeWeapon(WEAPON_SLOT2);
			(*itPlayer)->removeWeapon(WEAPON_SLOT4);
		}
		else
			print(__FILE__,__LINE__,"Unable to find the player wich pickups an item");
	}
}

void KnifeRoundMatchState::player_spawn(IGameEvent * event)
{
	// Set the money for the knife round (determines if some equipments can be bought)

	ServerPlugin * plugin = ServerPlugin::getInstance();

	list<ClanMember *> * playerlist = plugin->getPlayerlist();
	list<ClanMember *>::iterator invalidPlayer = playerlist->end();

	list<ClanMember *>::iterator itPlayer = 
		find_if(playerlist->begin(),invalidPlayer,PlayerHavingUserid(event->GetInt("userid")));
	if (itPlayer != invalidPlayer)
	{
		try
		{
			(*itPlayer)->setCash(plugin->getConVar("cssmatch_kniferound_money")->GetInt());
		}
		catch(const BaseConvarsAccessorException & e)
		{
			printException(e,__FILE__,__LINE__);
		}
	}
}

void KnifeRoundMatchState::round_end(IGameEvent * event)
{
	// If the winner is T or CT, end the knife round

	TeamCode winner = (TeamCode)event->GetInt("winner");
	if (winner > SPEC_TEAM)
	{
		endKniferound(winner);
	}
}

void KnifeRoundMatchState::bomb_beginplant(IGameEvent * event)
{
	// Prevent the bomb from being planted if needed

	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	I18nManager * i18n = plugin->get18nManager();

	try
	{
		if (! plugin->getConVar("cssmatch_kniferound_allows_c4")->GetBool())
		{
			list<ClanMember *> * playerlist = plugin->getPlayerlist();
			list<ClanMember *>::iterator invalidPlayer = playerlist->end();

			list<ClanMember *>::iterator itPlayer = 
				find_if(playerlist->begin(),invalidPlayer,PlayerHavingUserid(event->GetInt("userid")));
			if (itPlayer != invalidPlayer)
			{
				PlayerIdentity * identity = (*itPlayer)->getIdentity();

				interfaces->helpers->ClientCommand((*itPlayer)->getIdentity()->pEntity,"use weapon_knife");

				RecipientFilter recipients;
				recipients.addRecipient(identity->index);
				i18n->i18nChatSay(recipients,"kniferound_c4");
			}
			else
				print(__FILE__,__LINE__,"Unable to find the player who plants the bomb");
		}
	}
	catch(const BaseConvarsAccessorException & e)
	{
		printException(e,__FILE__,__LINE__);
	}
}

// TODO : recipient list is often global, could we construct in parallel with the player list in ServerPlugin ?
