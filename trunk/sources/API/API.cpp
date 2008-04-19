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
 
#include "API.h"

using std::string;
using std::vector;
using std::ostringstream;
using std::out_of_range;

IVEngineServer * API::engine = NULL;
IFileSystem * API::filesystem = NULL;
IGameEventManager2 * API::gameeventmanager = NULL;
IPlayerInfoManager * API::playerinfomanager = NULL;
IBotManager * API::botmanager = NULL;
IServerPluginHelpers * API::helpers = NULL;
IUniformRandomStream * API::randomStr = NULL;
IEngineTrace * API::enginetrace = NULL;
CGlobalVars * API::gpGlobals = NULL;
ICvar * API::cvars;

// Nous initialisons maxplayers � DEFAUT_MAXPLAYERS (nombre maximum de slots) tant que nous ne pouvons pas pr�ciser ce chiffre
// Au premier au ServerActivate(), ce nombre doit �tre actualis� de mani�re � ce que la suite du code en prenne compte
int API::maxplayers = DEFAUT_MAXPLAYERS;

bool API::initialisation(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory)
{
	if(	!(playerinfomanager = (IPlayerInfoManager *)gameServerFactory(INTERFACEVERSION_PLAYERINFOMANAGER,NULL)) ||
		!(engine = (IVEngineServer*)interfaceFactory(INTERFACEVERSION_VENGINESERVER, NULL)) ||
		!(gameeventmanager = (IGameEventManager2*) interfaceFactory( INTERFACEVERSION_GAMEEVENTSMANAGER2, NULL)) ||
		!(botmanager = (IBotManager *)gameServerFactory(INTERFACEVERSION_PLAYERBOTMANAGER, NULL)) ||
		!(filesystem = (IFileSystem*)interfaceFactory(FILESYSTEM_INTERFACE_VERSION, NULL)) ||
		!(helpers = (IServerPluginHelpers*)interfaceFactory(INTERFACEVERSION_ISERVERPLUGINHELPERS, NULL)) || 
		!(enginetrace = (IEngineTrace *)interfaceFactory(INTERFACEVERSION_ENGINETRACE_SERVER,NULL)) ||
		!(randomStr = (IUniformRandomStream *)interfaceFactory(VENGINE_SERVER_RANDOM_INTERFACE_VERSION, NULL)) ||
		!(cvars = (ICvar*)interfaceFactory(VENGINE_CVAR_INTERFACE_VERSION, NULL))
	)
		return false;
	gpGlobals = playerinfomanager->GetGlobalVars();
	if (!gpGlobals)
		return false;

	return true;
}


void API::serveurExecute(const string & cmd)
{
	// Insertion de la commande
	engine->ServerCommand(cmd.c_str());
	// Ex�cution des commandes en attente
	//engine->ServerExecute();
}

void API::configExecute(const string & config)
{
	// On pr�pare la commande
	string commande = "exec " + config + "\n";

	// Insertion de la commande
	engine->ServerCommand(commande.c_str());
	// Ex�cution des commandes en attente
	engine->ServerExecute();
	// Ex�cution des commandes en attente (celles lues dans le fichier de configuration)
	engine->ServerExecute();
}

bool API::hltvPresente()
{
	/*if (!ConVars::tv_enable || !ConVars::tv_enable->GetBool())
		return false;
	Ce n'est pas le r�le de cette fonction ! */

	// D�s que l'on trouve HLTV parmis les joueurs connect�s au serveur on retourne vrai
	for (int i=1;i<=maxplayers;i++)
	{
		edict_t * pEntity = engine->PEntityOfEntIndex(i);
		if (isValidePEntity(pEntity))
		{
			 // On n'utilise pas getIPlayerFromEntity qui exclu sourceTV
			IPlayerInfo * player = playerinfomanager->GetPlayerInfo(pEntity);
			if (player && player->IsConnected())
				if (player->IsHLTV())
					return true;
		}
	}
	return false;
}

