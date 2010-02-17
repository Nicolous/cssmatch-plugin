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

#include "ServerPlugin.h"
#include "BaseTimer.h"
#include "../configuration/ConfigurationFile.h"
#include "../convars/I18nConVar.h"
#include "../commands/I18nConCommand.h"
#include "../commands/ConCommandCallbacks.h"
#include "../commands/ConCommandHook.h"
#include "../convars/ConVarCallbacks.h"
#include "../entity/EntityProp.h"
#include "../player/ClanMember.h"
#include "../messages/I18nManager.h"
#include "../match/MatchManager.h"
#include "../match/DisabledMatchState.h"
#include "../match/KnifeRoundMatchState.h"

#include "filesystem.h"
#include "edict.h"
#include "eiface.h"
#include "dlls/iplayerinfo.h"
#include "igameevents.h"
#include "convar.h"
#include "interface.h"

#include <sstream>

using namespace cssmatch;

using std::string;
using std::list;
using std::map;
using std::for_each;
using std::count_if;
using std::find;
using std::find_if;
using std::ostringstream;

/*CON_COMMAND(cssm_test, "CSSMatch : Internal")
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	int index = atoi(plugin->getInterfaces()->engine->Cmd_Argv(1));
	Player player(plugin->getInterfaces()->engine,plugin->getInterfaces()->playerinfomanager,index);

	EntityProp prop(plugin->getInterfaces()->engine->Cmd_Argv(2),plugin->getInterfaces()->engine->Cmd_Argv(3));
	try
	{
		Msg("%s.%s : %i\n",plugin->getInterfaces()->engine->Cmd_Argv(2),plugin->getInterfaces()->engine->Cmd_Argv(3),prop.getProp<int>(player.getIdentity()->pEntity));
	}
	catch(const EntityPropException & e)
	{
		plugin->CSSMATCH_PRINT_EXCEPTION(e);
	}
}*/

ServerPlugin::ServerPlugin() : clientCommandIndex(0), adminMenu(NULL), bantimeMenu(NULL), match(NULL), i18n(NULL)
{
}

