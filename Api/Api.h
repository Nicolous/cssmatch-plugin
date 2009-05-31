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

// Visual C++ 2005 ne supporte pas la sp�cification des exceptions dans la d�claration des fonctions
#pragma warning(disable : 4290)

// Je suis au courant ! (variables locales non r�f�renc�es, typiquement dans les catch(std::exception e){})
#pragma warning(disable :4101)

#include <vector>
#include <map>
#include <stdio.h>
#include <ctime>
#include <string>
#include <locale>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <time.h>

#include "../Outils/Outils.h"

#include "interface.h"
/**
 * @see http://developer.valvesoftware.com/wiki/KeyValues_class#Important_Notes
 */
#include "filesystem.h"
#include "engine/iserverplugin.h"
#include "dlls/iplayerinfo.h"
#include "eiface.h"
#include "igameevents.h"
#include "convar.h"
//#include "Color.h"
//#include "vstdlib/random.h"
//#include "engine/IEngineTrace.h"
/** Permet de manipuler les ConVars */
#include "icvar.h"
#include "bitbuf.h"
/** Permet de manipuler des listes de joueurs destinataires des messages du plugin
 *
 * @author Voir la liste des auteurs sur le Wiki de VALVE
 *
 */
#include "../MRecipientFilter/MRecipientFilter.h"

/** Permet l'inclusion de cbase.h */
#define GAME_DLL 1
#include "cbase.h"
//#include "engine/IStaticPropMgr.h"
//#include "engine/IEngineSound.h"
//#include "entityapi.h"
//#include "entitylist.h"
#include "baseentity.h"

/** memdbgon must be the last include file in a .cpp file!!! */
#include "tier0/memdbgon.h"

#ifndef __API_H__
#define __API_H__

/** Exception sp�cifique � l'API du plugin */
class CSSMatchApiException : public std::exception
{
private:
    std::string msg;
public:
	CSSMatchApiException(const std::string & msg) : std::exception()
    {
        this->msg = msg;
    }

    virtual ~CSSMatchApiException() throw() {}

    virtual const char * what() const throw()
    {
		return this->msg.c_str();
    }
};

/** D�finit le nom du plugin */
#define CSSMATCH_NOM "CSSMatch"

/** D�finit le nom de la version du plugin. */
#define CSSMATCH_VERSION_LIGHT "1.3.5"

/** D�finit le site Web officiel du plugin. */
#define CSSMATCH_SITE "http://code.google.com/p/cssmatch-plugin-en/"

/** D�finit les informations sur le nom de version, l'auteur et le site associ� au plugin */
//#define CSSMATCH_VERSION "CSSMatch v" CSSMATCH_VERSION_LIGHT ", by Nicolous " CSSMATCH_SITE
#define CSSMATCH_VERSION "Nico's " CSSMATCH_NOM ", version " CSSMATCH_VERSION_LIGHT " (" __DATE__ "), " CSSMATCH_SITE


/** D�finit le nombre repr�sentant un userid erron� */
#define USERID_INVALIDE -1

/* D�finit le nombre repr�sentant un index erron� */
#define INDEX_INVALIDE 0

/* D�finit quel est le nombre maximum de slots d'un serveur, avant que celui-ci ne soit �ventuellement chang� */
//#define DEFAUT_MAXPLAYERS 64

/* Macro permettant d'obtenir les interfaces utilis�es par le plugin
 *
 * @author VALVE Corporation
 *
 */
/*#define GETINTERFACE(factory, retvar, interfacename, rettype, required) retvar = (rettype *)factory(interfacename, NULL);\
	if (retvar == NULL)\
	{ \
		Msg("Impossible d'obtenir l'interface \"" interfacename "\"\n");\
		if (required)\
		{\
			return false;\
		}\
	}
		// Remplac� par la fonction g�n�rique Api::getInterface
	*/

/** Associe � une constante les index des teams possibles sous CSS */
typedef enum CodeTeam
{
	/** Team inconnue */
	TEAM_INVALIDE = TEAM_INVALID,

	/** Les non-assign�s */
	TEAM_UN = TEAM_UNASSIGNED,

	/** Les spectateurs */
	TEAM_SPEC = TEAM_SPECTATOR,

	/** Les Terro */
	TEAM_T,

	/** Les CT */
	TEAM_CT,

	/** Toutes les teams */
	TEAM_ALL
};

/** Associe � une constante les index des slots possibles sous CSS */
typedef enum CodeWeaponSlot
{
	WEAPON_SLOT1 = 0,
	WEAPON_SLOT2,
	WEAPON_SLOT3,
	WEAPON_SLOT4,
	WEAPON_SLOT5
};

