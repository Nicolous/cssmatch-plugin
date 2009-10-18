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

#ifndef __EVENTLISTENER_H__
#define __EVENTLISTENER_H__

/** Ecoute les �v�nements du jeu */
class EventListener : public IGameEventListener2
{
private:
	/* Un joueur est bless�
	 *
	 * @param event L'instance de l'�v�nement
	 * @see IGameEventListener#FireGameEvent
	 */
	//void player_hurt(IGameEvent * event); // Remplac� par la modification de la prop m_lifeState

	/** Un �quipement est r�cup�r� par un joueur
	 *
	 * @param event L'instance de l'�v�nement
	 * @see IGameEventListener#FireGameEvent
	 */
	void item_pickup(IGameEvent * event);

	/** Un joueur arrive sur le serveur ou apparait sur la carte
	 *
	 * @param event L'instance de l'�v�nement
	 * @see IGameEventListener#FireGameEvent
	 */
	void player_spawn(IGameEvent * event);

	/** Un joueur a utilis� le TCHAT
	 *
	 * @param event L'instance de l'�v�nement
	 * @see IGameEventListener#FireGameEvent
	 */
	void player_say(IGameEvent * event);

	/** Un joueur est mort
	 *
	 * @param event L'instance de l'�v�nement
	 * @see IGameEventListener#FireGameEvent
	 */
	void player_death(IGameEvent * event);

	/** Un joueur s'est d�connect�
	 *
	 * @param event L'instance de l'�v�nement
	 * @see IGameEventListener#FireGameEvent
	 */
	void player_disconnect(IGameEvent * event);

	/** Un round commence
	 *
	 * @param event L'instance de l'�v�nement
	 * @see IGameEventListener#FireGameEvent
	 */
	void round_start(IGameEvent * event);

	/** Un round s'est termin�
	 *
	 * @param event L'instance de l'�v�nement
	 * @see IGameEventListener#FireGameEvent
	 */
	void round_end(IGameEvent * event);

	/** Un joueur change de camp
	 *
	 * @param event L'instance de l'�v�nement
	 * @see IGameEventListener#FireGameEvent
	 */
	void player_team(IGameEvent * event);

	/** Un joueur a commenc� � amorcer la bombe
	 *
	 * @param event L'instance de l'�v�nement
	 * @see IGameEventListener#FireGameEvent
	 */
	void bomb_beginplant(IGameEvent * event);

	/** Un joueur a chang� de pseudo
	 *
	 * @param event L'instance de l'�v�nement
	 * @see IGameEventListener#FireGameEvent
	 */
	void player_changename(IGameEvent * event);

public:
	/** Initialise l'�couteur */
	void initialise();

	/*	Appel� par le jeu lorsqu'un �v�nement a eu lieu dans la partie
	*
	* @param event L'instance de l'�v�nement
	* @see IGameEventListener#FireGameEvent
	*/
	void FireGameEvent(IGameEvent * event);
};

#endif // __EVENTLISTENER_H__
