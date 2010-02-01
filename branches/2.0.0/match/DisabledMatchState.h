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

#ifndef __DISABLE_MATCH_STATE_H__
#define __DISABLE_MATCH_STATE_H__

#include "BaseMatchState.h"
#include "../features/BaseSingleton.h"
#include "../messages/Menu.h"

namespace cssmatch
{
	class MatchManager;

	/** Settings of the next match */
	struct MatchSettings
	{
		/** The first state of the match */
		BaseMatchState * firstState;

		/** Does a warmup time have to be played ? */
		bool warmup;

		//MatchSettings() : firstState(NULL), warmup(true){}
	};

	/* List of the .cfg files 
	class ConfigMenu : public Menu
	{
	public:
		ConfigMenu(const std::string & menuTitle, MenuCallback menuCallback);
		//ConfigMenu(ConfigMenu & menu);

		// Menu methods
		void doCallback(Player * user, int choice);
		//void addLine(bool isI18nKeyword, const std::string & line);
		void send(	Player * recipient,
					int page,
					const std::map<std::string,std::string> & parameters = I18nManager::WITHOUT_PARAMETERS);
	};*/

	/** Base class for "no match in progress" state */
	class DisabledMatchState : public BaseMatchState, public BaseSingleton<DisabledMatchState>
	{
	private:
		/** Admin menu of this state */
		Menu * disabledMenu;

		/* New math menus */
		Menu * kniferoundQuestion;
		Menu * warmupQuestion;

		/** Settings of the next match */
		MatchSettings settings;

		friend class BaseSingleton<DisabledMatchState>;
		DisabledMatchState();
		~DisabledMatchState();
	public:
		/** Read/write accessor on the next match settings */
		MatchSettings * getMatchSettings();

		// BaseMatchState methods
		void startState();
		void endState();
		void showMenu(Player * recipient);

		// Other menus
		void showKniferoundQuestion(Player * recipient);
		void showWarmupQuestion(Player * recipient);
		void showConfigQuestion(Player * recipient);
	};
}

#endif // __DISABLE_MATCH_STATE_H__