ServerPlugin::~ServerPlugin()
{
	removeTimers();

	if (interfaces.convars != NULL)
		delete interfaces.convars;

	for_each(playerlist.begin(),playerlist.end(),PlayerToRemove());

	if (adminMenu != NULL)
		delete adminMenu;

	if (bantimeMenu != NULL)
		delete bantimeMenu;

	if (match != NULL)
		delete match;

	if (i18n != NULL)
		delete i18n;

	// TODO: Unlink and delete all ConVar/ConCommand when unloading
	// https://forums.alliedmods.net/showthread.php?p=319104#post319104 ?
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

bool ServerPlugin::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory)
{
	bool success = true;

	try
	{
		interfaces.convars = new ConvarsAccessor();
		interfaces.convars->initializeInterface(interfaceFactory);
	}
	catch(const ConvarsAccessorException & e)
	{
		success = false;
		Msg(CSSMATCH_NAME " : %s\n",e.what()); // Do not use printException here as interfaces.engine isn't initialized!
	}

	success &= 
		getInterface<IPlayerInfoManager>(gameServerFactory,interfaces.playerinfomanager,INTERFACEVERSION_PLAYERINFOMANAGER) &&
		getInterface<IVEngineServer>(interfaceFactory,interfaces.engine,INTERFACEVERSION_VENGINESERVER) &&
		getInterface<IGameEventManager2>(interfaceFactory,interfaces.gameeventmanager2,INTERFACEVERSION_GAMEEVENTSMANAGER2) &&
		getInterface<IFileSystem>(interfaceFactory,interfaces.filesystem,FILESYSTEM_INTERFACE_VERSION) &&
		getInterface<IServerPluginHelpers>(interfaceFactory,interfaces.helpers,INTERFACEVERSION_ISERVERPLUGINHELPERS) &&
		getInterface<IServerGameDLL>(gameServerFactory,interfaces.serverGameDll,INTERFACEVERSION_SERVERGAMEDLL);

	if (success)
	{
		interfaces.gpGlobals = interfaces.playerinfomanager->GetGlobalVars();

		MathLib_Init(2.2f,2.2f,0.0f,2.0f);

		// Initialize the admin menus
		adminMenu = new Menu("menu_administration",
			new MenuCallback<ServerPlugin>(this,&ServerPlugin::adminMenuCallback));
		adminMenu->addLine(true,"menu_changelevel");
		adminMenu->addLine(true,"menu_swap");
		adminMenu->addLine(true,"menu_spec");
		adminMenu->addLine(true,"menu_kick");
		adminMenu->addLine(true,"menu_ban");

		bantimeMenu = new Menu("menu_ban_time",
			new MenuCallback<ServerPlugin>(this,&ServerPlugin::bantimeMenuCallback));
		bantimeMenu->addLine(true,"menu_5_min");
		bantimeMenu->addLine(true,"menu_1_h");
		bantimeMenu->addLine(true,"menu_permanent");

		match = new MatchManager(DisabledMatchState::getInstance());
		
		//	Initialize the translations tools
		i18n = new I18nManager();
		I18nConVar * cssmatch_language = new I18nConVar(i18n,"cssmatch_language","english",FCVAR_NOTIFY|FCVAR_REPLICATED,"cssmatch_language");
		addPluginConVar(cssmatch_language);
		i18n->setDefaultLanguage(cssmatch_language);

		// Create the other convars
		addPluginConVar(new I18nConVar(i18n,"cssmatch_version",CSSMATCH_VERSION_LIGHT,FCVAR_NOTIFY|FCVAR_REPLICATED,"cssmatch_version",cssmatch_version));
		addPluginConVar(new I18nConVar(i18n,"cssmatch_advanced","0",FCVAR_NONE,"cssmatch_advanced",true,0.0f,true,1.0f));

		addPluginConVar(new I18nConVar(i18n,"cssmatch_report","1",FCVAR_NONE,"cssmatch_report",true,0.0f,true,1.0f));

		addPluginConVar(new I18nConVar(i18n,"cssmatch_kniferound","1",FCVAR_NONE,"cssmatch_kniferound",true,0.0f,true,1.0f));
		addPluginConVar(new I18nConVar(i18n,"cssmatch_kniferound_money","0",FCVAR_NONE,"cssmatch_kniferound_money",true,0.0f,true,16000.0f));
		addPluginConVar(new I18nConVar(i18n,"cssmatch_kniferound_allows_c4","1",FCVAR_NONE,"cssmatch_kniferound_allows_c4",true,0.0f,true,1.0f));
		addPluginConVar(new I18nConVar(i18n,"cssmatch_end_kniferound","20",FCVAR_NONE,"cssmatch_end_kniferound",true,5.0f,false,0.0f));

		addPluginConVar(new I18nConVar(i18n,"cssmatch_rounds","12",FCVAR_NONE,"cssmatch_rounds",true,0.0f,false,0.0f));
		addPluginConVar(new I18nConVar(i18n,"cssmatch_sets","2",FCVAR_NONE,"cssmatch_sets",true,0.0f,false,0.0f));
		addPluginConVar(new I18nConVar(i18n,"cssmatch_end_set","10",FCVAR_NONE,"cssmatch_end_set",true,5.0f,false,0.0f));

		addPluginConVar(new I18nConVar(i18n,"cssmatch_sourcetv","1",FCVAR_NONE,"cssmatch_sourcetv",true,0.0f,true,1.0f));
		addPluginConVar(new I18nConVar(i18n,"cssmatch_sourcetv_path","cfg/cssmatch/sourcetv",FCVAR_NONE,"cssmatch_sourcetv_path"));

		addPluginConVar(new I18nConVar(i18n,"cssmatch_warmup_time","5",FCVAR_NONE,"cssmatch_warmup_time",true,0.0f,false,0.0f));

		addPluginConVar(new I18nConVar(i18n,"cssmatch_hostname","CSSMatch : %s VS %s",FCVAR_NONE,"cssmatch_hostname"));
		addPluginConVar(new I18nConVar(i18n,"cssmatch_password","inwar",FCVAR_NONE,"cssmatch_password"));
		addPluginConVar(new I18nConVar(i18n,"cssmatch_default_config","server.cfg",FCVAR_NONE,"cssmatch_default_config"));

		addPluginConVar(new I18nConVar(i18n,"cssmatch_usermessages","28",FCVAR_NONE,"cssmatch_usermessages"));

		// Grab some existing ConVars
		ICvar * cvars = interfaces.convars->getConVarInterface();
		ConVar * sv_cheats = cvars->FindVar("sv_cheats");
		ConVar * sv_alltalk = cvars->FindVar("sv_alltalk");
		ConVar * hostname = cvars->FindVar("hostname");
		ConVar * sv_password = cvars->FindVar("sv_password");
		ConVar * tv_enable = cvars->FindVar("tv_enable");
		if ((sv_cheats == NULL) ||
			(sv_alltalk == NULL) ||
			(hostname == NULL) ||
			(sv_password == NULL) ||
			(tv_enable == NULL))
		{
			success = false;
			CSSMATCH_PRINT("At least one game ConVars was not found");
		}
		addPluginConVar(sv_cheats);
		addPluginConVar(sv_alltalk);
		addPluginConVar(hostname);
		addPluginConVar(sv_password);
		addPluginConVar(tv_enable);
		
		// Create the plugin's commands
		addPluginConCommand(new I18nConCommand(i18n,"cssm_help",cssm_help,"cssm_help"));
		addPluginConCommand(new I18nConCommand(i18n,"cssm_start",cssm_start,"cssm_start"));
		addPluginConCommand(new I18nConCommand(i18n,"cssm_stop",cssm_stop,"cssm_stop"));
		addPluginConCommand(new I18nConCommand(i18n,"cssm_retag",cssm_retag,"cssm_retag"));
		addPluginConCommand(new I18nConCommand(i18n,"cssm_go",cssm_go,"cssm_go"));
		addPluginConCommand(new I18nConCommand(i18n,"cssm_restartmanche",cssm_restartmanche,"cssm_restartmanche")); // backward compatibility
		addPluginConCommand(new I18nConCommand(i18n,"cssm_restarthalf",cssm_restartmanche,"cssm_restartmanche")); // same than cssm_restartmanche
		addPluginConCommand(new I18nConCommand(i18n,"cssm_restartround",cssm_restartround,"cssm_restartround"));
		addPluginConCommand(new I18nConCommand(i18n,"cssm_adminlist",cssm_adminlist,"cssm_adminlist"));
		addPluginConCommand(new I18nConCommand(i18n,"cssm_grant",cssm_grant,"cssm_grant"));
		addPluginConCommand(new I18nConCommand(i18n,"cssm_revoke",cssm_revoke,"cssm_revoke"));
		addPluginConCommand(new I18nConCommand(i18n,"cssm_teamt",cssm_teamt,"cssm_teamt"));
		addPluginConCommand(new I18nConCommand(i18n,"cssm_teamct",cssm_teamct,"cssm_teamct"));
		addPluginConCommand(new I18nConCommand(i18n,"cssm_swap",cssm_swap,"cssm_swap"));
		addPluginConCommand(new I18nConCommand(i18n,"cssm_spec",cssm_spec,"cssm_spec"));

		// Hook needed commands
		hookConCommand("say",say_hook);
		hookConCommand("say_team",say_hook);
		/*hookConCommand("tv_stoprecord",tv_stoprecord_hook);
		hookConCommand("tv_stop",tv_stoprecord_hook);*/
	}

	return success;
}

