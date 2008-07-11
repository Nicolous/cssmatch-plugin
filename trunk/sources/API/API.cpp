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
 
#include "API.h"

using std::string;
using std::vector;
using std::ostringstream;
using std::exception;

IVEngineServer * Api::engine = NULL;
IFileSystem * Api::filesystem = NULL;
IGameEventManager2 * Api::gameeventmanager = NULL;
IPlayerInfoManager * Api::playerinfomanager = NULL;
//IBotManager * Api::botmanager = NULL;
IServerPluginHelpers * Api::helpers = NULL;
//IUniformRandomStream * Api::randomStr = NULL;
//IEngineTrace * Api::enginetrace = NULL;
CGlobalVars * Api::gpGlobals = NULL;
ICvar * Api::cvars = NULL;
IServerGameDLL * Api::serverDll = NULL;

// Initialisons de maxplayers à DEFAUT_MAXPLAYERS (nombre maximum de slots) 
//		tant que ce nombre ne peut pas être plus précis
// Au premier appel de CSSMatch::ServerActivate(), 
//		ce nombre doit être actualisé de manière à ce que la suite du code en prenne compte

int Api::maxplayers = MAX_PLAYERS;

template<typename T>
bool Api::getInterface(CreateInterfaceFn factory, T * & aInitialiser, const string & versionInterface)
{
	bool succes = true;

	aInitialiser = (T *)factory(versionInterface.c_str(),NULL);

	/*if (aInitialiser != NULL)
	{
		Msg(string("CSSMatch : Interface \"" + versionInterface + "\" construite avec succes\n").c_str());
	}
	else 
	{
		Msg(string("CSSMatch : Impossible d'obtenir l'interface \"" + versionInterface + "\" !\n").c_str());
		succes = false;
	}*/
	if (aInitialiser == NULL)
	{
		Msg(string("CSSMatch : Impossible d'obtenir l'interface \"" + versionInterface + "\" !\n").c_str());
		succes = false; 
	}

	return succes;
}

bool Api::initialisation(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory)
{
	bool succes =	getInterface<IPlayerInfoManager>(gameServerFactory,playerinfomanager,INTERFACEVERSION_PLAYERINFOMANAGER)	&&
					getInterface<IVEngineServer>(interfaceFactory,engine,INTERFACEVERSION_VENGINESERVER)						&&
					getInterface<IGameEventManager2>(interfaceFactory,gameeventmanager,INTERFACEVERSION_GAMEEVENTSMANAGER2)		&&
					// getInterface<IBotManager>(gameServerFactory,botmanager,INTERFACEVERSION_PLAYERBOTMANAGER)				&&
					getInterface<IFileSystem>(interfaceFactory,filesystem,FILESYSTEM_INTERFACE_VERSION)							&&
					getInterface<IServerPluginHelpers>(interfaceFactory,helpers,INTERFACEVERSION_ISERVERPLUGINHELPERS)			&&
					//getInterface<IEngineTrace>(interfaceFactory,enginetrace,INTERFACEVERSION_ENGINETRACE_SERVER)				&&
					//getInterface<IUniformRandomStream>(interfaceFactory,randomStr,VENGINE_SERVER_RANDOM_INTERFACE_VERSION)	&&
					getInterface<ICvar>(interfaceFactory,cvars,VENGINE_CVAR_INTERFACE_VERSION)									&&
					//getInterface<IServerGameDLL>(gameServerFactory,serverDll,INTERFACEVERSION_SERVERGAMEDLL)					&&
					getInterface<IServerGameDLL>(gameServerFactory,serverDll,"ServerGameDLL006");

	if (playerinfomanager != NULL)
			succes &= (gpGlobals = playerinfomanager->GetGlobalVars()) != NULL;

	if (gpGlobals != NULL)
		Msg("CSSMatch : Les variables globales sont accessibles\n");
	else
		Msg("CSSMatch : Impossible d'acceder aux variables globales\n");

	return succes;
}


void Api::serveurExecute(const string & cmd)
{
	// Insertion de la commande
	engine->ServerCommand(cmd.c_str());

	// Exécution des commandes en attente
	//engine->ServerExecute();
}

