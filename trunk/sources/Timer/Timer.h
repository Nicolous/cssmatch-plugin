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

#ifndef __TIMER_H__
#define __TIMER_H__

/** Objet permettant de retarder l'exécution d'une fonction, qui prend en paramètre une std::string */
class Timer
{
protected:
	/** Date d'exécution de la fonction (en secondes) */
	float dateExecution;

	/** Pointeur sur la fonction retardée */
	void (*fonction)(const std::string & parametre);

	/** Paramètre de la fonction à appeler (chaîne vide si la fonction à appeler n'a pas de paramètre) */
	std::string parametre;

public:
	/** Ce constructeur n'est là que pour facilité l'utilisation du timer dans d'autres [patron de ]classes */
	Timer();

	/** Retarde l'exécution d'une fonction par rapport à la date courante
	 *
	 * @param retard Retard (en secondes) de l'exécution de la fonction
	 * @param fonction Un pointeur sur la fonction à exécuter <br/>
	 * Note : la fonction ne doit rien retourner et prendre une std::string en paramètre
	 * @param parametre (Optionnel) Le paramètre de la fonction à exécuter
	 *
	 */
	Timer(float retard, void (*fonction)(const std::string & parametre), const std::string & parametre = "");
	
	/** Constructeur par copie d'un timer
	 *
	 * @param timer Le timer à copier
	 *
	 */
	Timer(const Timer & timer);

	/** Affecte proprement un timer
	 *
	 * @param timer Le timer à copier
	 * @return Le timer à affecter
	 *
	 */
	Timer & operator = (const Timer & timer);

	/** Accesseur sur la date d'exécution de la fonction retardée
	 *
	 * @return La date d'exécution
	 *
	 */
	float getDateExecution() const;

	/** Accesseur sur le paramètre de la fonction retardée
	 *
	 * @return Le paramètre
	 *
	 */
	const std::string & getParametre() const;


	/** Exécute la fonction retardée <br/>
	 * Note : l'éventuelle destruction du timer est à la charge du code appelant
	 */
	void execute();

	/** Fonction contrôlant la validité du timer
	 *
	 * @return true si le timer est valide
	 *
	 */
	bool estValide() const;

	/** Construit une représentation du timer sous forme de chaîne de caractères
	 *
	 * @return La représentation du timer
	 *
	 */
	const std::string toString() const;
};

#endif // __TIMER_H__
