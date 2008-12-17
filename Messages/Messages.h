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
#include "../GestionJoueurs/GestionJoueurs.h"
#include "../MRecipientFilter/MRecipientFilter.h"
#include "../I18n/I18n.h"

#ifndef __MESSAGES_H__
#define __MESSAGES_H__

	/** Associe � une constante chaque type de messages qu'il est possible d'utiliser sous CSS */
enum CodeMessages
{
	MESSAGE_GEIGER = 0,
	MESSAGE_TRAIN,

	MESSAGE_HUDTEXT,
	/** Message dans le TCHAT de type 1 */
	MESSAGE_SAYTEXT,

	/** Message dans le TCHAT de type 2 */
	MESSAGE_SAYTEXT2,

	/** Message centr� */
	MESSAGE_TEXTMSG,

	MESSAGE_HUDMSG,
	MESSAGE_RESETHUD,
	MESSAGE_GAMETITLE,
	MESSAGE_ITEMPICKUP,

	/** Message fen�tr� de type "AMX" */
	MESSAGE_SHOWMENU,

	/** Fait trembler l'�cran */
	MESSAGE_SHAKE,

	/** Fait un fondu de l'�cran */
	MESSAGE_FADE,

	MESSAGE_VGUIMENU,
	MESSAGE_CLOSECAPTION,
	MESSAGE_SENDAUDIO,
	MESSAGE_RAWAUDIO,
	MESSAGE_VOICEMASK,
	MESSAGE_REQUESTSTATE,
	MESSAGE_BARTIME,
	MESSAGE_DAMAGE,
	MESSAGE_RADIOTEXT,

	/** Message dans le HUD centr� en bas de l'�cran */
	MESSAGE_HINTTEXT,

	MESSAGE_RELOADEFFECT,
	MESSAGE_PLAYERANIMEVENT,
	MESSAGE_AMMODENIED,
	MESSAGE_UPDATERADAR,
	MESSAGE_KILLCAM
};

/** Espace de nom qui englobe des outils permettant d'envoyer tous types de message */
namespace Messages
{
	/** Message global dans le tchat <br>
	 * La coloration commence � partir de \003
	 *
	 * @param message Le nom du message � afficher
	 * @param coloration Permet de colorer le message de la couleur de la team d'un joueur d�sign� par son index
	 * @param parametres Les param�tres du messages � afficher
	 * @see I18n
	 *
	 */
	void sayMsg(int coloration, const std::string & message, 
		const std::map<std::string, std::string> & parametres = I18n::SANS_PARAMETRE);

	/** Message global color� (blanc, bleu ou rouge de mani�re ind�termin�e) dans le tchat 
	 *
	 * @param message Le nom du message � afficher
	 * @param parametres Les param�tres du messages � afficher
	 * @see I18n
	 *
 	 */
	void sayColor(const std::string & message,
		const std::map<std::string,std::string> & parametres = I18n::SANS_PARAMETRE);

	/** Message priv� dans le tchat 
	 *
	 * @param coloration Permet de colorer le message de la couleur de la team d'un joueur d�sign� par son index
	 * @param indexJoueur Index du joueur destinataire
	 * @param message Le nom du message � afficher
	 * @param parametres Les param�tres du messages � afficher
	 * @see I18n
	 *
	 */
	void sayTell(int coloration, int indexJoueur, const std::string & message,
		const std::map<std::string, std::string> & parametres = I18n::SANS_PARAMETRE);

	/** Envoie un message de type popup. D�coupe le message en plusieurs UserMessage si n�cessaire
	 *
	 * @param filter Liste de destinataires
	 * @param texte Contenu du message
	 * @param vie Dur�e maximale d'affichage
	 * @param flagsSensibilite Champ de bits d�finissant quelles options seront sensibles � la s�lection
	 */
	void showMenu(MRecipientFilter & filter, const std::string & texte, int vie, int flagsSensibilite = OPTION_ALL);

	/** Message global de type Popup
	 *
	 * @param message Le message � afficher
	 * @param vie Dur�e de vie du message (en secondes)
	 * @param parametres Les param�tres du messages � afficher
	 * @see I18n
	 */
	void sayPopup(const std::string & message, int vie,
		const std::map<std::string, std::string> & parametres);

	/** Message priv� de type Popup
	 *
	 * @param indexJoueur Index du joueur destinataire
	 * @param message Le nom du message � afficher
	 * @param vie Dur�e maximale d'affichage du popup
	 * @param parametres Les param�tres du messages � afficher
	 * @see I18n
	 * @param vie Dur�e de vie du message (en secondes)
	 *
	 */
	void tellPopup(int indexJoueur, const std::string & message, int vie, 
		const std::map<std::string, std::string> & parametres = I18n::SANS_PARAMETRE);

	/** Message global dans le HUD centr� 
	 *
	 * @param message Le message � afficher
	 *
	 */
	void hintMsg(const std::string & message);

	/** Message priv� dans le HUD centr� 
	 *
	 * @param indexJoueur Index du joueur destinataire
	 * @param message Le message � afficher
	 *
	 */
	void hintTell(int indexJoueur, const std::string & message);

	/** Message global centr� 
	 *
	 * @param message Le nom du message � afficher
	 * @param parametres Les param�tres du messages � afficher
	 * @see I18n
	 *
	 */
	void centerMsg(const std::string & message,
		const std::map<std::string, std::string> & parametres = I18n::SANS_PARAMETRE);

	/** Message priv� centr� 
	 *
	 * @param indexJoueur Index du joueur destinataire
	 * @param message Le nom du message � afficher
	 * @param parametres Les param�tres du messages � afficher
	 * @see I18n
	 *
	 */
	void centerTell(const int indexJoueur,const std::string & message,
		const std::map<std::string, std::string> & parametres = I18n::SANS_PARAMETRE);

	/** Message global affich� dans la console d'un joueur
	 *
	 * @param message Le message � afficher
	 */
	void clientPrintMsg(const std::string & message);

	/** Message priv� affich� dans la console d'un joueur
	 *
	 * @param pEntity L'entit� repr�sentant le joueur
	 * @param message Le message � afficher
	 */
	void clientPrintTell(edict_t * pEntity,const std::string & message);

	/** Message affich� dans la console d'un joueur acc�dant � la console par RCON
	 *
	 * @param message Le nom du message � afficher
	 * @param parametres Les param�tres du messages � afficher
	 * @see I18n
	 */
	void rconPrintTell(const std::string & message,
		const std::map<std::string, std::string> & parametres = I18n::SANS_PARAMETRE);

	/** Construit un tableau contenant les rates des joueurs
	 *
	 * @param pEntity Le destinataire du message
	 * @return Le tableau
	 */
	void getRatesTab(edict_t * pEntity);

	/** Message global dans le tchat 
	 *
	 * @param message Le message
	 * @see Timer
	 *
	 */
	void timerSayMsg(const std::string & message, const std::map<std::string,std::string> & parametres);

	/** Message global de type Popup
	 *
	 * @param message Le message
	 * @see Timer
	 *
	 */
	void timerSayPopup(const std::string & message, const std::map<std::string,std::string> & parametres);
};

#endif // __MESSAGES_H__
