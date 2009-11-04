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

#ifndef __MATCH_MANAGER_H__
#define __MATCH_MANAGER_H__

#include "igameevents.h"

#include "../player/MatchClan.h" // MatchClan [, ClanMember]
#include "../common/common.h"
#include "../exceptions/BaseException.h"
#include "../timer/BaseTimer.h"
#include "../plugin/EventListener.h"

#include <string>

namespace cssmatch
{
	class BaseMatchState;
	class RunnableConfigurationFile;

	class MatchManagerException : public BaseException
	{
	public:
		MatchManagerException(const std::string & message) : BaseException(message){};
	};

	/** A match lign-up */
	struct MatchLignup
	{
		/** First clan */
		MatchClan clan1;

		/** Second clan */
		MatchClan clan2;

		MatchLignup() : clan1(T_TEAM), clan2(CT_TEAM){}
	};

	/** Information related to a match */
	struct MatchInfo
	{
		/** Set number */
		int setNumber;

		/** Round number */
		int roundNumber;

		/** Start date */
		tm * startTime;

		/** Kniferound winner */
		MatchClan * kniferoundWinner;

		/** Did a warmup was asked by the laucher ? */
		bool warmup;

		MatchInfo()
			: setNumber(1), roundNumber(1), startTime(getLocalTime()), kniferoundWinner(NULL), warmup(false){}
	};

	/** A match manager <br>
	 * Each match can be decomposed in somes states : <br>
	 * - a knife round <br>
	 * - a warmup time <br>
	 * - one or more sets of n rounds <br>
	 * The states plus this class implement a state pattern (This class is the context)
	 */
	class MatchManager
	{
	private:
		/** Event listener */
		EventListener<MatchManager> * listener;
	protected:
		/** Current match state (e.g. kniferound, warmup, etc.) */
		BaseMatchState * state;

		/** Access to the clans */
		MatchLignup lignup;

		/** Access to some information about the match */
		MatchInfo infos;
	public:
		MatchManager();

		/** Note : stop any current countdown here */
		~MatchManager();

		/** Get the match lignup */
		MatchLignup * getLignup();

		/** Get some informations about the match */
		MatchInfo * getInfos();

		/** Get a clan by team, depending to the current set 
		 * @param code The clan's team code 
		 */
		MatchClan * getClan(TeamCode code) throw(MatchManagerException);

		// Game event callbacks
		void player_disconnect(IGameEvent * event);
		void player_team(IGameEvent * event);
		void player_changename(IGameEvent * event);

		/** Redetect a clan name then announce it
		 * @param code The clan's team code
		 */
		void detectClanName(TeamCode code);

		/** Update "hostname" according to the clan names */
		void updateHostname();

		/** Set a new match state <br>
		 * Call the endState method of the previous state, then the startState of the new state
		 * @param newState The new match state
		 */
		void setMatchState(BaseMatchState * newState);

		/** Start a new math
		 * @param config The configuration of the match
		 * @param kniferound If a kniferound must be done
		 * @param warmup If a warmup must be done
		 * @param umpire The player who starts the match
		 */
		void start(RunnableConfigurationFile & config, bool kniferound, bool warmup, ClanMember * umpire = NULL);

		/** Stop a running match (TODO: this is a stub) */
		void stop();
	};


	/** Timer used to redetect the clan names */
	class ClanNameDetectionTimer : public BaseTimer
	{
	private:
		/* Clan's team */
		TeamCode team;
	public:
		ClanNameDetectionTimer(float date, TeamCode teamCode);

		/** @see BaseTimer */
		void execute();
	};
}

#endif // __MATCH_MANAGER_H__