IPlayerInfo * API::getIPlayerInfoFromEntity(edict_t * pEntity) throw (out_of_range)
{
	// Pour des raisons de stabilit� nous [re]faisons un test sur l'entit� ici
	if (!isValidePEntity(pEntity))
		throw out_of_range("Impossible de trouver l'IPlayerInfo d'une entité invalide");

	IPlayerInfo * player = playerinfomanager->GetPlayerInfo(pEntity);
	if (!isValidePlayer(player))
		throw out_of_range("Impossible de trouver l'IPlayerInfo de cette entité");

	return player;
}

edict_t * API::getEntityFromID(int userid) throw (out_of_range)
{
	// Si on trouve le bon userid parmis les joueurs connect�s au serveur on retourne le pointeur trouv�
	for (int i=1;i<=maxplayers;i++)
	{
		edict_t * pEntity = engine->PEntityOfEntIndex(i);
		if (isValidePEntity(pEntity) && (engine->GetPlayerUserId(pEntity) == userid))
			return pEntity;
	}
	// Sinon on jette une exception
	throw out_of_range("Impossible de trouver l'entité correspond à cet ID");
}

int API::getIndexFromUserId(int userid)
{
	// R�cup�ration de l'entit� associ�e � l'ID
	try
	{
		edict_t * pEntity = API::getEntityFromID(userid);
		return engine->IndexOfEdict(pEntity);
	}
	catch(const out_of_range & e)
	{
		API::debug(e.what());
		return INDEX_INVALIDE;
	}
}

int API::getUseridFromPEntity(edict_t * pEntity)
{
	// R�cup�ration d'un pointeur sur le joueur
	try
	{
		IPlayerInfo * player = getIPlayerInfoFromEntity(pEntity);
		return player->GetUserID();
	}
	catch(const out_of_range & e)
	{
		API::debug(e.what());
		return USERID_INVALIDE;
	}
}

int API::getUseridFromUsername(const string & pseudo)
{
	for (int i=1;i<=maxplayers;i++)
	{
		edict_t * pEntity = engine->PEntityOfEntIndex(i);
		try
		{
			IPlayerInfo * player = getIPlayerInfoFromEntity(pEntity);
			string name = player->GetName();
			if (pseudo == name)
				return player->GetUserID();
		}
		catch(const out_of_range & e){}
	}
	return USERID_INVALIDE;
}

int API::getIndexFromUsername(const string & pseudo)
{
	for (int i=1;i<=maxplayers;i++)
	{
		edict_t * pEntity = engine->PEntityOfEntIndex(i);
		try
		{
			IPlayerInfo * player = getIPlayerInfoFromEntity(pEntity);
			string name = player->GetName();
			if (pseudo == name)
				return engine->IndexOfEdict(pEntity);
		}
		catch(const out_of_range & e){}
	}
	return INDEX_INVALIDE;
}

CodeTeam API::getPEntityTeam(edict_t * pEntity)
{
	// Pour des raisons de stabilit� nous [re]faisons un test sur l'entit� ici
	if (!isValidePEntity(pEntity))
		return TEAM_INVALIDE;

	for (int i=1;i<=maxplayers;i++)
	{
		edict_t * pEntityTemp = engine->PEntityOfEntIndex(i);
		try
		{
			IPlayerInfo * player = getIPlayerInfoFromEntity(pEntity);
			if (pEntity == pEntityTemp)
				return (CodeTeam)player->GetTeamIndex();
		}
		catch(const out_of_range & e){}
	}
	return TEAM_INVALIDE;
}

IServerEntity * API::getIServerEntityFromEdict(edict_t * pEntity) throw (out_of_range)
{
	// Pour des raisons de stabilit� nous [re]faisons un test sur l'entit� ici
	if (!isValidePEntity(pEntity))
		throw out_of_range("Impossible de trouver l'IServerEntity d'une entité invalide");

	// R�cup�ration d'un pointeur g�n�rique sur le joueur
	IServerEntity * sEntity = pEntity->GetIServerEntity();
	if (!sEntity)
		throw out_of_range("L'IServerEntity de cette entité est nulle");

	return sEntity;
}

