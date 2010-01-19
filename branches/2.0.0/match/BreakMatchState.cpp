/* 
 * Copyright 2009, 2010 Nicolas Maingot
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
#include "../plugin/ServerPlugin.h"

using namespace cssmatch;

BreakMatchState::BreakMatchState() : duration(0), nextState(NULL)
{
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

	plugin->addTimer(new BreakMatchTimer(interfaces->gpGlobals->curtime + (float)duration,nextState));
	Countdown::getInstance()->fire(duration);
}

void BreakMatchState::endState()
{
	//Countdown::getInstance()->stop(); // in case of interuption ?
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

