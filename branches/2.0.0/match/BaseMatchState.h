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

#ifndef __BASE_MATCH_STATE_H__
#define __BASE_MATCH_STATE_H__

namespace cssmatch
{
	class MatchManager;

	/** Base match state (e.g. disabled, kniferound, warmup, set, etc.) <br>
	 *	Match states should not be directly used, use them through enum MatchStateId instead
	 */
	class BaseMatchState
	{
	public:
		virtual ~BaseMatchState();

		/** Apply this match state */
		virtual void startState() = 0;

		/** End this match state <br>
		 * Remove all event listeners we used here
		 */
		virtual void endState() = 0;
	};
}
#endif // __BASE_MATCH_STATE_H__
