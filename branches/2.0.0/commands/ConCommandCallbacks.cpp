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

#include "ConCommandCallbacks.h"

#include "../match/MatchManager.h"
#include "../sourcetv/TvRecord.h"
#include "../match/BaseMatchState.h"
#include "../match/KnifeRoundMatchState.h"
#include "../match/WarmupMatchState.h"
#include "../match/HalfMatchState.h"
#include "../messages/I18nManager.h"
#include "../plugin/ServerPlugin.h"
#include "../configuration/RunnableConfigurationFile.h"
#include "../messages/Countdown.h"

using namespace cssmatch;

#include "../match/WarmupMatchState.h"
#include "../player/ClanMember.h"

#include <list>
#include <algorithm>
//#include <cctype> // tolower

using std::list;
using std::find;
using std::for_each;
using std::istringstream;
using std::getline;
using std::transform;
using std::string;
using std::map;

// Syntax: cssm_help [command name]
void cssmatch::cssm_help()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	const map<string,ConCommand *> * pluginConCommands = plugin->getPluginConCommands();
	map<string,ConCommand *>::const_iterator lastConCommand = pluginConCommands->end();

	if (interfaces->engine->Cmd_Argc() == 1)
	{
		map<string,ConCommand *>::const_iterator itConCommand = pluginConCommands->begin();
		while (itConCommand != lastConCommand)
		{
			ConCommand * command = itConCommand->second;

			const char * helpText = command->GetHelpText();
			Msg("%s\n",helpText);
			delete helpText;

			itConCommand++;
		}
	}
	else
	{
		string commandName = interfaces->engine->Cmd_Argv(1);
		map<string,ConCommand *>::const_iterator itConCommand = pluginConCommands->find(commandName);
		if (itConCommand != lastConCommand)
		{
			const char * helpText = itConCommand->second->GetHelpText();
			Msg("%s\n",helpText);
			delete helpText;
		}
		else
		{
			I18nManager * i18n = plugin->getI18nManager();

			map<string,string> parameters;
			parameters["$command"] = commandName;
			i18n->i18nMsg("error_command_not_found",parameters);
		}
	}
}

// Syntax: cssm_start [configuration file from cstrike/cfg/cssmatch/configurations [-cutround] [-warmup]]
void cssmatch::cssm_start()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	I18nManager * i18n = plugin->getI18nManager();
	MatchManager * match = plugin->getMatch();

	bool kniferound = true;
	bool warmup = true;
	string configurationFile = DEFAULT_CONFIGURATION_FILE;

	switch(interfaces->engine->Cmd_Argc())
	{
	case 4:
	case 3:
		kniferound = string(interfaces->engine->Cmd_Args()).find("-kniferound") == string::npos;
		warmup = string(interfaces->engine->Cmd_Args()).find("-warmup") == string::npos;
		// break;
	case 2:
		configurationFile = interfaces->engine->Cmd_Argv(1);
		// break;
	case 1:
		try
		{
    		RunnableConfigurationFile configuration(CFG_FOLDER_PATH MATCH_CONFIGURATIONS_PATH + configurationFile);

			// Determine the initial match state
			BaseMatchState * initialState = NULL;
			if (plugin->getConVar("cssmatch_kniferound")->GetBool() && kniferound)
			{
				initialState = KnifeRoundMatchState::getInstance();
			}
			else if ((plugin->getConVar("cssmatch_warmup_time")->GetInt() > 0) && warmup)
			{
				initialState = WarmupMatchState::getInstance();
			}
			else if (plugin->getConVar("cssmatch_sets")->GetInt() > 0)
			{
				initialState = HalfMatchState::getInstance();
			}
			else // Error case
			{
				RecipientFilter recipients;
				recipients.addAllPlayers();
				i18n->i18nChatWarning(recipients,"match_config_error");
			}
			match->start(configuration,warmup,initialState);

			RecipientFilter recipients;
			recipients.addAllPlayers();
			i18n->i18nChatSay(recipients,"match_started");
		}
		catch(const ConfigurationFileException & e)
		{
			map<string,string> parameters;
			parameters["$file"] = configurationFile;

			i18n->i18nMsg("error_file_not_found",parameters);
		}
		catch(const MatchManagerException & e)
		{
			i18n->i18nMsg("match_in_progress");
		}
		break;
	default:
		Msg("cssm_start [configuration file from cstrike/cfg/cssmatch/configurations [-cutround] [-warmup]]\n"); 
	}
}

