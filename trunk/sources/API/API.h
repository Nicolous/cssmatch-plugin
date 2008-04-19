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
 
/** Interface entre l'API de VALVE et CSSMatch
 *
 *  Ce fichier regroupe le code nécessaire à interfacer l'API de VALVE et le plugin
 *
 */

// Visual C++ 2005/.NET FrameWork ne supporte pas la spécification des exceptions dans la déclaration des fonctions
#pragma warning( disable : 4290 )

// Je suis au courant ! (variables locales non référencées, typiquement dans les catch(){})
#pragma warning( disable : 4101 )

#include <vector>
#include <map>
#include <stdio.h>
#include <ctime>
#include <string>
#include <sstream>
#include <stdexcept>
#include <time.h>

#include "interface.h"
#include "filesystem.h"
#include "engine/iserverplugin.h"
#include "dlls/iplayerinfo.h"
#include "eiface.h"
#include "igameevents.h"
#include "convar.h"
#include "Color.h"
#include "vstdlib/random.h"
#include "engine/IEngineTrace.h"
/** Permet de manipuler les ConVars */
#include "ICvar.h"
#include "bitbuf.h"
/** Permet de manipuler des listes de joueurs destinataires des messages du plugin 
 *
 * @author La communauté de http://hl2coding.com
 *
 */
#include "../MRecipientFilter/MRecipientFilter.h"

/** Permet l'inclusion de cbase.h */
#define GAME_DLL 1
#include "cbase.h"
#include "engine/IStaticPropMgr.h"
#include "entityapi.h"
#include "entitylist.h"
#include "baseentity.h"

/** memdbgon must be the last include file in a .cpp file!!! */
#include "tier0/memdbgon.h"

#ifndef __API_H__
#define __API_H__

/** Défini le nom de la version du plugin. */
#define CSSMATCH_VERSION_LIGHT "1.1.0"

/** Défini le site Web officiel du plugin. */
#define CSSMATCH_SITE "http://nicolasmaingot.leobaillard.org/forums/"

/** Défini les informations sur le nom de version, l'auteur et le site associé au plugin */
#define CSSMATCH_VERSION "CSSMatch v" CSSMATCH_VERSION_LIGHT ", by Nicolous " CSSMATCH_SITE

/** Défini le nombre représentant un userid erroné */
#define USERID_INVALIDE -1

/** Défini le nombre représentant un index erroné */
#define INDEX_INVALIDE 0

/** Défini quel est le nombre maximum de slots d'un serveur, avant que celui-ci ne soit éventuellement baissé */
#define DEFAUT_MAXPLAYERS 64

/** Macro permettant d'obtenir les interfaces utilisées par le plugin 
 *
 * @author VALVE Corporation
 *
 */
#define GETINTERFACE(factory, retvar, interfacename, rettype, required) retvar = (rettype *)factory(interfacename, NULL); if (retvar == NULL) { Msg("Unable to get " interfacename " interface\n"); if (required) { return false; } } 

/** Associe à une constante les index des teams possibles sous CSS */
typedef enum CodeTeam
{
	/** Team inconnue */
	TEAM_INVALIDE = -1,
	/** Les non-assignés */
	TEAM_UN = 0,
	/** Les spectateurs */
	TEAM_SPEC,
	/** Les Terro */
	TEAM_T,
	/** Les CT */
	TEAM_CT,
	/** Toutes les teams */
	TEAM_ALL
};

/** Associe à une constante les index des slots possibles sous CSS */
typedef enum CodeWeaponSlot
{
	WEAPON_SLOT1 = 0,
	WEAPON_SLOT2,
	WEAPON_SLOT3, 
	WEAPON_SLOT4,
	WEAPON_SLOT5
};

/** Classe statique interfaçant l'API de VALVE avec le plugin */
class API
{
public:
	/** Interface avec le moteur */
	static IVEngineServer * engine;
	/** Intéraction avec le système de fichier */
	static IFileSystem * filesystem;
	/** Interface événementielle */
	static IGameEventManager2 * gameeventmanager;
	/** Interface fournissant des informations sur les joueurs */
	static IPlayerInfoManager * playerinfomanager;
	/** Intéraction avec les bots */
	static IBotManager * botmanager;
	/** Interfaces d'aide à l'écriture de plugins */
	static IServerPluginHelpers * helpers;
	/** Générateur de nombres aléatoire */
	static IUniformRandomStream * randomStr;
	/** Interface avec l'environnement virtuel */
	static IEngineTrace * enginetrace;
	/** Interface avec des variables globales */
	static CGlobalVars * gpGlobals;
	/** Interface avec les ConVars */
	static ICvar * cvars;

	/** Nombre limite de joueurs connectés au serveur */
	static int maxplayers;

