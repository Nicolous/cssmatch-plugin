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
#include "Ligne.h"
#include "../Team/Team.h"

#ifndef __MENU_H__
#define __MENU_H__

/** Type de spécialisation des menus */
enum TypeSpeMenu
{
	/** Le menu n'est pas spécialisé */
	NOLIST,
	/** Le menu est une liste de joueurs */
	PLAYERLIST,
	/** Le menu est une liste de joueurs non spectateurs */
	PLAYERLIST_SANS_SPEC,
	/** Le menu est une liste de configurations (voir DOSSIER_CONFIGURATIONS_MATCH) */
	CFGLIST,
	/** Le menu est une liste de lignes contenues dans un fichier (voir FICHIER_MAPLIST) */
	MAPLIST
};

/** Représente un menu à nombre d'options et de pages dynamiques */
class Menu
{
private:
	/** Titre du menu (retrouvé à la création grâce au codeMenu) */
	std::string titre;
	/** Code unique identifiant le menu */
	CodeMenu codeMenu;
	/** Code du menu parent */
	CodeMenu codeMenuParent;
	/** Lignes du menu (retour et suivant sont automatiquement placés au bon endroit) */
	std::vector<Ligne> lignes;

	/** Spécialisation du menu */
	TypeSpeMenu type;

	/** Fonction à appeler lors de l'utilisation du menu */
	void (* fonction)(int indexJoueur, int choix);

	/** Représente une ligne "Retour" */
	static const Ligne retour;

	/** Représente une ligne "Suivant" */
	static const Ligne suivant;

	/** Nombre maximum d'options sélectionnables dans un menu */
	static const int maxOptions;

public:
	/** Construit un menu totalement vierge */
	Menu();
	/** Construit une copie du menu */
	Menu(const Menu & menu);

	/** Détermine le titre du menu */
	void setTitre(const std::string & titre);
	/** Retourne le titre du menu */
	const std::string & getTitre() const;

	/** Détermination du codeMenu */
	void setCodeMenu(CodeMenu codeMenu);
	/** Retourne le codeMenu */
	CodeMenu getCodeMenu() const;

	/** Détermine le code unique du menu parent */
	void setCodeMenuParent(CodeMenu codeMenuParent);
	/** Retourne le code du menu parent */
	CodeMenu getCodeMenuParent() const;

	/** Spécialise le menu */
	void specialise(TypeSpeMenu type);
	/** Retourne la spécialisation du menu (cf TypeSpeMenu) */
	TypeSpeMenu getSpecialisation() const;

	/** Détermine la fonction à appeler à l'utilisation du menu */
	void setCallBack(void (* fonction)(int indexJoueur, int choix));
	/** Exécute la fonction associée à l'utilisation du menu */
	void callBack(int indexJoueur, int choix);

	/** Retourne le nombre de lignes du menu */
	int getNbrLignes() const;

	/** Ajoute une ligne au menu (retour et suivant sont automatiquement placés au bon endroit) */
	void addLigne(const std::string & ligne, int valeurAssociee = 0);
	/** Détermine le contenu d'une ligne existante <br>
	 * Convertit le ligne en std::string
	 */
	void setLigne(int indexLigne, const std::string & nouvTexte, int valeurAssociee = 0);
	/** Retourne le contenu d'une ligne particulière <br>
	 * Jette une exception de type std::out_of_range si l'index passé est invalide
	 */
	const Ligne & getLigne(int indexLigne) const;

	/** Retourne le choix fait à une page spécifique du menu 
	 * Jette une exception de type std::out_of_range si le choix passé est invalide
	 */
	const Ligne & getChoix(int choix, int pageMenu) const;

	/** Affiche la page du menu à un joueur désigné par son index <br>
	 * Jette une exception de type std::out_of_range si le page n'a pu être affichée
	 */
	void affiche(int indexJoueur, int pageMenu);

	/** Efface toutes les lignes du menu */
	void purge();

	/** Construit une liste de joueur dans le menu (les spectateurs et les non-assignés compris par défaut) */
	void playerlist(bool avecSpec = true);

	/** Construit une liste de fichier .cfg dans le menu à partir d'un répertoire donné en paramètre */
	void cfglist(const std::string & repertoire);

	/** Constuit un menu basé sur le contenu d'un fichier (1 ligne du menu = 1 ligne du fichier) <br>
	 * Utile pour contruire une liste de maps par exemple
	 */
	void filelist(const std::string & pathFichier);

	/** Opérateur d'affectation */
	Menu & operator = (const Menu & menu);
};

#endif // __MENU_H__