ValveInterfaces * ServerPlugin::getInterfaces()
{
	return &interfaces;
}

int ServerPlugin::GetCommandClient() const
{
	return clientCommandIndex; 
}

void ServerPlugin::SetCommandClient(int index)
{
	clientCommandIndex = index;
}

list<ClanMember *> * ServerPlugin::getPlayerlist()
{
	return &playerlist;
}

list<string> * ServerPlugin::getAdminlist()
{
	return &adminlist;
}

void ServerPlugin::showAdminMenu(Player * player)
{
	player->sendMenu(adminMenu,1);
}

void ServerPlugin::showChangelevelMenu(Player * player)
{
	Menu * maplist = new Menu("menu_map",
		new MenuCallback<ServerPlugin>(this,&ServerPlugin::changelevelMenuCallback));

	try
	{
		ConfigurationFile maplistfile(CFG_FOLDER_PATH "cssmatch/maplist.txt");

		list<string> maps;
		maplistfile.getLines(maps);

		list<string>::const_iterator itMap;
		for(itMap = maps.begin(); itMap != maps.end(); itMap++)
		{
			maplist->addLine(false,*itMap);
		}
	}
	catch(const ConfigurationFileException & e)
	{
		CSSMATCH_PRINT_EXCEPTION(e);
	}

	player->sendMenu(maplist,1,I18nManager::WITHOUT_PARAMETERS,true);
}