/** Espace de nom contenant diff�rentes fonctions agissant sur la partie */
namespace Api
{
	/** Force l'ex�cution imm�diate d'une commande dans la console
	 *
	 * @param cmd La commande � ex�cuter
	 *
	 */
	void serveurExecute(const std::string & cmd);

	/** Ex�cute un fichier de configuration via la commande "exec", <br>
	 * Force �galement l'ex�cution des commandes ainsi plac�es dans la file d'attente de la console
	 *
	 * @param configPath Le nom du fichier
	 *
	 */
	void configExecute(const std::string & configPath);

	/** D�termine si HLTV est pr�sente sur le serveur
	 *
	 * @return true si HLTV est pr�sente
	 */
	bool hltvPresente();

	/** Recherche le pointeur IPlayerInfo repr�sentant une entit� repr�sentant un joueur
	*
	* @param pEntity L'entit� repr�sentant le joueur
	* @return Un pointeur de type IPlayerInfo sur l'entit�
	* @throws CSSMatchApiException si l'IPlayerInfo n'a pu �tre trouv�
	*/
	IPlayerInfo * getIPlayerInfoFromEntity(edict_t * pEntity) throw (CSSMatchApiException);

	/** Recherche l'entit� correspondant � un userID
	*
	* @param userid L'userid du joueur
	* @return Un pointeur de type edict_t sur l'entit� trouv�e
	* @throws CSSMatchApiException si l'entit� n'a pu �tre trouv�e
	*/
	edict_t * getPEntityFromID(int userid) throw (CSSMatchApiException);

	/** Recherche l'entit� correspondant � un index
	*
	* @param index L'index du joueur
	* @return Un pointeur de type edict_t sur l'entit� trouv�e
	* @throws CSSMatchApiException si l'entit� n'a pu �tre trouv�e
	*
	*/
	edict_t * getPEntityFromIndex(int index) throw (CSSMatchApiException);

	/** Recherche l'index d'un joueur portant un userID donn�
	 *
	 * @param userid L'userid du joueur
	 * @return L'index du joueur (peut �tre �gal � INDEX_INVALIDE)
	 *
	 */
	int getIndexFromUserId(int userid);

	/** Recherche l'index d'un joueur correspondant � une entit�
	 *
	 * @param pEntity Le joueur
	 * @return L'index du joueur (peut �tre �gal � INDEX_INVALIDE)
	 *
	 */
	int getIndexFromPEntity(edict_t * pEntity);

	/** Recherche l'userID d'un joueur d�sign� par son pseudo
	 *
	 * @param pseudo Le pseudo du joueur
	 * @return L'userid du joueur (peut �tre �gal � USERID_INVALIDE)
	 *
	 */
	int getUseridFromUsername(const std::string & pseudo);

	/** Recherche l'userID d'une entit� repr�sentant un joueur
	 *
	 * @param pEntity L'entit� repr�sentant le joueur
	 * @return L'userid du joueur
	 *
	 */
	int getUseridFromPEntity(edict_t * pEntity);

	/** Recherche l'index d'un joueur � partir de son pseudo
	 *
	 * @param pseudo Le pseudo du joueur
	 * @return L'userid du joueur
	 *
	 */
	int getIndexFromUsername(const std::string & pseudo);

	/** Recherche un pointeur de type IServerEntity sur une entit� repr�sentant un joueur
	 *
	 * @param pEntity L'entit� repr�sentant un joueur
	 * @return Un pointeur de type IServerEntity repr�sentant le joueur
	 * @throws CSSMatchApiException si l'IServerEntity n'a pu �tre trouv�e
	 */
	IServerEntity * getIServerEntityFromEdict(edict_t * pEntity) throw (CSSMatchApiException);

	/** Recherche un pointeur de type CBaseEntity sur une entit� repr�sentant un joueur
	 *
	 * @param pEntity L'entit� repr�sentant un joueur
	 * @return Un pointeur de type CBaseEntity sur le joueur
	 * @throws CSSMatchApiException si le pointeur CBaseEntity correspondant n'a pu �tre trouv�
	 */
	CBaseEntity * getCBaseEntityFromEntity(edict_t * pEntity) throw (CSSMatchApiException);

	/** Recherche le CodeTeam d'une entit� repr�sentant un joueur
	 *
	 * @param pEntity L'une entit� repr�sentant le joueur
	 * @return Le CodeTeam du joueur
	 * @see L'unum�ration CodeTeam
	 *
	 */
	CodeTeam getPEntityTeam(edict_t * pEntity);

