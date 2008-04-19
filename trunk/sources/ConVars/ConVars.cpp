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

#include "ConVars.h"
#include "../GestionMenus/GestionMenus.h"

static void versionConstante(ConVar * var, const char * pOldString)
{
	if (!FStrEq(var->GetString(),CSSMATCH_VERSION_LIGHT))
		var->SetValue(CSSMATCH_VERSION_LIGHT);
}

/*static void debugLogOn(ConVar * var, const char * pOldString)
{
	if (var->GetBool())
		API::serveurExecute("log on\n");
}*/

static void swapAdvanced(ConVar * var, const char * pOldString)
{
	if (var->GetBool())
		GestionMenus::getMenus()->initialiseMenusAvecAdministration();
	else
		GestionMenus::getMenus()->initialiseMenus();
}

ConVar * ConVars::sv_cheats = NULL;
ConVar * ConVars::sv_alltalk = NULL;
ConVar * ConVars::hostname = NULL;
ConVar * ConVars::sv_password = NULL;
ConVar * ConVars::tv_enable = NULL;

ConVar ConVars::cssmatch_version = ConVar( "cssmatch_version", CSSMATCH_VERSION_LIGHT, FCVAR_NOTIFY|FCVAR_REPLICATED, "CSSMatch : Version du plugin", versionConstante);
//ConVar ConVars::cssmatch_debug = ConVar( "cssmatch_debug", "0", FCVAR_NONE, "CSSMatch : \"1\" = Active le mode dÃ©bogage, \"0\" = off",debugLogOn);
ConVar ConVars::cssmatch_advanced = ConVar( "cssmatch_advanced", "0", FCVAR_NONE, "CSSMatch : \"1\" = Les menus sont construits et gÃ©rÃ©s avec des options d'administration, \"0\" = Les menus sont normaux", true, 0.0f, true, 1.0f,swapAdvanced);

ConVar ConVars::cssmatch_cutround = ConVar( "cssmatch_cutround", "1", FCVAR_NONE, "CSSMatch : \"1\" = Faire un round au couteau, \"0\" = off", true, 0.0f, true, 1.0f);
ConVar ConVars::cssmatch_cutround_allows_c4 = ConVar( "cssmatch_cutround_allows_c4", "1", FCVAR_NONE, "CSSMatch : \"1\" = Autoriser la pose du C4 pendant le round au couteau, \"0\" = off", true, 0.0f, true, 1.0f);
ConVar ConVars::cssmatch_end_cutround = ConVar("cssmatch_end_cutround", "20", FCVAR_NONE, "CSSMatch : Durée (en secondes) entre la fin CutRound et le début du StratsTime", true, 5.0f, false, 0.0f);

ConVar ConVars::cssmatch_rounds = ConVar( "cssmatch_rounds", "12", FCVAR_NONE, "CSSMatch : Nombre de rounds par demi-match",true,0.0f,false,0.0f);;
ConVar ConVars::cssmatch_end_manche = ConVar("cssmatch_end_manche", "10", FCVAR_NONE, "CSSMatch : Durée (en secondes) entre la fin d'une manche et la suite du match (strats, nouvelle manche ou fin de match)", true, 5.0f, false, 0.0f);
ConVar ConVars::cssmatch_sourcetv = ConVar("cssmatch_sourcetv", "1", FCVAR_NONE, "CSSMatch : \"1\" = Activer l'enregistrement SourceTV si elle est connectée, \"0\" = off", true, 0.0f, true, 1.0f);
ConVar ConVars::cssmatch_sourcetv_path = ConVar( "cssmatch_sourcetv_path", "cfg/cssmatch/sourcetv", FCVAR_NONE, "CSSMatch : Dossier cible des enregistrements SourceTV du match");

ConVar ConVars::cssmatch_strats_time = ConVar( "cssmatch_strats_time", "5", FCVAR_NONE, "CSSMatch : Durée maximale (en minutes) du StratsTime (\"0\" = off)", true, 0.0f, false, 0.0f);

ConVar ConVars::cssmatch_hostname = ConVar( "cssmatch_hostname", "CSSMatch : %s VS %s", FCVAR_NONE, "CSSMatch : Nom du serveur pendant le match (%s sera remplacé par le tag des teams");
ConVar ConVars::cssmatch_password = ConVar( "cssmatch_password", "inwar", FCVAR_NONE, "CSSMatch : Mot de passe du serveur pendant le match");
ConVar ConVars::cssmatch_default_config = ConVar( "cssmatch_default_config", "server.cfg", FCVAR_NONE, "CSSMatch : Fichier de remise par défaut de la configuration du serveur");

void ConVars::initialise()
{
	sv_cheats = API::cvars->FindVar("sv_cheats");
	sv_alltalk = API::cvars->FindVar("sv_alltalk");
	hostname = API::cvars->FindVar("hostname");
	sv_password = API::cvars->FindVar("sv_password");
	tv_enable = API::cvars->FindVar("tv_enable");
}

void ConVars::swapAlltalk()
{
	// sv_alltalk non initialisé ?
	if (!sv_alltalk)
		return;

	// On passe par la console pour préserver la notification
	if (sv_alltalk->GetBool())
		API::serveurExecute("sv_alltalk 0\n");
	else
		API::serveurExecute("sv_alltalk 1\n");
}
