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
#include "../Timer/Timer.h"
#include "../GestionTimers/GestionTimers.h"
#include "../Messages/Messages.h"

#ifndef __DECOMPTE_H__
#define __DECOMPTE_H__

/** Singleton lan�ant un d�compte � l'�cran de tous les joueurs */
class Decompte
{
private:
	/** Instance unique de Decompte */
	static Decompte * decompte;

	/* Notre classe est un singleton, constructeurs et destructeurs sont donc priv�s */
	Decompte();
	~Decompte();

	/** Temps restant (en secondes) du d�compte */
	int tempsRestant;

	/** Fonction � ex�cuter en fin de d�compte */
	void (*fonction)(const std::string &);

	/** Param�tre de la fonction � ex�cuter */
	std::string parametre;

	/** Gestionnaire des timers �mis par le d�compte */
	GestionTimers moteur;
		
public:
	/** R�cup�re l'instance unique de Decompte 
	 *
	 * @return Un pointeur sur l'instance du d�compte
	 *
	 */
	static Decompte * getDecompte();

	/** D�truit l'instance unique de Decompte si elle existe */
	static void killDecompte();

	/** D�cr�mente le temps restant du d�compte 
	 *
	 * @return Le temps restant en secondes
	 *
	 */
	int decTempsRestant();

	/** Lance un Decompte
	 *
	 * @param duree Dur�e en secondes du d�compte
	 * @param fonction Un r�f�rence sur une fonction � ex�cuter en fin de d�compte <br/>
	 * La fonction ne doit rien retourner et prendre un std::string en param�tre
	 * @param parametre Le param�tre � passer � la fonction � ex�cuter en fin de d�compte
	 *
	 */
	void lanceDecompte(int duree, void (*fonction)(const std::string &), const std::string & parametre = "");
	
	/** Stoppe le d�compte */
	void finDecompte();

	/** Accesseur sur le temps restant du d�compte
	 *
	 * @return Le temps restant en secondes
	 *
	 */
	int getTempsRestant();

	/** Accesseur sur le gestionnaire de timers du d�compte 
	 *
	 * @return Un pointeur sur le gestionnaire de timers
	 *
	 */
	GestionTimers * getMoteur();
};

#endif // __DECOMPTE_H__