void ServerPlugin::constructPlayerlistMenu(Menu * to)
{
	list<ClanMember *>::const_iterator itPlayer;
	for(itPlayer = playerlist.begin(); itPlayer != playerlist.end(); itPlayer++)
	{
		IPlayerInfo * pInfo = (*itPlayer)->getPlayerInfo();
		if (isValidPlayerInfo(pInfo))
			to->addLine(false,pInfo->GetName(),new UseridMenuLineData(pInfo->GetUserID()));
	}
}

void ServerPlugin::showSwapMenu(Player * player)
{
	Menu * swapMenu = new Menu("menu_player",
		new MenuCallback<ServerPlugin>(this,&ServerPlugin::swapMenuCallback));

	constructPlayerlistMenu(swapMenu);

	player->sendMenu(swapMenu,1,I18nManager::WITHOUT_PARAMETERS,true);
}

void ServerPlugin::showSpecMenu(Player * player)
{
	Menu * swapMenu = new Menu("menu_player",
		new MenuCallback<ServerPlugin>(this,&ServerPlugin::specMenuCallback));

	constructPlayerlistMenu(swapMenu);

	player->sendMenu(swapMenu,1,I18nManager::WITHOUT_PARAMETERS,true);
}

void ServerPlugin::showKickMenu(Player * player)
{
	Menu * swapMenu = new Menu("menu_player",
		new MenuCallback<ServerPlugin>(this,&ServerPlugin::kickMenuCallback));

	constructPlayerlistMenu(swapMenu);

	player->sendMenu(swapMenu,1,I18nManager::WITHOUT_PARAMETERS,true);
}

void ServerPlugin::showBanMenu(Player * player)
{
	Menu * swapMenu = new Menu("menu_player",
		new MenuCallback<ServerPlugin>(this,&ServerPlugin::banMenuCallback));

	constructPlayerlistMenu(swapMenu);

	player->sendMenu(swapMenu,1,I18nManager::WITHOUT_PARAMETERS,true);
}

void ServerPlugin::showBanTimeMenu(Player * player)
{
	player->sendMenu(bantimeMenu,1);
}

void ServerPlugin::adminMenuCallback(Player * player, int choice, MenuLine * selected)
{
	switch(choice)
	{
	case 1:
		showChangelevelMenu(player);
		break;
	case 2:
		showSwapMenu(player);
		break;
	case 3:
		showSpecMenu(player);
		break;
	case 4:
		showKickMenu(player);
		break;
	case 5:
		showBanMenu(player);
		break;	
	default:
		player->quitMenu();
	}
}

void ServerPlugin::changelevelMenuCallback(Player * player, int choice, MenuLine * selected)
{
	if (choice != 10)
	{
		string mapname = selected->text;
		if (interfaces.engine->IsMapValid(mapname.c_str()))
			queueCommand("changelevel " + mapname + "\n");
		else
		{
			RecipientFilter recipients;
			recipients.addRecipient(player);

			map<string,string> parameters;
			parameters["$map"] = mapname;

			i18n->i18nChatSay(recipients,"admin_map_not_found",parameters);
		}
	}
	player->quitMenu();
}

