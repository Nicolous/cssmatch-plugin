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

#include "../Api/Api.h"
#include "Ligne.h"
#include "../Team/Team.h"
#include "../I18n/I18n.h"

#ifndef __MENU_H__
#define __MENU_H__

/** Associe � chaque menu cr�� dans le plugin un ID unique */
typedef enum CodeMenu
{
	/** Menu invalide */
	CODE_MENU_INVALIDE = 0,

	/** Menu du d�veloppeur */
	CODE_MENU_DEV,

	/** Menu listant les configurations disponibles */
	CODE_MENU_CONFIG,

	/** Menu demandant � l'arbitre si la connexion de SourceTv doit �tre forc�e */
	CODE_MENU_SOURCETV_QUESTION,

	/** Menu demandant � l'arbitre si un CutRound doit �tre jou� */
	CODE_MENU_CUTROUND_QUESTION,

	/** Menu demandant � l'arbitre si des strats devront �tre jou�s */
	CODE_MENU_STRATS_QUESTION,

	/** Menu lorsqu'aucun match n'est lanc� */
	CODE_MENU_OFF,

	/** Menu du CutRound */
	CODE_MENU_CUTROUND,

	/** Menu du StratsTime */
	CODE_MENU_STRATS,

	/** Menu des manches */
	CODE_MENU_MATCH,

	/** Menu principal d'administration */
	CODE_MENU_ADMIN,

	/** Menu de changement de map */
	CODE_MENU_CHANGELEVEL,

	/** Menu de choix d'un joueur � kicker */
	CODE_MENU_KICK,

	/** Menu de choix d'un joueur � bannir */
	CODE_MENU_BAN,

	/** Menu de choix de la dur�e du ban */
	CODE_MENU_BAN_TIME,

	/** Menu de choix d'un joueur � swapper */
	CODE_MENU_SWAP,

	/** Menu de choix d'un joueur � mettre en spectateur */
	CODE_MENU_SPEC,

	/** Menu de fin de match (avec administration) */
	CODE_MENU_FINMATCH
};

/** Associe � une constante les diff�rentes options de sensibilit�s possibles dans les messages de type popup */
enum OptionsSensibles
{
	OPTION_1 = 1<<0,
	OPTION_2 = 1<<1,
	OPTION_3 = 1<<2,
	OPTION_4 = 1<<3,
	OPTION_5 = 1<<4,
	OPTION_6 = 1<<5,
	OPTION_7 = 1<<6,
	OPTION_8 = 1<<7,
	OPTION_9 = 1<<8,

	/** Touche quelconque provoquant l'effacement de la fen�tre */
	OPTION_ANNULER = 1<<9,

	/** Toutes les options sensibles */
	OPTION_ALL = OPTION_ANNULER | OPTION_1 | OPTION_2 | OPTION_3 | OPTION_4 | OPTION_5 | OPTION_6 | OPTION_7 | OPTION_8 | OPTION_9

};

/** Code associ� � l'option "retour" dans les menus */
#define MENU_CODE_RETOUR -2

/** Code associ� � l'option "suivant" dans les menus */
#define MENU_CODE_SUIVANT -1

/* Nombre d'octets limite par message de type popup */
#define MENU_TAILLE_LIMITE 250 // 255 (- 5 octets ?)

/* Nombre d'octets limite par ligne d'un popup */
//#define MENU_TAILLE_PSEUDO_LIMITE 32

/** Exception sp�cifique aux menus du plugin */
class CSSMatchMenuException : public std::exception
{
private:
    std::string msg;
public:
	CSSMatchMenuException(const std::string & msg) : std::exception()
    {
        this->msg = msg;
    }

    virtual ~CSSMatchMenuException() throw() {}

    virtual const char * what() const throw()
    {
		return this->msg.c_str();
    }
};

/** Types de sp�cialisation des menus */
enum TypeSpeMenu
{
	/** Le menu n'est pas sp�cialis� */
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

/** Un menu � nombre d'options et de pages dynamiques */
class Menu
{
private:
	/** Titre du menu <br>
	* FIXME : retrouv� � la cr�ation gr�ce au codeMenu 
	*/
	std::string titre;

	/** Code unique identifiant le menu */
	CodeMenu codeMenu;

	/** Code du menu parent */
	CodeMenu codeMenuParent;

	/** Lignes du menu (retour et suivant sont automatiquement plac�s au bon endroit) */
	std::vector<Ligne> lignes;

	/** Sp�cialisation du menu */
	TypeSpeMenu type;

	/** Fonction � appeler lors de l'utilisation du menu */
	void (* fonction)(int indexJoueur, int choix);

	/** Nombre maximum d'options s�lectionnables dans un menu */
	static const int maxOptions;

public:
	/** Repr�sente une ligne "Retour" */
	static const Ligne retour;

	/** Repr�sente une ligne "Suivant" */
	static const Ligne suivant;

	/** Construit un menu totalement vierge */
	Menu();


	/** Accesseur sur le titre du menu
	 *
	 * @param titre Le nouveau titre du menu
	 *
	 */
	void setTitre(const std::string & titre);

	/** Accesseur sur le titre du menu
	 *
	 * @return Le titre du menu
	 *
	 */
	const std::string & getTitre() const;


