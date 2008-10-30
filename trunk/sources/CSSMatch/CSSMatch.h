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
#include "../Authentification/Authentification.h"
#include "../ConVars/ConVars.h"
#include "../GestionJoueurs/GestionJoueurs.h"
#include "../Messages/Messages.h"
#include "../GestionTimers/GestionTimers.h"
#include "../Decompte/Decompte.h"
#include "../Match/Match.h"
#include "../EventListener/EventListener.h"
#include "../PlayerProp/PlayerProp.h"

#ifndef __CSSMATCH_H__
#define __CSSMATCH_H__

/** Singleton interfaçant le plugin avec le jeu */
class CSSMatch: public IServerPluginCallbacks
{
private:
	/** Index du dernier joueur ayant exécuté une ConCommand */
	int m_iClientCommandIndex;

	/** Ecouteur d'évènement */
	EventListener ecouteur;

	/** Gestionnaire des arbitres */
	Authentification adminlist;

	/** Gestionnaire de match */
	Match match;

	/** Gestionnaire de timers */
	GestionTimers timers;

	CSSMatch();
	~CSSMatch();

	/** Instance unique de la classe */
	static CSSMatch * instance;

public:	
	/** Prop correspondant à la réserve d'argent des joueurs */
	PlayerProp propCash;

	/** Prop correspondant à la vitalité des joueurs */
	PlayerProp propLifeState;

	/** Accesseur sur l'instance unique de la classe */
	static CSSMatch * getInstance();

