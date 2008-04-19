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

#include "GestionMenus.h"

using std::out_of_range;
using std::string;

GestionMenus * GestionMenus::menus = NULL;
Menu GestionMenus::cutround_question;
Menu GestionMenus::phase_off;
Menu GestionMenus::phase_cutround;
Menu GestionMenus::phase_strats;
Menu GestionMenus::phase_match;
Menu GestionMenus::configurations;

Menu GestionMenus::administration;
Menu GestionMenus::admin_changelevel;
Menu GestionMenus::admin_swap;
Menu GestionMenus::admin_spec;
Menu GestionMenus::admin_kick;
Menu GestionMenus::admin_ban;
Menu GestionMenus::admin_ban_time;


void cutround_question_choix(int indexJoueur, int choix)
{
	// Récupération de l'entité correspondant à l'index du joueur
	edict_t * pEntity = API::engine->PEntityOfEntIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	switch(choix)
	{
	case 1:
		g_CSSMatchPlugin.getMatch()->setCutRound(true); // la variable est déjà à true dans le constructeur de match, mais le lancement du match peut encore être annulé
		gestionnaireMenus->afficheMenuRef(GestionMenus::configurations,indexJoueur);
		return;
	case 2:
		g_CSSMatchPlugin.getMatch()->setCutRound(false);
		gestionnaireMenus->afficheMenuRef(GestionMenus::configurations,indexJoueur);
		return;
	case 3:
		API::cexec(API::engine->PEntityOfEntIndex(indexJoueur),"cssmatch\n");
		return;
	}
	gestionnaireMenus->quitteMenu(indexJoueur);
}

void phase_off_choix(int indexJoueur, int choix)
{
	// Récupération de l'entité correspondant à l'index du joueur
	edict_t * pEntity = API::engine->PEntityOfEntIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	if (choix == 1)
	{
		if (ConVars::cssmatch_cutround.GetBool())
			gestionnaireMenus->afficheMenuRef(GestionMenus::cutround_question,indexJoueur);
		else
			gestionnaireMenus->afficheMenuRef(GestionMenus::configurations,indexJoueur); // cssmatch_cutround annulera l'effet de la variable Match::cutround
		return;
	}
	gestionnaireMenus->quitteMenu(indexJoueur);
}

void phase_cutround_choix(int indexJoueur, int choix)
{
	// Récupération de l'entité correspondant à l'index du joueur
	edict_t * pEntity = API::engine->PEntityOfEntIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	switch(choix)
	{
	case 1:
		ConVars::swapAlltalk();
		API::cexec(API::engine->PEntityOfEntIndex(indexJoueur),"cssmatch\n");
		break;
	case 2:
		g_CSSMatchPlugin.getMatch()->restartRound(API::engine->PEntityOfEntIndex(indexJoueur));
		break;
	case 3:
		if (API::isValidePEntity(pEntity))
			g_CSSMatchPlugin.getMatch()->finMatch(pEntity);
		else
			g_CSSMatchPlugin.getMatch()->finMatch();
		break;
	case 4:
		g_CSSMatchPlugin.getMatch()->detecteTags(true);
	}
	gestionnaireMenus->quitteMenu(indexJoueur);
}

void phase_strats_choix(int indexJoueur, int choix)
{
	// Récupération de l'entité correspondant à l'index du joueur
	edict_t * pEntity = API::engine->PEntityOfEntIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	switch(choix)
	{
	case 1:
		ConVars::swapAlltalk();
		API::cexec(API::engine->PEntityOfEntIndex(indexJoueur),"cssmatch\n");
		break;
	case 2:
		g_CSSMatchPlugin.getMatch()->restartRound(API::engine->PEntityOfEntIndex(indexJoueur));
		break;
	case 3:
		if (API::isValidePEntity(pEntity))
			g_CSSMatchPlugin.getMatch()->finMatch(pEntity);
		else
			g_CSSMatchPlugin.getMatch()->finMatch();
		break;
	case 4:
		g_CSSMatchPlugin.getMatch()->detecteTags(true);
		break;
	case 5:
		if (API::isValidePEntity(pEntity))
			g_CSSMatchPlugin.getMatch()->forceGo(pEntity);
		else
			g_CSSMatchPlugin.getMatch()->forceGo();
	}
	gestionnaireMenus->quitteMenu(indexJoueur);
}