void ServerPlugin::swapMenuCallback(Player * player, int choice, MenuLine * selected)
{
	if (choice != 10)
	{
		PlayerIdentity * pIdentity = player->getIdentity();
		RecipientFilter recipients;
		map<string,string> parameters;

		parameters["$username"] = selected->text;
		UseridMenuLineData * useridData = static_cast<UseridMenuLineData *>(selected->data);

		ClanMember * target = NULL;
		CSSMATCH_VALID_PLAYER(PlayerHavingUserid,useridData->userid,target)
		{
			IPlayerInfo * pInfo = player->getPlayerInfo();

			target->swap();

			if (isValidPlayerInfo(pInfo))
			{
				recipients.addAllPlayers();
				parameters["$admin"] = pInfo->GetName();
				i18n->i18nChatSay(recipients,"admin_swap",parameters,pIdentity->index);
			}
		}
		else
		{
			recipients.addRecipient(player);
			i18n->i18nChatSay(recipients,"admin_is_not_connected",parameters);
		}
		showSwapMenu(player);
	}
	else
		player->quitMenu();
}

void ServerPlugin::specMenuCallback(Player * player, int choice, MenuLine * selected)
{
	if (choice != 10)
	{
		PlayerIdentity * pIdentity = player->getIdentity();
		RecipientFilter recipients;
		map<string,string> parameters;

		parameters["$username"] = selected->text;
		UseridMenuLineData * useridData = static_cast<UseridMenuLineData *>(selected->data);

		ClanMember * target = NULL;
		CSSMATCH_VALID_PLAYER(PlayerHavingUserid,useridData->userid,target)
		{
			IPlayerInfo * pInfo = player->getPlayerInfo();

			target->spec();

			if (isValidPlayerInfo(pInfo))
			{
				recipients.addAllPlayers();
				parameters["$admin"] = pInfo->GetName();
				i18n->i18nChatSay(recipients,"admin_spec",parameters,pIdentity->index);
			}
		}
		else
		{
			recipients.addRecipient(player);
			i18n->i18nChatSay(recipients,"admin_is_not_connected",parameters);
		}
		showSpecMenu(player);
	}
	else
		player->quitMenu();
}

void ServerPlugin::kickMenuCallback(Player * player, int choice, MenuLine * selected)
{
	if (choice != 10)
	{
		PlayerIdentity * pIdentity = player->getIdentity();
		RecipientFilter recipients;
		map<string,string> parameters;

		parameters["$username"] = selected->text;
		UseridMenuLineData * useridData = static_cast<UseridMenuLineData *>(selected->data);

		ClanMember * target = NULL;
		CSSMATCH_VALID_PLAYER(PlayerHavingUserid,useridData->userid,target)
		{
			PlayerIdentity * identity = player->getIdentity();
			IPlayerInfo * pInfo = player->getPlayerInfo();

			target->kick("admin_kick");

			if (isValidPlayerInfo(pInfo))
			{
				recipients.addAllPlayers();
				parameters["$admin"] = pInfo->GetName();
				i18n->i18nChatSay(recipients,"admin_kick_by",parameters,pIdentity->index);
			}
		}
		else
		{
			recipients.addRecipient(player);
			i18n->i18nChatSay(recipients,"admin_is_not_connected",parameters);
		}
		showKickMenu(player);
	}
	else
		player->quitMenu();
}

void ServerPlugin::banMenuCallback(Player * player, int choice, MenuLine * selected)
{
	if (choice != 10)
	{
		UseridMenuLineData * useridData = static_cast<UseridMenuLineData *>(selected->data);

		player->storeMenuData(new PlayerMenuLineData(selected->text,useridData->userid));
		showBanTimeMenu(player);
	}
	else
		player->quitMenu();
}

void ServerPlugin::bantimeMenuCallback(Player * player, int choice, MenuLine * selected)
{
	if (choice != 10)
	{
		PlayerIdentity * pIdentity = player->getIdentity();

		RecipientFilter recipients;
		map<string,string> parameters;

		PlayerMenuLineData * const targetData = static_cast<PlayerMenuLineData * const>(player->getMenuData());
		parameters["$username"] = targetData->name;

		ClanMember * target = NULL;
		CSSMATCH_VALID_PLAYER(PlayerHavingUserid,targetData->userid,target)
		{
			PlayerIdentity * identity = player->getIdentity();
			IPlayerInfo * adminInfo = player->getPlayerInfo();

			int time = 0;
			switch(choice)
			{
			case 1:
				time = 5;
				break;
			case 2:
				time = 60;
				break;
			}
			target->ban(time,"admin_ban");

			if (adminInfo != NULL)
			{
				recipients.addAllPlayers();
				parameters["$admin"] = adminInfo->GetName();

				if (time == 0)
					i18n->i18nChatSay(recipients,"admin_permanently_ban",parameters,pIdentity->index);
				else
					i18n->i18nChatSay(recipients,"admin_temporaly_ban",parameters,pIdentity->index);
			}
		}
		else
		{
			recipients.addRecipient(player);
			i18n->i18nChatSay(recipients,"admin_is_not_connected",parameters);
		}
		showBanMenu(player); 
		// FIXME: last banned player still appear because kickid is not yet executed here
		// (engine->ServerExecute logs a "unknown command: kickid" message)
	}
	else
		player->quitMenu();
}

