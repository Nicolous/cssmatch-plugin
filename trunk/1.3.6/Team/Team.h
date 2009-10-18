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

#include "../Api/Api.h"
#include "../Joueur/Joueur.h"

/** Une team, plus sous forme d'agr�gat qu'autre chose... (pas de liste de joueurs) */
class Team
{
private:
	/** Tag de la team */
	std::string tag;

	/* Lignup de la team */
	//std::vector<Joueur> lignup;


	/** Score de la team */
	int score;

	/** Score de la team en terroriste */
	int scoreT;

	/** Score de la team en anti-terroriste */
	int scoreCT;


	/** Passe � vrai lorsque la team a tap�e !go lors du StratsTime */
	bool goStrats;

	/** Combien de joueurs sont dans la team ? */
	int nbrMembres;

	/** Fonction appel�e si aucun joueur n'est pr�sent sur le serveur <br>
	 * Note : attribue un tag refl�tant cette situation
	 */
	void aucunTag();

	/** Fonction appel�e s'il n'y a qu'un seul joueur dans le camp 
	 *
	 * @param codeTeam Le code de la team concerc�e par la recherche de tag
	 * @see L'�num�ration CodeTeam
	 *
	 */
	void aucuneTeam(CodeTeam codeTeam);


	/** Fonction appel�e s'il y a au moins 2 joueurs dans le camp 
	 *
	 * @param codeTeam Le code de la team concerc�e par la recherche de tag
	 * @see L'�num�ration CodeTeam
	 *
	 */
	void determineTag(CodeTeam codeTeam);


	/** Fonction localisant un �ventuel tag dans 2 pseudos, � partir d'un caract�re commun entre 2 pseudos <br>
	 *
	 * @param i index du premier caract�re commun dans le pseudo pseudoJ1
	 * @param j index du premier caract�re commun dans le pseudo pseudoJ1
	 * @param pseudoJ1 Pseudo du joueur 1
	 * @param pseudoJ2 Pseudo du joueur 2
	 * @return true si un tag d'au moins 3 caract�res a pu �tre trouv�
	 *
	 */
	bool confirmeTag(const std::string & pseudoJ1, int i, const std::string & pseudoJ2, int j);
	// Nom alternatif : confirmePointCommun...

public:
	Team();

	/* Gestion de la lignup */
	//std::vector<Joueur> * getLignup();

	/** Accesseur sur le score total de la team 
	 *
	 * @return Le score
	 *
	 */
	int getScore() const;

	/** Incr�mente le score total de la team 
	 *
	 * @return Le nouveau score de la team
	 *
	 */
	int incScore();

	/** Accesseur sur le score total de la team
	 *
	 * @param score Le nouveau score de la team
	 *
	 */
	void setScore(int score);


	/** Accesseur sur le score effectu� par la team en terroriste
	 *
	 * @return Le score
	 *
	 */
	int getScoreT() const;

	/** Incr�mente le score effectu� par la team en terroriste
	 *
	 * @return Le nouveau score de la team
	 *
	 */
	int incScoreT();

	/** Accesseur sur le score effectu� par la team en terroriste
	 *
	 * @param scoreT Le nouveau score de la team
	 *
	 */
	void setScoreT(int scoreT);

	
	/** Accesseur sur le score effectu� par la team en anti-terroriste
	 *
	 * @return Le score
	 *
	 */
	int getScoreCT() const;

	/** Incr�mente le score effectu� par la team en anti-terroriste
	 *
	 * @return Le nouveau score de la team
	 *
	 */
	int incScoreCT();

	/** Accesseur sur le score effectu� par la team en anti-terroriste
	 *
	 * @param scoreCT Le nouveau score de la team
	 *
	 */
	void setScoreCT(int scoreCT);

	
	/** Accesseur sur le tag de la team
	 *
	 * @return Le tag de la team
	 *
	 */
	const std::string & getTag() const;

	/** Accesseur sur le tag de la team
	 *
	 * @param tag Le nouveau tag de la team
	 *
	 */
	void setTag(const std::string & tag);

	
	/** D�termine si la team est pr�te � passer les strats
	 *
	 * @param go true si la team est pr�te
	 *
	 */
	void setGoStrats(bool go);

	
	/** Permet de savoir si la team est pr�te � passer les strats
	 *
	 * @return true si la team est pr�te
	 *
	 */
	bool getGoStrats() const;

	
	/** Accesseur sur le nombre de joueurs appartenant � la team
	 *
	 * @return Le nombre de joueur
	 *
	 */
	int getNbrMembres() const;

	/** Incr�mente le nombre de joueurs appartenant � la team
	 *
	 * @return Le nouveau nombre de joueur
	 *
	 */
	int incNbrMembres();

	/** D�cr�mente le nombre de joueurs appartenant � la team
	 *
	 * @return Le nouveau nombre de joueur
	 *
	 */
	int decNbrMembres();


	/** Recherche le tag de la team d�sign�e par son codeTeam
	 *
	 * @param codeTeam Le code de la team concern�e
	 * @return Le tag trouv�
	 * @see L'�num�ration CodeTeam
	 *
	 */
	const std::string & chercheTag(CodeTeam codeTeam);
};

#endif // __TEAM_H__