void phase_match_choix(int indexJoueur, int choix)
{
	// Récupération de l'entité correspondant à l'index du joueur
	edict_t * pEntity = API::engine->PEntityOfEntIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	switch(choix)
	{
	case 1:
		ConVars::swapAlltalk();
		API::cexec(API::engine->PEntityOfEntIndex(indexJoueur),"cssmatch\n");
		break;
	case 2:
		g_CSSMatchPlugin.getMatch()->restartRound(API::engine->PEntityOfEntIndex(indexJoueur));
		break;
	case 3:
		if (API::isValidePEntity(pEntity))
			g_CSSMatchPlugin.getMatch()->finMatch(pEntity);
		else
			g_CSSMatchPlugin.getMatch()->finMatch();
		break;
	case 4:
		g_CSSMatchPlugin.getMatch()->detecteTags(true);
		break;
	case 5:
		g_CSSMatchPlugin.getMatch()->restartManche(pEntity);
	}
	gestionnaireMenus->quitteMenu(indexJoueur);
}

void configurations_choix(int indexJoueur, int choix)
{
	// Récupération de l'entité correspondant à l'index du joueur
	edict_t * pEntity = API::engine->PEntityOfEntIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	if (choix!=10)
	{
		const Ligne * config;
		try
		{
			config = &menuJoueur->getChoix(choix,joueur->getPageMenu());
		}
		catch(const out_of_range & e)
		{
			API::debug(e.what());
			return;
		}

		switch(config->getValeur())
		{
		case MENU_CODE_RETOUR:
			gestionnaireMenus->afficheMenuPrecedent(indexJoueur);
			return;
		case MENU_CODE_SUIVANT:
			gestionnaireMenus->afficheMenuSuivant(indexJoueur);
			return;
		default:
			if (API::isValidePEntity(pEntity))
				g_CSSMatchPlugin.getMatch()->lanceMatch(new Configuration(config->getTexte()),pEntity); // Présence du fichier recontrôlée dans lanceMatch()
			else 
				g_CSSMatchPlugin.getMatch()->lanceMatch(new Configuration(config->getTexte()));
		}
	}
	gestionnaireMenus->quitteMenu(indexJoueur);
}

void phase_off_choixAvecAdmin(int indexJoueur, int choix)
{
	// Récupération de l'entité correspondant à l'index du joueur
	edict_t * pEntity = API::engine->PEntityOfEntIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	if (choix == 1)
	{
		gestionnaireMenus->afficheMenuRef(GestionMenus::administration,indexJoueur);
		return;
	}
	else if (choix == 2)
	{
		gestionnaireMenus->afficheMenuRef(GestionMenus::cutround_question,indexJoueur);
		return;
	}
	gestionnaireMenus->quitteMenu(indexJoueur);
}

void phase_cutround_choixAvecAdmin(int indexJoueur, int choix)
{
	// Récupération de l'entité correspondant à l'index du joueur
	edict_t * pEntity = API::engine->PEntityOfEntIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	switch(choix)
	{
	case 1:
		gestionnaireMenus->afficheMenuRef(GestionMenus::administration,indexJoueur);
		return;
	case 2:
		ConVars::swapAlltalk();
		API::cexec(API::engine->PEntityOfEntIndex(indexJoueur),"cssmatch\n");
		break;
	case 3:
		g_CSSMatchPlugin.getMatch()->restartRound(API::engine->PEntityOfEntIndex(indexJoueur));
		break;
	case 4:
		if (API::isValidePEntity(pEntity))
			g_CSSMatchPlugin.getMatch()->finMatch(pEntity);
		else
			g_CSSMatchPlugin.getMatch()->finMatch();
		break;
	case 5:
		g_CSSMatchPlugin.getMatch()->detecteTags(true);
	}
	gestionnaireMenus->quitteMenu(indexJoueur);
}

void phase_strats_choixAvecAdmin(int indexJoueur, int choix)
{
	// Récupération de l'entité correspondant à l'index du joueur
	edict_t * pEntity = API::engine->PEntityOfEntIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	switch(choix)
	{
	case 1:
		gestionnaireMenus->afficheMenuRef(GestionMenus::administration,indexJoueur);
		return;
	case 2:
		ConVars::swapAlltalk();
		API::cexec(API::engine->PEntityOfEntIndex(indexJoueur),"cssmatch\n");
		break;
	case 3:
		g_CSSMatchPlugin.getMatch()->restartRound(API::engine->PEntityOfEntIndex(indexJoueur));
		break;
	case 4:
		if (API::isValidePEntity(pEntity))
			g_CSSMatchPlugin.getMatch()->finMatch(pEntity);
		else
			g_CSSMatchPlugin.getMatch()->finMatch();
		break;
	case 5:
		g_CSSMatchPlugin.getMatch()->detecteTags(true);
		break;
	case 6:
		if (API::isValidePEntity(pEntity))
			g_CSSMatchPlugin.getMatch()->forceGo(pEntity);
		else
			g_CSSMatchPlugin.getMatch()->forceGo();
	}
	gestionnaireMenus->quitteMenu(indexJoueur);
}

