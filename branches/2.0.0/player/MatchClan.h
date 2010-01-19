/* 
 * Copyright 2008-2010 Nicolas Maingot
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

#ifndef __MATCH_CLAN_H__
#define __MATCH_CLAN_H__

#include "Player.h"

#include <string>
#include <list>

namespace cssmatch
{
	class ClanMember;

	/** Clan stats */
	struct ClanStats
	{
		/** Clan's score in Terrorist */
		int scoreT;

		/** Clan's score in Counter-Terrorist */
		int scoreCT;

		ClanStats() : scoreT(0), scoreCT(0){}
	};

	/** A CSSMatch clan */
	class MatchClan
	{
	protected:
		/** Clan's name */
		std::string name;

		/** Clan's stats in the previous half */
		ClanStats lastSetStats;

		/** Clan's stats */
		ClanStats stats;

		/** Is the clan ready to end the warmup time ? */
		bool ready;

		/** Determine if an automatically detected clan name can be considered as valid */
		static bool isValidClanName(const std::string & newName)
		{
			return newName.size() >= 3;
		}
	public:
		MatchClan();

		/** Get the clan name */
		const std::string * getName() const;

		/** Set the clan name */
		void setName(const std::string & newName);

		/** Fills the list with the players of this clan */
		void getMembers(std::list<ClanMember *> * members);

		/** Get the clan's stats of the previous half */
		ClanStats * getLastSetStats();

		/** Get clan's stats */
		ClanStats * getStats();

		/** Try to automatically detect the clan's name */
		void detectClanName();

		/** Is the clan ready to end the warmup time ? */
		bool isReady() const;

		/** Set if this clan ready to end the warmup time */
		void setReady(bool ready);
	};
}

#endif // __MATCH_CLAN_H__
