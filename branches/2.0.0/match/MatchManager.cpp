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

#include "MatchManager.h"

#include "BaseMatchState.h"
#include "DisabledMatchState.h"
#include "BreakMatchState.h"
#include "KnifeRoundMatchState.h"
#include "WarmupMatchState.h"
#include "SetMatchState.h"

#include "../configuration/RunnableConfigurationFile.h"
#include "../plugin/ServerPlugin.h"
#include "../common/common.h"
#include "../messages/Countdown.h"
#include "../messages/I18nManager.h"
#include "../player/Player.h"
#include "../player/ClanMember.h"

#include <algorithm>

using namespace cssmatch;

using std::string;
using std::list;
using std::find_if;
using std::map;

MatchManager::MatchManager()
{
	listener = new EventListener<MatchManager>(this);

	// Initialize all the match states
	states[DISABLED] = DisabledMatchState::getInstance();
	states[KNIFEROUND] = KnifeRoundMatchState::getInstance();
	states[WARMUP] = WarmupMatchState::getInstance();
	states[SET] = SetMatchState::getInstance();
	states[TIMEBREAK] = BreakMatchState::getInstance();

	// Set the initial state
	currentState = states.find(DISABLED);
}


MatchManager::~MatchManager()
{
	Countdown::getInstance()->stop();

	delete listener;
}

MatchLignup * MatchManager::getLignup()
{
	return &lignup;
}

MatchInfo * MatchManager::getInfos()
{
	return &infos;
}

MatchClan * MatchManager::getClan(TeamCode code) throw(MatchManagerException)
{
	MatchClan * clan = NULL;

	switch(code)
	{
	case T_TEAM:
		if (infos.setNumber & 1)
			clan = &lignup.clan1;
		else
			clan = &lignup.clan2;
		break;
	case CT_TEAM:
		if (infos.setNumber & 1)
			clan = &lignup.clan2;
		else
			clan = &lignup.clan1;	
		break;
	default:
		throw MatchManagerException("The plugin tried to grab a clan from an invalid team index");
	}

	return clan;
}

void MatchManager::player_disconnect(IGameEvent * event)
{
	// Announce any disconnection 

	ServerPlugin * plugin = ServerPlugin::getInstance();
	I18nManager * i18n = plugin->get18nManager();

	RecipientFilter recipients;
	recipients.addAllPlayers();
	map<string, string> parameters;
	parameters["$username"] = event->GetString("name");
	parameters["$reason"] = event->GetString("reason");

	i18n->i18nChatSay(recipients,"player_leave_game",0,parameters);
}