void phase_match_choixAvecAdmin(int indexJoueur, int choix)
{
	// Récupération de l'entité correspondant à l'index du joueur
	edict_t * pEntity = API::engine->PEntityOfEntIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	switch(choix)
	{
	case 1:
		gestionnaireMenus->afficheMenuRef(GestionMenus::administration,indexJoueur);
		return;
	case 2:
		ConVars::swapAlltalk();
		API::cexec(API::engine->PEntityOfEntIndex(indexJoueur),"cssmatch\n");
		break;
	case 3:
		g_CSSMatchPlugin.getMatch()->restartRound(API::engine->PEntityOfEntIndex(indexJoueur));
		break;
	case 4:
		if (API::isValidePEntity(pEntity))
			g_CSSMatchPlugin.getMatch()->finMatch(pEntity);
		else
			g_CSSMatchPlugin.getMatch()->finMatch();
		break;
	case 5:
		g_CSSMatchPlugin.getMatch()->detecteTags(true);
		break;
	case 6:
		g_CSSMatchPlugin.getMatch()->restartManche(pEntity);
	}
	gestionnaireMenus->quitteMenu(indexJoueur);
}

void administration_choix(int indexJoueur, int choix)
{
	// Récupération de l'entité correspondant à l'index du joueur
	edict_t * pEntity = API::engine->PEntityOfEntIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	switch(choix)
	{
	case 1:
		gestionnaireMenus->afficheMenuRef(GestionMenus::admin_changelevel,indexJoueur);
		return;
	case 2:
		gestionnaireMenus->afficheMenuRef(GestionMenus::admin_swap,indexJoueur);
		return;
	case 3:
		gestionnaireMenus->afficheMenuRef(GestionMenus::admin_spec,indexJoueur);
		return;
	case 4:
		gestionnaireMenus->afficheMenuRef(GestionMenus::admin_kick,indexJoueur);
		return;
	case 5:
		gestionnaireMenus->afficheMenuRef(GestionMenus::admin_ban,indexJoueur);
		return;
	case 6:
		API::cexec(API::engine->PEntityOfEntIndex(indexJoueur),"cssmatch\n");
		return;
	}
	gestionnaireMenus->quitteMenu(indexJoueur);
}

void admin_changelevel_choix(int indexJoueur, int choix)
{
	// Récupération de l'entité correspondant à l'index du joueur
	edict_t * pEntity = API::engine->PEntityOfEntIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	if (choix!=10)
	{
		const Ligne * ligne;
		try
		{
			ligne = &menuJoueur->getChoix(choix,joueur->getPageMenu());
		}
		catch(const out_of_range & e)
		{
			API::debug(e.what());
			return;
		}

		switch(ligne->getValeur())
		{
		case MENU_CODE_RETOUR:
			gestionnaireMenus->afficheMenuPrecedent(indexJoueur);
			return;
		case MENU_CODE_SUIVANT:
			gestionnaireMenus->afficheMenuSuivant(indexJoueur);
			return;
		default:
			{
				const string * nomMap = &ligne->getTexte();
			if (API::engine->IsMapValid(nomMap->c_str()))
				API::serveurExecute("changelevel " + *nomMap + "\n");
			else
				Messages::sayTell(indexJoueur,"Impossible de charger la map " + *nomMap + " !");
			}
		}
	}
	gestionnaireMenus->quitteMenu(indexJoueur);
}

void admin_swap_choix(int indexJoueur, int choix)
{
	// Récupération de l'entité correspondant à l'index du joueur
	edict_t * pEntity = API::engine->PEntityOfEntIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	if (choix!=10)
	{
		const Ligne * ligne;
		try
		{
			ligne = &menuJoueur->getChoix(choix,joueur->getPageMenu());
		}
		catch(const out_of_range & e)
		{
			API::debug(e.what());
			return;
		}

		switch(ligne->getValeur())
		{
		case MENU_CODE_RETOUR:
			gestionnaireMenus->afficheMenuPrecedent(indexJoueur);
			return;
		case MENU_CODE_SUIVANT:
			gestionnaireMenus->afficheMenuSuivant(indexJoueur);
			return;
		default:
			{
				int useridCible = ligne->getValeur();
				edict_t * pEntity;
				try
				{
					pEntity = API::getEntityFromID(useridCible);
					API::swap(API::getIPlayerInfoFromEntity(pEntity));
					Messages::sayMsg(ligne->getTexte() + " est swappÃ© par \003" + API::getPlayerName(API::engine->PEntityOfEntIndex(indexJoueur)),indexJoueur);
				}
				catch(const out_of_range & e)
				{
					Messages::sayTell(indexJoueur,ligne->getTexte() + " semble s'Ãªtre dÃ©connectÃ©");
					API::debug(e.what());
					return;
				}
			}
		}
	}
	gestionnaireMenus->quitteMenu(indexJoueur);
}

