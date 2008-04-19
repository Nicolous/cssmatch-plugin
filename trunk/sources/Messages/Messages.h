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
#include "../GestionJoueurs/GestionJoueurs.h"
#include "../MRecipientFilter/MRecipientFilter.h"

#ifndef __MESSAGES_H__
#define __MESSAGES_H__

/** Classe statique permettant d'envoyer tout type de message */
class Messages
{
public:
	/** Message global dans le tchat <br>
	 * La coloration commence � partir de \003
	 *
	 * @param message Le message � afficher
	 * @param indexJoueur Permet de colorer le message de la couleur de la team d'un joueur d�sign� par son index
	 *
	 */
	static void sayMsg(const std::string & message, int indexJoueur = 0);

	/** Message global color� (blanc, bleu ou rouge de mani�re ind�termin�e) dans le tchat 
	 *
	 * @param message Le message � afficher
	 *
 	 */
	static void sayColor(const std::string & message);

	/** Message priv� dans le tchat 
	 *
	 * @param indexJoueur Index du joueur destinataire
	 * @param message Le message � afficher
	 *
	 */
	static void sayTell(int indexJoueur, const std::string & message);

	/** Message global dans le HUD centr� 
	 *
	 * @param message Le message � afficher
	 *
	 */
	static void hintMsg(const std::string & message);

	/** Message priv� dans le HUD centr� 
	 *
	 * @param indexJoueur Index du joueur destinataire
	 * @param message Le message � afficher
	 *
	 */
	static void hintTell(int indexJoueur, const std::string & message);

	/** Message global de type AMX
	 *
	 * @param message Le message � afficher
	 * @param vie Dur�e de vie du message (en secondes)
	 *
	 */
	static void sayAMX(const std::string & message, int vie);

	/** Message priv� de type AMX 
	 *
	 * @param indexJoueur Index du joueur destinataire
	 * @param message Le message � afficher
	 * @param vie Dur�e de vie du message (en secondes)
	 *
	 */
	static void tellAMX(int indexJoueur, const std::string & message, int vie);

	/** Message global centr� 
	 *
	 * @param message Le message � afficher
	 *
	 */
	static void centerMsg(const std::string & message);

	/** Message priv� centr� 
	 *
	 * @param indexJoueur Index du joueur destinataire
	 * @param message Le message � afficher
	 *
	 */
	static void centerTell(const int indexJoueur,const std::string & message);
};

#endif // __MESSAGES_H__
