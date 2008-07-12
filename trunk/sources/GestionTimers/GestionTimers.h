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

#ifndef __GESTIONTIMERS_H__
#define __GESTIONTIMERS_H__

/** Un gestionnaire de timers <br/>
 *
 * Un timer ajout� � un gestionnaire est en attente d'ex�cution <br/>
 * Veillez � faire fonctionner le gestionnaire � intervalle r�gulier (ex: � chaque frame ou dans un thread)
 *
 * @see GestionTimers#execute
 *
 */
class GestionTimers
{
private:
	/** Tableau dynamique de timers */
	Timer * timers;

	/** Nombre maximum de timers g�r�s */
	int taille;

public:
	GestionTimers();

	/** Construit un gestionnaire de timers d'une capacit� donn�e
	 *
	 * @param taille Le nombre maximal de timers g�r�s par le gestionnaire
	 *
	 */
	GestionTimers(int taille);

	/** D�truit proprement le gestionnaire */
	~GestionTimers();

	/** Copie proprement le gestionnaire 
	 *
	 * @param gTimers Le gestionnaire � copier
	 *
	 */
	GestionTimers(const GestionTimers & gTimers);


	/** Affecte proprement le gestionnaire � une variable
	 *
	 * @param gTimers Le gestionnaire � copier
	 *
	 */
	GestionTimers & operator = (const GestionTimers & gTimers);

	/** Ajoute un timer au gestionnaire 
	 *
	 * @param timer Le timer � ajouter
	 *
	 */
	void addTimer(const Timer & timer);

	/** Ex�cute les timers qui le doivent */
	void execute();

	/** Efface tous les timers en attente */
	void purge();
};

#endif // __GESTIONTIMERS_H__
