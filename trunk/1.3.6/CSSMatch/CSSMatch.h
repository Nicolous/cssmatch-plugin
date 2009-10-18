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

/** Singleton interfa�ant le plugin avec le jeu */
class CSSMatch: public IServerPluginCallbacks, Incopiable<>
{
private:
	/** Initialise une interface avec le jeu
	 *
	 * @param factory Le fabriquant de l'interface
	 * @param aInitialiser La variable � initialiser
	 * @param versionInterface Le nom de la version de l'interface � initialiser
	 */
	template<typename T>
	static bool getInterface(CreateInterfaceFn factory, T * & aInitialiser, const std::string & versionInterface);

		/** Interface avec le moteur */
	IVEngineServer * engine;

	/** Int�raction avec le syst�me de fichier */
	IFileSystem * filesystem;

	/** Interface �v�nementielle */
	IGameEventManager2 * gameeventmanager;

	/** Interface fournissant des informations sur les joueurs */
	IPlayerInfoManager * playerinfomanager;

	/* Int�raction avec les bots */
	//IBotManager * botmanager;

	/** Interfaces d'aide � l'�criture de plugins */
	IServerPluginHelpers * helpers;

	/* G�n�rateur de nombres al�atoire */
	//IUniformRandomStream * randomStr;

	/* Interface avec l'environnement virtuel */
	//IEngineTrace * enginetrace;

	/** Interface avec des variables globales */
	CGlobalVars * gpGlobals;

	/** Interface avec les ConVars */
	ICvar * cvars;

	/** Interface avec le serveur */
	IServerGameDLL * serverDll;

	/** Nombre limite de joueurs connect�s au serveur */
	int maxplayers;

	/** Nom du fichier contenant la liste des arbitres */
	std::string fichierAdminlist;

	/** Nom du fichier contenant la liste des maps � lister dans le menu */
	std::string fichierMaplist;

	/** Initialise les champs
	 *
	 * @param interfaceFactory 
	 * @param gameServerFactory
	 * @return faux si l'une des interfaces n'a pu �tre initialis�e, vrai sinon
	 */
	bool initialisation(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory);

	/** Map courante */
	std::string mapCourante;

	/** Index du dernier joueur ayant ex�cut� une ConCommand */
	int m_iClientCommandIndex;

	/** Ecouteur d'�v�nement */
	EventListener ecouteur;

	/** Gestionnaire des arbitres */
	Authentification adminlist;

	/** Gestionnaire de match */
	Match match;

	/** Gestionnaire de timers */
	GestionTimers timers;

	CSSMatch();
	~CSSMatch();

public:
	/** Prop correspondant � la r�serve d'argent des joueurs */
	PlayerProp propCash;

	/** Prop correspondant � la vitalit� des joueurs */
	PlayerProp propLifeState;

	/** Accesseur sur l'instance unique de la classe */
	static CSSMatch * getInstance();

