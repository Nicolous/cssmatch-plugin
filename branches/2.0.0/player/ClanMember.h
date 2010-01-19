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

#ifndef __TEAM_MEMBER_H__
#define __TEAM_MEMBER_H__

#include "Player.h"

class IVEngineServer;
class IPlayerInfoManager;

#include <list>

namespace cssmatch
{
	/** Player's stats info */
	struct PlayerStats
	{
		/** Player's score */
		int kills;

		/** Player's deaths */
		int deaths;

		PlayerStats() : kills(0), deaths(0){}

	};

	/** CSSMatch player */
	class ClanMember : public Player
	{
	protected:
		/** Stats of this player at the previous round (used in case of restart) */
		PlayerStats lastRoundStats;

		/** Stats of this player at the previous round set (used in case of restart) */
		PlayerStats lastSetStats;

		/** Current stats of this player */
		PlayerStats currentStats;

		/** Is this player a referee ? */
		bool referee;

		// Functors
		friend struct ResetStats;
	public:
		ClanMember(int index, bool referee = false);

		PlayerStats * getLastRoundStats();
		PlayerStats * getLastSetStats();
		PlayerStats * getCurrentStats();

		bool isReferee() const;
		void setReferee(bool isReferee);
	};

	/** Functor to quickly reset the stats of a player */
	struct ResetStats
	{
		void operator()(ClanMember * member)
		{
			member->lastRoundStats = PlayerStats();
			member->lastSetStats = PlayerStats();
			member->currentStats = PlayerStats();
		}
	};
}

#endif // __TEAM_MEMBER_H__
