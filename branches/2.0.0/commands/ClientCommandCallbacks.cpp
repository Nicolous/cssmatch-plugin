/* 
 * Copyright 2008-2010 Nicolas Maingot
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
 * Portions of this code are also Copyright � 1996-2005 Valve Corporation, All rights reserved
 */

#include "../plugin/UpdateNotifier.h"

#include "ClientCommandCallbacks.h"

#include "../plugin/ServerPlugin.h"
#include "../match/MatchManager.h"
#include "../match/HalfMatchState.h"
#include "../player/ClanMember.h"

#include <string>
#include <list>
#include <sstream>

using std::string;
using std::list;
using std::ostringstream;
using std::endl;

PLUGIN_RESULT cssmatch::clientcmd_jointeam(ClanMember * user)
{
	// Stop a player from joining another team during the match 
	// (Excepted before the first half begins, and if the player  wants to join/come from  the spectators)

	PLUGIN_RESULT result = PLUGIN_CONTINUE;

	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	I18nManager * i18n = plugin->getI18nManager();
	MatchManager * match = plugin->getMatch();
	MatchInfo * matchInfo = match->getInfos();
	BaseMatchState * matchState = match->getMatchState();

	bool controlJointeam = false;
	if (matchInfo->halfNumber <= 1)
	{
		if (matchState == HalfMatchState::getInstance())
			controlJointeam = true;
	}
	else if (matchState != match->getInitialState())
		controlJointeam = true;

	if (controlJointeam)
	{
		// Check for command sanity (jointeam without argument causes a error message but swap the player)
		if (interfaces->engine->Cmd_Argc() > 1)
		{
			string arg1 = interfaces->engine->Cmd_Argv(1);

			// If the team the player wants to join is a non-spectator team
			if ((arg1 == "2") || (arg1 == "3"))
			{
				// If the team is not a spectator team
				if (user->getMyTeam() > SPEC_TEAM)
				{
					RecipientFilter recipients;
					recipients.addRecipient(user);

					i18n->i18nChatSay(recipients,"player_no_swap_in_match");
					result = PLUGIN_STOP;
				}
			}
			// Now test if the command is not invalid ("jointeam bla" will swap the player)
			else if (atoi(arg1.c_str()) == 0)
			{
				//Msg("Never trust the user input\n");
				result = PLUGIN_STOP;
			}
			// arg1 < 0 and arg1 > 3 is refused by the game
		}
		else
			result = PLUGIN_STOP;
	}
	return result;
}

PLUGIN_RESULT cssmatch::clientcmd_menuselect(ClanMember * user)
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();	

	Menu * menu = user->getMenu();
	if (menu != NULL)
	{
		if (interfaces->engine->Cmd_Argc() == 2)
		{
			int choice = atoi(interfaces->engine->Cmd_Argv(1));
			if (choice != 0)
			{
				user->cexec("playgamesound UI/buttonrollover.wav\n");
				menu->doCallback(user,choice);
			}
		}
	}
	return PLUGIN_CONTINUE;
}

PLUGIN_RESULT cssmatch::clientcmd_cssmatch(ClanMember * user)
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	UpdateNotifier * updateNotifier = plugin->getUpdateThread();
	I18nManager * i18n = plugin->getI18nManager();
	MatchManager * match = plugin->getMatch();

	if (user->isReferee())
	{
		// Announce any update
		if (match->getMatchState() == match->getInitialState())
		{
			if (updateNotifier != NULL) 
			{
				if (updateNotifier->getLastVer() != CSSMATCH_VERSION)
				{
					RecipientFilter recipients;
					recipients.addRecipient(user);

					i18n->i18nChatSay(recipients,"update_available");
				}
			}
		}

		// Display the menu
		match->showMenu(user);
	}
	else
	{
		PlayerIdentity * playerid = user->getIdentity();
		RecipientFilter recipients;
		recipients.addRecipient(user);

		i18n->i18nChatSay(recipients,"player_you_not_admin");
		plugin->log(playerid->steamid + " is not admin");

		plugin->queueCommand("cssm_adminlist\n");
	}
	return PLUGIN_STOP;
}

PLUGIN_RESULT cssmatch::clientcmd_rates(ClanMember * user)
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	I18nManager * i18n = plugin->getI18nManager();

	RecipientFilter recipients;
	recipients.addRecipient(user);

	list<ClanMember *> * playerlist = plugin->getPlayerlist();
	list<ClanMember *>::const_iterator currentPlayer;
	for(currentPlayer = playerlist->begin(); currentPlayer != playerlist->end(); currentPlayer++)
	{
		int playerIndex = (*currentPlayer)->getIdentity()->index;

		ostringstream message;

		message << string(interfaces->engine->GetClientConVarValue(playerIndex,"name")) << ": " << endl
				<< "  " << "cl_updaterate  = " << interfaces->engine->GetClientConVarValue(playerIndex,"cl_updaterate") << endl
				<< "  " << "cl_cmdrate     = " << interfaces->engine->GetClientConVarValue(playerIndex,"cl_cmdrate") << endl
				<< "  " << "cl_interpolate = " << interfaces->engine->GetClientConVarValue(playerIndex,"cl_interpolate") << endl
				<< "  " << "rate           = " << interfaces->engine->GetClientConVarValue(playerIndex,"rate") << endl
				<< endl;

		const string & finalMsg = message.str();
		i18n->consoleSay(recipients,finalMsg);
	}

	return PLUGIN_STOP;
}

