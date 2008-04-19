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
#include "../Joueur/Joueur.h"

#ifndef __GESTIONJOUEURS__
#define __GESTIONJOUEURS__

/** Singleton permettant une gestion simplifiée de la liste des joueurs connectés */
class GestionJoueurs
{
private:
	/** Instance unique de la classe */
	static GestionJoueurs * playerlist;

	/** Liste des joueurs connectés au serveur <br>
	 * L'index dans le vector correspond à l'index sur le serveur
	 */
	std::vector<Joueur> joueurs;

	/** Notre classe est un singleton, constructeurs et destructeurs sont donc privés */
	GestionJoueurs();
	~GestionJoueurs();

public:
	/** Retourne la liste des joueurs connectés au serveur */
	static GestionJoueurs * getPlayerList();

	/** Détermine la taille maximale de la liste */
	void setTaillePlayerList(int taille);

	/** Initialise le joueur dans la liste */
	void connexion(int indexJoueur);

	/** Retourne un pointeur sur un joueur désigné par son index */
	Joueur * getJoueur(int indexJoueur);

	/** Créer le point de restauration des scores des joueurs pour le round courant */
	void sauvStatsRound();

	/** Restaure les stats du round passé */
	void restaureStatsRound();

	/** Créer le point de restauration des scores des joueurs pour la manche en cours */
	void sauvStatsManche();

	/** Restaure les stats de la manche en cours */
	void restaureStatsManche();
};

#endif // __GESTIONJOUEURS__
