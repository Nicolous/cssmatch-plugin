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

#include "../Rapport.h"

#ifndef __RAPPORTXML_H__
#define __RAPPORTXML_H__

/** Un rapport de match sauvegard� dans le dossier cssmatch/reports au format .xml */
class RapportXml : public Rapport
{
private:
	/** Nettoie une cha�ne XML de caract�res interdits dans les contenus de balises (ex : <) <br>
	 * Note : les caract�res ind�sirables sont remplac�s par leur �quivalent entit� HTML
	 *
	 * @param chaine La cha�ne XML
	 * @return La cha�ne nettoy�e des caract�res ind�sirables
	 *
	 */
	std::string valideXml(const std::string & chaine) const;

	/** Surcharge de la m�thode qui �crit l'ent�te du fichier (entetes xml et racine)
	 *
	 * @param fichier La r�f�rence sur le flux de sortie
	 *
	 */
	void ecritEntete(std::ofstream & fichier) const;

	/** Surcharge de la m�thode qui �crit le corps du fichier
	 *
	 * @param fichier La r�f�rence sur le flux de sortie
	 *
	 */
	void ecritCorps(std::ofstream & fichier) const;

	/** Surcharge de la m�thode qui �crit le pied du fichier (fermeture de la racine)
	 *
	 * @param fichier La r�f�rence sur le flux de sortie
	 *
	 */
	void ecritPied(std::ofstream & fichier) const;

	/** Ecriture des donn�es relatives � la balise plugin
	 *
	 * @param fichier La r�f�rence sur le flux de sortie
	 *
	 */
	void ecritPlugin(std::ofstream & fichier) const;

	/** Ecriture des donn�es relatives � la balise match
	 *
	 * @param fichier La r�f�rence sur le flux de sortie
	 *
	 */
	void ecritMatch(std::ofstream & fichier) const;

	/** Ecriture des donn�es relatives � la balise match/teams
	 *
	 * @param fichier La r�f�rence sur le flux de sortie
	 * @param team1 Un pointeur sur la premi�re team
	 * @param team2 Un pointeur sur la deuxi�me team
	 *
	 */
	void ecritTeams(std::ofstream & fichier, Team * team1, Team * team2) const;

	/** Ecriture des donn�es relatives aux balises match/teams/team
	 *
	 * @param fichier La r�f�rence sur le flux de sortie
	 * @param team La team concern�e par la balise
	 * @param lignup La lignup de la team
	 *
	 */
	void ecritTeam(std::ofstream & fichier, Team * team, const std::vector<int> & lignup) const;

	/** Ecriture des donn�es relatives � la balise match/teams/team/joueurs
	 *
	 * @param fichier La r�f�rence sur le flux de sortie
	 * @param lignup La lignup de la team
	 *
	 */
	void ecritJoueurs(std::ofstream & fichier, const std::vector<int> & lignup) const;

	/** Ecriture des donn�es relatives � la balise match/teams/team/joueurs/joueur 
	 *
	 * @param indentation Niveau d'indentation de la balise
	 * @param fichier La r�f�rence sur le flux de sortie
	 * @param joueur Un pointeur sur le joueur concern�
	 * @param playerInfo Un pointeur de type IPLayerInfo sur le joueur concern�
	 *
	 */
	void ecritJoueur(const std::string & indentation, std::ofstream & fichier, Joueur * joueur, IPlayerInfo * playerInfo) const;

	/** Ecriture des donn�es relatives � la balise match/spectateurs
	 *
	 * @param fichier La r�f�rence sur le flux de sortie
	 *
	 */
	void ecritSpectateurs(std::ofstream & fichier) const;

	/** Ecriture des donn�es relatives � la balise sourcetv
	 *
	 * @param fichier La r�f�rence sur le flux de sortie
	 *
	 */
	void ecritSourcetv(std::ofstream & fichier) const;

public:
	/** Surcharge de la m�thode qui �crit le fichier sur le disque */
	void ecrit();

	/** Surcharge du constructeur 
	 *
	 * @see Rapport
	 *
	 */
	RapportXml(Match * match);
};

#endif // __RAPPORTXML_H__
