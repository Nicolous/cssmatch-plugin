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

#include "Countdown.h"

#include "../plugin/ServerPlugin.h"
#include "../messages/I18nManager.h"

#include <sstream>

using namespace cssmatch;

using std::ostringstream;
using std::list;

Countdown::CountdownTick::CountdownTick(float executionDate, int nextCount)
	: BaseTimer(executionDate), left(nextCount)
{
}

void Countdown::CountdownTick::execute()
{
	// Convert the time left to minutes/seconds values
	int seconds = left;
	int minutes = (int)(seconds/60);
	//seconds -= minutes*60;
	seconds %= 60;
	
	// Construct the message
	ostringstream message;
	if (seconds >= 10)
	{
		if (minutes >= 10)
			message << minutes << " : " << seconds;
		else
			message << "0" <<minutes << " : " << seconds;
	}
	else
	{
		if (minutes >= 10)
			message << minutes << " : " << "0" << seconds;
		else
			message << "0" << minutes << " : " << "0" << seconds;
	}

	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	I18nManager * i18n = plugin->getI18nManager();

	RecipientFilter recipients;
	recipients.addAllPlayers();

	i18n->hintSay(recipients,message.str());

	if (Countdown::getInstance()->decTimeLeft() >= 0)
	{
		plugin->addTimer(new CountdownTick(interfaces->gpGlobals->curtime+1.0f,left-1));
	}
}

int Countdown::decTimeLeft()
{
	return --left;
}

Countdown::Countdown() : left(0)
{
}

Countdown::~Countdown()
{
}

void Countdown::fire(int seconds)
{
	left = seconds;

	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	plugin->addTimer(new CountdownTick(interfaces->gpGlobals->curtime+1.0f,left));
}

void Countdown::stop()
{
	left = 0;
}