	/** Recherche un pointeur de type CBaseCombatCharacter sur une entit� repr�sentant un joueur
	 *
	 * @param pEntity L'entit� repr�sentant un joueur
	 * @return Un pointeur de type CBaseCombatCharacter sur le joueur
	 * @throws CSSMatchApiException si le pointeur CBaseCombatCharacter correspondant n'a pu �tre trouv�
	 *
	 */
	CBaseCombatCharacter * getCBaseCombatCharacterFromEntity(edict_t * pEntity) throw (CSSMatchApiException);

	/** Recherche un pointeur de type CBaseCombatWeapon sur une arme port�e par une entit� repr�sentant un joueur, � un slot donn�
	 *
	 * @param pEntity L'entit� repr�sentant le joueur
	 * @param slot Le CodeWeaponSlot de l'arme
	 * @throws CSSMatchApiException si le CBaseEntity n'a pu �tre trouv�
	 * @see L'�num�ration CodeWeaponSlot
	 *
	 */
	CBaseCombatWeapon * getCBaseCombatWeaponFromSlot(edict_t * pEntity, CodeWeaponSlot slot) throw (CSSMatchApiException);

	/** Recherche un pointeur de type CBasePlayer sur une entit� repr�sentant un joueur
	 *
	 * @param pEntity L'entit� repr�sentant le joueur
	 * @return Un pointeur de type CBasePlayer sur le joueur
	 * @throws CSSMatchApiException si le pointeur CBasePlayer correspondant n'a pu �tre trouv�
	 *
	 */
	CBasePlayer * getCBasePlayerFromEntity(edict_t * pEntity) throw (CSSMatchApiException);

	/** D�termine le nombre de joueurs appartenant � une team
	 *
	 * @param codeTeam Le CodeTeam de la team
	 * @return Le nombre de joueurs appartenant � la team
	 * @see L'�num�ration CodeTeam
	 *
	 */
	int getPlayerCount(CodeTeam codeTeam);

	/** Recherche le pseudo d'une entit� repr�sentant un joueur
	 *
	 * @param pEntity L'entit� repr�sentant le joueur
	 * @return Le pseudo du joueur
	 * @throws CSSMatchApiException si le pseudo n'a pu �tre trouv�
	 *
	 */
	std::string getPlayerName(edict_t * pEntity) throw (CSSMatchApiException);

	/** Recherche la steamID d'une entit� repr�sentant un joueur
	 *
	 * @param pEntity L'entit� repr�sentant le joueur
	 * @return La steamID du joueur
	 * @throws CSSMatchApiException si la steamID n'a pu �tre trouv�e
	 */
	std::string getPlayerSteamID(edict_t * pEntity) throw (CSSMatchApiException);

	/** Kick un joueur d�sign� par userID pour un motif d�termin�e
	 *
	 * @param useridCible L'userid du joueur
	 * @param motif Le motif du kick
	 *
	 */
	void kickid(int useridCible, const std::string & motif);

	/** Banni un joueur d�sign� par userID pour un motif d�termin�e, pour un temps donn�
	 *
	 * @param temps La dur�e (en minutes) du ban
	 * @param useridCible L'userid du joueur
	 * @param motif Le motif du kick
	 *
	 */
	void banid(int temps, int useridCible, const std::string & motif);

	/** Ex�cute une commande dans la console d'une entit� repr�sentant un joueur
	 *
	 * @param pEntity L'entit� repr�sentant le joueur
	 * @param cmd La commande � ex�cuter
	 *
	 */
	void cexec(edict_t * pEntity, const std::string & cmd);

	/** Ex�cute une commande dans la console des joueurs d'une team
	 *
	 * @param codeTeam Le CodeTeam de la team
	 * @param cmd La commande � ex�cuter
	 * @see L'�num�ration CodeTeam
	 *
	 */
	void cexecGroupe(CodeTeam codeTeam, const std::string & cmd);

	/** Force l'ex�cution d'une commande dans la console d'un joueur <br>
	 * Fonctionne m�me si la commande a subit un alias, et pour certaines commandes normalement prot�g�es
	 *
	 * @param pEntity L'entit� repr�sentant le joueur
	 * @param cmd La commande � ex�cuter
	 *
	 */
	void sexec(edict_t * pEntity, const std::string & cmd);

	/** Force l'ex�cution d'une commande dans la console des joueurs d'une team <br>
	 * Fonctionne m�me si la commande a subit un alias, et pour certaines commandes normalement prot�g�es
	 *
	 * @param codeTeam Le CodeTeam de la team
	 * @param cmd La commande � ex�cuter
	 * @see L'�num�ration CodeTeam
	 *
	 */
	void sexecGroupe(CodeTeam codeTeam, const std::string & cmd);