CBaseEntity * API::getCBaseEntityFromEntity(edict_t * pEntity) throw (out_of_range)
{
	// R�cup�ration d'un pointeur g�n�rique sur le joueur
	IServerEntity * sEntity;
	try
	{
		sEntity = getIServerEntityFromEdict(pEntity);
	}
	catch(const out_of_range & e)
	{
		API::debug(e.what());
		throw out_of_range("Impossible de trouver le BaseEntityEdict de cette entité");
	}

	// R�cup�ration du pointeur de type CBaseEntity sur le joueur
	CBaseEntity * bEntity = sEntity->GetBaseEntity();
	if (!bEntity)
		throw out_of_range("Le CBaseEntity de cette entité est nul");

	return bEntity;
}

CBaseCombatCharacter * API::getCBaseCombatCharacterFromEntity(edict_t * pEntity) throw (out_of_range)
{
	// R�cup�ration d'un pointeur g�n�rique valide sur l'entit�
	CBaseEntity * bEntity;
	try
	{
		bEntity = getCBaseEntityFromEntity(pEntity);
	}
	catch(const out_of_range & e)
	{
		API::debug(e.what());
		throw out_of_range("Impossible de trouver le CBaseCombatCharater d'une CBaseEntity invalide");
	}

	// R�cup�ration du CBaseCombatEntity
	CBaseCombatCharacter * cCharacter = bEntity->MyCombatCharacterPointer();
	if (!cCharacter)
		throw out_of_range("Impossible de trouver le CBaseCombatCharater de cette entité");

	return cCharacter;
}

CBaseCombatWeapon * API::getCBaseCombatWeaponFromSlot(edict_t * pEntity, int slot) throw (out_of_range)
{
	// R�cup�ration d'un pointeur g�n�rique valide sur l'entit�
	CBaseCombatCharacter * cCharacter;
	try
	{
		cCharacter = getCBaseCombatCharacterFromEntity(pEntity);
	}
	catch(const out_of_range & e)
	{
		throw out_of_range("Impossible de trouver le CBaseCombatWeapon d'un CBaseCombatCharacter invalide");
	}

	// R�cup�ration d'un pointeur sur l'arme situ� � ce slot
	CBaseCombatWeapon * cWeapon = cCharacter->Weapon_GetSlot(slot);
	if (!cWeapon)
		throw out_of_range("Impossible de trouver le CBaseCombatWeapon de cette entité");

	return cWeapon;
}

CBasePlayer * API::getCBasePlayerFromEntity(edict_t * pEntity) throw (out_of_range)
{
	// Pour des raisons de stabilit� nous [re]faisons un test sur l'entit� ici
	if (!isValidePEntity(pEntity))
		throw out_of_range("Impossible de trouver le CBasePlayer d'une entité invalide");

	CBasePlayer * cPlayer = reinterpret_cast<CBasePlayer*>(pEntity->GetUnknown()->GetBaseEntity());
	if (!cPlayer)
		throw out_of_range("Impossible de trouver le CBasePlayer de cette entité");

	return cPlayer;
}

int API::getPlayerCount(CodeTeam codeTeam /* crit�re de recherche : un camp particuli�re ou tous*/)
{
	// Variable contenant le nombre de joueurs correspond au crit�re de recherche
	int count = 0;

	// Le codeTeam correspond � une camp particuli�re
	if (codeTeam != TEAM_ALL)
	{
		// On compte tous les joueurs dans l'index de la team correspond au codeTeam
		for (int i=1;i<=maxplayers;i++)
		{
			edict_t * pEntity = engine->PEntityOfEntIndex(i);
			try
			{
				IPlayerInfo * player = getIPlayerInfoFromEntity(pEntity);
				if (player->GetTeamIndex() == codeTeam)
					count++;
			}
			catch(const out_of_range & e){}
		}
		return count;
	}

	// Le codeTeam correspond � toutes les teams confondues
	for (int i=1;i<=maxplayers;i++)
	{
		// On compte tous les joueurs
		edict_t * pEntity = engine->PEntityOfEntIndex(i);
		if (isValidePEntity(pEntity))
			count++;
	}
	return count;
}

