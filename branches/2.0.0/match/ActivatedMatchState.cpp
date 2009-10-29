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

#include "ActivatedMatchState.h"

#include "MatchManager.h"
#include "../messages/I18nManager.h"
#include "../plugin/SimplePlugin.h"
#include "../player/ClanMember.h"

#include "igameevents.h" // IGameEventManager2, IGameEvent
#include "dlls/iplayerinfo.h"

#include <string>
#include <map>
#include <algorithm>

using namespace cssmatch;

using std::string;
using std::list;
using std::map;
using std::for_each;
using std::find_if;

ActivatedMatchState::ActivatedMatchState()
{
	SimplePlugin * plugin = SimplePlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	listener = new EventListener<ActivatedMatchState>(this,interfaces->gameeventmanager2);
}

ActivatedMatchState::~ActivatedMatchState()
{
	delete listener;
}

void ActivatedMatchState::startState()
{
	// Subscribe to the needed events
	listener->addCallback("player_disconnect",&ActivatedMatchState::player_disconnect);
	listener->addCallback("player_team",&ActivatedMatchState::player_team);
	listener->addCallback("player_changename",&ActivatedMatchState::player_changename);
}

void ActivatedMatchState::endState()
{
	listener->removeCallbacks();
}

void ActivatedMatchState::player_disconnect(IGameEvent * event)
{
	// Announce any disconnection 

	SimplePlugin * plugin = SimplePlugin::getInstance();
	I18nManager * i18n = plugin->get18nManager();

	list<ClanMember *> * playerlist = plugin->getPlayerlist();
	list<ClanMember *>::const_iterator itPlayer = playerlist->begin();
	list<ClanMember *>::const_iterator invalidPlayer = playerlist->end();

	RecipientFilter recipients;
	for_each(itPlayer,invalidPlayer,PlayerToRecipient(&recipients));
	map<string, string> parameters;
	parameters["$username"] = event->GetString("name");
	parameters["$reason"] = event->GetString("reason");

	i18n->i18nChatSay(recipients,"player_leave_game",0,parameters);
}

void ActivatedMatchState::player_team(IGameEvent * event)
{
	// Search for any change in the team which requires a new clan name detection
	// i.e. n player to less than 2 players, 0 player to 1 player, 1 player to 2 players

	SimplePlugin * plugin = SimplePlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	I18nManager * i18n = plugin->get18nManager();

	TeamCode newSide = (TeamCode)event->GetInt("team");
	TeamCode oldSide = (TeamCode)event->GetInt("oldteam");

	int playercount = 0;
	TeamCode toReDetect = INVALID_TEAM;
	switch(newSide)
	{
	case T_TEAM:
		toReDetect = T_TEAM;
		playercount = plugin->getPlayerCount(T_TEAM) - 1;
		break;
	case CT_TEAM:
		toReDetect = CT_TEAM;
		playercount = plugin->getPlayerCount(CT_TEAM) - 1;
		break;
	}
	if ((toReDetect != INVALID_TEAM) && (playercount < 2)) // TODO: oops, "< 2" need doc
		plugin->addTimer(new ClanNameDetectionTimer(interfaces->gpGlobals->curtime+1.0f,toReDetect));

	toReDetect = INVALID_TEAM;
	switch(oldSide)
	{
	case T_TEAM:
		toReDetect = T_TEAM;
		playercount = plugin->getPlayerCount(T_TEAM);
		break;
	case CT_TEAM:
		toReDetect = CT_TEAM;
		playercount = plugin->getPlayerCount(CT_TEAM);
		break;
	}
	if ((toReDetect != INVALID_TEAM) && (playercount < 2))
		plugin->addTimer(new ClanNameDetectionTimer(interfaces->gpGlobals->curtime+1.0f,toReDetect));
}

void ActivatedMatchState::player_changename(IGameEvent * event)
{
	// Retect the corresponfing clan name if needed
	// i.e. if the player is alone is his team

	SimplePlugin * plugin = SimplePlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	I18nManager * i18n = plugin->get18nManager();

	list<ClanMember *> * playerlist = plugin->getPlayerlist();
	list<ClanMember *>::const_iterator itPlayer = playerlist->begin();
	list<ClanMember *>::const_iterator invalidPlayer = playerlist->end();

	list<ClanMember *>::const_iterator thisPlayer = 
		find_if(itPlayer,invalidPlayer,PlayerHavingUserid(event->GetInt("userid")));
	if (thisPlayer != invalidPlayer)
	{
		TeamCode playerteam = (*thisPlayer)->getMyTeam();
		if ((playerteam > SPEC_TEAM) && (plugin->getPlayerCount(playerteam) == 1))
			plugin->addTimer(new ClanNameDetectionTimer(interfaces->gpGlobals->curtime+1.0f,playerteam));
	}
}
