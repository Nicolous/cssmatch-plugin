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

#ifndef __TEAM_MEMBER_H__
#define __TEAM_MEMBER_H__

#include "Player.h"
#include "StatsRound.h"
#include "StatsSet.h"

class IVEngineServer;
class IPlayerInfoManager;

#include <list>

namespace cssmatch
{
	/** CSSMatch player */
	class ClanMember : public Player
	{
	protected:
		/** Round stats list of this player <br>
		 * TO CONFIRM: According to an user suggestion, 
		 *	the plugin could be able to return one (or more?) rounds backward
		 */
		std::list<StatsRound> roundStats;

		/** Set stats list of this player 
		 * TO CONFIRM: The plugin could be able to replay a round set
		 */
		std::list<StatsSet> setStats;

		/** Is this player a referee ? */
		bool referee;
	public:
		ClanMember(int index, bool referee = false);

		std::list<StatsRound> * getStatsRound();
		std::list<StatsSet> * getStatsSet();

		bool isReferee() const;
	};
}

#endif // __TEAM_MEMBER_H__
