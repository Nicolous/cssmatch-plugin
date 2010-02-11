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

#include "DisabledMatchState.h"

#include "igameevents.h"
#include "filesystem.h"

#include "../messages/Countdown.h"
#include "../player/Player.h"
#include "../plugin/ServerPlugin.h"
#include "../messages/Menu.h"
#include "../configuration/RunnableConfigurationFile.h"
#include "MatchManager.h"
#include "KnifeRoundMatchState.h"
#include "WarmupMatchState.h"
#include "SetMatchState.h"

#include <map>
#include <string>

using namespace cssmatch;
using std::map;
using std::string;

namespace cssmatch
{
	void disabledMenuCallback(Player * player, int choice, MenuLine * selected)
	{
		ServerPlugin * plugin = ServerPlugin::getInstance();
		DisabledMatchState * state = DisabledMatchState::getInstance();

		switch(choice)
		{
		case 1:
			plugin->queueCommand(string("sv_alltalk ") + (plugin->getConVar("sv_alltalk")->GetBool() ? "0\n" : "1\n"));
			player->cexec("cssmatch");
			break;
		case 2:
			state->showKniferoundQuestion(player);
			break;
		default:
			player->quitMenu();
		}
	}

	void disabledMenuWithAdminCallback(Player * player, int choice, MenuLine * selected)
	{
		if (choice == 1)
		{
			ServerPlugin * plugin = ServerPlugin::getInstance();
			plugin->showAdminMenu(player);
		}

		disabledMenuCallback(player,choice-1,selected);
	}

	void kniferoundQuestionCallback(Player * player, int choice, MenuLine * selected)
	{
		DisabledMatchState * state = DisabledMatchState::getInstance();

		switch(choice)
		{
		case 1:
			state->getMatchSettings()->firstState = KnifeRoundMatchState::getInstance();
			//state->showWarmupQuestion(player);
			//break;
		case 2:
			//state->getMatchSettings()->firstState = NULL;
			state->showWarmupQuestion(player);
			break;
		case 3:
			player->cexec("cssmatch");
			break;
		default:
			player->quitMenu();
		}
	}

	void warmupQuestionCallback(Player * player, int choice, MenuLine * selected)
	{
		DisabledMatchState * state = DisabledMatchState::getInstance();
		MatchSettings * settings = state->getMatchSettings();

		switch(choice)
		{
		case 1:
			//settings->warmup = true;
			if (settings->firstState == NULL)
				settings->firstState = WarmupMatchState::getInstance();
			state->showConfigQuestion(player);
			break;
		case 2:
			settings->warmup = false;
			if (settings->firstState == NULL)
				settings->firstState = SetMatchState::getInstance();
			state->showConfigQuestion(player);
			break;
		case 3:
			player->cexec("cssmatch");
			break;
		default:
			player->quitMenu();
		}
	}

	void configQuestionCallback(Player * player, int choice, MenuLine * selected)
	{
		if (choice != 10)
		{
			DisabledMatchState * state = DisabledMatchState::getInstance();
			MatchSettings * settings = state->getMatchSettings();
			ServerPlugin * plugin = ServerPlugin::getInstance();
			MatchManager * match = plugin->getMatch();
			I18nManager * i18n = plugin->getI18nManager();

			map<string,string> parameters;
			try
			{
				RunnableConfigurationFile config(CFG_FOLDER_PATH MATCH_CONFIGURATIONS_PATH + selected->text);

				match->start(config,settings->warmup,settings->firstState);

				RecipientFilter recipients;
				recipients.addAllPlayers();

				IPlayerInfo * pInfo = player->getPlayerInfo();
				if (pInfo != NULL)
				{
					parameters["$admin"] = pInfo->GetName();
					i18n->i18nChatSay(recipients,"match_started_by",parameters);
				}
				else
					i18n->i18nChatSay(recipients,"match_started");
			}
			catch(const ConfigurationFileException & e)
			{
				parameters["$file"] = selected->text;

				i18n->i18nMsg("error_file_not_found",parameters);
			}
			/*catch(const MatchManagerException & e)
			{
				// match in progress
			}*/
		}

		player->quitMenu();
	}
}

