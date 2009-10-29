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

#include "SimplePlugin.h"
#include "../convars/I18nConVar.h"
#include "../commands/I18nConCommand.h"
#include "../commands/ConCommandCallbacks.h"
#include "../convars/ConVarCallbacks.h"
#include "../entity/EntityProp.h"
#include "../player/ClanMember.h"
#include "../messages/I18nManager.h"
#include "../timer/BaseTimer.h"
#include "../match/MatchManager.h"

#include "filesystem.h"
#include "edict.h"
#include "eiface.h"
#include "dlls/iplayerinfo.h"
#include "igameevents.h"
#include "convar.h"
#include "interface.h"

#include <algorithm>
#include <sstream>

using namespace cssmatch;

using std::string;
using std::list;
using std::for_each;
using std::count_if;
using std::find_if;
using std::ostringstream;

/*CON_COMMAND(cssm_test, "CSSMatch : Internal")
{
	SimplePlugin * plugin = SimplePlugin::getInstance();
	int index = atoi(plugin->getInterfaces()->engine->Cmd_Argv(1));
	Player player(plugin->getInterfaces()->engine,plugin->getInterfaces()->playerinfomanager,index);

	EntityProp prop(plugin->getInterfaces()->engine->Cmd_Argv(2),plugin->getInterfaces()->engine->Cmd_Argv(3));
	try
	{
		Msg("%s.%s : %i\n",plugin->getInterfaces()->engine->Cmd_Argv(2),plugin->getInterfaces()->engine->Cmd_Argv(3),prop.getProp<int>(player.getIdentity()->pEntity));
	}
	catch(const EntityPropException & e)
	{
		plugin->printException(e,__FILE__,__LINE__);
	}
}*/

SimplePlugin::SimplePlugin() : clientCommandIndex(0), i18n(NULL)
{
}

SimplePlugin::~SimplePlugin()
{
	if (interfaces.convars != NULL)
		delete interfaces.convars;

	for_each(playerlist.begin(),playerlist.end(),PlayerToRemove());

	if (i18n != NULL)
		delete i18n;

	// TODO: Unlink and delete all ConVar/ConCommand when unloading
	/*std::list<ConVar *>::iterator itConVar = pluginConVars.begin();
	std::list<ConVar *>::iterator lastConVar = pluginConVars.end();
	while (itConVar != lastConVar)
	{
		delete (*itConVar);

		itConVar++;
	}

	std::list<ConCommand *>::iterator itConCommand = pluginConCommands.begin();
	std::list<ConCommand *>::iterator lastConCommand = pluginConCommands.end();
	while (itConCommand != lastConCommand)
	{
		delete (*itConCommand);

		itConCommand++;
	}*/
}