	/** Initialise les champs <br>
	 * Renvoi faux si l'une des interfaces n'a pu être initialisée
	 */
	static bool initialisation(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory);
	/** Force l'exécution immédiate d'une commande dans la console */
	static void serveurExecute(const std::string & cmd);
	/** Exécute un fichier de configuration via la commande "exec", <br>
	 * Force également l'exécution des commandes ainsi placées dans la file d'attente de la console
	 */
	static void configExecute(const std::string & config);
	/** Retourne vrai si HLTV est présente sur le serveur */
	static bool hltvPresente();
	/** Retourne un pointeur de type IPlayerInfo sur un joueur <br> 
	* Jette une exception de type std::out_of_range si l'IPlayerInfo n'a pu être trouvé
	*/
	static IPlayerInfo * getIPlayerInfoFromEntity(edict_t * pEntity) throw (std::out_of_range);
	/** Retourne l'entité correspondant à un ID <br> 
	* Jette une exception de type std::out_of_range si l'ID n'a pu être trouvé
	*/
	static edict_t * getEntityFromID(int userid) throw (std::out_of_range);
	/** Retourne l'index d'un joueur grâce à son ID */
	static int getIndexFromUserId(int userid);
	/** Retourne l'ID d'un joueur à partir de son pseudo */
	static int getUseridFromUsername(const std::string & pseudo);
	/** Retourne l'UD d'un joueur */
	static int getUseridFromPEntity(edict_t * pEntity);
	/** Retourne l'index d'un joueur à partir de son pseudo */
	static int getIndexFromUsername(const std::string & pseudo);
	/** Retourne un pointeur de type IServerEntity sur un joueur <br>
	 * Jette une exception de type std::out_of_range si l'IServerEntity n'a pu être trouvée
	 */
	static IServerEntity * getIServerEntityFromEdict(edict_t * pEntity) throw (std::out_of_range);
	/** Retourne un pointeur de type CBaseEntity sur un joueur <br>
	 * Jette une exception de type std::out_of_range si le CBaseEntity n'a pu être trouvé
	 */
	static CBaseEntity * getCBaseEntityFromEntity(edict_t * pEntity) throw (std::out_of_range);
	/** Retourne le codeTeam d'une entité pEntity */
	static CodeTeam getPEntityTeam(edict_t * pEntity);
	/** Retourne un pointeur de type CBaseCombatCharacter sur une entité <br>
	 * Jette une exception de type std::out_of_range si le CBaseCombatCharacter n'a pu être trouvé
	 */
	static CBaseCombatCharacter * getCBaseCombatCharacterFromEntity(edict_t * pEntity) throw (std::out_of_range);
	/** Retourne un pointeur sur une arme portée par un joueur à un slot donné <br>
	 * Jette une exception de type std::out_of_range si le CBaseEntity n'a pu être trouvé
	 */
	static CBaseCombatWeapon * getCBaseCombatWeaponFromSlot(edict_t * pEntity, int slot) throw (std::out_of_range);
	/** Retourne un pointeur de type CBasePlayer sur un joueur <br>
	 * Jette une exception de type std::out_of_range si le CBasePlayer n'a pu être trouvé
	 */
	static CBasePlayer * getCBasePlayerFromEntity(edict_t * pEntity) throw (std::out_of_range);
	/** Retourne le nombre de joueurs appartenant à une team */
	static int getPlayerCount(CodeTeam codeTeam);
	/** Retourne le pseudo d'un joueur <br>
	 * Jette une exception de type std::out_of_range si le pseudo n'a pu être trouvé
	 */
	static std::string getPlayerName(edict_t * pEntity) throw (std::out_of_range);
	/** Retourne le steamID d'un joueur <br>
	 * Jette une exception de type std::out_of_range si la steamID n'a pu être trouvée
	 */
	static std::string getPlayerSteamID(edict_t * pEntity) throw (std::out_of_range);

	/** Kick un joueur désigné par ID pour un motif déterminée */
	static void kickid(int useridCible, const std::string & motif);
	/** Banni un joueur désigné par ID pour un motif déterminée */
	static void banid(int temps, int useridCible, const std::string & motif);
	/** Exécute une commande dans la console d'un joueur */
	static void cexec(edict_t * pEntity, const std::string & cmd);
	/** Exécute une commande dans la console d'un groupe de joueurs */
	static void cexecGroupe(CodeTeam codeTeam, const std::string & cmd);
	/** Force l'exécution d'une commande dans la console d'un joueur même si la commande a subit un alias */
	static void sexec(edict_t * pEntity, const std::string & cmd);
	/** Force l'exécution d'une commande dans la console d'un groupe de joueurs même si la commande a subit un alias */
	static void sexecGroupe(CodeTeam codeTeam, const std::string & cmd);
	/** Swap un joueur dans le camp d'en face (si possible) */
	static void swap(IPlayerInfo * player);
	/** Swap un groupe de joueurs dans le camp d'en face (si possible) */
	static void swapGroupe(CodeTeam codeTeam);
	/** Met un joueur en spectateur */
	static void spec(IPlayerInfo * player);
	/** Met un groupe de joueurs en spectateur */
	static void specGroupe(CodeTeam codeTeam);
	/** Modifie la vie d'un joueur */
	static void setHealth(edict_t * pEntity, int nombreHP);
	/** Détruit l'arme d'un joueur situé à un slot donné */
	static void detruitArme(edict_t * pEntity, int slot);