/*ConfigMenu::ConfigMenu(const std::string & menuTitle, MenuCallback menuCallback)
	: Menu(menuTitle,menuCallback)
{
}

void ConfigMenu::doCallback(Player * user, int choice)
{
	Menu::doCallback(user,choice);
	
	int page = user->getMenuPage();
	MenuLine * selected = getLine(page,choice);
	if (selected->type == NORMAL)
		delete this;
}


void ConfigMenu::send(Player * recipient, int page, const std::map<std::string,std::string> & parameters)
{
	// Displaying the first page autocreates the list
	if (page == 1)
	{
		lines.clear();

		ServerPlugin * plugin = ServerPlugin::getInstance();
		ValveInterfaces * interfaces = plugin->getInterfaces();

		FileFindHandle_t fh;
		const char * cfg = interfaces->filesystem->FindFirstEx("cfg/" MATCH_CONFIGURATIONS_PATH "/*.cfg","MOD",&fh);
		while(cfg != NULL)
		{
			string filename = cfg;
			string copy = filename;
			if (normalizeFileName(filename))
			{
				interfaces->filesystem->RenameFile(
					("cfg/" MATCH_CONFIGURATIONS_PATH "/" + copy).c_str(), ("cfg/" MATCH_CONFIGURATIONS_PATH "/" + filename).c_str());
				plugin->log(copy + " has been renamed to " + filename);
			}
			
			addLine(false,filename);

			cfg = interfaces->filesystem->FindNext(fh);
		}
	}

	Menu::send(recipient,page,parameters);
	recipient->setMenu(new ConfigMenu(*this),page);
}*/

DisabledMatchState::DisabledMatchState()
{
	settings.firstState = KnifeRoundMatchState::getInstance();

	disabledMenu = new Menu("menu_no_match",disabledMenuCallback);
	disabledMenu->addLine(true,"menu_alltalk");
	disabledMenu->addLine(true,"menu_start");

	menuWithAdmin = new Menu("menu_no_match",disabledMenuWithAdminCallback);
	menuWithAdmin->addLine(true,"menu_administration_options");
	menuWithAdmin->addLine(true,"menu_alltalk");
	menuWithAdmin->addLine(true,"menu_start");

	kniferoundQuestion = new Menu("menu_play_kniferound",kniferoundQuestionCallback);
	kniferoundQuestion->addLine(true,"menu_yes");
	kniferoundQuestion->addLine(true,"menu_no");
	kniferoundQuestion->addLine(true,"menu_back");

	warmupQuestion = new Menu("menu_play_warmup",warmupQuestionCallback);
	warmupQuestion->addLine(true,"menu_yes");
	warmupQuestion->addLine(true,"menu_no");
	warmupQuestion->addLine(true,"menu_back");
}

DisabledMatchState::~DisabledMatchState()
{
	delete disabledMenu;
	delete menuWithAdmin;
	delete kniferoundQuestion;
	delete warmupQuestion;
}

MatchSettings * DisabledMatchState::getMatchSettings()
{
	return &settings;
}

void DisabledMatchState::startState()
{
	// Stop any countdown in progress
	Countdown::getInstance()->stop();
}

void DisabledMatchState::endState()
{
}

void DisabledMatchState::showMenu(Player * recipient)
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	I18nManager * i18n = plugin->getI18nManager();
	string language = interfaces->engine->GetClientConVarValue(recipient->getIdentity()->index,"cl_language");
	bool alltalk = plugin->getConVar("sv_alltalk")->GetBool();

	map<string,string> parameters;
	parameters["$action"] = i18n->getTranslation(language,alltalk ? "menu_disable" : "menu_enable");

	if (plugin->getConVar("cssmatch_advanced")->GetBool())
		recipient->sendMenu(menuWithAdmin,1,parameters);
	else
		recipient->sendMenu(disabledMenu,1,parameters);
}

void DisabledMatchState::showKniferoundQuestion(Player * recipient)
{
	recipient->sendMenu(kniferoundQuestion,1);
}

void DisabledMatchState::showWarmupQuestion(Player * recipient)
{
	recipient->sendMenu(warmupQuestion,1);
}

void DisabledMatchState::showConfigQuestion(Player * recipient)
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	Menu * configQuestion = new Menu("menu_config",configQuestionCallback);

	// Search for all .cfg file into cfg/cssmatch/configurations
	FileFindHandle_t fh;
	const char * cfg = interfaces->filesystem->FindFirstEx("cfg/" MATCH_CONFIGURATIONS_PATH "/*.cfg","MOD",&fh);
	while(cfg != NULL)
	{
		string filename = cfg;
		string copy = filename;
		if (normalizeFileName(filename))
		{
			interfaces->filesystem->RenameFile(
				("cfg/" MATCH_CONFIGURATIONS_PATH "/" + copy).c_str(), ("cfg/" MATCH_CONFIGURATIONS_PATH "/" + filename).c_str());
			plugin->log(copy + " has been renamed to " + filename);
		}
		
		configQuestion->addLine(false,filename);

		cfg = interfaces->filesystem->FindNext(fh);
	}
	
	recipient->sendMenu(configQuestion,1,I18nManager::WITHOUT_PARAMETERS,true);
}