bool SimplePlugin::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory)
{
	bool success = true;

	try
	{
		interfaces.convars = new ConvarsAccessor();
		interfaces.convars->initializeInterface(interfaceFactory);
	}
	catch(const BaseConvarsAccessorException & e)
	{
		success = false;
		Msg(PLUGIN_NAME " : %s\n",e.what()); // Do not use printException here as interfaces.engine isn't initialized !
	}

	success &= 
		getInterface<IPlayerInfoManager>(gameServerFactory,interfaces.playerinfomanager,INTERFACEVERSION_PLAYERINFOMANAGER) &&
		getInterface<IVEngineServer>(interfaceFactory,interfaces.engine,INTERFACEVERSION_VENGINESERVER) &&
		getInterface<IGameEventManager2>(interfaceFactory,interfaces.gameeventmanager2,INTERFACEVERSION_GAMEEVENTSMANAGER2) &&
		getInterface<IFileSystem>(interfaceFactory,interfaces.filesystem,FILESYSTEM_INTERFACE_VERSION) &&
		getInterface<IServerPluginHelpers>(interfaceFactory,interfaces.helpers,INTERFACEVERSION_ISERVERPLUGINHELPERS) &&
		getInterface<IServerGameDLL>(gameServerFactory,interfaces.serverGameDll,/*INTERFACEVERSION_SERVERGAMEDLL*/"ServerGameDLL006");

	if (success)
	{
		interfaces.gpGlobals = interfaces.playerinfomanager->GetGlobalVars();

		MathLib_Init(2.2f,2.2f,0.0f,2.0f);

		
		//	Initialize the translations tools
		// FIXME: ConVar should be I18nConVar
		ConVar * cssmatch_language = new ConVar("cssmatch_language","english",FCVAR_PLUGIN,PLUGIN_NAME " : Default language of CSSMatch (e.g. : \"english\" will use the file  cfg/cssmatch/languages/english.txt)");
		i18n = new I18nManager(interfaces.engine,cssmatch_language);
		addPluginConVar(cssmatch_language);

		// Create the plugin's convars
		addPluginConVar(new I18nConVar(i18n,"cssmatch_version",PLUGIN_VERSION_LIGHT,FCVAR_PLUGIN|FCVAR_NOTIFY|FCVAR_REPLICATED,"cssmatch_version",cssmatch_version));
		addPluginConVar(new I18nConVar(i18n,"cssmatch_advanced","0",FCVAR_PLUGIN,"cssmatch_advanced",true,0.0f,true,1.0f));

		addPluginConVar(new I18nConVar(i18n,"cssmatch_report","1",FCVAR_PLUGIN,"cssmatch_report",true,0.0f,true,1.0f));

		addPluginConVar(new I18nConVar(i18n,"cssmatch_kniferound","1",FCVAR_PLUGIN,"cssmatch_kniferound",true,0.0f,true,1.0f));
		addPluginConVar(new I18nConVar(i18n,"cssmatch_kniferound_money","0",FCVAR_PLUGIN,"cssmatch_kniferound_money",true,0.0f,true,16000.0f));
		addPluginConVar(new I18nConVar(i18n,"cssmatch_kniferound_allows_c4","1",FCVAR_PLUGIN,"cssmatch_kniferound_allows_c4",true,0.0f,true,1.0f));
		addPluginConVar(new I18nConVar(i18n,"cssmatch_end_kniferound","20",FCVAR_PLUGIN,"cssmatch_end_kniferound",true,5.0f,false,0.0f));

		addPluginConVar(new I18nConVar(i18n,"cssmatch_rounds","12",FCVAR_PLUGIN,"cssmatch_rounds",true,0.0f,false,0.0f));
		addPluginConVar(new I18nConVar(i18n,"cssmatch_sets","2",FCVAR_PLUGIN,"cssmatch_sets",true,0.0f,false,0.0f));
		addPluginConVar(new I18nConVar(i18n,"cssmatch_end_set","10",FCVAR_PLUGIN,"cssmatch_end_set",true,5.0f,false,0.0f));

		addPluginConVar(new I18nConVar(i18n,"cssmatch_sourcetv","1",FCVAR_PLUGIN,"cssmatch_sourcetv",true,0.0f,true,1.0f));
		addPluginConVar(new I18nConVar(i18n,"cssmatch_sourcetv_path","cfg/cssmatch/sourcetv",FCVAR_PLUGIN,"cssmatch_sourcetv_path"));

		addPluginConVar(new I18nConVar(i18n,"cssmatch_warmup_time","5",FCVAR_PLUGIN,"cssmatch_warmup_time",true,0.0f,false,0.0f));

		addPluginConVar(new I18nConVar(i18n,"cssmatch_hostname","CSSMatch : %s VS %s",FCVAR_PLUGIN,"cssmatch_hostname"));
		addPluginConVar(new I18nConVar(i18n,"cssmatch_password","inwar",FCVAR_PLUGIN,"cssmatch_password"));
		addPluginConVar(new I18nConVar(i18n,"cssmatch_default_config","server.cfg",FCVAR_PLUGIN,"cssmatch_default_config"));

		// Add existing ConVars
		ICvar * cvars = interfaces.convars->getConVarAccessor();
		addPluginConVar(cvars->FindVar("sv_cheats"));
		addPluginConVar(cvars->FindVar("sv_alltalk"));
		addPluginConVar(cvars->FindVar("hostname"));
		addPluginConVar(cvars->FindVar("sv_password"));
		addPluginConVar(cvars->FindVar("tv_enable"));

		
		// Create the plugin's commands
		addPluginConCommand(new I18nConCommand(i18n,"cssm_help",cssm_help,"cssm_help"));
		addPluginConCommand(new I18nConCommand(i18n,"cssm_start",cssm_start,"cssm_start"));
		addPluginConCommand(new I18nConCommand(i18n,"cssm_stop",cssm_stop,"cssm_stop"));
	}

	return success;
}

ValveInterfaces * SimplePlugin::getInterfaces()
{
	return &interfaces;
}

int SimplePlugin::GetCommandIndex() const
{
	return clientCommandIndex; 
}

void SimplePlugin::SetCommandClient(int index)
{
	clientCommandIndex = index;
}

list<ClanMember *> * SimplePlugin::getPlayerlist()
{
	return &playerlist;
}

MatchManager * SimplePlugin::getMatch()
{
	return &match;
}

void SimplePlugin::addPluginConVar(ConVar * variable)
{
	pluginConVars.push_back(variable);
}

const list<ConVar *> * SimplePlugin::getPluginConVars() const
{
	return &pluginConVars;
}

ConVar * SimplePlugin::getConVar(const std::string & name) throw(BaseConvarsAccessorException)
{
	list<ConVar *>::iterator invalidConVar = pluginConVars.end();
	list<ConVar *>::iterator itConVar = find_if(pluginConVars.begin(),invalidConVar,ConvarHavingName(name));

	if (itConVar == invalidConVar)
		throw BaseConvarsAccessorException("CSSMatch attempts to access to an unknown variable name");

	return *itConVar;
}

