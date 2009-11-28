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

#ifndef __CON_COMMAND_CALLBACKS_H__
#define __CON_COMMAND_CALLBACKS_H__

class IVEngineServer;

namespace cssmatch
{
	/** Help for commands use */
	void cssm_help();

	/** Starts a match */
	void cssm_start();

	/** Stops a match */
	void cssm_stop();

	/** Redetects the clan names */
	void cssm_retag();

	/** Declares a clan ready to end the warmup */
	void cssm_go();

	/** Restarts the current round set */
	void cssm_restartmanche();

	/** Restarts the current round */
	void cssm_restartround();

	/** Prints the referee steamid list */
	void cssm_adminlist();

	/** Makes a player a referee by steamid (until the next map change) */
	void cssm_grant();

	/** Removes a referee by steamid (until the next map change) */
	void cssm_revoke();

	/** Changes the name of the clan which plays in the terrorists team */
	void cssm_teamt();

	/** Changes the name of the clan which plays in the anti-terrorists team */
	void cssm_teamct();

	/** Swap a player by userid */
	void cssm_swap();

	/** Put a player to the spectator team, by userid */
	void cssm_spec();

	/** !go, !score, !teamt, etc. */
	bool say_hook(int userIndex, IVEngineServer * engine);

	/** Want to stop the cssmatch's record ? */
	bool tv_stoprecord_hook(int userIndex, IVEngineServer * engine);
}

#endif // __CON_COMMAND_CALLBACKS_H__
