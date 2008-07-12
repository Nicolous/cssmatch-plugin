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
 * Portions of this code are also Copyright � 1996-2005 Valve Corporation, All rights reserved
 */

#include "../API/API.h"

#ifndef __CONVARS_H__
#define __CONVARS_H__

/** Classe statique initialisant les ConVars */
class ConVars
{
public:
	/** R�f�rence sur la ConVar "sv_cheats" du jeu */
	static ConVar * sv_cheats;

	/** R�f�rence sur la ConVar "sv_alltalk" du jeu */
	static ConVar * sv_alltalk;

	/** R�f�rence sur la ConVar "hostname" du jeu */
	static ConVar * hostname;

	/** R�f�rence sur la ConVar "sv_password" du jeu */
	static ConVar * sv_password;

	/** R�f�rence sur la ConVar "tv_enable" du jeu */
	static ConVar * tv_enable;


	/** Contient le nom de la version du plugin */
	static ConVar cssmatch_version;

	//static ConVar cssmatch_debug; // supprim�e

	/** D�termine si la branche administrative du menu doit �tre affich�e */
	static ConVar cssmatch_advanced;

	// static ConVar cssmatch_language; // TODO: internationnalisation !

	
	/** D�termine si un round au couteau doit �tre jou� */
	static ConVar cssmatch_cutround;

	/** D�termine si la pose du C4 est autoris�e pendant le round au couteau */
	static ConVar cssmatch_cutround_allows_c4;

	/** D�termine la dur�e (en secondes) de transition entre le round au couteau et la suite du match */
	static ConVar cssmatch_end_cutround;


	/** D�termine le nombre de round par manche */
	static ConVar cssmatch_rounds;

	/** D�termine le nombre de manches par match */
	static ConVar cssmatch_manches;

	/** D�termine la dur�e (en secondes) de transition entre les deux manches */
	static ConVar cssmatch_end_manche;

	/** D�termine si SourceTV doit enregistrer le match */
	static ConVar cssmatch_sourcetv;

	/** D�termine le dossier de destination des enregistrements SourceTV (dossier fils du dossier "cstrike") */
	static ConVar cssmatch_sourcetv_path;

	/** D�termine la dur�e (en minutes) du Strats Time */
	static ConVar cssmatch_strats_time;

	/** D�termine le nom du serveur pendant le match <br/>
	 *
	 * Si les cha�nes "%1" et "%2" sont pr�sentes dans la valeur de la ConVar, <br/>
	 * elles seront remplac�e par les tags des teams
	 *
	 */
	static ConVar cssmatch_hostname;

	/** D�termine le mot de passe du serveur pendant le match */
	static ConVar cssmatch_password;

	/** D�termine le nom du fichier de configuration par d�faut (ex�cut� en fin de match) <br/>
	 * 
	 * Le chemin commence � partir du contenu du dossier "cstrike"
	 *
	 */
	static ConVar cssmatch_default_config;

	/** Recherche les pointeurs vers le variables de VALVE */
	static void initialise();

	/** Active ou D�sactive le alltalk suivant sa valeur */
	static void swapAlltalk();
};

#endif // __CONVARS_H__
