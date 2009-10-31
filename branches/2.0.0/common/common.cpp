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

#include "common.h"

#include "interface.h"

#include "filesystem.h"
// About: http://developer.valvesoftware.com/wiki/KeyValues_class#Important_Notes

#include "engine/iserverplugin.h"
#include "dlls/iplayerinfo.h"
#include "eiface.h"
#include "igameevents.h"
#include "convar.h"
#include "icvar.h"
#include "bitbuf.h"
#include "baseentity.h"

#include <sstream>
#include <ctime>

#include "../plugin/ServerPlugin.h"

using namespace cssmatch;

using std::string;
using std::ostringstream;

tm * cssmatch::getLocalTime()
{
	time_t date = time(NULL);
	return gmtime(&date);
}

void cssmatch::print(const string & fileName, int line, const string & message)
{
	ostringstream buffer;
	buffer << PLUGIN_NAME << " (" << fileName << " l." << line << ") : " << message << "\n";
	//Msg(buffer.str().c_str());
	ServerPlugin::getInstance()->log(buffer.str());
}

void cssmatch::printException(const BaseException & e, const string & fileName, int line)
{
	ostringstream buffer;
	buffer << e.what() << " (" << fileName << ", l." << line << ")";
	ServerPlugin::getInstance()->log(buffer.str());
}

IServerEntity * cssmatch::getServerEntity(edict_t * entity)
{
	IServerEntity * sEntity = entity->GetIServerEntity();

	if (! isValidServerEntity(sEntity))
	{
		print(__FILE__,__LINE__,"Unable to find the server entity corresponding to this entity");
		sEntity = NULL;
	}

	return sEntity;
}

CBaseEntity * cssmatch::getBaseEntity(edict_t * entity)
{
	CBaseEntity * bEntity = NULL;
	IServerEntity * sEntity = getServerEntity(entity);

	if (isValidServerEntity(sEntity))
		bEntity = sEntity->GetBaseEntity();

	if (! isValidBaseEntity(bEntity))
	{
		print(__FILE__,__LINE__,"Unable to find the base entity corresponding to this entity");
		bEntity = NULL;
	}

	return bEntity;
}

IServerUnknown * cssmatch::getServerUnknow(edict_t * entity)
{
	IServerUnknown * sUnknown = entity->GetUnknown();

	if (! isValidServerUnknown(sUnknown))
	{
		print(__FILE__,__LINE__,"Unable to find the server unknown corresponding to this entity");
		sUnknown = NULL;
	}

	return sUnknown;
}