MatchManager * ServerPlugin::getMatch()
{
	return match;
}

void ServerPlugin::addPluginConVar(ConVar * variable)
{
	pluginConVars[variable->GetName()] = variable;
}

ConVar * ServerPlugin::getConVar(const string & name)/* throw(ServerPluginException)*/
{
	/*map<string,ConVar *>::iterator invalidConVar = pluginConVars.end();
	map<string,ConVar *>::iterator itConVar = pluginConVars.find(name);

	if (itConVar == invalidConVar)
		throw ServerPluginException("Attempts to access to an unknown variable name");

	return itConVar->second;*/
	return pluginConVars.find(name)->second;
}

void ServerPlugin::addPluginConCommand(ConCommand * command)
{
	pluginConCommands[command->GetName()] = command;
}

const map<string,ConCommand *> * ServerPlugin::getPluginConCommands() const
{
	return &pluginConCommands;
}

void ServerPlugin::hookConCommand(const std::string & commandName, HookCallback callback)
{
	map<string,ConCommandHook *>::iterator invalidHook = hookConCommands.end();
	map<string,ConCommandHook *>::iterator itHook = hookConCommands.find(commandName);

	if (itHook == invalidHook)
	{
		char * cName = new char [commandName.size()];
		V_strcpy(cName,commandName.c_str());

		hookConCommands[commandName] = new ConCommandHook(cName,callback);
	}
	else
	{
		CSSMATCH_PRINT(commandName + " is already hooked");
	}
}

I18nManager * ServerPlugin::getI18nManager()
{
	return i18n;
}

void ServerPlugin::addTimer(BaseTimer * timer)
{
	timers.push_front(timer); 
	// push front, to allow timer invoking others timers, which could have to be immediately executed
}

void ServerPlugin::removeTimers()
{
	list<BaseTimer *>::iterator itTimer;
	for(itTimer = timers.begin(); itTimer != timers.end(); itTimer++)
	{
		delete *itTimer;
	}
	timers.clear();
}

void ServerPlugin::Unload()
{
}

void ServerPlugin::Pause()
{
}

void ServerPlugin::UnPause()
{
}

const char * ServerPlugin::GetPluginDescription()
{
	return CSSMATCH_VERSION;
}

void ServerPlugin::LevelInit(char const * pMapName)
{
	// End any match in progress
	if (match->getMatchState() != match->getInitialState())
		match->switchToInitialState();

	// Update the referee steamid list
	adminlist.clear();
	try
	{
		ConfigurationFile adminlistFile(CFG_FOLDER_PATH "cssmatch/adminlist.txt");
		adminlistFile.getLines(adminlist);
	}
	catch(const ConfigurationFileException & e)
	{
		CSSMATCH_PRINT_EXCEPTION(e);
	}

	// Delete all pending timers
	removeTimers();
}

void ServerPlugin::ServerActivate(edict_t * pEdictList, int edictCount, int clientMax)
{
}

void ServerPlugin::GameFrame(bool simulating)
{
	// Execute and remove the timers out of date
	timers.remove_if(TimerOutOfDate(interfaces.gpGlobals->curtime));
}

void ServerPlugin::LevelShutdown() // !!!!this can get called multiple times per map change
{
}

void ServerPlugin::ClientActive(edict_t * pEntity)
{
}

void ServerPlugin::ClientDisconnect(edict_t * pEntity)
{
	ClanMember * toRemove = NULL;
	CSSMATCH_VALID_PLAYER(PlayerHavingPEntity,pEntity,toRemove)
	{
		playerlist.remove(toRemove);
		delete toRemove;
	}
}

