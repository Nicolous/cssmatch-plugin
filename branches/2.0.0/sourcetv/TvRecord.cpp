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

#include "TvRecord.h"
#include "../plugin/ServerPlugin.h"

#include "filesystem.h"

using namespace cssmatch;

using std::string;
using std::ostringstream;

TvRecord::TvRecord(std::string & recordName) throw (TvRecordException) : recording(false)
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	if (plugin->hltvConnected())
	{
		// Strip unsupported characters
		normalizeFileName(recordName);

		// Construct the path
		string basePath = plugin->getConVar("cssmatch_sourcetv_path")->GetString();
		string baseName = recordName + ".dem";
		if (basePath.size() > 0)
			baseName = basePath + '/' + baseName;

		/*if (baseName.size() == 0)
			throw TvRecordException("The demo name is empty");*/

		// Rename the file while it matches another existing file name on the hd
		// (Obselete with the current way we construct the demo name, but can change)
		name = baseName;
		int fileCount = 1;
		while(interfaces->filesystem->FileExists(name.c_str(),"MOD"))
		{
			ostringstream newName;
			newName << '[' << fileCount << ']' << baseName;
			name = newName.str();

			fileCount++;
		}
	}
	else
		throw TvRecordException("SourceTv not connected");
}

const string * TvRecord::getName() const
{
	return &name;
}

bool TvRecord::isRecording() const
{
	return recording;
}

void TvRecord::start() throw (TvRecordException)
{
	if (! recording)
	{
		ServerPlugin * plugin = ServerPlugin::getInstance();
		
		plugin->executeCommand("tv_stoprecord;tv_record " + name + "\n"); // Stops any record in progress (e.g. a record lauched by tv_autorecord)

		recording = true;
	}
	else
		throw TvRecordException("Record already in progress");
}
		
void TvRecord::stop() throw (TvRecordException)
{
	if (recording)
	{
		recording = false;

		ServerPlugin::getInstance()->executeCommand("tv_stoprecord " + name + "\n");
	}
	else
		throw TvRecordException("No record in progress");
}