	/** Accesseur sur le code du menu
	 *
	 * @param codeMenu Le nouveau code du menu
	 * @see L'�num�ration CodeMenu
	 *
	 */
	void setCodeMenu(CodeMenu codeMenu);

	/** Accesseur sur le code du menu
	 *
	 * @return Le code du menu
	 * @see L'�num�ration CodeMenu
	 *
	 */
	CodeMenu getCodeMenu() const;

	
	/** Accesseur sur le code du menu parent
	 *
	 * @param codeMenuParent Le nouveau code du menu parent
	 * @see L'�num�ration CodeMenu
	 *
	 */
	void setCodeMenuParent(CodeMenu codeMenuParent);

	/** Accesseur sur le code du menu parent
	 *
	 * @return Le code du menu parent
	 * @see L'�num�ration CodeMenu
	 *
	 */
	CodeMenu getCodeMenuParent() const;


	/** Accesseur sur la sp�cialisation du menu
	 *
	 * @param type Le nouveau code de la sp�cialisation du menu
	 * @see L'�num�ration TypeSpeMenu
	 *
	 */
	void specialise(TypeSpeMenu type);

	/** Accesseur sur la sp�cialisation du menu
	 *
	 * @return Le code de la sp�cialisation du menu
	 * @see L'�num�ration TypeSpeMenu
	 *
	 */
	TypeSpeMenu getSpecialisation() const;


	/** Accesseur sur la fonction � appeler � l'utilisation du menu 
	 *
	 * @param fonction Un pointeur sur la fonction � appeler <br>
	 * Ne doit rien retourner et prendre l'index du joueur et de son choix dans le menu en param�tre
	 *
	 */
	void setCallBack(void (* fonction)(int indexJoueur, int choix));

	/** Ex�cute la fonction associ�e � l'utilisation du menu
	 *
	 * @param indexJoueur L'index du joueur
	 * @param choix L'index de son choix dans le menu
	 *
	 */
	void callBack(int indexJoueur, int choix);


	/** Accesseur sur le nombre de lignes du menu
	 *
	 * @return Le nombre de lignes du menu (pr�c�dent et retour inclus)
	 *
	 */
	int getNbrLignes() const;


	/** Ajoute une ligne au menu (retour et suivant sont automatiquement plac�s au bons endroits) 
	 *
	 * @param ligne Le corps de la ligne
	 * @param valeurAssociee Associe une valeur � une ligne (ex : un userid � prendre pour cible) (optionnel)
	 *
	 */
	void addLigne(const std::string & ligne, int valeurAssociee = 0);

	/** D�termine le contenu d'une ligne existante
	 *
	 * @param indexLigne L'index de la ligne (ind�x� � partir de 1, comme pour les index des choix dans le menu)
	 * @param nouvTexte Le nouveau corps de la ligne
	 * @param valeurAssociee La nouvelle valeur associ�e � la ligne (optionnel)
	 *
	 */
	void setLigne(int indexLigne, const std::string & nouvTexte, int valeurAssociee = 0);

	/** Accesseur sur le contenu d'une ligne du menu
	 *
	 * @param indexLigne L'index de la ligne (ind�x� � partir de 1)
	 * @return La ligne correspondante
	 * @throws CSSMatchMenuException si l'index est invalide
	 *
	 */
	const Ligne & getLigne(int indexLigne) const;


	/** Trouve le corps de la ligne choisie par le joueur � une page sp�cifique du menu 
	 *
	 * @param choix Le choix du joueur (typiquement l'argument de menuselect)
	 * @param pageMenu Le num�ro de la page du menu o� le choix a �t� fait
	 * @return La ligne correspondante
	 * @throws CSSMatchMenuException si le choix pass� est invalide
	 *
	 */
	const Ligne & getChoix(int choix, int pageMenu) const;


	/** Affiche la page du menu � un joueur
	 *
	 * @param indexJoueur L'index du joueur destinataire
	 * @param pageMenu Le num�ro de la page du menu vue par le joueur
	 * @param parametres Param�tres des lignes du menu
	 * @throws CSSMatchMenuException si le page n'a pu �tre affich�e
	 *
	 */
	void affiche(int indexJoueur, int pageMenu,
		const std::map<std::string,std::string> & parametres = I18n::SANS_PARAMETRE);

	/** Efface toutes les lignes du menu */
	void purge();

	/** Construit une liste de joueur dans le menu
	 *
	 * @param avecSpec true (par d�faut) si les spectateurs et les non-assign�s doivent �tre list�s
	 *
	 */
	void playerlist(bool avecSpec = true);

	/** Construit une liste de fichier CFG dans le menu � partir d'un r�pertoire donn� en param�tre 
	 *
	 * @param repertoire Le r�pertoire contenant les fichiers (� partir du dossier "cstrike")
	 *
	 */
	void cfglist(const std::string & repertoire);

	/** Constuit un menu bas� sur le contenu d'un fichier (1 ligne du menu = 1 ligne du fichier)
	 *
	 * @param pathFichier Le chemin et le nom du fichier
	 *
	 */
	void filelist(const std::string & pathFichier);
};

#endif // __MENU_H__