void admin_spec_choix(int indexJoueur, int choix)
{
	// Récupération de l'entité correspondant à l'index du joueur
	edict_t * pEntity = API::engine->PEntityOfEntIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	if (choix!=10)
	{
		const Ligne * ligne;
		try
		{
			ligne = &menuJoueur->getChoix(choix,joueur->getPageMenu());
		}
		catch(const out_of_range & e)
		{
			API::debug(e.what());
			return;
		}

		switch(ligne->getValeur())
		{
		case MENU_CODE_RETOUR:
			gestionnaireMenus->afficheMenuPrecedent(indexJoueur);
			return;
		case MENU_CODE_SUIVANT:
			gestionnaireMenus->afficheMenuSuivant(indexJoueur);
			return;
		default:
			{
				int useridCible = ligne->getValeur();
				edict_t * pEntity;
				try
				{
					pEntity = API::getEntityFromID(useridCible);
					API::spec(API::getIPlayerInfoFromEntity(pEntity));
					Messages::sayMsg(ligne->getTexte() + " est mit en spectateur par \003" + API::getPlayerName(API::engine->PEntityOfEntIndex(indexJoueur)),indexJoueur);
				}
				catch(const out_of_range & e)
				{
					Messages::sayTell(indexJoueur,ligne->getTexte() + " semble s'Ãªtre dÃ©connectÃ©");
					API::debug(e.what());
					return;
				}
			}
		}
	}
	gestionnaireMenus->quitteMenu(indexJoueur);
}

void admin_kick_choix(int indexJoueur, int choix)
{
	// Récupération de l'entité correspondant à l'index du joueur
	edict_t * pEntity = API::engine->PEntityOfEntIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	if (choix!=10)
	{
		const Ligne * ligne;
		try
		{
			ligne = &menuJoueur->getChoix(choix,joueur->getPageMenu());
		}
		catch(const out_of_range & e)
		{
			API::debug(e.what());
			return;
		}

		switch(ligne->getValeur())
		{
		case MENU_CODE_RETOUR:
			gestionnaireMenus->afficheMenuPrecedent(indexJoueur);
			return;
		case MENU_CODE_SUIVANT:
			gestionnaireMenus->afficheMenuSuivant(indexJoueur);
			return;
		default:
			{
				int useridCible = ligne->getValeur();
				edict_t * pEntity;
				try
				{
					pEntity = API::getEntityFromID(useridCible);
					API::kickid(useridCible,"Kick par un arbitre");	
					Messages::sayMsg(ligne->getTexte() + " est kickÃ© par \003" + API::getPlayerName(API::engine->PEntityOfEntIndex(indexJoueur)),indexJoueur);
				}
				catch(const out_of_range & e)
				{
					Messages::sayTell(indexJoueur,ligne->getTexte() + " semble s'Ãªtre dÃ©connectÃ©");
					API::debug(e.what());
					return;
				}
			}
		}
	}
	gestionnaireMenus->quitteMenu(indexJoueur);
}

void admin_ban_choix(int indexJoueur, int choix)
{
	// Récupération de l'entité correspondant à l'index du joueur
	edict_t * pEntity = API::engine->PEntityOfEntIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	if (choix!=10)
	{
		const Ligne * ligne;
		try
		{
			ligne = &menuJoueur->getChoix(choix,joueur->getPageMenu());
		}
		catch(const out_of_range & e)
		{
			API::debug(e.what());
			return;
		}

		switch(ligne->getValeur())
		{
		case MENU_CODE_RETOUR:
			gestionnaireMenus->afficheMenuPrecedent(indexJoueur);
			return;
		case MENU_CODE_SUIVANT:
			gestionnaireMenus->afficheMenuSuivant(indexJoueur);
			return;
		default:
			{
				int useridCible = ligne->getValeur();
				edict_t * pEntity;
				try
				{
					pEntity = API::getEntityFromID(useridCible);
				}
				catch(const out_of_range & e)
				{
					Messages::sayTell(indexJoueur,ligne->getTexte() + " semble s'Ãªtre dÃ©connectÃ©");
					API::debug(e.what());
					return;
				}
				joueur->setUseridCible(useridCible);
				gestionnaireMenus->afficheMenuRef(GestionMenus::admin_ban_time,indexJoueur);
				return;
			}
		}
	}
	gestionnaireMenus->quitteMenu(indexJoueur);
}