// Syntax: cssm_stop
void cssmatch::cssm_stop()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();

	try
	{
		match->stop();

		/*RecipientFilter recipients;
		recipients.addAllPlayers();
		i18n->i18nChatSay(recipients,"match_started");*/
	}
	catch(const MatchManagerException & e)
	{
		I18nManager * i18n = plugin->getI18nManager();

		i18n->i18nMsg("match_not_in_progress");
	}
}

// Syntax: cssm_retag
void cssmatch::cssm_retag()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	I18nManager * i18n = plugin->getI18nManager();

	try
	{
		MatchLignup * lignup = match->getLignup();

		match->detectClanName(T_TEAM);
		match->detectClanName(CT_TEAM);

		map<string,string> parameters;
		parameters["$team1"] = *lignup->clan1.getName();
		parameters["$team2"] = *lignup->clan2.getName();
		i18n->i18nMsg("match_name",parameters);
	}
	catch(const MatchManagerException & e)
	{
		i18n->i18nMsg("match_not_in_progress");
	}
}

// Syntax: cssm_go
void cssmatch::cssm_go()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	I18nManager * i18n = plugin->getI18nManager();

	WarmupMatchState * warmupState = WarmupMatchState::getInstance();
	BaseMatchState * currentState = match->getMatchState();
	if (currentState == warmupState)
	{
		warmupState->endWarmup();

		RecipientFilter recipients;
		recipients.addAllPlayers();
		i18n->i18nChatSay(recipients,"admin_all_teams_say_ready");
	}
	else if (currentState != match->getInitialState())
	{
		i18n->i18nMsg("warmup_disable");
	}
	else
	{
		i18n->i18nMsg("match_not_in_progress");
	}
}

// Syntax: cssm_restartmanche
// or cssm_restarthalf
void cssmatch::cssm_restartmanche()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	I18nManager * i18n = plugin->getI18nManager();

	try
	{
		match->restartHalf();

		RecipientFilter recipients;
		recipients.addAllPlayers();
		i18n->i18nChatSay(recipients,"admin_manche_restarted");
	}
	catch(const MatchManagerException & e)
	{
		i18n->i18nMsg("match_not_in_progress");
	}
}

void cssmatch::cssm_restartround()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	I18nManager * i18n = plugin->getI18nManager();

	try
	{
		match->restartRound();

		RecipientFilter recipients;
		recipients.addAllPlayers();
		i18n->i18nChatSay(recipients,"admin_round_restarted");
	}
	catch(const MatchManagerException & e)
	{
		i18n->i18nMsg("match_not_in_progress");
	}
}

// Syntax: cssm_adminlist
void cssmatch::cssm_adminlist()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();

	list<string> * adminlist = plugin->getAdminlist();

	plugin->log("Admin list:");
	list<string>::const_iterator itSteamid;
	for(itSteamid = adminlist->begin(); itSteamid != adminlist->end(); itSteamid++)
	{
		plugin->log(*itSteamid);
	}
}