void Api::configExecute(const string & configPath)
{
	string commande("exec " + configPath + "\n");

	// Insertion de la commande
	engine->ServerCommand(commande.c_str());

	// Exécution des commandes en attente
	engine->ServerExecute();

	// Exécution des commandes en attente (celles lues dans le fichier de configuration)
	engine->ServerExecute();
}

bool Api::hltvPresente()
{
	/*if (!ConVars::tv_enable || !ConVars::tv_enable->GetBool())
		return false;
	Ce n'est pas le rôle de cette fonction ! */

	bool hltvPresente = false;

	// Recherche SourceTV parmis les joueurs
	edict_t * pEntity = NULL;
	IPlayerInfo * player = NULL;
	for (int i=1;i<=maxplayers;i++)
	{
		try
		{
			pEntity = getPEntityFromIndex(i);

			// On n'utilise pas getIPlayerFromEntity qui exclue sourceTV
			player = playerinfomanager->GetPlayerInfo(pEntity);
			if (player && player->IsConnected() && player->IsHLTV())
			{
				hltvPresente = true;
				i = maxplayers + 1;
			}
		}
		catch(const CSSMatchApiException & e) {}
	}
	return hltvPresente;
}

IPlayerInfo * Api::getIPlayerInfoFromEntity(edict_t * pEntity) throw (CSSMatchApiException)
{
	// Pour des raisons de stabilité nous [re]faisons un test sur l'entité ici
	if (isValidePEntity(pEntity))
	{
		IPlayerInfo * player = playerinfomanager->GetPlayerInfo(pEntity);
		if (isValidePlayer(player))
			return player;
		else
			throw CSSMatchApiException("Impossible de trouver l'IPlayerInfo de cette entitÃ©");
	}
	else
		throw CSSMatchApiException("Impossible de trouver l'IPlayerInfo d'une entitÃ© invalide");
}

edict_t * Api::getPEntityFromID(int userid) throw (CSSMatchApiException)
{
	// Recherche de l'entité portant cet userid
	edict_t * pEntity = NULL;
	for (int i=1;i<=maxplayers;i++)
	{
		try
		{
			pEntity = getPEntityFromIndex(i);
			if (engine->GetPlayerUserId(pEntity) == userid)
				return pEntity;
		}
		catch(const CSSMatchApiException & e) {}
	}

	throw CSSMatchApiException("Impossible de trouver l'entitÃ© correspond Ã  cet ID");
}

edict_t * Api::getPEntityFromIndex(int index) throw (CSSMatchApiException)
{
	// Recherche de l'entité portant cet index
	edict_t * pEntity = engine->PEntityOfEntIndex(index);
	if (isValidePEntity(pEntity))
		return pEntity;

	throw CSSMatchApiException("Impossible de trouver l'entitÃ© correspond Ã  cet index");
}

int Api::getIndexFromUserId(int userid)
{
	int index = INDEX_INVALIDE;

	// Récupération de l'index associée à l'ID
	try
	{
		edict_t * pEntity = Api::getPEntityFromID(userid);
		index = engine->IndexOfEdict(pEntity);
	}
	catch(const CSSMatchApiException & e)
	{
		reporteException(e,__FILE__,__LINE__);
	}
	return index;
}

int Api::getUseridFromPEntity(edict_t * pEntity)
{
	int userid = USERID_INVALIDE;

	// Récupération d'un pointeur sur le joueur
	try
	{
		IPlayerInfo * player = getIPlayerInfoFromEntity(pEntity);
		userid = player->GetUserID();
	}
	catch(const CSSMatchApiException & e)
	{
		reporteException(e,__FILE__,__LINE__);
	}
	return userid;
}

int Api::getUseridFromUsername(const string & pseudo)
{
	int userid = USERID_INVALIDE;

	edict_t * pEntity = NULL;
	IPlayerInfo * player = NULL;
	for (int i=1;i<=maxplayers;i++)
	{
		try
		{
			pEntity = getPEntityFromIndex(i);
			player = getIPlayerInfoFromEntity(pEntity);
			string name(player->GetName());
			if (pseudo == name)
			{
				userid = player->GetUserID();
				i = maxplayers + 1;
			}
		}
		catch(const CSSMatchApiException & e){}
	}
	return userid;
}