void admin_ban_time_choix(int indexJoueur, int choix)
{
	// Récupération de l'entité correspondant à l'index du joueur
	edict_t * pEntity = API::engine->PEntityOfEntIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	try
	{
		switch(choix)
		{
		case 1:
			{
				int useridCible = joueur->getUseridCible();
				API::banid(5,useridCible,"Ban par un arbitre");
				Messages::sayMsg(API::getPlayerName(API::getEntityFromID(useridCible)) + " est temporairement banni par \003" + API::getPlayerName(API::engine->PEntityOfEntIndex(indexJoueur)),indexJoueur);
				break;
			}
		case 2:
			{
				int useridCible = joueur->getUseridCible();
				API::banid(60,useridCible,"Ban par un arbitre");
				Messages::sayMsg(API::getPlayerName(API::getEntityFromID(useridCible)) + " est temporairement banni par \003" + API::getPlayerName(API::engine->PEntityOfEntIndex(indexJoueur)),indexJoueur);
				break;
			}
		case 3:
			{
				int useridCible = joueur->getUseridCible();
				API::banid(0,useridCible,"Ban par un arbitre");
				Messages::sayMsg(API::getPlayerName(API::getEntityFromID(useridCible)) + " est banni de faÃ§on permanente par \003" + API::getPlayerName(API::engine->PEntityOfEntIndex(indexJoueur)),indexJoueur);
				break;
			}
		case 4:
			gestionnaireMenus->afficheMenuRef(GestionMenus::admin_ban,indexJoueur);
		}
	}
	catch(const out_of_range & e)
	{
		API::debug(e.what());
	}
	gestionnaireMenus->quitteMenu(indexJoueur);
}

GestionMenus::GestionMenus()
{
}

GestionMenus::~GestionMenus()
{
	if (menus)
		delete menus;
}

GestionMenus * GestionMenus::getMenus()
{
	// Si aucune instance n'existe, on la créer
	if (menus == NULL)
		menus = new GestionMenus();
	// On retourne l'instance
	return menus;
}

void GestionMenus::initialiseMenuChoixCutRound()
{
	cutround_question.setCodeMenu(CODE_MENU_CUTROUND_QUESTION);
	cutround_question.setTitre("CSSMatch : Voulez-vous jouer un round au couteau ?");
	cutround_question.addLigne("Oui");
	cutround_question.addLigne("Non");
	cutround_question.addLigne("Retour");
}

void GestionMenus::initialiseMenuOff()
{
	phase_off.setCodeMenu(CODE_MENU_OFF);
	phase_off.setTitre("CSSMatch : Voulez-vous lancer un match ?");
	phase_off.addLigne("Oui");
	phase_off.addLigne("Non");
}

void GestionMenus::initialiseMenuCutRound()
{
	phase_cutround.setCodeMenu(CODE_MENU_CUTROUND);
	phase_cutround.setTitre("CSSMatch : Round au couteau");
	phase_cutround.addLigne("%s le alltalk");
	phase_cutround.addLigne("Restart");
	phase_cutround.addLigne("Stopper le match");
	phase_cutround.addLigne("RedÃ©tecter les tags");
}

void GestionMenus::initialiseMenuStratsTime()
{
	phase_strats.setCodeMenu(CODE_MENU_STRATS);
	phase_strats.setTitre("CSSMatch : Strats");
	phase_strats.addLigne("%s le alltalk");
	phase_strats.addLigne("Restart");
	phase_strats.addLigne("Stopper le match");
	phase_strats.addLigne("RedÃ©tecter les tags");
	phase_strats.addLigne("Forcer le !go de toutes les teams");
}

void GestionMenus::initialiseMenuMatch()
{
	phase_match.setCodeMenu(CODE_MENU_MATCH);
	phase_match.setTitre("CSSMatch : Match");
	phase_match.addLigne("%s le alltalk");
	phase_match.addLigne("Restart");
	phase_match.addLigne("Stopper le match");
	phase_match.addLigne("RedÃ©tecter les tags");
	phase_match.addLigne("Recommencer la manche");
}

void GestionMenus::initialiseMenuConfiguration()
{
	configurations.setCodeMenu(CODE_MENU_CONFIG);
	configurations.setCodeMenuParent(CODE_MENU_CUTROUND_QUESTION);
	configurations.setTitre("CSSMatch : Choix d'une configuration");
	configurations.specialise(CFGLIST);
	// Corps mit à jour à chaque appel du menu
}

void GestionMenus::initialiseMenuAdministration()
{
	administration.setCodeMenu(CODE_MENU_ADMIN);
	administration.setTitre("CSSMatch : Administration");
	administration.addLigne("Changer de map");
	administration.addLigne("Swapper un joueur");
	administration.addLigne("Mettre un joueur en spectateur");
	administration.addLigne("Kicker un joueur");
	administration.addLigne("Bannir un joueur");
	administration.addLigne("Retour"); // Remplace le codeMenuParent, car le menu parent dépend de la phase de match
}