// Syntax: cssm_grant steamid
void cssmatch::cssm_grant()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	if (interfaces->engine->Cmd_Argc() > 1)
	{
		I18nManager * i18n = plugin->getI18nManager();
		list<string> * adminlist = plugin->getAdminlist();

		string steamid = interfaces->engine->Cmd_Args();

		// Remove the spaces added between each ":" by the console
		size_t iRemove;
		while((iRemove = steamid.find(' ')) != string::npos)
			steamid.replace(iRemove,1,"",0,0);

		// Remove the quotes
		while((iRemove = steamid.find('"')) != string::npos)
			steamid.replace(iRemove,1,"",0,0);

		// Remove the tab
		while((iRemove = steamid.find('\t')) != string::npos)
			steamid.replace(iRemove,1,"",0,0);

		// Notify the user
		map<string,string> parameters;
		parameters["$steamid"] = steamid;

		list<string>::iterator invalidSteamid = adminlist->end();
		if (find(adminlist->begin(),invalidSteamid,steamid) == invalidSteamid)
		{
			adminlist->push_back(steamid);
			i18n->i18nMsg("admin_new_admin",parameters);

			// Update the player rights if he's connected
			ClanMember * player = NULL;
			CSSMATCH_VALID_PLAYER(PlayerHavingSteamid,steamid,player)
			{
				player->setReferee(true);
			}
		}
		else
			i18n->i18nMsg("admin_is_already_admin",parameters);
	}
	else
		Msg("cssm_grant steamid\n");
}

// Syntax: cssm_revoke steamid
void cssmatch::cssm_revoke()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	if (interfaces->engine->Cmd_Argc() > 1)
	{
		I18nManager * i18n = plugin->getI18nManager();
		list<string> * adminlist = plugin->getAdminlist();

		string steamid = interfaces->engine->Cmd_Args();

		// Remove the spaces added between each ":" by the console
		size_t iRemove;
		while((iRemove = steamid.find(' ')) != string::npos)
			steamid.replace(iRemove,1,"",0,0);

		// Remove the quotes
		while((iRemove = steamid.find('"')) != string::npos)
			steamid.replace(iRemove,1,"",0,0);

		// Remove the tab
		while((iRemove = steamid.find('\t')) != string::npos)
			steamid.replace(iRemove,1,"",0,0);

		// Notify the user
		map<string,string> parameters;
		parameters["$steamid"] = steamid;

		list<string>::iterator invalidSteamid = adminlist->end();
		list<string>::iterator itSteamid = find(adminlist->begin(),invalidSteamid,steamid);
		if (itSteamid != invalidSteamid)
		{
			adminlist->erase(itSteamid);
			i18n->i18nMsg("admin_old_admin",parameters);

			// Update the player rights if he's connected
			ClanMember * player = NULL;
			CSSMATCH_VALID_PLAYER(PlayerHavingSteamid,steamid,player)
			{
				player->setReferee(false);
			}
		}
		else
			i18n->i18nMsg("admin_is_not_admin",parameters);
	}
	else
		Msg("cssm_revoke steamid\n");
}

void cssmatch::cssm_teamt()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	if (interfaces->engine->Cmd_Argc() > 1)
	{
		MatchManager * match = plugin->getMatch();
		I18nManager * i18n = plugin->getI18nManager();
		try
		{
			MatchClan * clan = match->getClan(T_TEAM);
			string name = interfaces->engine->Cmd_Args();

			clan->setName(name,true);
		
			map<string,string> parameters;
			parameters["$team"] = name;
			i18n->i18nMsg("admin_new_t_team_name",parameters);

		}
		catch(const MatchManagerException & e)
		{
			i18n->i18nMsg("match_not_in_progress");
		}
	}
	else
		Msg("cssm_teamt name\n");
}

void cssmatch::cssm_teamct()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	if (interfaces->engine->Cmd_Argc() > 1)
	{
		MatchManager * match = plugin->getMatch();
		I18nManager * i18n = plugin->getI18nManager();
		try
		{
			MatchClan * clan = match->getClan(CT_TEAM);
			string name = interfaces->engine->Cmd_Args();

			clan->setName(name,true);
		
			map<string,string> parameters;
			parameters["$team"] = name;
			i18n->i18nMsg("admin_new_ct_team_name",parameters);

		}
		catch(const MatchManagerException & e)
		{
			i18n->i18nMsg("match_not_in_progress");
		}	
	}
	else
		Msg("cssm_teamct name\n");
}

