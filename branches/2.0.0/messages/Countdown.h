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

#ifndef __COUNTDOWN_H__
#define __COUNTDOWN_H__

#include "../features/BaseSingleton.h"
#include "../plugin/BaseTimer.h"

namespace cssmatch
{
	/** A countdown displayed to each player */
	class Countdown : public BaseSingleton<Countdown>
	{
	private:
		/** Countdown step */
		class CountdownTick : public BaseTimer
		{
		private:
			/** Current count */
			int left;
		public:
			/** 
			 * @param executionDate see BaseTimer
			 * @param nextCount The next count to display
			 */
			CountdownTick(float executionDate, int nextCount);

			/**
			 * @see BaseTimer
			 */
			virtual void execute();
		};

		/** Seconds left before the end of the countdown */
		int left;

		/** Decrement the time left 
		 * @return The new time left
		 */
		int decTimeLeft();

		friend class BaseSingleton<Countdown>;
		Countdown();
		~Countdown();
	public:
		/** Start the countdown
		 * @param seconds Seconds left until the end of the countdown
		 */
		void fire(int seconds);

		/** Stop the countdown */
		void stop();
	};
}

#endif // __COUNT_DOWN_H__
