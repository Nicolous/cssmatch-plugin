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

/** Protection contre l'inclusion r�cursive infinie, pardon... */
#ifndef __MENU_H__
#include "../Menu/Menu.h"
#endif

#ifndef __JOUEUR_H__
#define __JOUEUR_H__

/** Repr�sente un joueur connect� au serveur */
class Joueur
{
private:
	/** index du joueur sur le serveur */
	int index;

	// Kills/deaths
	int kills;
	int deaths;

	// Nous souhaitons �tre capable de r�tablir les scores des joueurs en cas de restart du round ou de la manche en cours.
	int killsDernierRound;
	int killsDerniereManche;
	int deathsDernierRound;
	int deathsDerniereManche;

	/** Menu utilis� <br>
	 * Le menu utilis� par le joueur sera copi� pour parer aux probl�mes d'actualisations
	 */
	Menu menu;

	/** Num�ro de la page du menu utilis� */
	int pageMenu;

	/** Un arbitre peut prendre l'ID d'un Joueur pour cible dans les options d'administration 
	 * Il n'y a pas de classe s�parant le simple joueur de l'arbitre, <br>
	 * car � tout moment un joueur peut devenir arbitre ce qui implique des boucles et des <br>
	 * tests lourds pour 1 variable de diff�rence je trouve <br>
	 */
	int useridCible;
public:
	Joueur();

	// Gestion de l'index
	void setIndex(int index);
	int getIndex() const;

	// Gestion des kills
	int getKills() const;
	int incKills();
	int decKills();
	void setKills(int kills);

	// Gestion des deaths
	int getDeaths() const;
	int incDeaths();
	int decDeaths();
	void setDeaths(int deaths);

	/** Retourne le ratio du Joueur */
	float getRatio() const;

	// Met � jour notre historique des kills/deaths � chaque round et � chaque manche
	void sauveStatsRound();
	void sauveStatsManche();

	// Restaure les kills/deaths du round ou de la manche pr�c�dente dans les variables kills et deaths
	void restoreStatsRound();
	void restoreStatsManche();

	/** Retourne un pointeur sur le menu utilis� par le joueur */
	Menu * getMenu();
	/** Nous ferons une copie des menus utilis�s par le joueur pour palier aux probl�mes de mise � jour du contenu */
	void setMenu(Menu menu);

	// Gestion de la page du menu utilis�
	int getPageMenu() const;
	void setPageMenu(int numPage);
	int incPageMenu();
	int decPageMenu();

	// Gestion de l'userid cible du joueur dans le menu d'administration
	int getUseridCible() const;
	void setUseridCible(int useridCible);
};

#endif // __JOUEUR_H__
