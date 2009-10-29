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

#include "WarmupMatchState.h"

#include "MatchManager.h"
#include "../plugin/SimplePlugin.h"
#include "../player/ClanMember.h"
#include "../messages/Countdown.h"
#include "../messages/I18nManager.h"

#include "SetMatchState.h"

#include "igameevents.h"

#include <algorithm>

using namespace cssmatch;

using std::string;
using std::list;
using std::map;
using std::find_if;

WarmupMatchState::WarmupMatchState()
{
	SimplePlugin * plugin = SimplePlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	listener = new EventListener<WarmupMatchState>(this,interfaces->gameeventmanager2);
}

WarmupMatchState::~WarmupMatchState()
{
	delete listener;
}

void WarmupMatchState::endWarmup()
{
	SimplePlugin * plugin = SimplePlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	I18nManager * i18n = plugin->get18nManager();

	list<ClanMember *> * playerlist = plugin->getPlayerlist();
	list<ClanMember *>::iterator invalidPlayer = playerlist->end();

	RecipientFilter recipients;
	for_each(playerlist->begin(),playerlist->end(),PlayerToRecipient(&recipients));

	i18n->i18nChatSay(recipients,"warmup_all_ready");
	Countdown::getInstance()->stop(); // Do not place it in ::endState(), otherwise 00:00 arrives | TODO : hu ? correct this comment :-p
	plugin->removeTimers(); // Prevent the timeout to happen

	i18n->i18nChatSay(recipients,"warmup_end");

	try
	{
		if (plugin->getConVar("cssmatch_sets")->GetInt() > 0)
		{
			match->setMatchState(SetMatchState::getInstance());
		}
		/*else
		{
			std::list<ClanMember *> * playerlist = plugin->getPlayerlist();
			RecipientFilter recipients;
			std::for_each(playerlist->begin(),playerlist->end(),PlayerToRecipient(&recipients));

			i18n->i18nChatWarning(recipients,"match_config_error");
		}*/ // CSSMatch could only be used for the warmup feature
	}
	catch(const BaseConvarsAccessorException & e)
	{
		printException(e,__FILE__,__LINE__);
	}
}

void WarmupMatchState::startState()
{
	ActivatedMatchState::startState();

	SimplePlugin * plugin = SimplePlugin::getInstance();
	MatchManager * match = plugin->getMatch();

	MatchLignup * lignup = match->getLignup();
	lignup->clan1.setReady(false); // override any previous warmup settings
	lignup->clan2.setReady(false);

	plugin->queueCommand("mp_restartgame 2\n");
	//plugin->addTimer(new TimerWarmupCountdown(interfaces->gpGlobals->curtime + 3.0f));

	// Subscribe to the needed game events
	listener->addCallback("player_spawn",&WarmupMatchState::player_spawn);
	listener->addCallback("player_say",&WarmupMatchState::player_say);
	listener->addCallback("round_start",&WarmupMatchState::round_start);
	listener->addCallback("bomb_beginplant",&WarmupMatchState::bomb_beginplant);

	match->getInfos()->roundNumber = -2; // negative round number causes a game restart (see round_start)
}

void WarmupMatchState::endState()
{
	ActivatedMatchState::endState();

	listener->removeCallbacks();
}

void WarmupMatchState::player_spawn(IGameEvent * event)
{
	// Make each player impervious to bullets

	SimplePlugin * plugin = SimplePlugin::getInstance();

	list<ClanMember *> * playerlist = plugin->getPlayerlist();
	list<ClanMember *>::iterator invalidPlayer = playerlist->end();

	list<ClanMember *>::iterator itPlayer = 
		find_if(playerlist->begin(),invalidPlayer,PlayerHavingUserid(event->GetInt("userid")));
	if (itPlayer != invalidPlayer)
	{
		(*itPlayer)->setLifeState(0);
	}
}

void WarmupMatchState::player_say(IGameEvent * event)
{ // FIXME : these commands should work even in the other stats
		// => ConCommand + kind of typeid ?

	// Defines the chat commands used to end the warmup before its timeout

	string text = event->GetString("text");

	if ((text == "!go") || (text == "ready")) // ready to end the warmup time
	{
		SimplePlugin * plugin = SimplePlugin::getInstance();
		ValveInterfaces * interfaces = plugin->getInterfaces();
		MatchManager * match = plugin->getMatch();
		I18nManager * i18n = plugin->get18nManager();

		list<ClanMember *> * playerlist = plugin->getPlayerlist();
		list<ClanMember *>::iterator invalidPlayer = playerlist->end();

		RecipientFilter recipients;
		map<string, string> parameters;
		for_each(playerlist->begin(),playerlist->end(),PlayerToRecipient(&recipients));

		// TODO : a method to find a player in SimplePlugin ?
		list<ClanMember *>::iterator itPlayer = 
		find_if(playerlist->begin(),invalidPlayer,PlayerHavingUserid(event->GetInt("userid")));
		if (itPlayer != invalidPlayer)
		{
			try
			{
				TeamCode team = (*itPlayer)->getMyTeam();
				MatchLignup * lignup = match->getLignup();

				MatchClan * clan = match->getClan(team);
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
				i18n->i18nChatSay(recipients,message,(*itPlayer)->getIdentity()->index,parameters);

				// If both clan1 and clan2 are ready, end the warmup
				if (clan->isReady() && otherClan->isReady())
				{
					endWarmup();
				}
			}
			catch(const MatchManagerException & e)
			{
				printException(e,__FILE__,__LINE__);
			}
		}
	}
}

void WarmupMatchState::round_start(IGameEvent * event)
{
	// Do the restarts and announce the begin of each warmup round

	SimplePlugin * plugin = SimplePlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	I18nManager * i18n = plugin->get18nManager();

	std::list<ClanMember *> * playerlist = plugin->getPlayerlist();
	RecipientFilter recipients;

	switch(match->getInfos()->roundNumber++)
	{
	case -2:
		plugin->queueCommand("mp_restartgame 1\n");
		break;
	case -1:
		plugin->queueCommand("mp_restartgame 2\n");
		break;
	case 0:
		try
		{
			Countdown::getInstance()->fire(plugin->getConVar("cssmatch_warmup_time")->GetInt());
		}
		catch(const BaseConvarsAccessorException & e)
		{
			printException(e,__FILE__,__LINE__);
		}
	//	break;
	default:
		for_each(playerlist->begin(),playerlist->end(),PlayerToRecipient(&recipients));
		i18n->i18nChatSay(recipients,"warmup_announcement");
		// TODO : detect clan names ?
		break;
	}
}

void WarmupMatchState::bomb_beginplant(IGameEvent * event)
{
	// Prevent the bomb from being planted if needed

	SimplePlugin * plugin = SimplePlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	I18nManager * i18n = plugin->get18nManager();

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
		i18n->i18nChatSay(recipients,"warmup_c4");
	}
	else
		print(__FILE__,__LINE__,"Unable to find the player who plants the bomb");

}

TimerWarmup::TimerWarmup(float date, WarmupMatchState * state) : BaseTimer(date), warmupState(state)
{
}

void TimerWarmup::execute()
{
	warmupState->endWarmup();
}