	/** Retourne vrai si la std::string passée en paramètre est nulle ou est en commentaire "//"  <br>
	 * Cette fonction n'est utile et efficace que sur les chaînes extraites d'un fichier
	 */
	static bool estNulleOuCommentee(const std::string & chaine);
	/** Retourne une sous-chaîne de la chaîne passée en paramètre tronquée à la rencontre d'un \r, \n ou \r\n */
	static std::string extraitChaineFormelle(const std::string & chaine);
	/** Remplace les caractères interdits dans les noms de fichiers par des "-"
	 * - remplace les caractères interdits pour Windows
	 * - remplace les caractères d'échappement de la console
	 */
	static void valideNomDeFichier(std::string & nom);

	/** Valide une entité edict_t */
	static bool isValidePEntity(edict_t * pEntity);
	/** Valide un IPlayerInfo */
	static bool isValidePlayer(IPlayerInfo * player);
	/** Valide un index */
	static bool isValideIndex(int index);

	/** Ecrit un message dans la console et dans le log du serveur */
	static void debug(const std::string & message);
};

/** Function to initialize any cvars/command in this plugin 
 *
 * @author VALVE Corporation
 *
 */
void InitCVars( CreateInterfaceFn cvarFactory );

/** Useful helper func
 *
 * @author VALVE Corporation
 *
 */
inline bool FStrEq(const char *sz1, const char *sz2);

/** Associe à chaque menu créé dans le plugin un ID unique */
typedef enum CodeMenu
{
	/** Menu invalide */
	CODE_MENU_INVALIDE = 0,
	/** Menu du développeur */
	CODE_MENU_DEV,
	/** Menu listant les configurations disponibles */
	CODE_MENU_CONFIG,
	/** Menu demandant à l'arbitre si un CutRound doit être joué */
	CODE_MENU_CUTROUND_QUESTION,
	/** Menu lorsqu'aucun match n'est lancé */
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
	/** Menu de choix d'un joueur à kicker */
	CODE_MENU_KICK,
	/** Menu de choix d'un joueur à bannir */
	CODE_MENU_BAN,
	/** Menu de choix de la durée du ban */
	CODE_MENU_BAN_TIME,
	/** Menu de choix d'un joueur à swapper */
	CODE_MENU_SWAP,
	/** Menu de choix d'un joueur à mettre en spectateur */
	CODE_MENU_SPEC,
	/** Menu de fin de match (avec administration) */
	CODE_MENU_FINMATCH
};

/** Associe à une constante chaque type de messages qu'il est possible d'utiliser sous CSS */
enum CodeMessages
{
	MESSAGE_GEIGER = 0,
	MESSAGE_TRAIN,
	MESSAGE_HUDTEXT,
	/** Message dans le TCHAT de type 1 */
	MESSAGE_SAYTEXT,
	/** Message dans le TCHAT de type 2 */
	MESSAGE_SAYTEXT2,
	/** Message centré */
	MESSAGE_TEXTMSG,
	MESSAGE_HUDMSG,
	MESSAGE_RESETHUD,
	MESSAGE_GAMETITLE,
	MESSAGE_ITEMPICKUP,
	/** Message fenêtré de type "AMX" */
	MESSAGE_SHOWMENU,
	/** Fait trembler l'écran */
	MESSAGE_SHAKE,
	/** Fait un fondu de l'écran */
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
	/** Message dans le HUD centré en bas de l'écran */
	MESSAGE_HINTTEXT,
	MESSAGE_RELOADEFFECT,
	MESSAGE_PLAYERANIMEVENT,
	MESSAGE_AMMODENIED,
	MESSAGE_UPDATERADAR,
	MESSAGE_KILLCAM
};

/** Associe à une constante les différentes options de sensibilités possibles dans les messages de type "AMX" */
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
	/** Touche quelconque provoquant l'effacement de la fenêtre */
	OPTION_ANNULER = 1<<9,
	/** Toutes les options sensibles */
	OPTION_ALL = OPTION_ANNULER | OPTION_1 | OPTION_2 | OPTION_3 | OPTION_4 | OPTION_5 | OPTION_6 | OPTION_7 | OPTION_8 | OPTION_9

};

/** Texte associé à l'option "retour" dans les menus AMX */
#define MENU_RETOUR "Retour"
/** Texte associé à l'option "suivant" dans les menus AMX */
#define MENU_SUIVANT "Suivant"

/** Code associé à l'optoin "retour" dans les menus */
#define MENU_CODE_RETOUR -2

/** Code associé à l'optoin "suivant" dans les menus */
#define MENU_CODE_SUIVANT -1

/** Nombre d'octets limite dans les messages de type "AMX" */
#define MENU_TAILLE_LIMITE 255

/** Nom du fichier contenant la liste des maps à lister dans le menu */
#define FICHIER_MAPLIST "cfg/cssmatch/maplist.txt"

/** Dossier dans lequel sont recherchés les configurations de match */
#define DOSSIER_CONFIGURATIONS_MATCH "cfg/cssmatch/configurations"

#endif // __API_H__

