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
 * Portions of this code are also Copyright © 1996-2005 Valve Corporation, All rights reserved
 */

#include "BreakMatchState.h"

#include "MatchManager.h"
#include "../messages/Countdown.h"
#include "../messages/Menu.h"
#include "../plugin/ServerPlugin.h"

#include <string>
#include <map>

using namespace cssmatch;

using std::string;
using std::map;

namespace cssmatch
{
	void breakMenuCallback(Player * player, int choice, MenuLine * selected)
	{
		ServerPlugin * plugin = ServerPlugin::getInstance();
		MatchManager * match = plugin->getMatch();
		I18nManager * i18n = plugin->getI18nManager();

		switch(choice)
		{
		case 1:
			plugin->queueCommand(string("sv_alltalk ") + (plugin->getConVar("sv_alltalk")->GetBool() ? "0\n" : "1\n"));
			player->cexec("cssmatch\n");
			break;
		case 2:
			match->stop();
			player->quitMenu();
			break;
		case 3:
			{
				MatchLignup * lignup = match->getLignup();
				match->detectClanName(T_TEAM);
				match->detectClanName(CT_TEAM);

				RecipientFilter recipients;
				recipients.addRecipient(player);
				map<string,string> parameters;
				parameters["$team1"] = *lignup->clan1.getName();
				parameters["$team2"] = *lignup->clan2.getName();
				i18n->i18nChatSay(recipients,"match_name",parameters);

				player->quitMenu();
			}
			break;
		default:
			player->quitMenu();
		}
	}

	void breakMenuWithAdminCallback(Player * player, int choice, MenuLine * selected)
	{
		breakMenuCallback(player,choice-1,selected);

		// Have to be here because the above callback could invoke player->quitMenu()
		if (choice == 1)
		{
			ServerPlugin * plugin = ServerPlugin::getInstance();
			plugin->showAdminMenu(player);
		}
	}
}

BreakMatchState::BreakMatchState() : duration(0), nextState(NULL), timer(NULL)
{
	breakMenu = new Menu("menu_time-out",breakMenuCallback);
	breakMenu->addLine(true,"menu_alltalk");
	breakMenu->addLine(true,"menu_stop");
	breakMenu->addLine(true,"menu_retag");
	
	menuWithAdmin = new Menu("menu_time-out",breakMenuWithAdminCallback);
	menuWithAdmin->addLine(true,"menu_administration_options");
	menuWithAdmin->addLine(true,"menu_alltalk");
	menuWithAdmin->addLine(true,"menu_stop");
	menuWithAdmin->addLine(true,"menu_retag");
}

BreakMatchState::~BreakMatchState()
{
	delete breakMenu;
	delete menuWithAdmin;
}

void BreakMatchState::doBreak(int breakDuration, BaseMatchState * state)
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	BreakMatchState * breakState = BreakMatchState::getInstance();

	breakState->duration = breakDuration;
	breakState->nextState = state;

	match->setMatchState(breakState);
}

void BreakMatchState::startState()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	timer = new BreakMatchTimer(interfaces->gpGlobals->curtime + (float)duration,nextState);
	plugin->addTimer(timer);
	Countdown::getInstance()->fire(duration);
}

void BreakMatchState::endState()
{
	//Countdown::getInstance()->stop(); // in case of interuption ?

	if (timer != NULL)
	{
		timer->cancel();
		timer = NULL;
	}
}

void BreakMatchState::showMenu(Player * recipient)
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	I18nManager * i18n = plugin->getI18nManager();
	string language = interfaces->engine->GetClientConVarValue(recipient->getIdentity()->index,"cl_language");
	bool alltalk = plugin->getConVar("sv_alltalk")->GetBool();

	map<string,string> parameters;
	parameters["$action"] = i18n->getTranslation(language,alltalk ? "menu_disable" : "menu_enable");

	if (plugin->getConVar("cssmatch_advanced")->GetBool())
		recipient->sendMenu(menuWithAdmin,1,parameters);
	else
		recipient->sendMenu(breakMenu,1,parameters);
}

BreakMatchTimer::BreakMatchTimer(float date, BaseMatchState * state)
	: BaseTimer(date), nextState(state)
{
}

void BreakMatchTimer::execute()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();

	match->setMatchState(nextState);
}