	/** Swap un joueur dans le camp d'en face (si possible)
	 *
	 * @param player Un pointeur de type IPlayerInfo sur le joueur
	 *
	 */
	void swap(IPlayerInfo * player);

	/** Swap les joueurs d'une team dans le camp d'en face (si possible)
	 *
	 * @param codeTeam Le CodeTeam de la team
	 * @see L'�num�ration CodeTeam
	 *
	 */
	void swapGroupe(CodeTeam codeTeam);

	/** Met un joueur en spectateur
	 *
	 * @param player Un pointeur de type IPlayerInfo sur le joueur
	 *
	 */
	void spec(IPlayerInfo * player);

	/** Met les joueurs d'une team en spectateur
	 *
	 * @param codeTeam Le CodeTeam de la team
	 * @see L'�num�ration CodeTeam
	 *
	 */
	void specGroupe(CodeTeam codeTeam);

	/** Modifie la vie d'une entit� repr�sentant un joueur
	 *
	 * @param pEntity L'entit� repr�sentant le joueur
	 * @param nombreHP Le nombre de points de vie � attribuer au joueur
	 *
	 */
	void setHealth(edict_t * pEntity, int nombreHP);

	/** D�truit l'arme d'une entit� repr�sentant un joueur, situ� � un slot donn�
	 *
	 * @param pEntity L'entit� repr�sentant le joueur
	 * @param slot Le CodeWeaponSlot de l'arme
	 *
	 */
	void detruitArme(edict_t * pEntity, CodeWeaponSlot slot);

	/** Evalue la validit� d'une entit� edict_t
	 *
	 * @param pEntity L'entit� � valider
	 * @return true si l'entit� est valide
	 *
	 */
	bool isValidePEntity(edict_t * pEntity);

	/** Evalue la validit� d'une entit� IPlayerInfo
	 *
	 * @param player L'entit� � valider
	 * @return true le l'entit� est valide
	 *
	 */
	bool isValidePlayer(IPlayerInfo * player);

	/** Evalue la validit� de l'index d'un joueur
	 *
	 * @param index L'index � analyser
	 * @return true si l'index est valide
	 *
	 */
	bool isValideIndex(int index);

	/** D�termine si un index est occup� par un joueur connect�
	 *
	 * @param index L'index � analyser
	 * @return true si un joueur occupe cet index
	 *
	 */
	bool isOnline(int index);

	/** Ecrit un message dans la console et dans le log du serveur
	 *
	 * @param message Le message
	 *
	 */
	void debug(const std::string & message);

	/** Reporte une exception dans les logs du serveur
	 *
	 * @param e L'exception
	 * @param fichier Le nom du fichier dans lequel l'exception a �t� rattrap�e
	 * @param ligne La ligne du fichier o� l'exception a �t� rattrap�e
	 */
	void reporteException(const std::exception & e, const std::string & fichier, int ligne);
};

// Fonctions outils pour les timers (nos timers ne supportent pas les fonctions membres)
/** Ex�cute une commande dans la console des joueurs terroristes
 *
 * @param commande La commande � ex�cuter
 *
 */
 void timerCexecT(const std::string & commande, const std::map<std::string,std::string> & parametres);

/** Ex�cute une commande dans la console des joueurs anti-terroristes
 *
 * @param commande La commande � ex�cuter
 *
 */
void timerCexecCT(const std::string & commande, const std::map<std::string,std::string> & parametres);

/** Swap tous les joueurs
 *
 * @param parametre Une cha�ne vide
 *
 */
void timerSwapALL(const std::string & parametre, const std::map<std::string,std::string> & parametres);

/** Function to initialize any cvars/command in this plugin
 *
 * @author VALVE Corporation
 *
 */
void InitCVars(CreateInterfaceFn cvarFactory);

/* D�termine si 2 cha�nes de caract�res char* sont �gales
 *
 * @author VALVE Corporation
 * @return true si les cha�nes sont �gales
 *
 */
//inline bool FStrEq(const char *sz1, const char *sz2);

/** Nom du fichier contenant la liste des arbitres */
#define FICHIER_ADMINLIST "cstrike/cfg/cssmatch/adminlist.txt"

/** Nom du fichier contenant la liste des maps � lister dans le menu */
#define FICHIER_MAPLIST "cstrike/cfg/cssmatch/maplist.txt"

/** Dossier dans lequel sont recherch�s les configurations de match */
#define DOSSIER_CONFIGURATIONS_MATCH "cfg/cssmatch/configurations"

/** Dossier dans lequel sont enregistr�s les rapports de match */
#define DOSSIER_RAPPORTS_MATCH "cstrike/cfg/cssmatch/reports"

#endif // __API_H__
