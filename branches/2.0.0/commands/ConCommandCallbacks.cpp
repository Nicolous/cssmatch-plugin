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

#include "ConCommandCallbacks.h"

#include "../match/MatchManager.h"
#include "../match/DisabledMatchState.h"
#include "../match/BaseMatchState.h"
#include "../match/KnifeRoundMatchState.h"
#include "../match/WarmupMatchState.h"
#include "../match/SetMatchState.h"
#include "../messages/I18nManager.h"
#include "../plugin/ServerPlugin.h"
#include "../configuration/RunnableConfigurationFile.h"
#include "../messages/Countdown.h"

using namespace cssmatch;

#include "../match/WarmupMatchState.h"
#include "../player/ClanMember.h"
#include <list>
#include <algorithm>
using std::list;
using std::find_if;

using std::string;
using std::map;

// Syntax: cssm_help [command name]
void cssmatch::cssm_help()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	const map<string,ConCommand *> * pluginConCommands = plugin->getPluginConCommands();
	map<string,ConCommand *>::const_iterator lastConCommand = pluginConCommands->end();

	if (interfaces->engine->Cmd_Argc() == 1)
	{
		map<string,ConCommand *>::const_iterator itConCommand = pluginConCommands->begin();
		while (itConCommand != lastConCommand)
		{
			ConCommand * command = itConCommand->second;
			plugin->log(string(command->GetName()) + ": " + command->GetHelpText());

			itConCommand++;
		}
	}
	else
	{
		map<string,ConCommand *>::const_iterator itConCommand = pluginConCommands->find(interfaces->engine->Cmd_Argv(1));
		if (itConCommand != lastConCommand)
		{
			plugin->log(itConCommand->first + ": " + itConCommand->second->GetHelpText());
		}
	}
}

// Syntax: cssm_start [configuration file from cstrike/cfg/cssmatch/configurations [-cutround] [-warmup]]
void cssmatch::cssm_start()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	I18nManager * i18n = plugin->getI18nManager();
	MatchManager * match = plugin->getMatch();

	bool kniferound = true;
	bool warmup = true;
	string configurationFile = DEFAULT_CONFIGURATION_FILE;

	switch(interfaces->engine->Cmd_Argc())
	{
	case 4:
	case 3:
		kniferound = string(interfaces->engine->Cmd_Args()).find("-kniferound") == string::npos;
		warmup = string(interfaces->engine->Cmd_Args()).find("-warmup") == string::npos;
		// break;
	case 2:
		configurationFile = interfaces->engine->Cmd_Argv(1);
		// break;
	case 1:
		try
		{
    		RunnableConfigurationFile configuration(CFG_FOLDER_PATH MATCH_CONFIGURATIONS_PATH + configurationFile);

			// Determine the initial match state
			BaseMatchState * initialState = NULL;
			if (plugin->getConVar("cssmatch_kniferound")->GetBool() && kniferound)
			{
				initialState = KnifeRoundMatchState::getInstance();
			}
			else if ((plugin->getConVar("cssmatch_warmup_time")->GetInt() > 0) && warmup)
			{
				initialState = WarmupMatchState::getInstance();
			}
			else if (plugin->getConVar("cssmatch_sets")->GetInt() > 0)
			{
				initialState = SetMatchState::getInstance();
			}
			else // Error case
			{
				RecipientFilter recipients;
				recipients.addAllPlayers();
				i18n->i18nChatWarning(recipients,"match_config_error");
			}

			match->start(configuration,warmup,initialState);
		}
		catch(const ConfigurationFileException & e)
		{
			plugin->log(e.what());
		}
		catch(const ServerPluginException & e)
		{
			plugin->log(e.what());
		}
		break;
	default:
		plugin->log(
			string(interfaces->engine->Cmd_Argv(0)) +
				" [configuration file from cstrike/cfg/cssmatch/configurations [-cutround] [-warmup]]"); 
	}
}

// Syntax: cssm_stop
void cssmatch::cssm_stop()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	Countdown * countdown = Countdown::getInstance();

	plugin->removeTimers();
	countdown->stop();
	match->stop();
}

// Syntax: cssm_retag
void cssmatch::cssm_retag()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	I18nManager * i18n = plugin->getI18nManager();

	if (match->getMatchState()->getId() != DisabledMatchState::getInstance()->getId())
	{
		match->detectClanName(T_TEAM);
		match->detectClanName(CT_TEAM);
	}
	else
	{
		i18n->i18nMsg("match_not_in_progress");
	}
}

// Syntax: cssm_go
void cssmatch::cssm_go()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	I18nManager * i18n = plugin->getI18nManager();

	WarmupMatchState * warmupState = WarmupMatchState::getInstance();
	BaseMatchState * currentState = match->getMatchState();
	if (currentState->getId() == warmupState->getId())
	{
		RecipientFilter recipients;
		recipients.addAllPlayers();

		i18n->i18nChatSay(recipients,"admin_all_teams_say_ready");
		warmupState->endWarmup();
	}
	else if (currentState->getId() != DisabledMatchState::getInstance()->getId())
	{
		i18n->i18nMsg("warmup_disable");
	}
	else
	{
		i18n->i18nMsg("match_not_in_progress");
	}
}

/*void cssmatch::cssm_restartset()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	I18nManager * i18n = plugin->getI18nManager();

	if (match->getMatchState()->getId() != DisabledMatchState::getInstance()->getId())
	{
		
	}
	else
	{
		i18n->i18nMsg("match_not_in_progress");
	}
}*/


// ***************************
// Hook callbacks and tools
// ***************************

bool cssmatch::say_hook(int userIndex, IVEngineServer * engine)
{
	bool eat = false;

	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	I18nManager * i18n = plugin->getI18nManager();

	string chatCommand = engine->Cmd_Argv(1);

	// !go, ready: a clan is ready to end the warmup
	if ((chatCommand == "!go") || (chatCommand == "ready"))
	{
		BaseMatchState * currentState = match->getMatchState();
		if (currentState->getId() == WarmupMatchState::getInstance()->getId())
		{
			list<ClanMember *> * playerlist = plugin->getPlayerlist();
			list<ClanMember *>::iterator invalidPlayer = playerlist->end();
			list<ClanMember *>::iterator itPlayer = find_if(playerlist->begin(),invalidPlayer,PlayerHavingIndex(userIndex));

			if (itPlayer != invalidPlayer)
				WarmupMatchState::getInstance()->doGo(*itPlayer);
			else
				print(__FILE__,__LINE__,"Unable to find the player who typed !go/ready");		
		}
		else
		{
			RecipientFilter recipients;
			recipients.addAllPlayers();	
			if (currentState->getId() != DisabledMatchState::getInstance()->getId())
			{
				i18n->i18nChatSay(recipients,"warmup_disable");
			}
			else
			{
				i18n->i18nChatSay(recipients,"match_not_in_progress");
			}
		}
	}

	return eat;
}