int Api::getIndexFromUsername(const string & pseudo)
{
	int index = INDEX_INVALIDE;

	edict_t * pEntity = NULL;
	IPlayerInfo * player = NULL;
	for (int i=1;i<=maxplayers;i++)
	{
		try
		{
			pEntity = getPEntityFromIndex(i);
			player = getIPlayerInfoFromEntity(pEntity);
			string name(player->GetName());
			if (pseudo == name)
				index = engine->IndexOfEdict(pEntity);
		}
		catch(const CSSMatchApiException & e){}
	}
	return index;
}

CodeTeam Api::getPEntityTeam(edict_t * pEntity)
{
	CodeTeam indexTeam = TEAM_INVALIDE;

	// Pour des raisons de stabilité nous [re]faisons un test sur l'entité ici
	if (isValidePEntity(pEntity))
	{
		edict_t * pEntityTemp = NULL;
		IPlayerInfo * player = NULL;
		for (int i=1;i<=maxplayers;i++)
		{
			try
			{
				pEntityTemp = getPEntityFromIndex(i);
				player = getIPlayerInfoFromEntity(pEntity);
				if (pEntity == pEntityTemp)
					indexTeam = (CodeTeam)player->GetTeamIndex();
			}
			catch(const CSSMatchApiException & e){}
		}
	}
	return indexTeam;
}

IServerEntity * Api::getIServerEntityFromEdict(edict_t * pEntity) throw (CSSMatchApiException)
{
	// Pour des raisons de stabilité nous [re]faisons un test sur l'entité ici
	if (isValidePEntity(pEntity))
	{
		// Récupération d'un pointeur générique sur le joueur
		IServerEntity * sEntity = pEntity->GetIServerEntity();
		if (sEntity != NULL)
			return sEntity;
		else
			throw CSSMatchApiException("L'IServerEntity de cette entitÃ© est nulle");
	}
	else
		throw CSSMatchApiException("Impossible de trouver l'IServerEntity d'une entite invalide");
}

CBaseEntity * Api::getCBaseEntityFromEntity(edict_t * pEntity) throw (CSSMatchApiException)
{
	// Récupération d'un pointeur générique valide sur le joueur
	IServerEntity * sEntity = NULL;
	CBaseEntity * bEntity = NULL;
	try
	{
		sEntity = getIServerEntityFromEdict(pEntity);
		bEntity = sEntity->GetBaseEntity();
	}
	catch(const CSSMatchApiException & e)
	{
		reporteException(e,__FILE__,__LINE__);
		throw CSSMatchApiException("Impossible de trouver le BaseEntityEdict de cette entite");
	}

	if (bEntity != NULL)
		return bEntity;
	else
		throw CSSMatchApiException("Le CBaseEntity de cette entite est nul");
}

CBaseCombatCharacter * Api::getCBaseCombatCharacterFromEntity(edict_t * pEntity) throw (CSSMatchApiException)
{
	CBaseEntity * bEntity = NULL;
	CBaseCombatCharacter * cCharacter = NULL;
	try
	{
		// Récupération d'un pointeur générique valide sur l'entité
		bEntity = getCBaseEntityFromEntity(pEntity);

		cCharacter = bEntity->MyCombatCharacterPointer();
	}
	catch(const CSSMatchApiException & e)
	{
		reporteException(e,__FILE__,__LINE__);
		throw CSSMatchApiException("Impossible de trouver le CBaseCombatCharater d'une CBaseEntity invalide");
	}

	if (cCharacter != NULL)
		return cCharacter;
	else
		throw CSSMatchApiException("Impossible de trouver le CBaseCombatCharater de cette entite");
}

