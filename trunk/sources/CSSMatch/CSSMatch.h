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

#ifndef __CSSMATCH_H__
#define __CSSMATCH_H__

/** Purpose: a sample 3rd party plugin class 
 *
 * @author VALVE Corporation
 *
 */
class CSSMatch: public IServerPluginCallbacks, public IGameEventListener2
{
private:
	/** Index du dernier joueur ayant exécuté une ConCommand */
	int m_iClientCommandIndex;
	/* Gestionnaire des arbitres */
	Authentification adminlist;
	/** Gestionnaire de match */
	Match match;
	/** Gestionnaire de timers */
	GestionTimers timers;

public:
	CSSMatch();
	~CSSMatch();

	// IServerPluginCallbacks methods
	 bool			Load(	CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory );
	 void			Unload( void );
	 void			Pause( void );
	 void			UnPause( void );
	 const char     *GetPluginDescription( void );      
	 void			LevelInit( char const *pMapName );
	 void			ServerActivate( edict_t *pEdictList, int edictCount, int clientMax );
	 void			GameFrame( bool simulating );
	 void			LevelShutdown( void );
	 void			ClientActive( edict_t *pEntity );
	 void			ClientDisconnect( edict_t *pEntity );
	 void			ClientPutInServer( edict_t *pEntity, char const *playername );
	 void			SetCommandClient( int index );
	 void			ClientSettingsChanged( edict_t *pEdict );
	 PLUGIN_RESULT	ClientConnect( bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen );
	 PLUGIN_RESULT	ClientCommand( edict_t *pEntity );
	 PLUGIN_RESULT	NetworkIDValidated( const char *pszUserName, const char *pszNetworkID );

	/** IGameEventListener Interface */
	 void FireGameEvent( IGameEvent * event );

	 int GetCommandIndex() { return m_iClientCommandIndex; }
	
	/** Retourne un pointeur sur l'instance Authentification du plugin <br>
	 * Nous ne metterons pas le qualificateur const pour permettre la modification de l'adminlist de l'extérieur
	 */
	Authentification * getAdminList();

	/** Retourne un pointeur sur l'instance Match du plugin */
	Match * getMatch();

	/** Retourne un pointeur sur le gestionnaire de timers du plugin */
	GestionTimers * getTimers();
};

/** Singleton du plugin */
extern CSSMatch g_CSSMatchPlugin;

#endif // __CSSMATCH_H__