void ServerPlugin::ClientPutInServer(edict_t * pEntity, char const * playername)
{
	int index = interfaces.engine->IndexOfEdict(pEntity);
    if (isValidPlayerInfoIndex(index,interfaces.gpGlobals->maxClients))
    {
		// First remove the player if he's already in the list
		list<ClanMember *>::iterator invalidPlayer = playerlist.end();
		list<ClanMember *>::iterator itPlayer =
			find_if(playerlist.begin(),invalidPlayer,PlayerHavingIndex(index));
		if (itPlayer != invalidPlayer)
		{
			delete (*itPlayer);
			playerlist.erase(itPlayer);
		}

		// Then add the new player to the player list
        try
        {
			list<string>::iterator itSteamid = adminlist.begin();
			list<string>::iterator invalidSteamid = adminlist.end();

			bool isReferee = find(itSteamid,invalidSteamid,interfaces.engine->GetPlayerNetworkIDString(pEntity)) != invalidSteamid;
			playerlist.push_back(new ClanMember(index,isReferee));
        }
        catch(const PlayerException & e)
        {
            CSSMATCH_PRINT_EXCEPTION(e);
        }
    }
}

void ServerPlugin::ClientSettingsChanged(edict_t * pEdict)
{
}

PLUGIN_RESULT ServerPlugin::ClientConnect(bool * bAllowConnect,
										edict_t * pEntity,
										const char * pszName,
										const char * pszAddress,
										char * reject,
										int maxrejectlen)
{
	return PLUGIN_CONTINUE;
}