CBaseCombatWeapon * Api::getCBaseCombatWeaponFromSlot(edict_t * pEntity, CodeWeaponSlot slot) throw (CSSMatchApiException)
{
	CBaseCombatCharacter * cCharacter = NULL;
	CBaseCombatWeapon * cWeapon = NULL;
	try
	{
		// Récupération d'un pointeur générique valide sur l'entité
		cCharacter = getCBaseCombatCharacterFromEntity(pEntity);

		// Récupération d'un pointeur sur l'arme situé à ce slot
		cWeapon = cCharacter->Weapon_GetSlot(slot);
	}
	catch(const CSSMatchApiException & e)
	{
		reporteException(e,__FILE__,__LINE__);
		throw CSSMatchApiException("Impossible de trouver le CBaseCombatWeapon d'un CBaseCombatCharacter invalide");
	}
	if (cWeapon != NULL)
		return cWeapon;
	else
		throw CSSMatchApiException("Impossible de trouver le CBaseCombatWeapon de cette entite");
}

CBasePlayer * Api::getCBasePlayerFromEntity(edict_t * pEntity) throw (CSSMatchApiException)
{
	// Pour des raisons de stabilité nous [re]faisons un test sur l'entité ici
	if (isValidePEntity(pEntity))
	{
		CBasePlayer * cPlayer = reinterpret_cast<CBasePlayer*>(pEntity->GetUnknown()->GetBaseEntity());
		if (cPlayer != NULL)
			return cPlayer;
		else
			throw CSSMatchApiException("Impossible de trouver le CBasePlayer de cette entite");
	}
	else
		throw CSSMatchApiException("Impossible de trouver le CBasePlayer d'une entitÃ© invalide");
}

int Api::getPlayerCount(CodeTeam codeTeam /* critère de recherche : un camp particulière ou tous*/)
{
	// Variable contenant le nombre de joueurs correspond au critère de recherche
	int count = 0;

	// Le codeTeam correspond à un camp particulier
	edict_t * pEntity = NULL;
	IPlayerInfo * player = NULL;
	if (codeTeam != TEAM_ALL)
	{
		// On compte tous les joueurs dans l'index de la team correspond au codeTeam
		for (int i=1;i<=maxplayers;i++)
		{
			try
			{
				pEntity = getPEntityFromIndex(i);
				player = getIPlayerInfoFromEntity(pEntity);
				if (player->GetTeamIndex() == codeTeam)
					count++;
			}
			catch(const CSSMatchApiException & e){}
		}
	}
	// Le codeTeam correspond à toutes les teams confondues
	else
	{
		for (int i=1;i<=maxplayers;i++)
		{
			try
			{
				// On compte tous les joueurs
				pEntity = getPEntityFromIndex(i); // Vérifie aussi si le joueur  est valide
				count++;
			}
			catch(const CSSMatchApiException & e) {}
		}
	}
	return count;
}

string Api::getPlayerName(edict_t * pEntity) throw (CSSMatchApiException)
{
	// Pour des raisons de stabilité nous [re]faisons un test sur l'entité ici
	if (isValidePEntity(pEntity))
	{
		// Récupération du pseudo
		IPlayerInfo * player = NULL;
		try
		{
			player = getIPlayerInfoFromEntity(pEntity);
		}
		catch(const CSSMatchApiException & e)
		{
			// ...Arrive typiquement lorsque le match est arbitré par RCON
			//reporteException(e,__FILE__,__LINE__);
			throw CSSMatchApiException("Impossible de trouver le pseudo d'un joueur invalide");
		}
		const char * pseudo = player->GetName();

		// Vérification de la validité du retour de VALVE
		if (pseudo != NULL)
			return pseudo;
		else
			throw CSSMatchApiException("Impossible de trouver le pseudo de cette entitÃ©");
	}
	else
		throw CSSMatchApiException("Impossible de trouver le pseudo d'une entitÃ© invalide");
}

string Api::getPlayerSteamID(edict_t * pEntity) throw (CSSMatchApiException)
{
	// Pour des raisons de stabilité nous [re]faisons un test sur l'entité ici
	if (isValidePEntity(pEntity))
	{
		// Récupération du steamID
		const char * steamID = engine->GetPlayerNetworkIDString(pEntity);
		// Vérification de la validité du retour de VALVE
		if (steamID != NULL)
			return steamID;
		else
			throw CSSMatchApiException("Impossible de trouver la steamID de cette entite");
	}
	else
		throw CSSMatchApiException("Impossible de trouver la steamID d'une entite invalide");
}

