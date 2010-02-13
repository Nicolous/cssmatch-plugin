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

#include "SetMatchState.h"

#include "../common/common.h"
#include "../plugin/ServerPlugin.h"
#include "../player/Player.h"
#include "../player/ClanMember.h"
#include "../messages/I18nManager.h"
#include "../sourcetv/TvRecord.h"
#include "../messages/Menu.h"
#include "MatchManager.h"
#include "DisabledMatchState.h"
#include "BreakMatchState.h"
#include "WarmupMatchState.h"

#include "igameevents.h"

#include <ctime>
#include <algorithm>

using namespace cssmatch;

using std::string;
using std::list;
using std::map;
using std::find_if;
using std::ostringstream;

namespace cssmatch
{
	void setStateMenuCallback(Player * player, int choice, MenuLine * selected)
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
			{
				RecipientFilter recipients;
				map<string,string> parameters;
				IPlayerInfo * pInfo = player->getPlayerInfo();
				PlayerIdentity * identity = player->getIdentity();

				recipients.addAllPlayers();
				if (isValidPlayer(pInfo))
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
				recipients.addRecipient(player->getIdentity()->index);
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
				if (isValidPlayer(pInfo))
				{
					parameters["$admin"] = pInfo->GetName();
					i18n->i18nChatSay(recipients,"admin_manche_restarted_by",parameters,identity->index);
				}
				else
					i18n->i18nChatSay(recipients,"admin_manche_restarted");

				match->restartSet();
			}
			player->quitMenu();
			break;
		default:
			player->quitMenu();
		}
	}

	void setStateMenuWithAdminCallback(Player * player, int choice, MenuLine * selected)
	{
		setStateMenuCallback(player,choice-1,selected);

		// Have to be here because the above callback could invoke player->quitMenu()
		if (choice == 1)
		{
			ServerPlugin * plugin = ServerPlugin::getInstance();
			plugin->showAdminMenu(player);
		}
	}
}

SetMatchState::SetMatchState() : finished(false)
{
	listener = new EventListener<SetMatchState>(this);

	setStateMenu = new Menu("menu_match",setStateMenuCallback);
	setStateMenu->addLine(true,"menu_alltalk");
	setStateMenu->addLine(true,"menu_restart");
	setStateMenu->addLine(true,"menu_stop");
	setStateMenu->addLine(true,"menu_retag");
	setStateMenu->addLine(true,"menu_restart_manche");

	menuWithAdmin = new Menu("menu_match",setStateMenuWithAdminCallback);
	menuWithAdmin->addLine(true,"menu_administration_options");
	menuWithAdmin->addLine(true,"menu_alltalk");
	menuWithAdmin->addLine(true,"menu_restart");
	menuWithAdmin->addLine(true,"menu_stop");
	menuWithAdmin->addLine(true,"menu_retag");
	menuWithAdmin->addLine(true,"menu_restart_manche");
}

SetMatchState::~SetMatchState()
{
	delete listener;
	delete setStateMenu;
	delete menuWithAdmin;
}

void SetMatchState::startState()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	MatchManager * match = plugin->getMatch();
	MatchInfo * infos = match->getInfos();
	I18nManager * i18n = plugin->getI18nManager();

	// Subscribe to the needed game events
	listener->addCallback("player_death",&SetMatchState::player_death);
	listener->addCallback("round_start",&SetMatchState::round_start);
	listener->addCallback("round_end",&SetMatchState::round_end);

	infos->roundNumber = -2; // a negative round number causes a game restart (see round_start)
	finished = false;

	RecipientFilter recipients;
	recipients.addAllPlayers();

	if (plugin->getConVar("cssmatch_sourcetv")->GetBool())
	{
		// Start a new record
		tm * date = getLocalTime();
		char dateBuffer[20];
		strftime(dateBuffer,sizeof(dateBuffer),"%Y-%m-%d_%Hh%M",date);

		ostringstream recordName;
		recordName << dateBuffer << '_' << interfaces->gpGlobals->mapname.ToCStr() << "_set" << infos->setNumber;

		TvRecord * record = NULL;
		try
		{
			record = new TvRecord(recordName.str());
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
	parameters["$current"] = toString(infos->setNumber);
	parameters["$total"] = plugin->getConVar("cssmatch_sets")->GetString();
	i18n->i18nChatSay(recipients,"match_start_manche",parameters);

	i18n->i18nChatSay(recipients,"match_restarts");

	plugin->queueCommand("mp_restartgame 2\n");
}

void SetMatchState::endState()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();

	listener->removeCallbacks();

	// Stop the last record lauched (if any)
	list<TvRecord *> * recordlist = match->getRecords();
	if (! recordlist->empty())
	{
		list<TvRecord *>::reference refLastRecord = recordlist->back();
		if (refLastRecord->isRecording())
			refLastRecord->stop(); // TODO: delay?
	}
}

void SetMatchState::showMenu(Player * recipient)
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	I18nManager * i18n = plugin->getI18nManager();
	string language = interfaces->engine->GetClientConVarValue(recipient->getIdentity()->index,"cl_language");
	bool alltalk = plugin->getConVar("sv_alltalk")->GetBool();

	map<string,string> parameters;
	parameters["$action"] = i18n->getTranslation(language,alltalk ? "menu_disable" : "menu_enable");
	recipient->sendMenu(setStateMenu,1,parameters);
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
		//i18n->i18nConsoleSay(recipients,"match_end_manche_popup",parameters);

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
	if (finished)
	{
		// All rounds done, stop this state and the sourcetv records

		endSet();
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
			parameters["$total"] = plugin->getConVar("cssmatch_rounds")->GetString();
			parameters["$team1"] = *lignup->clan1.getName();
			parameters["$score1"] = toString(statsClan1->scoreCT + statsClan1->scoreT);
			parameters["$team2"] = *lignup->clan2.getName();
			parameters["$score2"] = toString(statsClan2->scoreCT + statsClan2->scoreT);
			plugin->addTimer(
				new TimerI18nPopupSay(0.5f,recipients,"match_round_popup",5,parameters));
		}
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
		finished = infos->roundNumber >= plugin->getConVar("cssmatch_rounds")->GetInt();
	}
	catch(const MatchManagerException & e)
	{
		// CSSMATCH_PRINT_EXCEPTION(e); // round draw
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

