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
#include "../Messages/Messages.h"
#include "../ConVars/ConVars.h"
#include "../Menu/Menu.h"
#include "../GestionJoueurs/GestionJoueurs.h"
#include "../Configuration/Configuration.h"

#ifndef __MATCH_H__
#include "../Match/Match.h"
#endif

#ifndef __CSSMATCH_H__
#include "../CSSMatch/CSSMatch.h"
#endif

#ifndef __GESTIONMENUS_H__
#define __GESTIONMENUS_H__

/** Singleton fournissant des fonctions d'aides à l'utilisation sécurisée des menus */
class GestionMenus
{
private:
	/* Instance unique de la classe */
	static GestionMenus * menus;

	/** Initialise le menu du développeur */
	void initialiseMenuDev();
	/** Initialise le menu de choix du CutRound */
	void initialiseMenuChoixCutRound();
	/** Initialise le menu hors match */
	void initialiseMenuOff();
	/** Initialise le menu du CutRound */
	void initialiseMenuCutRound();
	/** Initialise le menu du StratsTime */
	void initialiseMenuStratsTime();
	/** Initialise le menu du match */
	void initialiseMenuMatch();
	/** Initialise le menu de choix d'une configuration */
	void initialiseMenuConfiguration();

	/** Initialise le menu d'administration */
	void initialiseMenuAdministration();
	/** Initialise le menu de changement de map */
	void initialiseMenuChangelevel();
	/** Initialise le menu de swap */
	void initialiseMenuSwap();
	/** Initialise le menu de mise en spectateur */
	void initialiseMenuSpec();
	/** Initialise le menu de kick */
	void initialiseMenuKick();
	/** Initialise le menu de ban */
	void initialiseMenuBan();
	/** Initialise le menu de choix d'une durée pour le ban */
	void initialiseMenuBanTime();

	// Protection des constructeurs et destructeurs du singleton
	GestionMenus();
	~GestionMenus();

	/** Affiche un menu SANS les options d'administration à un joueur désigné par son index (valide!) */
	void afficheMenuSansAdministration(CodePhase phase, int indexJoueur);

	/** Affiche un menu AVEC les options d'administration à un joueur désigné par son index (valide!) */
	void afficheMenuAvecAdministration(CodePhase phase, int indexJoueur);

public:
	// Les menus
	static Menu cutround_question;
	static Menu phase_off;
	static Menu phase_cutround;
	static Menu phase_strats;
	static Menu phase_match;
	static Menu configurations;

	static Menu administration;
	static Menu admin_changelevel;
	static Menu admin_swap;
	static Menu admin_spec;
	static Menu admin_kick;
	static Menu admin_ban;
	static Menu admin_ban_time;

	/** Construit les menus SANS les options d'administration */
	void initialiseMenus();

	/** Construit les menus AVEC les options d'administration */
	void initialiseMenusAvecAdministration();

	/** Retourne l'instance unique de la classe */
	static GestionMenus * getMenus();

	/** Finalise l'affiche d'un menu */
	void afficheMenuRef(Menu & menu,int indexJoueur);

	/** Affiche le menu d'arbitrage à un joueur désigné par son index (valide!) */
	void afficheMenu(CodePhase phase, int indexJoueur);

	/** Exécute le code associé au choix d'un joueur désigné par son index dans le menu qu'il utilise
	 * @param match Instance du match nécessité par le menu
	 */
	void actionMenu(int indexJoueur, int choix);

	/** Affiche la page précédente du menu utilisé par le joueur */
	void afficheMenuPrecedent(int indexJoueur);

	/** Affiche la page précédente du menu utilisé par le joueur */
	void afficheMenuSuivant(int indexJoueur);

	/** Le joueur n'aura plus la main sur aucun menu */
	void quitteMenu(int indexJoueur);
};

#endif  // __GESTIONMENUS_H__
