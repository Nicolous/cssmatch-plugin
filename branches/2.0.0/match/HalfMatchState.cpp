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

#include "HalfMatchState.h"

#include "../misc/common.h"
#include "../plugin/ServerPlugin.h"
#include "../player/Player.h"
#include "../player/ClanMember.h"
#include "../messages/I18nManager.h"
#include "../sourcetv/TvRecord.h"
#include "MatchManager.h"
#include "DisabledMatchState.h"
#include "TimeoutMatchState.h"
#include "WarmupMatchState.h"

#include <ctime>

using namespace cssmatch;

using std::string;
using std::list;
using std::map;
using std::ostringstream;

HalfMatchState::HalfMatchState() : finished(false)
{
	halfMenu = new Menu("menu_match",new MenuCallback<HalfMatchState>(this,&HalfMatchState::halfMenuCallback));
	halfMenu->addLine(true,"menu_alltalk");
	halfMenu->addLine(true,"menu_restart");
	halfMenu->addLine(true,"menu_stop");
	halfMenu->addLine(true,"menu_retag");
	halfMenu->addLine(true,"menu_restart_manche");

	menuWithAdmin = new Menu("menu_match",new MenuCallback<HalfMatchState>(this,&HalfMatchState::menuWithAdminCallback));
	menuWithAdmin->addLine(true,"menu_administration_options");
	menuWithAdmin->addLine(true,"menu_alltalk");
	menuWithAdmin->addLine(true,"menu_restart");
	menuWithAdmin->addLine(true,"menu_stop");
	menuWithAdmin->addLine(true,"menu_retag");
	menuWithAdmin->addLine(true,"menu_restart_manche");

	eventCallbacks["player_death"] = &HalfMatchState::player_death;
	eventCallbacks["round_start"] = &HalfMatchState::round_start;
	eventCallbacks["round_end"] = &HalfMatchState::round_end;
}

HalfMatchState::~HalfMatchState()
{
	delete halfMenu;
	delete menuWithAdmin;
}

void HalfMatchState::startState()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	MatchManager * match = plugin->getMatch();
	MatchInfo * infos = match->getInfos();
	I18nManager * i18n = plugin->getI18nManager();

	// Subscribe to the needed game events
	map<string,EventCallback>::iterator itEvent;
	for(itEvent = eventCallbacks.begin(); itEvent != eventCallbacks.end(); itEvent++)
	{
		interfaces->gameeventmanager2->AddListener(this,itEvent->first.c_str(),true);
	}

	infos->roundNumber = -2; // a negative round number causes a game restart (see round_start)
	finished = false; // (This half is not finished yet)

	RecipientFilter recipients;
	recipients.addAllPlayers();

	if (plugin->getConVar("cssmatch_sourcetv")->GetBool())
	{
		// Start a new record
		tm * date = getLocalTime();
		char dateBuffer[20];
		strftime(dateBuffer,sizeof(dateBuffer),"%Y-%m-%d_%Hh%M",date);

		ostringstream recordName;
		recordName << dateBuffer << '_' << interfaces->gpGlobals->mapname.ToCStr() << "_set" << infos->halfNumber;

		TvRecord * record = NULL;
		try
		{
		    string finalName = recordName.str(); // for gcc
			record = new TvRecord(finalName);
			record->start();
			match->getRecords()->push_back(record);
		}
		catch(const TvRecordException & e)
		{
			i18n->i18nChatWarning(recipients,"error_tv_not_connected");
		}
	}

	// Announce
	map<string,string> parameters;
	parameters["$current"] = toString(infos->halfNumber);
	parameters["$total"] = plugin->getConVar("cssmatch_sets")->GetString();
	i18n->i18nChatSay(recipients,"match_start_manche",parameters);

	i18n->i18nChatSay(recipients,"match_restarts");

	plugin->queueCommand("mp_restartgame 2\n");
}

