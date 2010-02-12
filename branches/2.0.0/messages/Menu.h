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

#ifndef __MENU_H__
#define __MENU_H__

#include "../exceptions/BaseException.h"
#include "I18nManager.h"

#include <string>
#include <map>
#include <vector>

namespace cssmatch
{
	class I18nManager;
	class Player;

	class MenuException : public BaseException
	{
	public:
		MenuException(const std::string & message) : BaseException(message){}
	};

	/** What kind of line ? */
	enum MenuLineType
	{
		NORMAL,
		/** This line is the "back" option */
		BACK,
		/** This line is the "next" option */
		NEXT
	};

	/** Menu line */
	struct MenuLine
	{
		/** The menu line type */
		MenuLineType type;

		/** Is the menu line a keyword from the translation files ? */
		bool i18n;

		/** The text content of this line */
		std::string text;

		/** Data carried by the line */
		int data; // FIXME: Make me generic

		MenuLine(	MenuLineType lineType,
					bool isI18n,
					const std::string & content,
					int hiddenData = CSSMATCH_INVALID_USERID)
			: type(lineType), i18n(isI18n), text(content), data(hiddenData) {};
	};

	/** Menu callback 
	 * @param user The player who uses the menu
	 * @param choice The option (1...10) selected by the player
	 * @param selected The line selected by the player (may be an empty line if the player quit the menu)
	 */
	typedef void (*MenuCallback)(Player * user, int choice, MenuLine * selected);

	/** A popup based menu */
	class Menu
	{
	protected:
		/* Parent menu (can be NULL) */
		//Menu * parent;

		/** Title of this menu */
		std::string title;

		/** Menu callback */
		MenuCallback callback;

		/** Menu lines */
		std::vector<MenuLine *> lines;
	public:
		/** 
		 * @param menuTitle The menu title
		 * @param menuCallback The function to call each time the menu is used
		 */
		Menu(/*Menu * parentMenu, */const std::string & menuTitle, MenuCallback menuCallback);
		~Menu();

		/** Call the menu callback with the provided info 
		 * @param user The player who used the menu
		 * @param choice The option selected (1...10) by the player
		 */
		void doCallback(Player * user, int choice);

		/** Add a line to the menu 
		 * @param isI18nKeyword Is the line a keyword from the translation files ?
		 * @param line The line to add
		 * @param hiddenData The hidden data shipped in this line
		 */
		void addLine(bool isI18nKeyword, const std::string & line, int hiddenData = CSSMATCH_INVALID_USERID);

		/** Returns a pointer to a line 
		 * @param page The page where the line is
		 * @param choice The index of the line on the page
		 * @throws MenuException if the index is invalid
		 */
		MenuLine * getLine(int page, int choice) throw(MenuException);

		/** Display the menu to a player <br>
		 * IMPORTANT: Use Player::sendMenu instead, otherwise the player will not be able to select anything
		 * @param recipient The player
		 * @param page The page number to send to the player
		 * @param parameters I18n parameters if needed
		 */
		void send(	Player * recipient,
					int page,
					const std::map<std::string,std::string> & parameters = I18nManager::WITHOUT_PARAMETERS);
	};
}

#endif // __MENU_H__
