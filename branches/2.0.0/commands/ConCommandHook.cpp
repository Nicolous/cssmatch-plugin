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

#include "ConCommandHook.h"
#include "../plugin/ServerPlugin.h"

#include <string>
#include <list>

using namespace cssmatch;

using std::string;
using std::list;

ConCommandHook::ConCommandHook(const string & name, const string & helpString)
	: ConCommand(strdup(name.c_str()),NULL,strdup(helpString.c_str()),FCVAR_GAMEDLL), hooked(NULL)
{
	Init();
}

void ConCommandHook::Init()
{
	// Find the command to hook

	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	ICvar * cvars = interfaces->convars->getConVarAccessor();

	if (cvars != NULL)
	{
		const ConCommandBase * listedCommand = cvars->GetCommands();
		bool success = false;

		while(listedCommand != NULL)
		{
			if (listedCommand->IsCommand() &&
				(listedCommand != this) &&
				(V_strcmp(listedCommand->GetName(),GetName()) == 0))
			{
				hooked = (ConCommand *)const_cast<ConCommandBase *>(listedCommand);
				success = true;
				break;
			}

			listedCommand = listedCommand->GetNext();
		}
		
		if (success)
		{
			ConCommand::Init();
		}
		else
		{
			print(__FILE__,__LINE__,string("Unable to hook ") + GetName() + ": command not found");
		}
	}
	else
	{
		print(__FILE__,__LINE__,string("Unable to hook ") + GetName() + ": the interface is not ready");
	}
}

void ConCommandHook::Dispatch()
{
	// Call the corresponding callback, and eat the command call if asked

	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	try
	{
		if (! plugin->getHookCallback(GetName())(plugin->GetCommandClient()+1,interfaces->engine))
		{
			hooked->Dispatch();
		}
	}
	catch(const ServerPluginException & e)
	{
		printException(e,__FILE__,__LINE__);
	}
}
