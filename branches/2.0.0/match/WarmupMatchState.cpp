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

#include "WarmupMatchState.h"

#include "../plugin/ServerPlugin.h"
#include "../player/ClanMember.h"
#include "../messages/Countdown.h"
#include "../messages/I18nManager.h"
#include "MatchManager.h"
#include "DisabledMatchState.h"
#include "HalfMatchState.h"

using namespace cssmatch;

using std::string;
using std::list;
using std::map;

WarmupMatchState::WarmupMatchState() : timer(NULL)
		
{
	warmupMenu = new Menu("menu_warmup",
		new MenuCallback<WarmupMatchState>(this,&WarmupMatchState::warmupMenuCallback));
	warmupMenu->addLine(true,"menu_alltalk");
	warmupMenu->addLine(true,"menu_restart");
	warmupMenu->addLine(true,"menu_stop");
	warmupMenu->addLine(true,"menu_retag");
	warmupMenu->addLine(true,"menu_go");

	menuWithAdmin = new Menu("menu_warmup",
		new MenuCallback<WarmupMatchState>(this,&WarmupMatchState::menuWithAdminCallback));
	menuWithAdmin->addLine(true,"menu_administration_options");
	menuWithAdmin->addLine(true,"menu_alltalk");
	menuWithAdmin->addLine(true,"menu_restart");
	menuWithAdmin->addLine(true,"menu_stop");
	menuWithAdmin->addLine(true,"menu_retag");
	menuWithAdmin->addLine(true,"menu_go");

	eventCallbacks["player_spawn"] = &WarmupMatchState::player_spawn;
	eventCallbacks["round_start"] = &WarmupMatchState::round_start;
	eventCallbacks["bomb_beginplant"] = &WarmupMatchState::bomb_beginplant;
}

WarmupMatchState::~WarmupMatchState()
{
	delete warmupMenu;
	delete menuWithAdmin;
}

void WarmupMatchState::endWarmup()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	I18nManager * i18n = plugin->getI18nManager();

	RecipientFilter recipients;
	recipients.addAllPlayers();

	i18n->i18nChatSay(recipients,"warmup_end");

	Countdown::getInstance()->stop();
	if (timer != NULL) // All the clans typed "!go" before the third restart ?
	{
		timer->cancel();
	}

	if (plugin->getConVar("cssmatch_sets")->GetInt() > 0)
	{
		match->setMatchState(HalfMatchState::getInstance());
	}
	else
	{
		match->stop();
	}
}

void WarmupMatchState::doGo(Player * player)
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	I18nManager * i18n = plugin->getI18nManager();

	RecipientFilter recipients;
	recipients.addAllPlayers();
	map<string, string> parameters;

	try
	{
		TeamCode team = player->getMyTeam();
		MatchClan * clan = match->getClan(team);

		MatchLignup * lignup = match->getLignup();
		MatchClan * otherClan = (&lignup->clan1 != clan) ? &lignup->clan1 : &lignup->clan2;

		parameters["$team"] = *clan->getName();
		string message;
		if (clan->isReady())
		{
			// Clan already "ready"
			message = "warmup_already_ready";
		}
		else
		{
			clan->setReady(true);
			message = "warmup_ready";
		}
		i18n->i18nChatSay(recipients,message,parameters,player->getIdentity()->index);

		// If both clan1 and clan2 are ready, end the warmup
		if (clan->isReady() && otherClan->isReady())
		{
			i18n->i18nChatSay(recipients,"warmup_all_ready");
			endWarmup();
		}
	}
	catch(const MatchManagerException & e)
	{
		CSSMATCH_PRINT_EXCEPTION(e);
	}
}

void WarmupMatchState::startState()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	MatchManager * match = plugin->getMatch();

	MatchLignup * lignup = match->getLignup();
	lignup->clan1.setReady(false); // override any previous warmup settings
	lignup->clan2.setReady(false);

	plugin->queueCommand("mp_restartgame 2\n");

	// Subscribe to the needed game events
	// Subscribe to the needed game events
	map<string,EventCallback>::iterator itEvent = eventCallbacks.begin();
	map<string,EventCallback>::iterator invalidEvent = eventCallbacks.end();
	while(itEvent != invalidEvent)
	{
		interfaces->gameeventmanager2->AddListener(this,itEvent->first.c_str(),true);
		itEvent++;
	}

	match->getInfos()->roundNumber = -2; // negative round number causes a game restart (see round_start)
}

void WarmupMatchState::endState()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	interfaces->gameeventmanager2->RemoveListener(this);
}

