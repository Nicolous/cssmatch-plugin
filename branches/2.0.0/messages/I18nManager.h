/* 
 * Copyright 2009, 2010 Nicolas Maingot
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

#ifndef __I18N_MANAGER_H__
#define __I18N_MANAGER_H__

#include "UserMessagesManager.h"
#include "RecipientFilter.h"
#include "../features/CannotBeCopied.h"
#include "../timer/BaseTimer.h"

class ConVar;
class IVEngineServer;

#include <map>
#include <string>

#define TRANSLATIONS_FOLDER "cstrike/cfg/cssmatch/languages/"

namespace cssmatch
{
	class TranslationFile;

	/** Support for internationalized messages <br>
	 * Messages can have parameters, prefixed by $ (e.g. : "The attacker is $attackername") <br>
	 * These parameters have to be passed under the form of a {parameter => value} map
	 */
	class I18nManager : public CannotBeCopied, public UserMessagesManager
	{
	private:
		/** What is the default language to use ? */
		ConVar * defaultLanguage;

		/** {language name => translation set} */
		std::map<std::string,TranslationFile *> languages;
	public:
		/** Empty map for messages which have no option to parse */
		static std::map<std::string, std::string> WITHOUT_PARAMETERS;

		/**
		 * @see UserMessagesManager
		 */
		I18nManager(IVEngineServer * engine);
		~I18nManager();

		/** Set the default language to use if no translation is found for a particular keyword */
		void setDefaultLanguage(ConVar * language);

		/** Get the current default language */
		std::string getDefaultLanguage() const;

		/** Retrieve the TranslationFile instance corresponding to a language <br>
		 * Store/cache it if it's not already done 
		 * @param language The language which has to be used
		 * @return The corresponding translation file 
		 */
		TranslationFile * getTranslationFile(const std::string & language);

		/** Retrieve the translation of a message
		 * @param language The language of the translation
		 * @param keyword The identifier of the translation to retrieve
		 * @param parameters If specified, the message's parameters and their value
		 */
		std::string getTranslation(	const std::string & language,
									const std::string & keyword, 
									std::map<std::string, std::string> & parameters = WITHOUT_PARAMETERS);


		/** Send a chat message <br>
		 * \001, \003 and \004 will colour the message
		 * @param recipients Recipient list
		 * @param keyword The identifier of the translation to use
		 * @param playerIndex If specified, any part of the message after \003 will appear in the color corresponding to the player's team
		 * @param parameters If specified, the message's parameters and their value
		 * @see UserMessagesManager::chatSay
		 */
		void i18nChatSay(	RecipientFilter & recipients,
							const std::string & keyword,
							std::map<std::string, std::string> & parameters = WITHOUT_PARAMETERS,
							int playerIndex = INVALID_ENTITY_INDEX);


		/** Send a colorful chat message
		 * @param recipients Recipient list
		 * @param keyword The identifier of the translation to use
		 * @param parameters If specified, the message's parameters and their value
		 */
		void i18nChatWarning(	RecipientFilter & recipients, 
								const std::string & keyword,
								std::map<std::string, std::string> & parameters = WITHOUT_PARAMETERS);

		/** Send a popup (windowed) message to the clients
		 * @param recipients Recipient list
		 * @param keyword The identifier of the translation to use
		 * @param lifeTime Display time (in seconds)
		 * @param flags Options which the play can select
		 * @param parameters If specified, the message's parameters and their value
		 */
		void i18nPopupSay(	RecipientFilter & recipients,
							const std::string & keyword,
							int lifeTime,
							std::map<std::string, std::string> & parameters = WITHOUT_PARAMETERS,
							PopupSensitivityFlags flags = OPTION_ALL);

		/** Send a centered (windowed) popup message
		 * @param recipients Recipient list
		 * @param keyword The identifier of the translation to use
		 * @param parameters If specified, the message's parameters and their value
		 */
		void i18nHintSay(	RecipientFilter & recipients,
							const std::string & keyword,
							std::map<std::string, std::string> & parameters = WITHOUT_PARAMETERS);

		/** Send a centered message
		 * @param recipients Recipient list
		 * @param keyword The identifier of the translation to use
		 * @param parameters If specified, the message's parameters and their value
		 */
		void i18nCenterSay(	RecipientFilter & recipients,
							const std::string & keyword,
							std::map<std::string, std::string> & parameters = WITHOUT_PARAMETERS);

		/** Send a console message
		 * @param recipients Recipient list
		 * @param keyword The identifier of the translation to use
		 * @param parameters If specified, the message's parameters and their value
		 */
		void i18nConsoleSay(RecipientFilter & recipients,
							const std::string & keyword,
							std::map<std::string, std::string> & parameters = WITHOUT_PARAMETERS);

		/** Send a message to the user of the RCON command (or the server console)
		 * @param keyword The identifier of the translation to use
		 * @param parameters If specified, the message's parameters and their value
		 */
		void i18nMsg(	const std::string & keyword,
						std::map<std::string, std::string> & parameters = WITHOUT_PARAMETERS);
	};

	/** Send a delayed message in the chat area 
	 * @see BaseTimer
	 */
	class TimerI18nChatSay : public BaseTimer
	{
	private:
		/** The message manager */
		I18nManager * i18n;

		/** @see I18nManager::I18nChatSay */
		RecipientFilter recipients;

		/** @see I18nManager::I18nChatSay */
		std::string keyword;

		/** @see I18nManager::I18nChatSay */
		int playerIndex;

		/** @see I18nManager::I18nChatSay */
		std::map<std::string, std::string> parameters; // FIXME : that copies a std::map
	public:
		/**
		 * @param i18n The message manager
		 * @param executionDate When this timer will be executed
		 * @see I18nManager::I18nChatSay
		 */
		TimerI18nChatSay(	float executionDate,
							RecipientFilter & recipients,
							const std::string & keyword,
							std::map<std::string, std::string> & parameters = I18nManager::WITHOUT_PARAMETERS,
							int playerIndex = INVALID_ENTITY_INDEX);

		/** @see BaseTimer */
		void execute();
	};

	/** Send a delayed popup message
	 * @see BaseTimer
	 */
	class TimerI18nPopupSay : public BaseTimer
	{
	private:
		/** The message manager */
		I18nManager * i18n;

		/** Recipient list */
		RecipientFilter recipients;

		/** @see I18nManager::I18nChatSay */
		std::string keyword;

		/** @see I18nManager::I18nPopupSay */
		int lifeTime;

		/** @see I18nManager::I18nPopupSay */
		PopupSensitivityFlags flags;

		/** @see I18nManager::I18nChatSay */
		std::map<std::string, std::string> parameters; // FIXME : that copies a std::map
	public:
		/**
		 * @param i18n The message manager
		 * @param executionDate When this timer will be executed
		 * @see I18nManager::I18nChatSay
		 */
		TimerI18nPopupSay(	float executionDate,
							RecipientFilter & recipients,
							const std::string & keyword,
							int lifeTime,
							std::map<std::string, std::string> & parameters = I18nManager::WITHOUT_PARAMETERS,
							PopupSensitivityFlags flags = OPTION_ALL);

		/** @see BaseTimer */
		void execute();
	};
}

#endif // __I18N_MANAGER_H__