void Api::kickid(int useridCible, const string & motif)
{
	ostringstream cmd;
	cmd << "kickid " << useridCible << " " << motif << "\n";

	Api::serveurExecute(cmd.str());
}

void Api::banid (int temps, int useridCible, const std::string & motif)
{
	ostringstream cmd;
	cmd << "banid " << temps << " " << useridCible  << "\n";
	Api::serveurExecute(cmd.str());

	// Si le ban est permanent on l'écrit dans le banned_user.cfg
	if (temps == 0)
		Api::serveurExecute("writeid\n");

	kickid(useridCible,motif);
}

void Api::cexec(edict_t * pEntity, const string & cmd)
{
	// On vérifie que la team du joueur est valide et différent de "non-assignée" (cas particulier d'un bot créé malgré le paramètre -nobots)
	CodeTeam codeTeam = getPEntityTeam(pEntity); // contrôle également la validité de l'entité
	if (codeTeam>TEAM_UN)
		engine->ClientCommand(pEntity,cmd.c_str());
}

void Api::cexecGroupe(CodeTeam codeTeam, const string & cmd)
{
	// Nous ferons 2 boucles séparées pour optimiser le code

	// Commande à exécuter chez tous les joueurs
	edict_t * pEntity = NULL;
	IPlayerInfo * player = NULL;

	if (codeTeam==TEAM_ALL)
	{
		for (int i=1;i<=Api::maxplayers;i++)
		{
			try
			{
				pEntity = getPEntityFromIndex(i);
				player = getIPlayerInfoFromEntity(pEntity);
				if (! player->IsFakeClient())
					cexec(pEntity,cmd);
			}
			catch(const CSSMatchApiException & e) {}
		}
	}
	else
	{
		// Commande à n'exécuter que pour une certaine camp
		for (int i=1;i<=Api::maxplayers;i++)
		{
			try
			{
				pEntity = getPEntityFromIndex(i);
				player = getIPlayerInfoFromEntity(pEntity);
				if (player->IsFakeClient()==false && player->GetTeamIndex()==codeTeam)
					cexec(pEntity,cmd);
			}
			catch(const CSSMatchApiException & e) {}
		}
	}
}

void Api::sexec(edict_t * pEntity, const string & cmd)
{
	// On vérifie que la team du joueur est valide et différent de "non-assignée" (cas particulier d'un bot créé malgré le paramètre -nobots)
	CodeTeam codeTeam = getPEntityTeam(pEntity); // contrôle également la validité de l'entité
	if (codeTeam>TEAM_UN)
		helpers->ClientCommand(pEntity,cmd.c_str());
}

void Api::sexecGroupe(CodeTeam codeTeam, const string & cmd)
{
	// Nous ferons 2 boucles séparées pour optimiser le code

	// Commande à exécuter chez tous les joueurs
	edict_t * pEntity = NULL;
	IPlayerInfo * player = NULL;

	if (codeTeam == TEAM_ALL)
	{
		for (int i=1;i<=Api::maxplayers;i++)
		{
			try
			{
				pEntity = getPEntityFromIndex(i);
				player = getIPlayerInfoFromEntity(pEntity);
				if (!player->IsFakeClient())
					sexec(pEntity,cmd);
			}
			catch(const CSSMatchApiException & e){}
		}
	}
	// Commande à n'exécuter que pour un certain camp
	else
	{
		for (int i=1;i<=Api::maxplayers;i++)
		{
			try
			{
				pEntity = getPEntityFromIndex(i);
				player = getIPlayerInfoFromEntity(pEntity);
				if (!player->IsFakeClient() && player->GetTeamIndex()==codeTeam)
					sexec(pEntity,cmd);
			}
			catch(const CSSMatchApiException & e){}
		}
	}
}

void Api::swap(IPlayerInfo * player)
{
	// Pour des raisons de stabilité nous [re]faisons un test sur IPlayerInfo ici
	if (isValidePlayer(player))
	{
		// Selon le camp actuel du joueur, nous le swappons dans le camp opposée
		// Les bots seront kickés plutôt que swappés, pour éviter qu'il ne renaissent pas
		int teamActuelle = player->GetTeamIndex();
		switch(teamActuelle)
		{
		case TEAM_T:
			if (player->IsFakeClient())
				kickid(player->GetUserID(),"Swap Bot");
			else
				player->ChangeTeam(TEAM_CT);
			break;
		case TEAM_CT:
			if (player->IsFakeClient())
				kickid(player->GetUserID(),"Swap Bot");
			else
				player->ChangeTeam(TEAM_T);
		}
	}
}