void GestionMenus::initialiseMenuChangelevel()
{
	admin_changelevel.setCodeMenu(CODE_MENU_CHANGELEVEL);
	admin_changelevel.setCodeMenuParent(CODE_MENU_ADMIN);
	admin_changelevel.setTitre("CSSMatch : Choisissez une map");
	admin_changelevel.specialise(MAPLIST);
	// Corps en fonction des joueurs connectés
}

void GestionMenus::initialiseMenuSwap()
{
	admin_swap.setCodeMenu(CODE_MENU_SWAP);
	admin_swap.setCodeMenuParent(CODE_MENU_ADMIN);
	admin_swap.setTitre("CSSMatch : Choix d'un joueur Ã  swapper");
	admin_swap.specialise(PLAYERLIST_SANS_SPEC);
	// Corps en fonction des joueurs connectés
}

void GestionMenus::initialiseMenuSpec()
{
	admin_spec.setCodeMenu(CODE_MENU_SPEC);
	admin_spec.setCodeMenuParent(CODE_MENU_ADMIN);
	admin_spec.setTitre("CSSMatch : Choix d'un joueur Ã  mettre en spectateur");
	admin_spec.specialise(PLAYERLIST);
	// Corps en fonction des joueurs connectés
}

void GestionMenus::initialiseMenuKick()
{
	admin_kick.setCodeMenu(CODE_MENU_KICK);
	admin_kick.setCodeMenuParent(CODE_MENU_ADMIN);
	admin_kick.setTitre("CSSMatch : Choix d'un joueur Ã  kicker");
	admin_kick.specialise(PLAYERLIST);
	// Corps en fonction des joueurs connectés
}

void GestionMenus::initialiseMenuBan()
{
	admin_ban.setCodeMenu(CODE_MENU_BAN);
	admin_ban.setCodeMenuParent(CODE_MENU_ADMIN);
	admin_ban.setTitre("CSSMatch : Choix d'un joueur Ã  bannir");
	admin_ban.specialise(PLAYERLIST);
	// Corps en fonction des joueurs connectés
}

void GestionMenus::initialiseMenuBanTime()
{
	admin_ban_time.setCodeMenu(CODE_MENU_BAN_TIME);
	admin_ban_time.setTitre("CSSMatch : Choix d'une durÃ©e pour le ban");
	admin_ban_time.addLigne("5 min");
	admin_ban_time.addLigne("1 H");
	admin_ban_time.addLigne("Permanent");
	admin_ban_time.addLigne("Retour"); // Remplace le codeMenuParent, car le menu parent dépend des joueurs connectés
}

void GestionMenus::initialiseMenus()
{
	cutround_question.purge();	
	initialiseMenuChoixCutRound();
	cutround_question.setCallBack(cutround_question_choix);

	phase_off.purge();
	initialiseMenuOff();
	phase_off.setCallBack(phase_off_choix);

	phase_cutround.purge();
	initialiseMenuCutRound();
	phase_cutround.setCallBack(phase_cutround_choix);

	phase_strats.purge();
	initialiseMenuStratsTime();
	phase_strats.setCallBack(phase_strats_choix);

	phase_match.purge();
	initialiseMenuMatch();
	phase_match.setCallBack(phase_match_choix);

	// configurations.purge(); // dans Menu::cfglist()
	initialiseMenuConfiguration();
	configurations.setCallBack(configurations_choix);

	// On purge les menus que l'on utilisera plus si l'on passe cssmatch_advaced de 1 à 0
	administration.purge();
	admin_changelevel.purge();
	admin_swap.purge();
	admin_spec.purge();
}

void GestionMenus::initialiseMenusAvecAdministration()
{
	cutround_question.purge();	
	initialiseMenuChoixCutRound();
	cutround_question.setCallBack(cutround_question_choix);

	phase_off.purge();
	phase_off.addLigne("Options d'administration");
	phase_off.addLigne("Lancer un match");
	phase_off.setCallBack(phase_off_choixAvecAdmin);

	phase_cutround.purge();
	phase_cutround.addLigne("Options d'administration");
	initialiseMenuCutRound();
	phase_cutround.setCallBack(phase_cutround_choixAvecAdmin);

	phase_strats.purge();
	phase_strats.addLigne("Options d'administration");
	initialiseMenuStratsTime();
	phase_strats.setCallBack(phase_strats_choixAvecAdmin);

	phase_match.purge();
	phase_match.addLigne("Options d'administration");
	initialiseMenuMatch();
	phase_match.setCallBack(phase_match_choixAvecAdmin);

	// configurations.purge(); // dans Menu::cfglist()
	initialiseMenuConfiguration();
	configurations.setCallBack(configurations_choix);

	administration.purge();
	initialiseMenuAdministration();
	administration.setCallBack(administration_choix);

	// admin_changelevel.purge(); // 
	initialiseMenuChangelevel();
	admin_changelevel.setCallBack(admin_changelevel_choix);

	// admin_swap.purge(); // dans le playerlist()
	initialiseMenuSwap();
	admin_swap.setCallBack(admin_swap_choix);

	// admin_spec.purge(); // dans le playerlist()
	initialiseMenuSpec();
	admin_spec.setCallBack(admin_spec_choix);

	// admin_kick.purge() // dans le playerlist()
	initialiseMenuKick();
	admin_kick.setCallBack(admin_kick_choix);

	// admin_ban.purge() dans le playerlist()
	initialiseMenuBan();
	admin_ban.setCallBack(admin_ban_choix);

	admin_ban_time.purge();
	initialiseMenuBanTime();
	admin_ban_time.setCallBack(admin_ban_time_choix);
}