void cssmatch::cssm_swap()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	if (interfaces->engine->Cmd_Argc() > 1)
	{
		I18nManager * i18n = plugin->getI18nManager();

		ClanMember * target = NULL;
		CSSMATCH_VALID_PLAYER(PlayerHavingUserid,atoi(interfaces->engine->Cmd_Argv(1)),target)
		{
			if (! target->swap())
				i18n->i18nMsg("admin_spectator_player");
		}
		else
			i18n->i18nMsg("error_invalid_player");
	}
	else
		Msg("cssm_swap userid\n");
}

void cssmatch::cssm_spec()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	if (interfaces->engine->Cmd_Argc() > 1)
	{
		I18nManager * i18n = plugin->getI18nManager();

		ClanMember * target = NULL;
		CSSMATCH_VALID_PLAYER(PlayerHavingUserid,atoi(interfaces->engine->Cmd_Argv(1)),target)
		{
			if (! target->spec())
				i18n->i18nMsg("admin_spectator_player");
		}
		else
			i18n->i18nMsg("error_invalid_player");
	}
	else
		Msg("cssm_spec userid\n");
}

// ***************
// Hooks callbacks 
// ***************

bool cssmatch::say_hook(int userIndex)
{
	bool eat = false;

	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	MatchManager * match = plugin->getMatch();
	I18nManager * i18n = plugin->getI18nManager();

	istringstream commandString(interfaces->engine->Cmd_Argv(1));
	string chatCommand;
	commandString >> chatCommand;
	std::transform(chatCommand.begin(),chatCommand.end(),chatCommand.begin(),tolower);
	

	// cssmatch: open the referee menu
	if (chatCommand == "cssmatch")
	{
		eat = true;

		ClanMember * user = NULL;
		CSSMATCH_VALID_PLAYER(PlayerHavingIndex,userIndex,user)
		{
			if (user->isReferee())
			{
				user->cexec("cssmatch");
			}
			else
			{
				RecipientFilter recipients;
				recipients.addRecipient(user);
				i18n->i18nChatSay(recipients,"player_you_not_admin");
				plugin->queueCommand("cssm_adminlist\n");
			}
		}
		else
			CSSMATCH_PRINT("Unable to find the player who typed cssmatch");
	}
	// !go, ready: a clan wants to end the warmup
	else if ((chatCommand == "!go") || (chatCommand == "ready"))
	{
		BaseMatchState * currentState = match->getMatchState();
		if (currentState == WarmupMatchState::getInstance())
		{
			ClanMember * user = NULL;
			CSSMATCH_VALID_PLAYER(PlayerHavingIndex,userIndex,user)
			{
				WarmupMatchState::getInstance()->doGo(user);
			}
			else
				CSSMATCH_PRINT("Unable to find the player who typed !go/ready");		
		}
		else
		{
			RecipientFilter recipients;
			recipients.addAllPlayers();	
			if (currentState != match->getInitialState())
			{
				i18n->i18nChatSay(recipients,"warmup_disable");
			}
			else
			{
				i18n->i18nChatSay(recipients,"match_not_in_progress");
			}
		}
	}
	// !scores: Display the current/last scores
	else if ((chatCommand == "!score") || (chatCommand == "!scores"))
	{
		MatchLignup * lignup = match->getLignup();
		ClanStats * stats1 = lignup->clan1.getStats();
		ClanStats * stats2 = lignup->clan2.getStats();

		map<string,string> parameters1;
		parameters1["$team"] = *lignup->clan1.getName();
		parameters1["$score"] = toString(stats1->scoreT + stats1->scoreCT);

		map<string,string> parameters2;
		parameters2["$team"] = *lignup->clan2.getName();
		parameters2["$score"] = toString(stats2->scoreT + stats2->scoreCT);

		RecipientFilter recipients;
		recipients.addAllPlayers();

		i18n->i18nChatSay(recipients,"match_scores");
		i18n->i18nChatSay(recipients,"match_scores_team",parameters1);
		i18n->i18nChatSay(recipients,"match_scores_team",parameters2);
	}
	// !teamt: cf cssm_teamt
	else if (chatCommand == "!teamt")
	{
		eat = true;

		ClanMember * user = NULL;
		CSSMATCH_VALID_PLAYER(PlayerHavingIndex,userIndex,user)
		{
			if (user->isReferee())
			{
				RecipientFilter recipients;

				// Get the new clan name
				string newName;
				getline(commandString,newName);

				if (! newName.empty())
				{
					// Remove the space at the begin of the clan name
					string::iterator itSpace = newName.begin();
					newName.erase(itSpace,itSpace+1);

					try
					{
						MatchClan * clan = match->getClan(T_TEAM);
						clan->setName(newName,true);

						recipients.addAllPlayers();

						map<string,string> parameters;
						parameters["$team"] = newName;
						i18n->i18nChatSay(recipients,"admin_new_t_team_name",parameters);
					}
					catch(const MatchManagerException & e)
					{
						recipients.addRecipient(user);
						i18n->i18nChatSay(recipients,"match_not_in_progress");
					}
				}
				else
				{
					recipients.addRecipient(user);
					i18n->i18nChatSay(recipients,"admin_new_tag");
				}
			}
			else
			{
				RecipientFilter recipients;
				recipients.addRecipient(user);
				i18n->i18nChatSay(recipients,"player_you_not_admin");
				plugin->queueCommand("cssm_adminlist\n");
			}
		}
		else
			CSSMATCH_PRINT("Unable to find the player who typed !teamt");
	}
	// !teamct: cf cssm_teamct
	else if (chatCommand == "!teamct")
	{
		eat = true;

		ClanMember * user = NULL;
		CSSMATCH_VALID_PLAYER(PlayerHavingIndex,userIndex,user)
		{
			if (user->isReferee())
			{
				RecipientFilter recipients;

				// Get the new clan name
				string newName;
				getline(commandString,newName);

				if (! newName.empty())
				{
					// Remove the space at the begin of the clan name
					string::iterator itSpace = newName.begin();
					newName.erase(itSpace,itSpace+1);

					try
					{
						MatchClan * clan = match->getClan(CT_TEAM);
						clan->setName(newName,true);

						recipients.addAllPlayers();

						map<string,string> parameters;
						parameters["$team"] = newName;
						i18n->i18nChatSay(recipients,"admin_new_ct_team_name",parameters);
					}
					catch(const MatchManagerException & e)
					{
						recipients.addRecipient(user);
						i18n->i18nChatSay(recipients,"match_not_in_progress");
					}
				}
				else
				{
					recipients.addRecipient(user);
					i18n->i18nChatSay(recipients,"admin_new_tag");
				}
			}
			else
			{
				RecipientFilter recipients;
				recipients.addRecipient(user);
				i18n->i18nChatSay(recipients,"player_you_not_admin");
				plugin->queueCommand("cssm_adminlist\n");
			}
		}
		else
			CSSMATCH_PRINT("Unable to find the player who typed !teamct");
	}
	// !update: consult the last update changelog
	else if (chatCommand == "!update")
	{
		eat = true;

		ClanMember * user = NULL;
		CSSMATCH_VALID_PLAYER(PlayerHavingIndex,userIndex,user)
		{
			RecipientFilter recipients;
			recipients.addRecipient(user);

			string updatesite = plugin->getConVar("cssmatch_updatesite")->GetString();
			i18n->motdSay(recipients,URL,"CSSMatch changelog",updatesite + CSSMATCH_CHANGELOG_FILE);
		}
	}

	return eat;
}

/*bool cssmatch::tv_stoprecord_hook(int userIndex)
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();

	list<TvRecord *> * recordlist = match->getRecords();
	if (! recordlist->empty())
	{
		list<TvRecord *>::reference refLastRecord = recordlist->back();
		if (refLastRecord->isRecording())
			refLastRecord->stop();
	}

	return false;
}
*/
