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

#ifndef __STATS_SET_H__
#define __STATS_SET_H__

#include "StatsSnapshot.h"

namespace cssmatch
{
	// FIXME: Duplicate with StatsRound ?

	/** Round set stats snapshot <br>
	 * Each set, the player's stats are memorized (see Player.h for explaination)
	 */
	class StatsSet : public StatsSnapshot
	{
	private:
		int setNumber;
	public:
		StatsSet(int setNumber, const PlayerStats & stats);

		int getSetNumber() const;
	};
}


#endif // __STATS_SET_H__