string API::getPlayerName(edict_t * pEntity) throw (out_of_range)
{
	// Pour des raisons de stabilit� nous [re]faisons un test sur l'entit� ici
	if (!isValidePEntity(pEntity))
		throw out_of_range("Impossible de trouver le pseudo d'une entité invalide");

	// R�cup�ration du pseudo
	IPlayerInfo * player;
	try
	{
		player = getIPlayerInfoFromEntity(pEntity);
	}
	catch(const out_of_range & e)
	{
		API::debug(e.what());
		throw out_of_range("Impossible de trouver le pseudo d'un joueur invalide");
	}
	const char * pseudo = player->GetName();

	// V�rification de la validit� du retour de VALVE
	if (!pseudo)
		throw out_of_range("Impossible de trouver le pseudo de cette entité");

	return pseudo;
}

string API::getPlayerSteamID(edict_t * pEntity) throw (out_of_range)
{
	// Pour des raisons de stabilit� nous [re]faisons un test sur l'entit� ici
	if (!isValidePEntity(pEntity))
		throw out_of_range("Impossible de trouver la steamID d'une entité invalide");

	// R�cup�ration du steamID
	const char * steamID = engine->GetPlayerNetworkIDString(pEntity);
	// V�rification de la validit� du retour de VALVE
	if (!steamID)
		throw out_of_range("Impossible de trouver la steamID de cette entité");

	return steamID;
}

void API::kickid(int useridCible, const string & motif)
{
	ostringstream cmd;
	cmd << "kickid " << useridCible << " " << motif << "\n";

	API::serveurExecute(cmd.str());
}

void API::banid (int temps, int useridCible, const std::string & motif)
{
	// Ban
	ostringstream cmd;
	cmd << "banid " << temps << " " << useridCible  << "\n";
	API::serveurExecute(cmd.str());

	// Si le ban est permanent on l'�crit dans le banned_user.cfg
	if (temps==0)
		API::serveurExecute("writeid\n");

	// Kick avec motif
	kickid(useridCible,motif);
}

void API::cexec(edict_t * pEntity, const string & cmd)
{
	// On v�rifie que la team du joueur est valide et diff�rent de "non-assign�e" (cas particulier d'un bot cr�� malgr� le param�tre -nobots)
	CodeTeam codeTeam = getPEntityTeam(pEntity); // contr�le �galement la validit� de l'entit�
	if (codeTeam<=TEAM_UN)
		return;

	engine->ClientCommand(pEntity,cmd.c_str());
}

void API::cexecGroupe(CodeTeam codeTeam, const string & cmd)
{
	// Nous ferons 2 boucles s�par�es pour optimiser le code

	// Commande � ex�cuter chez tous les joueurs
	if (codeTeam==TEAM_ALL)
	{
		for (int i=1;i<=API::maxplayers;i++)
		{
			edict_t * pEntity = engine->PEntityOfEntIndex(i);
			try
			{
				IPlayerInfo * player = getIPlayerInfoFromEntity(pEntity);
				if (!player->IsFakeClient())
					cexec(pEntity,cmd);
			}
			catch(const out_of_range & e) {}
		}
		return;
	}

	// Commande � n'ex�cuter que pour une certaine camp
	for (int i=1;i<=API::maxplayers;i++)
	{
		edict_t * pEntity = engine->PEntityOfEntIndex(i);
		try
		{
			IPlayerInfo * player = getIPlayerInfoFromEntity(pEntity);
			if (!player->IsFakeClient() && player->GetTeamIndex()==codeTeam)
				cexec(pEntity,cmd);
		}
		catch(const out_of_range & e) {}
	}
}

void API::sexec(edict_t * pEntity, const string & cmd)
{
	// On v�rifie que la team du joueur est valide et diff�rent de "non-assign�e" (cas particulier d'un bot cr�� malgr� le param�tre -nobots)
	CodeTeam codeTeam = getPEntityTeam(pEntity); // contr�le �galement la validit� de l'entit�
	if (codeTeam<=TEAM_UN)
		return;

	helpers->ClientCommand(pEntity,cmd.c_str());
}