void SimplePlugin::addPluginConCommand(ConCommand * command)
{
	pluginConCommands.push_back(command);
}

const list<ConCommand *> * SimplePlugin::getPluginConCommands() const
{
	return &pluginConCommands;
}

I18nManager * SimplePlugin::get18nManager()
{
	return i18n;
}

void SimplePlugin::addTimer(BaseTimer * timer)
{
	timers.push_front(timer); 
	// push front, to allow timer invoking others timers, which could have to be immediately executed
}

void SimplePlugin::removeTimers()
{
	list<BaseTimer *>::iterator itTimer = timers.begin();
	list<BaseTimer *>::iterator invalidTimer = timers.end();
	while(itTimer != invalidTimer)
	{
		delete *itTimer;
		itTimer++;
	}
	timers.clear();
}

void SimplePlugin::setConvarsAccessor(BaseConvarsAccessor * convarsAccessor)
{
	if (interfaces.convars != NULL)
		delete interfaces.convars;

	interfaces.convars = convarsAccessor;
}

void SimplePlugin::Unload()
{
}

void SimplePlugin::Pause()
{
}

void SimplePlugin::UnPause()
{
}

const char * SimplePlugin::GetPluginDescription()
{
	return PLUGIN_VERSION;
}

void SimplePlugin::LevelInit(char const * pMapName)
{
}

void SimplePlugin::ServerActivate(edict_t * pEdictList, int edictCount, int clientMax)
{
}

void SimplePlugin::GameFrame(bool simulating)
{
	// Execute and remove the timers out of date
	timers.remove_if(TimerOutOfDate(interfaces.gpGlobals->curtime));
}

void SimplePlugin::LevelShutdown() // !!!!this can get called multiple times per map change
{
}

void SimplePlugin::ClientActive(edict_t * pEntity)
{
}

void SimplePlugin::ClientDisconnect(edict_t * pEntity)
{
	list<ClanMember *>::iterator endPlayer = playerlist.end();
	list<ClanMember *>::iterator itPlayer = find_if(playerlist.begin(),endPlayer,PlayerHavingPEntity(pEntity));
	if (itPlayer != endPlayer)
	{
		ClanMember * toRemove = *itPlayer;
		playerlist.remove(toRemove);
		// PlayerToRemove()(toRemove);
		delete toRemove;
	}
}

void SimplePlugin::ClientPutInServer(edict_t * pEntity, char const * playername)
{
	int index = interfaces.engine->IndexOfEdict(pEntity);
    if (isValidPlayerIndex(index,interfaces.gpGlobals->maxClients))
    {
            try
            {
				playerlist.push_back(new ClanMember(index));
            }
            catch(const PlayerException & e)
            {
                printException(e,__FILE__,__LINE__);
            }
    }
}

void SimplePlugin::ClientSettingsChanged(edict_t * pEdict)
{
}

PLUGIN_RESULT SimplePlugin::ClientConnect(bool * bAllowConnect,
										edict_t * pEntity,
										const char * pszName,
										const char * pszAddress,
										char * reject,
										int maxrejectlen)
{
	return PLUGIN_CONTINUE;
}

PLUGIN_RESULT SimplePlugin::ClientCommand(edict_t * pEntity)
{
	return PLUGIN_CONTINUE;
}

PLUGIN_RESULT SimplePlugin::NetworkIDValidated(const char * pszUserName, const char * pszNetworkID)
{
	return PLUGIN_CONTINUE;
}

void SimplePlugin::log(const std::string & message) const
{
	ostringstream buffer;
	buffer << PLUGIN_NAME << " : " << message << "\n";
	interfaces.engine->LogPrint(buffer.str().c_str());
}

void SimplePlugin::queueCommand(const string & command) const
{
	interfaces.engine->ServerCommand(command.c_str());
}

void SimplePlugin::kickid(int userid, const string & reason) const
{
	ostringstream command;
	command << "kickid " << userid << " " << reason;
	queueCommand(command.str());
}

bool SimplePlugin::hltvConnected() const
{
	list<ClanMember *>::const_iterator invalidPlayer = playerlist.end();
	return find_if(playerlist.begin(),invalidPlayer,PlayerIsHltv()) != invalidPlayer;
}

int SimplePlugin::getPlayerCount(TeamCode team) const
{
	int playerCount = 0;

	if (team == INVALID_TEAM)
			playerCount = (int)playerlist.size();
	else
			playerCount = count_if(playerlist.begin(),playerlist.end(),PlayerHavingTeam(team));
	return playerCount;
}

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(	SimplePlugin, 
									IServerPluginCallbacks,
									INTERFACEVERSION_ISERVERPLUGINCALLBACKS,
									*cssmatch::SimplePlugin::getInstance());