void Api::swapGroupe(CodeTeam codeTeam)
{
	// Nous ferons 2 boucles séparées pour optimiser le code

	// Commande à exécuter chez tous les joueurs
	edict_t * pEntity = NULL;
	IPlayerInfo * player = NULL;

	if (codeTeam==TEAM_ALL)
	{
		for (int i=1;i<=Api::maxplayers;i++)
		{
			try
			{
				pEntity = getPEntityFromIndex(i);
				player = getIPlayerInfoFromEntity(pEntity);
				swap(player); // nous savons que swap valide l'IPlayerInfo
			}
			catch(const CSSMatchApiException & e){}
		}
	}
	// Commande à n'exécuter que pour un certain camp
	else
	{
		for (int i=1;i<=Api::maxplayers;i++)
		{
			try
			{
				pEntity = getPEntityFromIndex(i);
				player = getIPlayerInfoFromEntity(pEntity);
				if (player->GetTeamIndex()==codeTeam)
					swap(player);
			}
			catch(const CSSMatchApiException & e){}
		}
	}
}

void Api::spec(IPlayerInfo * player)
{
	// Pour des raisons de stabilité nous [re]faisons un test sur IPlayerInfo ici
	if (isValidePlayer(player))
	{
		// On ne placera pas les bots en spectateurs, on les kick pour éviter qu'ils ne soient "bloqués"
		if (player->IsFakeClient())
			Api::kickid(player->GetUserID(),"CSSMatch : Spec Bot");
		else
			player->ChangeTeam(TEAM_SPEC);
	}
}

void Api::specGroupe(CodeTeam codeTeam)
{
	// Nous ferons 2 boucles séparées pour optimiser le code

	// Commande à exécuter chez tous les joueurs
	edict_t * pEntity = NULL;
	IPlayerInfo * player = NULL;

	if (codeTeam==TEAM_ALL)
	{
		for (int i=1;i<=Api::maxplayers;i++)
		{
			try
			{
				pEntity = getPEntityFromIndex(i);
				player = getIPlayerInfoFromEntity(pEntity);
				spec(player); // nous savons que spec valide l'IPlayerInfo
			}
			catch(const CSSMatchApiException & e){}
		}
	}
	// Commande à n'exécuter que pour un certain camp
	else
	{
		for (int i=1;i<=Api::maxplayers;i++)
		{
			try
			{
				pEntity = getPEntityFromIndex(i);
				player = getIPlayerInfoFromEntity(pEntity);
				if (player->GetTeamIndex()==codeTeam)
					spec(player);
			}
			catch(const CSSMatchApiException & e){}
		}
	}
}

void Api::setHealth(edict_t * pEntity, int nombreHP)
{
	// Pour des raisons de stabilité nous [re]faisons un test sur IPlayerInfo ici
	if (isValidePEntity(pEntity))
	{
		// On vérifie que le joueur n'est pas mort
		IPlayerInfo * player = NULL;
		try
		{
			player = getIPlayerInfoFromEntity(pEntity);
			if (! player->IsDead())
			{
				// Si la vie que l'on va ôter au joueur le nécessite, on le tue simplement
				if (nombreHP <= 0)
					cexec(pEntity,"kill\n");
				// Sinon on modifie sa vie
				else
				{
					CBasePlayer * bPlayer = getCBasePlayerFromEntity(pEntity);
					bPlayer->SetHealth(nombreHP);
				}
			}
		}
		catch(const CSSMatchApiException & e)
		{
			reporteException(e,__FILE__,__LINE__);
			return;
		}
	}
}

void Api::detruitArme(edict_t * pEntity, CodeWeaponSlot slot)
{
	// Récupération d'un pointeur valide sur l'arme
	try
	{
		CBaseCombatWeapon * weapon = getCBaseCombatWeaponFromSlot(pEntity,slot);
		weapon->Kill();
	}
	catch(const CSSMatchApiException & e)
	{
		//reporteException(e,__FILE__,__LINE__);
	}
}