void GestionMenus::afficheMenuRef(Menu & menu,int indexJoueur)
{
	// On récupère un pointeur sur le joueur concerné
	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);

	// On donne au joueur sa propre copie du menu
	joueur->setMenu(menu);

	// On récupère l'adresse mémoire de la copie du menu spécifique au joueur
	Menu * menuJoueur = joueur->getMenu();

	// Si le menu est spécialisé, on le construit 
	switch(menuJoueur->getSpecialisation())
	{
	case PLAYERLIST:
		menuJoueur->playerlist();
		break;
	case PLAYERLIST_SANS_SPEC:
		menuJoueur->playerlist(false);
		break;
	case CFGLIST:
		menuJoueur->cfglist(DOSSIER_CONFIGURATIONS_MATCH);
		break;
	case MAPLIST:
		menuJoueur->filelist(FICHIER_MAPLIST);
	}

	try
	{
		menuJoueur->affiche(indexJoueur,1);
	}
	catch(const out_of_range & e)
	{
		Messages::sayTell(indexJoueur,"Impossible d'afficher cette page du menu ! Veuillez en informer l'auteur !");
		API::debug("Impossible d'afficher une page du menu \"" + menuJoueur->getTitre() + "\" ! (" + string(e.what()) + ")");
	}
}

void GestionMenus::afficheMenu(CodePhase phase, int indexJoueur)
{
	// En fonction de la valeur de cssmatch_advanced on prend en compte ou pas les options d'administration
	if (ConVars::cssmatch_advanced.GetBool())
		afficheMenuAvecAdministration(phase,indexJoueur);
	else
		afficheMenuSansAdministration(phase,indexJoueur);
}

void GestionMenus::afficheMenuSansAdministration(CodePhase phase, int indexJoueur)
{
	// Nous allons remplacer le %s de l'option AllTalk on/off par "Activer" ou "Désactiver" le temps de l'envoi du menu
	string backupAllTalkOption;
	char ligneTemp[32];

	// Le menu à afficher sera différent selon la phase de match
	switch(phase)
	{
	case PHASE_OFF:
	case PHASE_FIN:
		afficheMenuRef(phase_off,indexJoueur);
		break;
	case PHASE_PASSAGE_CUTROUND:
	case PHASE_CUTROUND:
		backupAllTalkOption = phase_cutround.getLigne(1).getTexte();
		if (ConVars::sv_alltalk->GetBool())
			V_snprintf(ligneTemp,sizeof(ligneTemp),backupAllTalkOption.c_str(),"DÃ©sactiver");
		else
			V_snprintf(ligneTemp,sizeof(ligneTemp),backupAllTalkOption.c_str(),"Activer");
		phase_cutround.setLigne(1,ligneTemp);
		afficheMenuRef(phase_cutround,indexJoueur);
		phase_cutround.setLigne(1,backupAllTalkOption);
		break;
	case PHASE_PASSAGE_STRATS:
	case PHASE_STRATS:
		backupAllTalkOption = phase_strats.getLigne(1).getTexte();
		if (ConVars::sv_alltalk->GetBool())
			V_snprintf(ligneTemp,sizeof(ligneTemp),backupAllTalkOption.c_str(),"DÃ©sactiver");
		else
			V_snprintf(ligneTemp,sizeof(ligneTemp),backupAllTalkOption.c_str(),"Activer");
		phase_strats.setLigne(1,ligneTemp);
		afficheMenuRef(phase_strats,indexJoueur);
		phase_strats.setLigne(1,backupAllTalkOption);
		break;
	case PHASE_PASSAGE_MANCHE:
	case PHASE_MANCHE:
		backupAllTalkOption = phase_match.getLigne(1).getTexte();
		if (ConVars::sv_alltalk->GetBool())
			V_snprintf(ligneTemp,sizeof(ligneTemp),backupAllTalkOption.c_str(),"DÃ©sactiver");
		else
			V_snprintf(ligneTemp,sizeof(ligneTemp),backupAllTalkOption.c_str(),"Activer");
		phase_match.setLigne(1,ligneTemp);
		afficheMenuRef(phase_match,indexJoueur);
		phase_match.setLigne(1,backupAllTalkOption);
	}
}