void HalfMatchState::endState()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	MatchManager * match = plugin->getMatch();

	interfaces->gameeventmanager2->RemoveListener(this);

	// Stop the last record lauched (if any)
	list<TvRecord *> * recordlist = match->getRecords();
	if (! recordlist->empty())
	{
		list<TvRecord *>::reference refLastRecord = recordlist->back();
		if (refLastRecord->isRecording())
			refLastRecord->stop();
	}
}

void HalfMatchState::showMenu(Player * recipient)
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
		recipient->sendMenu(halfMenu,1,parameters);
}

void HalfMatchState::halfMenuCallback(Player * player, int choice, MenuLine * selected)
{
	// 1. Enable/Disable alltalk
	// 2. Restart round
	// 3. Clan name detection
	// 4. Half restart

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
				i18n->i18nChatSay(recipients,"admin_manche_restarted_by",parameters,identity->index);
			}
			else
				i18n->i18nChatSay(recipients,"admin_manche_restarted");

			match->restartHalf();
		}
		player->quitMenu();
		break;
	default:
		player->quitMenu();
	}
}

void HalfMatchState::menuWithAdminCallback(Player * player, int choice, MenuLine * selected)
{
	halfMenuCallback(player,choice-1,selected);

	// Here because the above callback could invoke player->quitMenu()
	if (choice == 1)
	{
		ServerPlugin * plugin = ServerPlugin::getInstance();
		plugin->showAdminMenu(player);
	}
}

void HalfMatchState::endHalf()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	MatchInfo * infos = match->getInfos();

	plugin->queueCommand("plugin_print\n");

	// Update the score history of each player
	list<ClanMember *> * playerlist = plugin->getPlayerlist();
	list<ClanMember *>::iterator itPlayer;
	for(itPlayer = playerlist->begin(); itPlayer != playerlist->end(); itPlayer++)
	{
		PlayerStats * currentStats = (*itPlayer)->getCurrentStats();
		PlayerStats * lastSetStats = (*itPlayer)->getLastSetStats();

		lastSetStats->deaths = currentStats->deaths;
		lastSetStats->kills = currentStats->kills;
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

	RecipientFilter recipients;
	recipients.addAllPlayers();

	map<string,string> parameters;
	parameters["$password"] = plugin->getConVar("cssmatch_password")->GetString();
	plugin->addTimer(new TimerI18nChatSay(5.0f,recipients,"match_password_popup",parameters));

	if (infos->halfNumber < plugin->getConVar("cssmatch_sets")->GetInt())
	{
		// There is at least one more half to play

		BaseMatchState * nextState = this;
		if ((plugin->getConVar("cssmatch_warmup_time")->GetInt() > 0) && infos->warmup)
		{
			nextState = WarmupMatchState::getInstance();
		}

		// Do a time-out (if any) before starting the next state
		int timeoutDuration = plugin->getConVar("cssmatch_end_set")->GetInt();
		if (timeoutDuration > 0)
		{
			map<string,string> timeoutParameters;
			timeoutParameters["$time"] = toString(timeoutDuration);
			plugin->addTimer(new TimerI18nChatSay(2.0f,recipients,"match_dead_time",timeoutParameters));

			TimeoutMatchState::doTimeout(timeoutDuration,nextState);
		}
		else
		{
			match->setMatchState(nextState);
		}
		
		// One more half started
		infos->halfNumber++;

		// Swap every players
		plugin->addTimer(new SwapTimer((float)timeoutDuration));
	}
	else
	{
		// End of the match
		match->stop();
	}
}