void API::sexecGroupe(CodeTeam codeTeam, const string & cmd)
{
	// Nous ferons 2 boucles s�par�es pour optimiser le code

	// Commande � ex�cuter chez tous les joueurs
	if (codeTeam==TEAM_ALL)
	{
		for (int i=1;i<=API::maxplayers;i++)
		{
			edict_t * pEntity = engine->PEntityOfEntIndex(i);
			try
			{
				IPlayerInfo * player = getIPlayerInfoFromEntity(pEntity);
				if (!player->IsFakeClient())
					sexec(pEntity,cmd);
			}
			catch(const out_of_range & e){}
		}
		return;
	}

	// Commande � n'ex�cuter que pour une certaine camp
	for (int i=1;i<=API::maxplayers;i++)
	{
		edict_t * pEntity = engine->PEntityOfEntIndex(i);
		try
		{
			IPlayerInfo * player = getIPlayerInfoFromEntity(pEntity);
			if (!player->IsFakeClient() && player->GetTeamIndex()==codeTeam)
				sexec(pEntity,cmd);
		}
		catch(const out_of_range & e){}
	}
}

void API::swap(IPlayerInfo * player)
{
	// Pour des raisons de stabilit� nous [re]faisons un test sur IPlayerInfo ici
	if (!isValidePlayer(player))
		return;

	// Selon le camp actuelle du joueur, nous le swappons dans le camp oppos�e
	// Les bots seront kick�s plut�t que swapp�s, pour �viter qu'il ne renaissent pas
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

void API::swapGroupe(CodeTeam codeTeam)
{
	// Nous ferons 2 boucles s�par�es pour optimiser le code

	// Commande � ex�cuter chez tous les joueurs
	if (codeTeam==TEAM_ALL)
	{
		for (int i=1;i<=API::maxplayers;i++)
		{
			edict_t * pEntity = engine->PEntityOfEntIndex(i);
			try
			{
				IPlayerInfo * player = getIPlayerInfoFromEntity(pEntity);
				swap(player); // nous savons que swap valide l'IPlayerInfo
			}
			catch(const out_of_range & e){}
		}
		return;
	}

	// Commande � n'ex�cuter que pour une certaine camp
	for (int i=1;i<=API::maxplayers;i++)
	{
		edict_t * pEntity = engine->PEntityOfEntIndex(i);
		try
		{
			IPlayerInfo * player = getIPlayerInfoFromEntity(pEntity);
			if (player->GetTeamIndex()==codeTeam)
				swap(player);
		}
		catch(const out_of_range & e){}
	}
}

void API::spec(IPlayerInfo * player)
{
	// Pour des raisons de stabilit� nous [re]faisons un test sur IPlayerInfo ici
	if (!isValidePlayer(player))
		return;

	// On ne placera pas les bots en spectateurs, on les kick pour �viter qu'ils ne soient "bloqu�s"
	if (player->IsFakeClient())
	{
		API::kickid(player->GetUserID(),"CSSMatch : Spec Bot");
		return;
	}

	player->ChangeTeam(TEAM_SPEC);
}

void API::specGroupe(CodeTeam codeTeam)
{
	// Nous ferons 2 boucles s�par�es pour optimiser le code

	// Commande � ex�cuter chez tous les joueurs
	if (codeTeam==TEAM_ALL)
	{
		for (int i=1;i<=API::maxplayers;i++)
		{
			edict_t * pEntity = engine->PEntityOfEntIndex(i);
			try
			{
				IPlayerInfo * player = getIPlayerInfoFromEntity(pEntity);
				spec(player); // nous savons que spec valide l'IPlayerInfo
			}
			catch(const out_of_range & e){}
		}
		return;
	}

	// Commande � n'ex�cuter que pour une certaine camp
	for (int i=1;i<=API::maxplayers;i++)
	{
		edict_t * pEntity = engine->PEntityOfEntIndex(i);
		try
		{
			IPlayerInfo * player = getIPlayerInfoFromEntity(pEntity);
			if (player->GetTeamIndex()==codeTeam)
				spec(player);
		}
		catch(const out_of_range & e){}
	}
}

void API::setHealth(edict_t * pEntity, int nombreHP)
{
	// Pour des raisons de stabilit� nous [re]faisons un test sur IPlayerInfo ici
	if (!isValidePEntity(pEntity))
		return;

	// On v�rifie que le joueur n'est pas mort
	IPlayerInfo * player;
	try
	{
		player = getIPlayerInfoFromEntity(pEntity);
	}
	catch(const out_of_range & e)
	{
		API::debug(e.what());
		return;
	}
	// Si oui, on ne fait rien
	if (player->IsDead())
		return;

	// Si la vie que l'on va �ter au joueur le n�cessite, on le tue simplement
	if (nombreHP <= 0)
	{
		cexec(pEntity,"kill\n");
		return;
	}

	// Sinon, on modifie sa vie
	CBasePlayer * bPlayer;
	try
	{
		bPlayer = getCBasePlayerFromEntity(pEntity);
	}
	catch(const out_of_range & e)
	{
		API::debug(e.what());
		return;
	}
	bPlayer->SetHealth(nombreHP);
}

void API::detruitArme(edict_t * pEntity, int slot)
{
	// R�cup�ration d'un pointeur valide sur l'arme
	CBaseCombatWeapon * weapon;
	try
	{
		weapon = getCBaseCombatWeaponFromSlot(pEntity,slot);
	}
	catch(const out_of_range & e)
	{
		//API::debug(e.what());
		return;
	}
	weapon->Kill();
}

bool API::estNulleOuCommentee(const string & chaine)
{
	// On stocke la taille de la cha�ne
	int tailleChaine = (int)chaine.size();

	// La chaine est nulle ou ne contient qu'un \n
	if (tailleChaine==0 || chaine[0]=='\r' || chaine[0]=='\n' || chaine[0]=='\0')
		return true;

	// On v�rifie que la cha�ne n'est pas comment�e (en ignorant les espaces et les tabulations situ�s en d�but de cha�ne)
	int indexCaractere = 0;
	while(indexCaractere<tailleChaine && (chaine[indexCaractere]==' ' || chaine[indexCaractere]=='\t')) { indexCaractere++; }
	if (tailleChaine>=2 && chaine.substr(indexCaractere,2)=="//")
		return true;

	// La cha�ne n'est pas comment�e
	return false;
}

string API::extraitChaineFormelle(const std::string & chaine)
{
	// On tronque la cha�ne au premier retour chariot rencontr�
	string resultat = chaine.substr(0,chaine.find_first_of("\r"));
	resultat = resultat.substr(0,resultat.find_first_of("\n"));

	// On tronque la cha�ne � partir du premier commentaire rencontr�
	resultat = resultat.substr(0,resultat.find_first_of("//"));

	// On tronque les espaces et les tabulations situ�s en d�but de cha�ne
	int tailleChaine = (int)resultat.size();
	int debutChaine = 0;
	while (debutChaine<tailleChaine && (resultat[debutChaine] == ' ' || resultat[debutChaine] == '\t')) { debutChaine++; }
    resultat = resultat.substr(debutChaine,tailleChaine);

	// On tronque les espaces et les tabulations situ�s en fin de cha�ne
	tailleChaine = (int)resultat.size();
	int finChaine = 0;
	while (finChaine<tailleChaine && (resultat[finChaine] != ' ' && resultat[finChaine] != '\t')) { finChaine++; }
    return resultat.substr(0,finChaine);

/*	int tailleChaine = chaine.size();
	for (int i=0;i<tailleChaine;i++)
		if (chaine[i]<'*' || chaine[i]>'}')
			return chaine.substr(0,i);
	return chaine;*/
}

void API::valideNomDeFichier(string & nom)
{
	// Si l'on trouve un caract�re g�nant pour windows ou pour la console on le remplace par un "-"
	string::iterator caractere = nom.begin();
	while(caractere!=nom.end())
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

bool API::isValidePEntity(edict_t * pEntity)
{
	return pEntity && !pEntity->IsFree();
}

bool API::isValidePlayer(IPlayerInfo * player)
{
	return player && player->IsConnected() && player->IsPlayer() && !player->IsHLTV();
}

bool API::isValideIndex(int index)
{
	return index>=1 || index<=maxplayers;
}

void API::debug(const string & message)
{
	// On pr�pare la commande
	string commande = "CSSMatch : " + message + "\n";

	// On affiche le message dans le log et la console du serveur
	const char * debug = commande.c_str();
	Warning(debug);
	engine->LogPrint(debug);
}
