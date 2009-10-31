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

#include "../match/DisabledMatchState.h"
#include "../match/MatchManager.h"
#include "../plugin/ServerPlugin.h"
#include "../configuration/RunnableConfigurationFile.h"
#include "../messages/Countdown.h"

using namespace cssmatch;

using std::string;
using std::list;

// Syntax: cssm_help
void cssmatch::cssm_help()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();

	const list<ConCommand *> * pluginConCommands = plugin->getPluginConCommands();
	list<ConCommand *>::const_iterator itConCommand = pluginConCommands->begin();
	list<ConCommand *>::const_iterator lastConCommand = pluginConCommands->end();
	while (itConCommand != lastConCommand)
	{
		ConCommand * command = *itConCommand;
		plugin->log(string(command->GetName()) + " : " + command->GetHelpText());

		itConCommand++;
	}
}

// Syntax: cssm_start [configuration file from cstrike/cfg/cssmatch/configurations [-cutround] [-warmup]]
void cssmatch::cssm_start()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
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
			// FIXME: if -kniferound is not used, a warmup is started even if -warmup was specified
		// break;
	case 2:
		configurationFile = interfaces->engine->Cmd_Argv(1);
		// break;
	case 1:
		try
		{
    		RunnableConfigurationFile configuration(CFG_FOLDER_PATH MATCH_CONFIGURATIONS_PATH + configurationFile);
			match->start(configuration,kniferound,warmup);
		}
		catch(const ConfigurationFileException & e)
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
	match->setMatchState(DisabledMatchState::getInstance());
}