void WarmupMatchState::showMenu(Player * recipient)
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
		recipient->sendMenu(warmupMenu,1,parameters);
}

void WarmupMatchState::warmupMenuCallback(Player * player, int choice, MenuLine * selected)
{
	// 1. Enable/Disable alltalk
	// 2. Round restart
	// 3. Clan names detection
	// 4. Force !go

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
		{
			RecipientFilter recipients;
			map<string,string> parameters;
			IPlayerInfo * pInfo = player->getPlayerInfo();
			PlayerIdentity * identity = player->getIdentity();

			recipients.addAllPlayers();
			if (isValidPlayerInfo(pInfo))
			{
				parameters["$admin"] = pInfo->GetName();
				i18n->i18nChatSay(recipients,"admin_round_restarted_by",parameters,identity->index);
			}
			else
				i18n->i18nChatSay(recipients,"admin_round_restarted");

			match->restartRound();
		}
		player->quitMenu();
		break;
	case 3:
		match->stop();
		player->quitMenu();
		break;
	case 4:
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
	case 5:
		{
			RecipientFilter recipients;
			map<string,string> parameters;
			IPlayerInfo * pInfo = player->getPlayerInfo();
			PlayerIdentity * identity = player->getIdentity();

			recipients.addAllPlayers();
			if (isValidPlayerInfo(pInfo))
			{
				parameters["$admin"] = pInfo->GetName();
				i18n->i18nChatSay(recipients,"admin_all_teams_say_ready_by",parameters,identity->index);
			}
			else
				i18n->i18nChatSay(recipients,"admin_all_teams_say_ready");

			endWarmup();
		}
		player->quitMenu();
		break;
	default:
		player->quitMenu();
	}
}

void WarmupMatchState::menuWithAdminCallback(Player * player, int choice, MenuLine * selected)
{
	warmupMenuCallback(player,choice-1,selected);

	// Here because the above callback could invoke player->quitMenu()
	if (choice == 1)
	{
		ServerPlugin * plugin = ServerPlugin::getInstance();
		plugin->showAdminMenu(player);
	}
}

void WarmupMatchState::FireGameEvent(IGameEvent * event)
{
	(this->*eventCallbacks[event->GetName()])(event);
}

void WarmupMatchState::player_spawn(IGameEvent * event)
{
	// Make each player impervious to bullets

	ClanMember * player = NULL;
	CSSMATCH_VALID_PLAYER(PlayerHavingUserid,event->GetInt("userid"),player)
	{
		player->setLifeState(0);
	}
}

void WarmupMatchState::round_start(IGameEvent * event)
{
	// Do the restarts and announce the begin of each warmup round

	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	MatchManager * match = plugin->getMatch();
	MatchInfo * infos = match->getInfos();
	I18nManager * i18n = plugin->getI18nManager();

	RecipientFilter recipients;

	switch(infos->roundNumber++)
	{
	case -2:
		plugin->queueCommand("mp_restartgame 1\n");
		break;
	case -1:
		plugin->queueCommand("mp_restartgame 2\n");
		break;
	case 0:
		{
			int duration = plugin->getConVar("cssmatch_warmup_time")->GetInt()*60;

			Countdown::getInstance()->fire(duration);
			timer = new WarmupTimer(interfaces->gpGlobals->curtime + (float)duration + 1.0f,this);
			plugin->addTimer(timer);

			// Trick: Increment the round number so a restart will not re-lauch the countdown
			infos->roundNumber++;
		}
	//	break;
	default:
		recipients.addAllPlayers();
		i18n->i18nChatSay(recipients,"warmup_announcement");
		break;
	}
}

void WarmupMatchState::bomb_beginplant(IGameEvent * event)
{
	// Prevent the bomb from being planted if needed

	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	I18nManager * i18n = plugin->getI18nManager();

	ClanMember * player = NULL;
	CSSMATCH_VALID_PLAYER(PlayerHavingUserid,event->GetInt("userid"),player)
	{
		PlayerIdentity * identity = player->getIdentity();

		interfaces->helpers->ClientCommand(identity->pEntity,"use weapon_knife");

		RecipientFilter recipients;
		recipients.addRecipient(player);
		i18n->i18nChatSay(recipients,"warmup_c4");
	}
	else
		CSSMATCH_PRINT("Unable to find the player who plants the bomb");

}

WarmupTimer::WarmupTimer(float date, WarmupMatchState * state) : BaseTimer(date), warmupState(state)
{
}

void WarmupTimer::execute()
{
	warmupState->endWarmup();
}