void GestionMenus::afficheMenuAvecAdministration(CodePhase phase, int indexJoueur)
{
		// Nous allons remplacer le %s de l'option par on ou off le temps de l'envoi du menu
	string backupAllTalkOption;
	char ligneTemp[32];

	// Le menu à afficher sera différent selon la phase de match
	switch(phase)
	{
	case PHASE_OFF:
	case PHASE_FIN:
		GestionMenus::afficheMenuRef(phase_off,indexJoueur);
		break;
	case PHASE_PASSAGE_CUTROUND:
	case PHASE_CUTROUND:
		backupAllTalkOption = phase_cutround.getLigne(2).getTexte();
		if (ConVars::sv_alltalk->GetBool())
			V_snprintf(ligneTemp,sizeof(ligneTemp),backupAllTalkOption.c_str(),"DÃ©sactiver");
		else
			V_snprintf(ligneTemp,sizeof(ligneTemp),backupAllTalkOption.c_str(),"Activer");
		phase_cutround.setLigne(2,ligneTemp);
		afficheMenuRef(phase_cutround,indexJoueur);
		phase_cutround.setLigne(2,backupAllTalkOption);
		break;
	case PHASE_PASSAGE_STRATS:
	case PHASE_STRATS:
		backupAllTalkOption = phase_strats.getLigne(2).getTexte();
		if (ConVars::sv_alltalk->GetBool())
			V_snprintf(ligneTemp,sizeof(ligneTemp),backupAllTalkOption.c_str(),"DÃ©sactiver");
		else
			V_snprintf(ligneTemp,sizeof(ligneTemp),backupAllTalkOption.c_str(),"Activer");
		phase_strats.setLigne(2,ligneTemp);
		afficheMenuRef(phase_strats,indexJoueur);
		phase_strats.setLigne(2,backupAllTalkOption);
		break;
	case PHASE_PASSAGE_MANCHE:
	case PHASE_MANCHE:
		backupAllTalkOption = phase_match.getLigne(2).getTexte();
		if (ConVars::sv_alltalk->GetBool())
			V_snprintf(ligneTemp,sizeof(ligneTemp),backupAllTalkOption.c_str(),"DÃ©sactiver");
		else
			V_snprintf(ligneTemp,sizeof(ligneTemp),backupAllTalkOption.c_str(),"Activer");
		phase_match.setLigne(2,ligneTemp);
		afficheMenuRef(phase_match,indexJoueur);
		phase_match.setLigne(2,backupAllTalkOption);
	}
}

void GestionMenus::actionMenu(int indexJoueur, int choix)
{
	// On récupère le menu actuellement utilisé par le joueur
	Menu * menu = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur)->getMenu();

	// Si un menu du plugin est effectivement utilisé, on appelle la fonction associée
	if (menu->getCodeMenu() != CODE_MENU_INVALIDE)
		menu->callBack(indexJoueur,choix);
}

void GestionMenus::afficheMenuPrecedent(int indexJoueur)
{
	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	int pageMenu = joueur->decPageMenu();
	if (pageMenu>0)
	{
		try
		{
			menuJoueur->affiche(indexJoueur,pageMenu);
		}
		catch(const out_of_range & e)
		{
			Messages::sayTell(indexJoueur,"Impossible d'afficher cette page du menu ! Veuillez en informer l'auteur !");
			API::debug("Impossible d'afficher une page du menu \"" + menuJoueur->getTitre() + "\" ! (" + string(e.what()) + ")");
		}
	}
	else
	{
		switch(menuJoueur->getCodeMenuParent())
		{
		case CODE_MENU_CUTROUND_QUESTION:
			afficheMenuRef(cutround_question,indexJoueur);
			break;
		case CODE_MENU_ADMIN:
			afficheMenuRef(administration,indexJoueur);
		}
	}
}

void GestionMenus::afficheMenuSuivant(int indexJoueur)
{
	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	try
	{
		menuJoueur->affiche(indexJoueur,joueur->incPageMenu());
	}
	catch(const out_of_range & e)
	{
		Messages::sayTell(indexJoueur,"Impossible d'afficher cette page du menu ! Veuillez en informer l'auteur !");
		API::debug("Impossible d'afficher une page du menu \"" + menuJoueur->getTitre() + "\" ! (" + string(e.what()) + ")");
	}
}

void GestionMenus::quitteMenu(int indexJoueur)
{
	// On altère la copie du menu du joueur
	GestionJoueurs::getPlayerList()->getJoueur(indexJoueur)->getMenu()->setCodeMenu(CODE_MENU_INVALIDE);
}
