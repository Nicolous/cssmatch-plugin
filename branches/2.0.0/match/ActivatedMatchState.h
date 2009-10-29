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

#ifndef __ACTIVATED_MATCH_STATE_H__
#define __ACTIVATED_MATCH_STATE_H__

#include "BaseMatchState.h" // BaseMatchState
#include "../features/BaseSingleton.h"
#include "../plugin/EventListener.h"

class IGameEventManager2;
class IGameEvent;

namespace cssmatch
{
	class MatchManager;

	/** Base class for "match in progress" state <br>
	 * It shouldn't be necessary to directly instance this class, only its children
	 */
	class ActivatedMatchState : public BaseMatchState
	{
	private: // private => all inherited classes will have their proper listener
		EventListener<ActivatedMatchState> * listener;
	public:
		ActivatedMatchState();
		~ActivatedMatchState();

		// BaseMatchState methods
		//	Please override these methods in the derivated classes
		virtual void startState();
		virtual void endState();
		//	Please call these methods in the derivated classes
		//		They define a common behavior for all derivated states

		// Game event callbacks
		void player_disconnect(IGameEvent * event);
		void player_team(IGameEvent * event);
		void player_changename(IGameEvent * event);
	};
}

#endif // __ACTIVATED_MATCH_STATE_H__