void HalfMatchState::finish()
{
	finished = true;

	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	MatchInfo * infos = match->getInfos();
	I18nManager * i18n = plugin->getI18nManager();

	// Send the scores here, so SourceTv receives them before being stopped
	MatchLignup * lignup = match->getLignup();

	RecipientFilter recipients;
	recipients.addAllPlayers();
	map<string,string> parameters;

	parameters["$current"] = toString(infos->halfNumber);

	i18n->i18nChatSay(recipients,"match_end_current_manche",parameters);

	ClanStats * statsClan1 = lignup->clan1.getStats();
	parameters["$team1"] = *lignup->clan1.getName();
	parameters["$score1"] = toString(statsClan1->scoreCT + statsClan1->scoreT);

	ClanStats * statsClan2 = lignup->clan2.getStats();
	parameters["$team2"] = *lignup->clan2.getName();
	parameters["$score2"] = toString(statsClan2->scoreCT + statsClan2->scoreT);

	i18n->i18nPopupSay(recipients,"match_end_manche_popup",6,parameters);
	//i18n->i18nConsoleSay(recipients,"match_end_manche_popup",parameters);
}

void HalfMatchState::FireGameEvent(IGameEvent * event)
{
	try
	{
		(this->*eventCallbacks[event->GetName()])(event);
	}
	catch(const BaseException & e)
	{
		CSSMATCH_PRINT_EXCEPTION(e);
	}
}

void HalfMatchState::player_death(IGameEvent * event)
{
	// Update the score [history] of the involved players

	ServerPlugin * plugin = ServerPlugin::getInstance();

	int idVictim = event->GetInt("userid");
	ClanMember * victim = NULL;
	CSSMATCH_VALID_PLAYER(PlayerHavingUserid,idVictim,victim)
	{
		PlayerStats * currentStats = victim->getCurrentStats();
		currentStats->deaths++;
	}

	int idAttacker = event->GetInt("attacker");
	if (idAttacker != idVictim)
	{
		ClanMember * attacker = NULL;
		CSSMATCH_VALID_PLAYER(PlayerHavingUserid,idAttacker,attacker)
		{
			PlayerStats * currentStats = attacker->getCurrentStats();
			if (attacker->getMyTeam() != attacker->getMyTeam())
				currentStats->kills++;
			else
				currentStats->kills--;
		}
	}
}

void HalfMatchState::round_start(IGameEvent * event)
{
	if (finished)
	{
		// All rounds done, stop this state and the sourcetv records

		endHalf();
	}
	else
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
		list<ClanMember *>::iterator itPlayer;
		for (itPlayer = playerlist->begin(); itPlayer != playerlist->end(); itPlayer++)
		{
			PlayerStats * currentStats = (*itPlayer)->getCurrentStats();
			PlayerStats * lastRoundStats = (*itPlayer)->getLastRoundStats();

			lastRoundStats->deaths = currentStats->deaths;
			lastRoundStats->kills = currentStats->kills;
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
			parameters["$total"] = plugin->getConVar("cssmatch_rounds")->GetString();
			parameters["$team1"] = *lignup->clan1.getName();
			parameters["$score1"] = toString(statsClan1->scoreCT + statsClan1->scoreT);
			parameters["$team2"] = *lignup->clan2.getName();
			parameters["$score2"] = toString(statsClan2->scoreCT + statsClan2->scoreT);
			plugin->addTimer(new TimerI18nPopupSay(1.5f,recipients,"match_round_popup",5,parameters));
		}
	}
}

void HalfMatchState::round_end(IGameEvent * event)
{
	// Update the score of each clan, then end the half if roundNumber >= cssmatch_rounds

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
			finish();
	}
	catch(const MatchManagerException & e)
	{
		// CSSMATCH_PRINT_EXCEPTION(e); // round draw
	}
}

SwapTimer::SwapTimer(float delay) : BaseTimer(delay)
{
}

void SwapTimer::execute()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();

	list<ClanMember *> * playerlist = plugin->getPlayerlist();
	list<ClanMember *>::iterator itPlayer;
	for(itPlayer = playerlist->begin(); itPlayer != playerlist->end(); itPlayer++)
	{
		(*itPlayer)->swap();
	}
}

