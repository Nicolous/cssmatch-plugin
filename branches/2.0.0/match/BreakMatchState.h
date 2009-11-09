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

#ifndef __BREAK_MATCH_STATE_H__
#define __BREAK_MATCH_STATE_H__

#include "BaseMatchState.h"
#include "MatchManager.h"
#include "../features/BaseSingleton.h"
#include "../timer/BaseTimer.h"

class IGameEventManager2;

namespace cssmatch
{
	/** Time break, between two main match states 
	 * Ends with a timeout
	 */
	class BreakMatchState : public BaseMatchState, private BaseSingleton<BreakMatchState>
	{
	private:
		/** The time break duration (in secs) before lauch the next match state */
		int duration;

		/** The state to lauch once the break ended */
		BaseMatchState * nextState;

		friend class BaseSingleton<BreakMatchState>;
		BreakMatchState();
	public:
		/** Do a time break
		 * @param duration The break duration (in secs)
		 * @param nextState The state the lauch once the break ends
		 */
		static void doBreak(int duration, BaseMatchState * nextState);

		// BaseMatchState methods
		void startState();
		void endState();
	};

	namespace
	{
		/** Break timer used for the timeout */
		class BreakMatchTimer : public BaseTimer
		{
		private:
			/** The state to lauch once the break is ended */
			BaseMatchState * nextState;
		public:
			/** 
			 * @param match The state manager required to lauch the new state
			 * @param nextState The state the lauch once the break ended
			 */
			BreakMatchTimer(float date, BaseMatchState * nextState);

			// BaseTimer method
			void execute();
		};
	}
}

#endif // __BREAK_MATCH_STATE_H__