PLUGIN_RESULT ServerPlugin::ClientCommand(edict_t * pEntity)
{
	PLUGIN_RESULT result = PLUGIN_CONTINUE;

	try
	{
		if (isValidEntity(pEntity))
		{
			string command = interfaces.engine->Cmd_Argv(0);
			
			// Stop a player from joining another team during the match 
			// (Excepted during the kniferound, or if the player wants to join the spectators or come from the spectators)
			if (command == "jointeam")
			{
				BaseMatchState * matchState = match->getMatchState();
				if (matchState != DisabledMatchState::getInstance() &&
					matchState != KnifeRoundMatchState::getInstance())
				{
					// Check for command sanity (jointeam without argument causes a error message but swap the player)
					if (interfaces.engine->Cmd_Argc() > 1)
					{
						string arg1 = interfaces.engine->Cmd_Argv(1);

						// If the team the player wants to join is a non-spectator team
						if ((arg1 == "2") || (arg1 == "3"))
						{
							ClanMember * player = NULL;
							CSSMATCH_VALID_PLAYER(PlayerHavingPEntity,pEntity,player)
							{
								// If the team is not a spectator team
								if (player->getMyTeam() > SPEC_TEAM)
								{
									RecipientFilter recipients;
									recipients.addRecipient(player);

									i18n->i18nChatSay(recipients,"player_no_swap_in_match");
									result = PLUGIN_STOP;
								}
							}
							else
								CSSMATCH_PRINT("Unable to find the player who typed jointeam");
						}
						// Now test if the command is not invalid ("jointeam bla" will swap the player)
						else if (atoi(arg1.c_str()) == 0)
						{
							//Msg("Never trust the user input\n");
							result = PLUGIN_STOP;
						}
						// arg1 < 0 and arg1 > 3 is refused by the game
					}
					else
						result = PLUGIN_STOP;
				}
			}
			else if (command == "menuselect")
			{
				ClanMember * player = NULL;
				CSSMATCH_VALID_PLAYER(PlayerHavingPEntity,pEntity,player)
				{
					Menu * menu = player->getMenu();
					if (menu != NULL)
					{
						if (interfaces.engine->Cmd_Argc() == 2)
						{
							int choice = atoi(interfaces.engine->Cmd_Argv(1));
							if (choice != 0)
							{
								player->cexec("playgamesound UI/buttonrollover.wav\n");
								menu->doCallback(player,choice);
							}
						}
					}
				}
			}
			else if (command == "cssmatch")
			{
				ClanMember * player = NULL;
				CSSMATCH_VALID_PLAYER(PlayerHavingPEntity,pEntity,player)
				{
					if (player->isReferee())
					{
						match->showMenu(player);
					}
					else
					{
						PlayerIdentity * playerid = player->getIdentity();
						RecipientFilter recipients;
						recipients.addRecipient(player);

						i18n->i18nChatSay(recipients,"player_you_not_admin");
						log(playerid->steamid + " is not admin");

						log("Admin list:");
						list<string>::const_iterator itAdmin;
						for(itAdmin = adminlist.begin(); itAdmin != adminlist.end(); itAdmin++)
						{
							log(*itAdmin);
						}
					}
				}

				result = PLUGIN_STOP;
			}
			else if (command == "cssm_rates")
			{
				ClanMember * player = NULL;
				CSSMATCH_VALID_PLAYER(PlayerHavingPEntity,pEntity,player)
				{
					RecipientFilter recipients;
					recipients.addRecipient(player);

					list<ClanMember *>::const_iterator currentPlayer = playerlist.begin();
					list<ClanMember *>::const_iterator invalidPlayer = playerlist.end();
					while(currentPlayer != invalidPlayer)
					{
						int playerIndex = (*currentPlayer)->getIdentity()->index;

						ostringstream message;
						CSSMATCH_PRINT(interfaces.engine->GetClientConVarValue(playerIndex,"cl_updaterate"));
						CSSMATCH_PRINT(interfaces.engine->GetClientConVarValue(playerIndex,"cl_cmdrate"));
						CSSMATCH_PRINT(interfaces.engine->GetClientConVarValue(playerIndex,"cl_interpolate"));
						CSSMATCH_PRINT(interfaces.engine->GetClientConVarValue(playerIndex,"rate"));

						message << string(interfaces.engine->GetClientConVarValue(playerIndex,"name")) << ": " << std::endl
								<< "\t" << "cl_updaterate  = " << interfaces.engine->GetClientConVarValue(playerIndex,"cl_updaterate") << std::endl
								<< "\t" << "cl_cmdrate     = " << interfaces.engine->GetClientConVarValue(playerIndex,"cl_cmdrate") << std::endl
								<< "\t" << "cl_interpolate = " << interfaces.engine->GetClientConVarValue(playerIndex,"cl_interpolate") << std::endl
								<< "\t" << "rate           = " << interfaces.engine->GetClientConVarValue(playerIndex,"rate") << std::endl
								<< std::endl;

						i18n->consoleSay(recipients,message.str());
							
						currentPlayer++;
					}
				}
				else
					CSSMATCH_PRINT("Unable to find the player who typed jointeam");

				result = PLUGIN_STOP;
			}
		}
	}
	catch(const BaseException & e)
	{
		CSSMATCH_PRINT_EXCEPTION(e);
	}

	return result;
}

PLUGIN_RESULT ServerPlugin::NetworkIDValidated(const char * pszUserName, const char * pszNetworkID)
{
	return PLUGIN_CONTINUE;
}

void ServerPlugin::log(const std::string & message) const
{
	ostringstream buffer;
	buffer << CSSMATCH_NAME << ": " << message << "\n";
	interfaces.engine->LogPrint(buffer.str().c_str());
}

void ServerPlugin::queueCommand(const string & command) const
{
	interfaces.engine->ServerCommand(command.c_str());
}

void ServerPlugin::executeCommand(const std::string & command) const
{
	//interfaces.engine->InsertServerCommand(command.c_str());
	queueCommand(command);
	interfaces.engine->ServerExecute();
}

bool ServerPlugin::hltvConnected() const
{
	list<ClanMember *>::const_iterator invalidPlayer = playerlist.end();
	return find_if(playerlist.begin(),invalidPlayer,PlayerIsHltv()) != invalidPlayer;
}

int ServerPlugin::getPlayerCount(TeamCode team) const
{
	int playerCount = 0;

	if (team == INVALID_TEAM)
			playerCount = (int)playerlist.size();
	else
			playerCount = count_if(playerlist.begin(),playerlist.end(),PlayerHavingTeam(team));

	return playerCount;
}

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(	ServerPlugin, 
									IServerPluginCallbacks,
									INTERFACEVERSION_ISERVERPLUGINCALLBACKS,
									*cssmatch::ServerPlugin::getInstance());
