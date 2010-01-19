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

#ifndef __USER_MESSAGES_MANAGER_H__
#define __USER_MESSAGES_MANAGER_H__

#include "../common/common.h"

class IVEngineServer;

#include <string>

/** Max popup size <br>
 * Beyond this value, the popup can't be send in one message to the client, <br>
 * It has to be split in two or more popup
 */
#define POPUP_MAX_SIZE 250

namespace cssmatch
{
	class RecipientFilter;

	enum UserMessageType
	{
		MESSAGE_GEIGER = 0,
		MESSAGE_TRAIN,
		MESSAGE_HUDTEXT,

		/** Chat message */
		MESSAGE_SAYTEXT,

		/** Chat message */
		MESSAGE_SAYTEXT2,

		/** Centered message */
		MESSAGE_TEXTMSG,

		MESSAGE_HUDMSG,
		MESSAGE_RESETHUD,
		MESSAGE_GAMETITLE,
		MESSAGE_ITEMPICKUP,

		/** Popup (windowed) message */
		MESSAGE_SHOWMENU,

		/** Shake the client screen */
		MESSAGE_SHAKE,

		/** Fade the client screen */
		MESSAGE_FADE,

		MESSAGE_VGUIMENU,
		MESSAGE_CLOSECAPTION,
		MESSAGE_SENDAUDIO,
		MESSAGE_RAWAUDIO,
		MESSAGE_VOICEMASK,
		MESSAGE_REQUESTSTATE,
		MESSAGE_BARTIME,
		MESSAGE_DAMAGE,
		MESSAGE_RADIOTEXT,

		/** Centerd popup (windowed) message */
		MESSAGE_HINTTEXT,

		MESSAGE_RELOADEFFECT,
		MESSAGE_PLAYERANIMEVENT,
		MESSAGE_AMMODENIED,
		MESSAGE_UPDATERADAR,
		MESSAGE_KILLCAM
	};

	/** Popup options which can be selected */
	enum PopupSensitivityFlags
	{
		OPTION_1 = 1<<0,
		OPTION_2 = 1<<1,
		OPTION_3 = 1<<2,
		OPTION_4 = 1<<3,
		OPTION_5 = 1<<4,
		OPTION_6 = 1<<5,
		OPTION_7 = 1<<6,
		OPTION_8 = 1<<7,
		OPTION_9 = 1<<8,

		/** The client closes the popup */
		OPTION_CANCEL = 1<<9,

		/** Any option is sensitive */
		OPTION_ALL =	OPTION_1 | 
						OPTION_2 | 
						OPTION_3 | 
						OPTION_4 | 
						OPTION_5 | 
						OPTION_6 | 
						OPTION_7 | 
						OPTION_8 | 
						OPTION_9 |
						OPTION_CANCEL
	};

	/** Send different kind of message to the players */
	class UserMessagesManager
	{
	protected:
		/** Valve's IVEngineServer instance */
		IVEngineServer * engine;

		/** Tool : send a console message to a client 
		 * @param index The player's index
		 * @param message The message to send
		 */
		void consoleTell(int index, const std::string & message);
	public:
		/** 
		 * @param engine Because it will be used in a loop, the Valve's IVEngineServer instance
		 */
		UserMessagesManager(IVEngineServer * engine);

		virtual ~UserMessagesManager();

		/** Send a chat message <br>
		 * \001, \003 and \004 will colour the message
		 * @param recipients Recipient list
		 * @param message The message to send
		 * @param playerIndex If specified, any part of the message after \003 will appear in the color corresponding to the player's team
		 */
		void chatSay(	RecipientFilter & recipients,
						const std::string & message,
						int playerIndex = INVALID_ENTITY_INDEX);


		/** Send a colorful chat message
		 * @param recipients Recipient list
		 * @param message The message to send
		 */
		void chatWarning(RecipientFilter & recipients, const std::string & message);

		/** Send a (windowed) popup message to the clients
		 * @param recipients Recipient list
		 * @param message The message to send
		 * @param lifeTime Display time (in seconds)
		 * @param flags Options that the player can select
		 */
		void popupSay(	RecipientFilter & recipients,
						const std::string & message,
						int lifeTime,
						PopupSensitivityFlags flags = OPTION_ALL);

		/** Send a centered (windowed) popup message
		 * @param recipients Recipient list
		 * @param message The message to send
		 */
		void hintSay(RecipientFilter & recipients, const std::string & message);

		/** Send a centered message
		 * @param recipients Recipient list
		 * @param message The message to send
		 */
		void centerSay(RecipientFilter & recipients, const std::string & message);

		/** Send a console message
		 * @param recipients Recipient list
		 * @param message The message to send
		 */
		void consoleSay(RecipientFilter & recipients, const std::string & message);
	};
}

#endif // __USER_MESSAGES_MANAGER_H__
