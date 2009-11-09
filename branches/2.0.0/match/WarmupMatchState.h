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

#ifndef __WARMUP_MATCH_STATE_H__
#define __WARMUP_MATCH_STATE_H__

class IGameEventManager2;
class IGameEvent;

#include "BaseMatchState.h"
#include "../features/BaseSingleton.h"
#include "../timer/BaseTimer.h"
#include "../plugin/EventListener.h"

class IVEngineServer;

namespace cssmatch
{
	class MatchManager;
	class WarmupTimer;
	class Player;

	/** Warmup time in progress <br>
	 * Ends with a timeout, or if at least one player of each side types "ready"
	 */
	class WarmupMatchState : public BaseMatchState, public BaseSingleton<WarmupMatchState>
	{
	private:
		EventListener<WarmupMatchState> * listener;

		friend class BaseSingleton<WarmupMatchState>;
		WarmupMatchState();
		~WarmupMatchState();
	protected:
		/** "end of warmup" timer */
		WarmupTimer * timer;
	public:
		/** End the warmup time <br>
		 * Here is the code which musn't be executed if the match is interupted
		 */
		void endWarmup();

		/** A player typed !go */
		void doGo(Player * player);

		// BaseMatchState methods
		void startState();
		void endState();

		// Game event callbacks
		void player_spawn(IGameEvent * event);
		void round_start(IGameEvent * event);
		void bomb_beginplant(IGameEvent * event);
	};

	/** "End of warmup" timer */
	class WarmupTimer : public BaseTimer
	{
	private:
		/** Warmup state instance */
		WarmupMatchState * warmupState;
	public:
		/** 
		 * @param warmupState The instance of the warmup state
		 */
		WarmupTimer(float date, WarmupMatchState * warmupState);
		
		// BaseTimer method
		void execute();
	};
}

#endif // __WARMUP_MATCH_STATE_H__