bool Api::estNulleOuCommentee(const string & chaine)
{
	bool nulleOuCommentee = false;

	int tailleChaine = (int)chaine.size();

	// Est-ce que la chaine est nulle ou ne contient qu'un \n ?
	if (tailleChaine==0 || chaine[0]=='\r' || chaine[0]=='\n' || chaine[0]=='\0')
		nulleOuCommentee = true;
	else
	{
		// On vérifie que la chaîne n'est pas commentée (en ignorant les espaces et les tabulations situés en début de chaîne)
		int indexCaractere = 0;
		while(indexCaractere<tailleChaine && (chaine[indexCaractere]==' ' || chaine[indexCaractere]=='\t')) { indexCaractere++; }
		if (tailleChaine>=2 && chaine.substr(indexCaractere,2)=="//")
			nulleOuCommentee = true;
	}

	// La chaîne n'est pas commentée
	return nulleOuCommentee;
}

const string Api::extraitChaineFormelle(const string & chaine)
{
	// On tronque la chaîne au premier retour chariot rencontré
	string resultat(chaine.substr(0,chaine.find_first_of("\r")));
	resultat = resultat.substr(0,resultat.find_first_of("\n"));

	// On tronque la chaîne à partir du premier commentaire rencontré
	resultat = resultat.substr(0,resultat.find_first_of("//"));

	// On tronque les espaces et les tabulations situés en début de chaîne
	int tailleChaine = (int)resultat.size();
	int debutChaine = 0;
	while (debutChaine<tailleChaine && (resultat[debutChaine] == ' ' || resultat[debutChaine] == '\t')) { debutChaine++; }
    resultat = resultat.substr(debutChaine,tailleChaine);

	// On tronque les espaces et les tabulations situés en fin de chaîne
	tailleChaine = (int)resultat.size();
	int finChaine = 0;
	while (finChaine<tailleChaine && (resultat[finChaine] != ' ' && resultat[finChaine] != '\t')) { finChaine++; }
    return resultat.substr(0,finChaine);
}

void Api::valideNomDeFichier(string & nom)
{
	// Si l'on trouve un caractère génant pour windows ou pour la console on le remplace par un "-"
	string::iterator caractere = nom.begin();
	string::const_iterator finChaine = nom.end();
	while(caractere != finChaine)
	{
		if (
			*caractere=='/' ||
			*caractere=='\\' ||
			*caractere==':' ||
			*caractere=='*' ||
			*caractere=='?' ||
			*caractere=='"' ||
			*caractere=='<' ||
			*caractere=='>' ||
			*caractere=='|' ||
			*caractere==' ' ||
			*caractere=='\t' ||
			*caractere==';' ||
			*caractere=='{' ||
			*caractere=='}' ||
			*caractere=='(' ||
			*caractere==')' ||
			*caractere=='\''
			)
			*caractere='-';
		caractere++;
	}
}

bool Api::isValidePEntity(edict_t * pEntity)
{
	return pEntity && !pEntity->IsFree();
}

bool Api::isValidePlayer(IPlayerInfo * player)
{
	return player && player->IsConnected() && player->IsPlayer() && !player->IsHLTV();
}

bool Api::isValideIndex(int index)
{
	return index>=1 || index<=maxplayers;
}

void Api::debug(const string & message)
{
	string commande("CSSMatch : " + message + "\n");

	const char * debug = commande.c_str();
	Warning(debug);
	engine->LogPrint(debug);
}

void Api::reporteException(const exception & e, const string & fichier, int ligne)
{
	ostringstream message;
	message << e.what() << " (" << fichier << ", l." << ligne << ")";
	debug(message.str());
}

void timerCexecT(const string & commande)
{
	Api::cexecGroupe(TEAM_T,commande);
}

void timerCexecCT(const string & commande)
{
	Api::cexecGroupe(TEAM_CT,commande);
}

void timerSwapALL(const string & parametre)
{
	Api::swapGroupe(TEAM_ALL);
}
