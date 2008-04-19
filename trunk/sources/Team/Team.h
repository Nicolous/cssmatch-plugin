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

#ifndef __TEAM_H__
#define __TEAM_H__

#include "../API/API.h"

/** Repr�sente une team, plus sous forme d'agr�gat qu'autre chose... (pas de liste de joueurs) */
class Team
{
private:
	/** Tag de la team */
	std::string tag;
	/** Score de la team */
	int score;
	/** Score de la team en terroriste */
	int scoreT;
	/** Score de la team en anti-terroriste */
	int scoreCT;

	/** Passe � vrai lorsque la team a tap�e !go lors du StratsTime */
	bool goStrats;

	/** Fonction appel�e si aucun joueur n'est pr�sent sur le serveur (attribution d'un tag refl�tant cette situation) */
	void aucunTag();

	/** Fonction appel�e s'il n'y a qu'un seul joueur dans le camp */
	void aucuneTeam(CodeTeam codeTeam);

	/** Fonction appel�e s'il y a au moins 2 joueurs dans le camp */
	void determineTag(CodeTeam codeTeam);

	/** Fonction localisant un �ventuel tag dans 2 pseudos, � partir d'un caract�re commun entre 2 pseudos <br>
	 * Retourne vrai si un tag d'au moins 3 caract�res a pu �tre trouv�
	 *
	 * @param i index du premier caract�re commun dans le pseudo pseudoJ1
	 * @param j index du premier caract�re commun dans le pseudo pseudoJ1
	 * @param pseudoJ1 Pseudo du joueur 1
	 * @param pseudoJ2 Pseudo du joueur 2
	 *
	 */
	bool confirmeTag(const std::string & pseudoJ1, int i, const std::string & pseudoJ2, int j);
	// Nom alternatif : confirmePointCommun...

public:
	Team();

	// Gestion du score
	int getScore();
	int incScore();
	void setScore(int score);

	// Gestion du score en terro
	int getScoreT();
	int incScoreT();
	void setScoreT(int scoreT);

	// Gestion du score en CT
	int getScoreCT();
	int incScoreCT();
	void setScoreCT(int scoreCT);

	// Gestion du tag
	const std::string & getTag() const;
	void setTag(const std::string & tag);

	// Gestion du !go en StratsTime
	void setGoStrats(bool go);
	bool getGoStrats() const;

	// Recherche le tag de la team � partir d'un codeTeam de joueurs � rechercher
	// Ici codeTeam d�signe les camps du jeu : un, spec, t et ct...
	// Retourne le tag trouv�
	const std::string & chercheTag(CodeTeam codeTeam);
};

#endif // __TEAM_H__