void MatchManager::player_team(IGameEvent * event)
{
	// Search for any change in the team which requires a new clan name detection
	// i.e. n player to less than 2 players, 0 player to 1 player, 1 player to 2 players

	ServerPlugin * plugin = ServerPlugin::getInstance();
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
	if ((toReDetect != INVALID_TEAM) && (playercount < 2))
			// "< 2" because the game does not immediatly update the player's team got via IPlayerInfo
			// And that's why we use a timer to redetect the clan's name
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

void MatchManager::player_changename(IGameEvent * event)
{
	// Retect the corresponfing clan name if needed
	// i.e. if the player is alone is his team

	ServerPlugin * plugin = ServerPlugin::getInstance();
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

void MatchManager::detectClanName(TeamCode code)
{
	/*ServerPlugin * plugin = ServerPlugin::getInstance();
	I18nManager * i18n = plugin->get18nManager();*/

	try
	{
		getClan(code)->detectClanName();

		/*RecipientFilter recipients;
		recipients.addAllPlayers();

		i18n->i18nChatSay(recipients,"match_retag");

		map<string,string> parameters;
		parameters["$team1"] = *(lignup.clan1.getName());
		parameters["$team2"] = *(lignup.clan2.getName());
		i18n->i18nChatSay(recipients,"match_name",0,parameters);*/

		updateHostname();
	}
	catch(const MatchManagerException & e)
	{
		printException(e,__FILE__,__LINE__);
	}
}

void MatchManager::updateHostname()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	try
	{
		ConVar * hostname = plugin->getConVar("cssmatch_hostname");
		string newHostname = hostname->GetString();

		// Replace %s by the clan names
		size_t clanNameSlot = newHostname.find("%s");
		if (clanNameSlot != string::npos)
		{
			const string * clan1Name = lignup.clan1.getName();
			newHostname.replace(clanNameSlot,2,*clan1Name,0,clan1Name->size());
		}

		clanNameSlot = newHostname.find("%s");
		if (clanNameSlot != string::npos)
		{
			const std::string * clan2Name = lignup.clan2.getName();
			newHostname.replace(clanNameSlot,2,*clan2Name,0,clan2Name->size());
		}

		// Set the new hostname
		hostname->SetValue(newHostname.c_str());
	}
	catch(const ServerPluginException & e)
	{
		printException(e,__FILE__,__LINE__);
	}

}

void MatchManager::setMatchState(MatchStateId newState)
{
	if (currentState->second != NULL) // Maybe there was no current state
		currentState->second->endState();

	currentState = states.find(newState);

	if (currentState != states.end())
	{
		currentState->second->startState();
	}
	else
	{
		print(__FILE__,__LINE__,"Invalid match state");
	}
}

MatchStateId MatchManager::getMatchState() const
{
	return currentState->first;
}

void MatchManager::doTimeBreak(int duration,MatchStateId nextState)
{
	BreakMatchState * breakState = BreakMatchState::getInstance();
	breakState->setBreak(duration,nextState);

	setMatchState(TIMEBREAK);
}

void MatchManager::start(RunnableConfigurationFile & config, bool kniferound, bool warmup, ClanMember * umpire)
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	I18nManager * i18n = plugin->get18nManager();

	// Global recipient list
	RecipientFilter recipients;
	recipients.addAllPlayers();

	// Update match infos
	infos.setNumber = 1;
	infos.roundNumber = 1;

	// Cancel any timers in progress
	plugin->removeTimers();

	// Execute the configuration file
	config.execute();

	// Print the plugin list to the server log
	plugin->queueCommand("plugin_print\n");

	// Save the current date
	infos.startTime = getLocalTime();

	// Try to find the clan names
	lignup.clan1.detectClanName();
	lignup.clan2.detectClanName();
	updateHostname();

	// Start to listen some events
	listener->addCallback("player_disconnect",&MatchManager::player_disconnect);
	listener->addCallback("player_team",&MatchManager::player_team);
	listener->addCallback("player_changename",&MatchManager::player_changename);

	try
	{
		// Set the new server password
		string password = plugin->getConVar("cssmatch_password")->GetString();
		plugin->getConVar("sv_password")->SetValue(password.c_str());

		map<string, string> parameters;
		parameters["$password"] = password;
		plugin->addTimer(
			new TimerI18nPopupSay(
				i18n,interfaces->gpGlobals->curtime+5.0f,recipients,"match_password_popup",5,OPTION_ALL,parameters));

		// Set the suitable match state
		infos.warmup = warmup;
		if (plugin->getConVar("cssmatch_kniferound")->GetBool() && kniferound)
			// default value for kniferound is true
		{
			setMatchState(KNIFEROUND);
		}
		else if ((plugin->getConVar("cssmatch_warmup_time")->GetInt() > 0) && warmup)
			// default value for warmup is true
		{
			setMatchState(WARMUP);
		}
		else if (plugin->getConVar("cssmatch_sets")->GetInt() > 0)
		{
			setMatchState(SET);
		}
		else // Error case
		{
			i18n->i18nChatWarning(recipients,"match_config_error");
		}
	}
	catch(const ServerPluginException & e)
	{
		printException(e,__FILE__,__LINE__);
	}

	// Announcement
	if (umpire != NULL)
	{
		IPlayerInfo * playerInfo = umpire->getPlayerInfo();
		if (isValidPlayer(playerInfo))
		{
			map<string,string> parameters;
			parameters["$admin"] = playerInfo->GetName();
			i18n->i18nChatSay(recipients,"match_started_by",umpire->getIdentity()->index,parameters);
		}
	}
	else
		i18n->i18nChatSay(recipients,"match_started");
}

void MatchManager::stop()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	// Stop all event listeners
	listener->removeCallbacks();
}


ClanNameDetectionTimer::ClanNameDetectionTimer(float date, TeamCode teamCode)
	: BaseTimer(date), team(teamCode)
{
}

void ClanNameDetectionTimer::execute()
{
	ServerPlugin * plugin = ServerPlugin::getInstance(); 
	MatchManager * manager = plugin->getMatch();
	manager->detectClanName(team);
}

