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

#include "ActivatedMatchState.h"

#ifndef __SET_MATCH_STATE_H__
#define __SET_MATCH_STATE_H__

class IGameEventManager2;
class IGameEvent;

namespace cssmatch
{
	class MatchManager;

	/** Match [round] Set in progress <br>
	 * Ends when the number of scheduled rounds is reached
	 */
	class SetMatchState : public ActivatedMatchState, public BaseSingleton<SetMatchState>
	{
	private:
		EventListener<SetMatchState> * listener;

		friend class BaseSingleton<SetMatchState>;
		SetMatchState();
		~SetMatchState();
	public:
		/** End the current [round] set <br>
		 * Here is the code which musn't be executed if the match is interupted
		 */
		void endSet();

		// BaseMatchState methods
		virtual void startState();
		virtual void endState();

		// Game event callbacks
		void player_death(IGameEvent * event);
		void round_start(IGameEvent * event);
		void round_end(IGameEvent * event);

	};
}

#endif // __SET_MATCH_STATE_H__