	/**	Appelé par le jeu lors du chargement du plugin
	 *
	 * @param interfaceFactory 
	 * @param gameServerFactory 
	 * @see IServerPluginCallbacks#Load
	 * @see interface.h
	 *
	 */
	bool Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory);

	/**	Appelé par le jeu lors du déchargement du plugin
	 *
	 * @see IServerPluginCallbacks#Unload
	 *
	 */
	void Unload();

	/**	Appelé par le jeu lorsque le plugin est mit en pause
	 *
	 * @see IServerPluginCallbacks#Pause
	 *
	 */
	void Pause();

	/**	Appelé par le jeu lorsque la pause du plugin est désactivée
	 *
	 * @see IServerPluginCallbacks#UnPause
	 *
	 */
	void UnPause();

	/**	Appelé par le jeu lors d'un plugin_print
	 *
	 * @return La description du plugin
	 * @see IServerPluginCallbacks#GetPluginDescription
	 *
	 */
	const char * GetPluginDescription();

	/**	Appelé par le jeu juste après un changement de map
	 *
	 * @param pMapName Le nom de la nouvelle map
	 * @see IServerPluginCallbacks#LevelInit
	 *
	 */
	void LevelInit(const char * pMapName);

	/**	Appelé par le jeu lors d'un changement de map dès que le serveur accepte les nouvelles connexions
	 *
	 * @param pEdictList La liste des entités (joueurs) connectées au serveur
	 * @param edictCount Nom d'entités (joueurs) connectées au serveur
	 * @param clientMax Le nombre limite de joueurs autorisé
	 * @see IServerPluginCallbacks#ServerActivate
	 *
	 */
	void ServerActivate(edict_t * pEdictList, int edictCount, int clientMax);

	/**	Appelé par le jeu à chaque trame du serveur <br/>
	 * Note : n'est plus exécuté si aucun joueur n'est connecté
	 *
	 * @param simulating
	 * @see IServerPluginCallbacks#GameFrame
	 *
	 */
	void GameFrame(bool simulating);

	/**	Appelé par le jeu lorsque le serveur est quitté ou lors d'un changement de map
	 * Note : peut être appelé plusieurs fois par changement de map
	 *
	 * @see IServerPluginCallbacks#LevelShutdown
	 *
	 */
	void LevelShutdown();

	/**	Appelé par le jeu lorsqu'un joueur arrive sur le serveur (est "activé")
	 *
	 * @param pEntity L'entité correspondant au joueur
	 * @see IServerPluginCallbacks#ClientActive
	 *
	 */
	void ClientActive(edict_t * pEntity);

	/**	Appelé par le jeu lorsqu'un joueur quitte le serveur
	 *
	 * @param pEntity L'entité correspondant au joueur
	 * @see IServerPluginCallbacks#ClientDisconnect
	 *
	 */
	void ClientDisconnect(edict_t * pEntity);

	/**	Appelé par le jeu lorsqu'un joueur termine sa connexion au serveur
	 *
	 * @param pEntity L'entité correspondant au joueur
	 * @param playername Le pseudo du joueur
	 * @see IServerPluginCallbacks#ClientPutInServer
	 *
	 */
	void ClientPutInServer(edict_t * pEntity, const char * playername);

	/**	Appelé par le jeu lorsqu'un joueur utilise une commande cliente
	 *
	 * @param index L'index du joueur
	 * @see IServerPluginCallbacks#SetCommandClient
	 *
	 */
	void SetCommandClient(int index);

	/**	Appelé par le jeu lorsqu'un joueur modifie une Convar marqué par le flag FCVAR_REPLICATED
	 *
	 * @param pEdict L'entité correspondant au joueur
	 * @see IServerPluginCallbacks#ClientSettingsChanged
	 * @see convar.h
	 *
	 */
	void ClientSettingsChanged(edict_t * pEdict);


	/**	Appelé par le jeu lorsqu'un joueur entâme une connexion au serveur
	 *
	 * @param bAllowConnect true si le joueur est autorisé à se connecter
	 * @param pEntity L'entité correspondant au joueur
	 * @param pszName Le pseudo du joueur
	 * @param pszAddress Adresse IP du joueur ?
	 * @param reject Motif du refus de la connexion si bAllowConnect est passé à false
	 * @param maxrejectlen Taille maximale de la variable reject (VIVE std !!!)
	 * @return La décision PLUGIN_RESULT du plugin
	 * @see IServerPluginCallbacks#ClientConnect
	 * @see L'énumération PLUGIN_RESULT (iserverplugin.h)
	 *
	 */
	PLUGIN_RESULT ClientConnect(bool * bAllowConnect,
								edict_t * pEntity,
								const char * pszName,
								const char * pszAddress,
								char *reject,
								int maxrejectlen);

	/**	Appelé par le jeu lorsqu'un joueur tape une commande (non CON_COMMAND) dans sa console
	 *
	 * @param pEntity L'entité correspondant au joueur
	 * @return La décision PLUGIN_RESULT du plugin
	 * @see IServerPluginCallbacks#ClientCommand
	 * @see L'énumération PLUGIN_RESULT (iserverplugin.h)
	 *
	 */
	PLUGIN_RESULT ClientCommand(edict_t *pEntity);

	/**	Appelé par le jeu lorsque le steamID d'un joueur qui s'est connecté au serveur est authentifié
	 *
	 * @param pszUserName Le pseudo du joueur
	 * @param pszNetworkID Le steamID du joueur
	 * @return La décision PLUGIN_RESULT du plugin
	 * @see IServerPluginCallbacks#NetworkIDValidated
	 * @see L'énumération PLUGIN_RESULT (iserverplugin.h)
	 *
	 */
	PLUGIN_RESULT NetworkIDValidated(const char * pszUserName, const char * pszNetworkID);

	/** Accesseur sur l'index du dernier joueur qui a utilisé une commande cliente
	*
	* @return L'index du joueur
	*
	*/
	int GetCommandIndex();

	/** Accesseur sur les arbitres
	*
	* @return Un pointeur sur le gestionnaire de la liste des arbitres
	*
	*/
	Authentification * getAdminList();

	/** Accesseur sur le dernier match <br/>
	* Note : Si aucun match n'a eu lieu l'intance retournée est celle du constructeur par défaut de Match
	*
	* @return Un pointeur sur le dernier match
	*
	*/
	Match * getMatch();

	/** Accesseur sur le gestionnaire de timers
	*
	* @return Un pointeur sur le gestionnaire de timers
	*
	*/
	GestionTimers * getTimers();

	/** Détermine lorsqu'une team est prête lors des strats 
	 *
	 * @param team Le pointeur sur la team concernée
	 * @param pEntity Le joueur qui a déclaré la team prête
	 *
	 */
	void goTeam(Team * team, edict_t * pEntity);
};

#endif // __CSSMATCH_H__
