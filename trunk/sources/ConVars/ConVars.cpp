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

/** Rétablit la valeur par défaut de la ConVar "cssmatch_version" si sa valeur est modifiée 
 *
 * @param var Le pointeur sur la ConVar concernée
 * @param pOldString L'ancienne valeur de la ConVar
 * @see ConVars::cssmatch_version
 *
 */
static void versionConstante(ConVar * var, const char * pOldString)
{
	if (! FStrEq(var->GetString(),CSSMATCH_VERSION_LIGHT))
		var->SetValue(CSSMATCH_VERSION_LIGHT);
}

/*static void debugLogOn(ConVar * var, const char * pOldString)
{
	if (var->GetBool())
		Api::serveurExecute("log on\n");
}*/

/** Charge ou décharge le menu d'administration selon la valeur de la ConVar "cssmatch_advanced"
 *
 * @param var Le pointeur sur la ConVar concernée
 * @param pOldString L'ancienne valeur de la ConVar
 * @see ConVars::cssmatch_advanced
 *
 */
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

ConVar ConVars::cssmatch_version = ConVar( "cssmatch_version", CSSMATCH_VERSION_LIGHT, FCVAR_PLUGIN|FCVAR_NOTIFY|FCVAR_REPLICATED, "CSSMatch : Version du plugin", versionConstante);
//ConVar ConVars::cssmatch_debug = ConVar( "cssmatch_debug", "0", FCVAR_PLUGIN, "CSSMatch : \"1\" = Active le mode dÃ©bogage, \"0\" = off",debugLogOn);
ConVar ConVars::cssmatch_advanced = ConVar( "cssmatch_advanced", "0", FCVAR_PLUGIN, "CSSMatch : \"1\" = Les menus sont construits et gÃ©rÃ©s avec des options d'administration, \"0\" = Les menus sont normaux", true, 0.0f, true, 1.0f,swapAdvanced);
//ConVar ConVars::cssmatch_language = ConVar( "cssmatch_language", "french.txt", FCVAR_PLUGIN, "CSSMatch : Nom du fichier du dossier cfg/cssmatch/languages contenant les messages du plugin", true, 0.0f, true, 1.0f,swapAdvanced);

ConVar ConVars::cssmatch_cutround = ConVar( "cssmatch_cutround", "1", FCVAR_PLUGIN, "CSSMatch : \"1\" = Faire un round au couteau, \"0\" = off", true, 0.0f, true, 1.0f);
ConVar ConVars::cssmatch_cutround_allows_c4 = ConVar( "cssmatch_cutround_allows_c4", "1", FCVAR_PLUGIN, "CSSMatch : \"1\" = Autoriser la pose du C4 pendant le round au couteau, \"0\" = off", true, 0.0f, true, 1.0f);
ConVar ConVars::cssmatch_end_cutround = ConVar("cssmatch_end_cutround", "20", FCVAR_PLUGIN, "CSSMatch : Durée (en secondes) entre la fin CutRound et le début du StratsTime", true, 5.0f, false, 0.0f);

ConVar ConVars::cssmatch_rounds = ConVar( "cssmatch_rounds", "12", FCVAR_PLUGIN, "CSSMatch : Nombre de rounds par demi-match",true,0.0f,false,0.0f);;
ConVar ConVars::cssmatch_manches = ConVar( "cssmatch_manches", "2", FCVAR_PLUGIN, "CSSMatch : Nombre de manches par match",true,0.0f,false,0.0f);;
ConVar ConVars::cssmatch_end_manche = ConVar("cssmatch_end_manche", "10", FCVAR_PLUGIN, "CSSMatch : Durée (en secondes) entre la fin d'une manche et la suite du match (strats, nouvelle manche ou fin de match)", true, 5.0f, false, 0.0f);
ConVar ConVars::cssmatch_sourcetv = ConVar("cssmatch_sourcetv", "1", FCVAR_PLUGIN, "CSSMatch : \"1\" = Activer l'enregistrement SourceTV si elle est connectée, \"0\" = off", true, 0.0f, true, 1.0f);
ConVar ConVars::cssmatch_sourcetv_path = ConVar( "cssmatch_sourcetv_path", "cfg/cssmatch/sourcetv", FCVAR_PLUGIN, "CSSMatch : Dossier cible des enregistrements SourceTV du match");

ConVar ConVars::cssmatch_strats_time = ConVar( "cssmatch_strats_time", "5", FCVAR_PLUGIN, "CSSMatch : Durée maximale (en minutes) du StratsTime (\"0\" = off)", true, 0.0f, false, 0.0f);

ConVar ConVars::cssmatch_hostname = ConVar( "cssmatch_hostname", "CSSMatch : %s VS %s", FCVAR_PLUGIN, "CSSMatch : Nom du serveur pendant le match (%s sera remplacé par le tag des teams");
ConVar ConVars::cssmatch_password = ConVar( "cssmatch_password", "inwar", FCVAR_PLUGIN, "CSSMatch : Mot de passe du serveur pendant le match");
ConVar ConVars::cssmatch_default_config = ConVar( "cssmatch_default_config", "server.cfg", FCVAR_PLUGIN, "CSSMatch : Fichier de remise par défaut de la configuration du serveur");

void ConVars::initialise()
{
	sv_cheats = Api::cvars->FindVar("sv_cheats");
	sv_alltalk = Api::cvars->FindVar("sv_alltalk");
	hostname = Api::cvars->FindVar("hostname");
	sv_password = Api::cvars->FindVar("sv_password");
	tv_enable = Api::cvars->FindVar("tv_enable");
}

void ConVars::swapAlltalk()
{
	if (sv_alltalk != NULL)
	{
		// On passe par la console pour préserver la notification
		if (sv_alltalk->GetBool())
			Api::serveurExecute("sv_alltalk 0\n");
		else
			Api::serveurExecute("sv_alltalk 1\n");
	}
	else
		Api::debug("sv_alltalk est inaccessible");
}
