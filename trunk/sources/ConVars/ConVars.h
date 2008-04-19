/* 
 * Copyright 2007, 2008 Nicolas Maingot
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

#include "../API/API.h"

#ifndef __CONVARS_H__
#define __CONVARS_H__

/** Classe statique initialisant les ConVars */
class ConVars
{
public:
	// ConVars du jeu
	static ConVar * sv_cheats;
	static ConVar * sv_alltalk;
	static ConVar * hostname;
	static ConVar * sv_password;
	static ConVar * tv_enable;

	// ConVars de gestion du plugin
	static ConVar cssmatch_version;
	static ConVar cssmatch_debug;
	static ConVar cssmatch_advanced;

	// ConVars propres au CutRound
	static ConVar cssmatch_cutround;
	static ConVar cssmatch_cutround_allows_c4;
	static ConVar cssmatch_end_cutround;

	// ConVars propres aux Manche
	static ConVar cssmatch_rounds;
	static ConVar cssmatch_end_manche;
	static ConVar cssmatch_sourcetv;
	static ConVar cssmatch_sourcetv_path;

	// ConVars propres aux StratsTime
	static ConVar cssmatch_strats_time;

	// ConVars propres à la configuration du serveur pendant et après le match
	static ConVar cssmatch_hostname;
	static ConVar cssmatch_password;
	static ConVar cssmatch_default_config;

	/** Recherche les pointeurs vers le variables de VALVE */
	static void initialise();

	/** Active ou Désactive le alltalk suivant sa valeur */
	static void swapAlltalk();
};

#endif // __CONVARS_H__