	/**	Appel� par le jeu lors du chargement du plugin
	 *
	 * @param interfaceFactory 
	 * @param gameServerFactory 
	 * @see IServerPluginCallbacks#Load
	 * @see interface.h
	 *
	 */
	bool Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory);

	/**	Appel� par le jeu lors du d�chargement du plugin
	 *
	 * @see IServerPluginCallbacks#Unload
	 *
	 */
	void Unload();

	/**	Appel� par le jeu lorsque le plugin est mit en pause
	 *
	 * @see IServerPluginCallbacks#Pause
	 *
	 */
	void Pause();

	/**	Appel� par le jeu lorsque la pause du plugin est d�sactiv�e
	 *
	 * @see IServerPluginCallbacks#UnPause
	 *
	 */
	void UnPause();

	/**	Appel� par le jeu lors d'un plugin_print
	 *
	 * @return La description du plugin
	 * @see IServerPluginCallbacks#GetPluginDescription
	 *
	 */
	const char * GetPluginDescription();

	/**	Appel� par le jeu juste apr�s un changement de map
	 *
	 * @param pMapName Le nom de la nouvelle map
	 * @see IServerPluginCallbacks#LevelInit
	 *
	 */
	void LevelInit(const char * pMapName);

	/**	Appel� par le jeu lors d'un changement de map d�s que le serveur accepte les nouvelles connexions
	 *
	 * @param pEdictList La liste des entit�s (joueurs) connect�es au serveur
	 * @param edictCount Nom d'entit�s (joueurs) connect�es au serveur
	 * @param clientMax Le nombre limite de joueurs autoris�
	 * @see IServerPluginCallbacks#ServerActivate
	 *
	 */
	void ServerActivate(edict_t * pEdictList, int edictCount, int clientMax);

	/**	Appel� par le jeu � chaque trame du serveur <br>
	 * Note : n'est plus ex�cut� si aucun joueur n'est connect�
	 *
	 * @param simulating
	 * @see IServerPluginCallbacks#GameFrame
	 *
	 */
	void GameFrame(bool simulating);

	/**	Appel� par le jeu lorsque le serveur est quitt� ou lors d'un changement de map
	 * Note : peut �tre appel� plusieurs fois par changement de map
	 *
	 * @see IServerPluginCallbacks#LevelShutdown
	 *
	 */
	void LevelShutdown();

	/**	Appel� par le jeu lorsqu'un joueur arrive sur le serveur (est "activ�")
	 *
	 * @param pEntity L'entit� correspondant au joueur
	 * @see IServerPluginCallbacks#ClientActive
	 *
	 */
	void ClientActive(edict_t * pEntity);

	/**	Appel� par le jeu lorsqu'un joueur quitte le serveur
	 *
	 * @param pEntity L'entit� correspondant au joueur
	 * @see IServerPluginCallbacks#ClientDisconnect
	 *
	 */
	void ClientDisconnect(edict_t * pEntity);

	/**	Appel� par le jeu lorsqu'un joueur termine sa connexion au serveur
	 *
	 * @param pEntity L'entit� correspondant au joueur
	 * @param playername Le pseudo du joueur
	 * @see IServerPluginCallbacks#ClientPutInServer
	 *
	 */
	void ClientPutInServer(edict_t * pEntity, const char * playername);

	/**	Appel� par le jeu lorsqu'un joueur utilise une commande cliente
	 *
	 * @param index L'index du joueur
	 * @see IServerPluginCallbacks#SetCommandClient
	 *
	 */
	void SetCommandClient(int index);

	/**	Appel� par le jeu lorsqu'un joueur modifie une Convar marqu� par le flag FCVAR_REPLICATED
	 *
	 * @param pEdict L'entit� correspondant au joueur
	 * @see IServerPluginCallbacks#ClientSettingsChanged
	 * @see convar.h
	 *
	 */
	void ClientSettingsChanged(edict_t * pEdict);


	/**	Appel� par le jeu lorsqu'un joueur ent�me une connexion au serveur
	 *
	 * @param bAllowConnect true si le joueur est autoris� � se connecter
	 * @param pEntity L'entit� correspondant au joueur
	 * @param pszName Le pseudo du joueur
	 * @param pszAddress Adresse IP du joueur ?
	 * @param reject Motif du refus de la connexion si bAllowConnect est pass� � false
	 * @param maxrejectlen Taille maximale de la variable reject (VIVE std !!!)
	 * @return La d�cision PLUGIN_RESULT du plugin
	 * @see IServerPluginCallbacks#ClientConnect
	 * @see L'�num�ration PLUGIN_RESULT (iserverplugin.h)
	 *
	 */
	PLUGIN_RESULT ClientConnect(bool * bAllowConnect,
								edict_t * pEntity,
								const char * pszName,
								const char * pszAddress,
								char *reject,
								int maxrejectlen);

	/**	Appel� par le jeu lorsqu'un joueur tape une commande (non CON_COMMAND) dans sa console
	 *
	 * @param pEntity L'entit� correspondant au joueur
	 * @return La d�cision PLUGIN_RESULT du plugin
	 * @see IServerPluginCallbacks#ClientCommand
	 * @see L'�num�ration PLUGIN_RESULT (iserverplugin.h)
	 *
	 */
	PLUGIN_RESULT ClientCommand(edict_t *pEntity);

	/**	Appel� par le jeu lorsque le steamID d'un joueur qui s'est connect� au serveur est authentifi�
	 *
	 * @param pszUserName Le pseudo du joueur
	 * @param pszNetworkID Le steamID du joueur
	 * @return La d�cision PLUGIN_RESULT du plugin
	 * @see IServerPluginCallbacks#NetworkIDValidated
	 * @see L'�num�ration PLUGIN_RESULT (iserverplugin.h)
	 *
	 */
	PLUGIN_RESULT NetworkIDValidated(const char * pszUserName, const char * pszNetworkID);

	IVEngineServer * getEngine();

	IFileSystem * getFileSystem();

	IGameEventManager2 * getGameEventManager();

	IPlayerInfoManager * getPlayerInfoManager();

	IServerPluginHelpers * getServerPluginHelpers();

	CGlobalVars * getGlobalVars();

	ICvar * getCVars();

	IServerGameDLL * getServerGameDll();

	int getMaxPlayers() const;

	/** Accesseur sur la map courante
	 *
	 * @return Le nom de la map courante
	 */
	const std::string & getMapCourante() const;

	/** Accesseur sur l'index du dernier joueur qui a utilis� une commande cliente
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

	/** Accesseur sur le dernier match <br>
	* Note : Si aucun match n'a eu lieu l'intance retourn�e est celle du constructeur par d�faut de Match
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
};

#endif // __CSSMATCH_H__
