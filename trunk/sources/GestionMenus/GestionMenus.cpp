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

using std::string;

GestionMenus * GestionMenus::menus = NULL;
Menu GestionMenus::cutround_question;
Menu GestionMenus::strats_question;
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


void cutround_question_choix(int indexJoueur, int choix) throw(CSSMatchApiException)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	switch(choix)
	{
	case 1:
		CSSMatch::getInstance()->getMatch()->setCutRound(true); // la variable est déjà à true dans le constructeur de match, mais le lancement du match peut encore être annulé
		gestionnaireMenus->afficheMenuRef(GestionMenus::strats_question,indexJoueur);
		return;
	case 2:
		CSSMatch::getInstance()->getMatch()->setCutRound(false);
		gestionnaireMenus->afficheMenuRef(GestionMenus::strats_question,indexJoueur);
		return;
	case 3:
		Api::cexec(pEntity,"cssmatch\n");
		return;
	}
	gestionnaireMenus->quitteMenu(indexJoueur);
}

void strats_question_choix(int indexJoueur, int choix) throw(CSSMatchApiException)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	switch(choix)
	{
	case 1:
		CSSMatch::getInstance()->getMatch()->setStrats(true); // la variable est déjà à true dans le constructeur de match, mais le lancement du match peut encore être annulé
		gestionnaireMenus->afficheMenuRef(GestionMenus::configurations,indexJoueur);
		return;
	case 2:
		CSSMatch::getInstance()->getMatch()->setStrats(false);
		gestionnaireMenus->afficheMenuRef(GestionMenus::configurations,indexJoueur);
		return;
	case 3:
		Api::cexec(pEntity,"cssmatch\n");
		return;
	}
	gestionnaireMenus->quitteMenu(indexJoueur);
}

void phase_off_choix(int indexJoueur, int choix) throw(CSSMatchApiException)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	switch(choix)
	{
	case 1:
		ConVars::swapAlltalk();
		Api::cexec(pEntity,"cssmatch\n");
		break;
	case 2:
		gestionnaireMenus->afficheMenuRef(GestionMenus::cutround_question,indexJoueur);
		break;
	default:
		gestionnaireMenus->quitteMenu(indexJoueur);
	}		
}

void phase_cutround_choix(int indexJoueur, int choix) throw(CSSMatchApiException)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	switch(choix)
	{
	case 1:
		ConVars::swapAlltalk();
		Api::cexec(pEntity,"cssmatch\n");
		break;
	case 2:
		CSSMatch::getInstance()->getMatch()->restartRound(pEntity);
		break;
	case 3:
		CSSMatch::getInstance()->getMatch()->finMatch(pEntity);
		break;
	case 4:
		CSSMatch::getInstance()->getMatch()->detecteTags(true);
	}
	gestionnaireMenus->quitteMenu(indexJoueur);
}

void phase_strats_choix(int indexJoueur, int choix) throw(CSSMatchApiException)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	switch(choix)
	{
	case 1:
		ConVars::swapAlltalk();
		Api::cexec(pEntity,"cssmatch\n");
		break;
	case 2:
		CSSMatch::getInstance()->getMatch()->restartRound(pEntity);
		break;
	case 3:
		CSSMatch::getInstance()->getMatch()->finMatch(pEntity);
		break;
	case 4:
		CSSMatch::getInstance()->getMatch()->detecteTags(true);
		break;
	case 5:
		CSSMatch::getInstance()->getMatch()->forceGo(pEntity);
	}
	gestionnaireMenus->quitteMenu(indexJoueur);
}

void phase_match_choix(int indexJoueur, int choix) throw(CSSMatchApiException)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	switch(choix)
	{
	case 1:
		ConVars::swapAlltalk();
		Api::cexec(pEntity,"cssmatch\n");
		break;
	case 2:
		CSSMatch::getInstance()->getMatch()->restartRound(pEntity);
		break;
	case 3:
		CSSMatch::getInstance()->getMatch()->finMatch(pEntity);
		break;
	case 4:
		CSSMatch::getInstance()->getMatch()->detecteTags(true);
		break;
	case 5:
		CSSMatch::getInstance()->getMatch()->restartManche(pEntity);
		break;
	}
	gestionnaireMenus->quitteMenu(indexJoueur);
}

void configurations_choix(int indexJoueur, int choix) throw(CSSMatchApiException)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	if (choix!=10)
	{
		const Ligne * config;
		try
		{
			config = &menuJoueur->getChoix(choix,joueur->getPageMenu());
			switch(config->getValeur())
			{
			case MENU_CODE_RETOUR:
				gestionnaireMenus->afficheMenuPrecedent(indexJoueur);
				break;
			case MENU_CODE_SUIVANT:
				gestionnaireMenus->afficheMenuSuivant(indexJoueur);
				break;
			default:
				CSSMatch::getInstance()->getMatch()->lanceMatch(new Configuration(config->getTexte()),pEntity); // Présence du fichier recontrôlée dans lanceMatch()
				gestionnaireMenus->quitteMenu(indexJoueur);
			}
		}
		catch(const CSSMatchMenuException & e)
		{
			Api::reporteException(e,__FILE__,__LINE__);
			gestionnaireMenus->quitteMenu(indexJoueur);
		}
	}
	else
		gestionnaireMenus->quitteMenu(indexJoueur);
}

void phase_off_choixAvecAdmin(int indexJoueur, int choix)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	switch(choix)
	{
	case 1:
		gestionnaireMenus->afficheMenuRef(GestionMenus::administration,indexJoueur);
		break;
	case 2:
		ConVars::swapAlltalk();
		Api::cexec(pEntity,"cssmatch\n");
		break;
	case 3:
		gestionnaireMenus->afficheMenuRef(GestionMenus::cutround_question,indexJoueur);
		break;
	default:
		gestionnaireMenus->quitteMenu(indexJoueur);
	}
}

void phase_cutround_choixAvecAdmin(int indexJoueur, int choix) throw(CSSMatchApiException)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	switch(choix)
	{
	case 1:
		gestionnaireMenus->afficheMenuRef(GestionMenus::administration,indexJoueur);
		break;
	case 2:
		ConVars::swapAlltalk();
		Api::cexec(pEntity,"cssmatch\n");
		gestionnaireMenus->quitteMenu(indexJoueur);
		break;
	case 3:
		CSSMatch::getInstance()->getMatch()->restartRound(pEntity);
		gestionnaireMenus->quitteMenu(indexJoueur);
		break;
	case 4:
		CSSMatch::getInstance()->getMatch()->finMatch(pEntity);
		gestionnaireMenus->quitteMenu(indexJoueur);
		break;
	case 5:
		CSSMatch::getInstance()->getMatch()->detecteTags(true);
		gestionnaireMenus->quitteMenu(indexJoueur);
	default:
		gestionnaireMenus->quitteMenu(indexJoueur);
	}
}

void phase_strats_choixAvecAdmin(int indexJoueur, int choix) throw(CSSMatchApiException)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	switch(choix)
	{
	case 1:
		gestionnaireMenus->afficheMenuRef(GestionMenus::administration,indexJoueur);
		break;
	case 2:
		ConVars::swapAlltalk();
		Api::cexec(pEntity,"cssmatch\n");
		break;
	case 3:
		CSSMatch::getInstance()->getMatch()->restartRound(pEntity);
		break;
	case 4:
		CSSMatch::getInstance()->getMatch()->finMatch(pEntity);
		break;
	case 5:
		CSSMatch::getInstance()->getMatch()->detecteTags(true);
		break;
	case 6:
		CSSMatch::getInstance()->getMatch()->forceGo(pEntity);
		break;
	default:
		gestionnaireMenus->quitteMenu(indexJoueur);
	}
	
}

void phase_match_choixAvecAdmin(int indexJoueur, int choix) throw(CSSMatchApiException)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	switch(choix)
	{
	case 1:
		gestionnaireMenus->afficheMenuRef(GestionMenus::administration,indexJoueur);
		break;
	case 2:
		ConVars::swapAlltalk();
		Api::cexec(pEntity,"cssmatch\n");
		break;
	case 3:
		CSSMatch::getInstance()->getMatch()->restartRound(pEntity);
		break;
	case 4:
		CSSMatch::getInstance()->getMatch()->finMatch(pEntity);
		break;
	case 5:
		CSSMatch::getInstance()->getMatch()->detecteTags(true);
		break;
	case 6:
		CSSMatch::getInstance()->getMatch()->restartManche(pEntity);
		break;
	default:
		gestionnaireMenus->quitteMenu(indexJoueur);
	}
}

void administration_choix(int indexJoueur, int choix) throw(CSSMatchApiException)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	switch(choix)
	{
	case 1:
		gestionnaireMenus->afficheMenuRef(GestionMenus::admin_changelevel,indexJoueur);
		break;
	case 2:
		gestionnaireMenus->afficheMenuRef(GestionMenus::admin_swap,indexJoueur);
		break;
	case 3:
		gestionnaireMenus->afficheMenuRef(GestionMenus::admin_spec,indexJoueur);
		break;
	case 4:
		gestionnaireMenus->afficheMenuRef(GestionMenus::admin_kick,indexJoueur);
		break;
	case 5:
		gestionnaireMenus->afficheMenuRef(GestionMenus::admin_ban,indexJoueur);
		break;
	case 6:
		Api::cexec(pEntity,"cssmatch\n");
		break;
	default:
		gestionnaireMenus->quitteMenu(indexJoueur);
	}
}

void admin_changelevel_choix(int indexJoueur, int choix)
{
	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	if (choix!=10)
	{
		const Ligne * ligne = NULL;
		try
		{
			ligne = &menuJoueur->getChoix(choix,joueur->getPageMenu());
			switch(ligne->getValeur())
			{
			case MENU_CODE_RETOUR:
				gestionnaireMenus->afficheMenuPrecedent(indexJoueur);
				break;
			case MENU_CODE_SUIVANT:
				gestionnaireMenus->afficheMenuSuivant(indexJoueur);
				break;
			default:
				{
					const string * nomMap = &ligne->getTexte();
					if (Api::engine->IsMapValid(nomMap->c_str()))
						Api::serveurExecute("changelevel " + *nomMap + "\n");
					else
						Messages::sayTell(indexJoueur,"Impossible de charger la map " + *nomMap + " !");
					gestionnaireMenus->quitteMenu(indexJoueur);
				}
			}
		}
		catch(const CSSMatchMenuException & e)
		{
			Api::reporteException(e,__FILE__,__LINE__);
			gestionnaireMenus->quitteMenu(indexJoueur);
		}
	}
	else
		gestionnaireMenus->quitteMenu(indexJoueur);
}

void admin_swap_choix(int indexJoueur, int choix)
{
	// Récupération de l'entité correspondant à l'index du joueur
	edict_t * pEntity = Api::engine->PEntityOfEntIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	if (choix!=10)
	{
		const Ligne * ligne = NULL;
		try
		{
			ligne = &menuJoueur->getChoix(choix,joueur->getPageMenu());
			switch(ligne->getValeur())
			{
			case MENU_CODE_RETOUR:
				gestionnaireMenus->afficheMenuPrecedent(indexJoueur);
				break;
			case MENU_CODE_SUIVANT:
				gestionnaireMenus->afficheMenuSuivant(indexJoueur);
				break;
			default:
				{
					int useridCible = ligne->getValeur();
					edict_t * pEntity = NULL;
					try
					{
						pEntity = Api::getPEntityFromID(useridCible);
						Api::swap(Api::getIPlayerInfoFromEntity(pEntity));
						Messages::sayMsg(ligne->getTexte() + " est swappÃ© par \003" + Api::getPlayerName(Api::engine->PEntityOfEntIndex(indexJoueur)),indexJoueur);
					}
					catch(const CSSMatchApiException & e)
					{
						Messages::sayTell(indexJoueur,ligne->getTexte() + " semble s'Ãªtre dÃ©connectÃ©");
						Api::reporteException(e,__FILE__,__LINE__);
					}
					gestionnaireMenus->afficheMenuRef(GestionMenus::admin_swap,indexJoueur);
				}
			}
		}
		catch(const CSSMatchMenuException & e)
		{
			Api::reporteException(e,__FILE__,__LINE__);
			gestionnaireMenus->quitteMenu(indexJoueur);
		}
	}
	else
		gestionnaireMenus->quitteMenu(indexJoueur);
}

void admin_spec_choix(int indexJoueur, int choix)
{
	// Récupération de l'entité correspondant à l'index du joueur
	edict_t * pEntity = Api::engine->PEntityOfEntIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	if (choix!=10)
	{
		const Ligne * ligne = NULL;
		try
		{
			ligne = &menuJoueur->getChoix(choix,joueur->getPageMenu());
			switch(ligne->getValeur())
			{
			case MENU_CODE_RETOUR:
				gestionnaireMenus->afficheMenuPrecedent(indexJoueur);
				break;
			case MENU_CODE_SUIVANT:
				gestionnaireMenus->afficheMenuSuivant(indexJoueur);
				break;
			default:
				{
					int useridCible = ligne->getValeur();
					edict_t * pEntity = NULL;
					try
					{
						pEntity = Api::getPEntityFromID(useridCible);
						Api::spec(Api::getIPlayerInfoFromEntity(pEntity));
						Messages::sayMsg(ligne->getTexte() + " est mit en spectateur par \003" + Api::getPlayerName(Api::engine->PEntityOfEntIndex(indexJoueur)),indexJoueur);
					}
					catch(const CSSMatchApiException & e)
					{
						Messages::sayTell(indexJoueur,ligne->getTexte() + " semble s'Ãªtre dÃ©connectÃ©");
						Api::reporteException(e,__FILE__,__LINE__);
					}
					gestionnaireMenus->afficheMenuRef(GestionMenus::admin_spec,indexJoueur);
				}
			}
		}
		catch(const CSSMatchMenuException & e)
		{
			Api::reporteException(e,__FILE__,__LINE__);
			gestionnaireMenus->quitteMenu(indexJoueur);
		}
	}
	else
		gestionnaireMenus->quitteMenu(indexJoueur);
}

void admin_kick_choix(int indexJoueur, int choix)
{
	// Récupération de l'entité correspondant à l'index du joueur
	edict_t * pEntity = Api::engine->PEntityOfEntIndex(indexJoueur);

	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	if (choix!=10)
	{
		const Ligne * ligne = NULL;
		try
		{
			ligne = &menuJoueur->getChoix(choix,joueur->getPageMenu());
			switch(ligne->getValeur())
			{
			case MENU_CODE_RETOUR:
				gestionnaireMenus->afficheMenuPrecedent(indexJoueur);
				break;
			case MENU_CODE_SUIVANT:
				gestionnaireMenus->afficheMenuSuivant(indexJoueur);
				break;
			default:
				{
					int useridCible = ligne->getValeur();
					edict_t * pEntity = NULL;
					try
					{
						pEntity = Api::getPEntityFromID(useridCible);
						Api::kickid(useridCible,"Kick par un arbitre");	
						Messages::sayMsg(ligne->getTexte() + " est kickÃ© par \003" + Api::getPlayerName(Api::engine->PEntityOfEntIndex(indexJoueur)),indexJoueur);
					}
					catch(const CSSMatchApiException & e)
					{
						Messages::sayTell(indexJoueur,ligne->getTexte() + " semble s'Ãªtre dÃ©connectÃ©");
						Api::reporteException(e,__FILE__,__LINE__);
					}
					gestionnaireMenus->afficheMenuRef(GestionMenus::administration,indexJoueur);
				}
			}
		}
		catch(const CSSMatchMenuException & e)
		{
			Api::reporteException(e,__FILE__,__LINE__);
			gestionnaireMenus->afficheMenuRef(GestionMenus::administration,indexJoueur);
		}
	}
	else
		gestionnaireMenus->quitteMenu(indexJoueur);
}

void admin_ban_choix(int indexJoueur, int choix)
{
	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * gestionnaireMenus = GestionMenus::getMenus();

	if (choix!=10)
	{
		const Ligne * ligne = NULL;
		try
		{
			ligne = &menuJoueur->getChoix(choix,joueur->getPageMenu());
			switch(ligne->getValeur())
			{
			case MENU_CODE_RETOUR:
				gestionnaireMenus->afficheMenuPrecedent(indexJoueur);
				break;
			case MENU_CODE_SUIVANT:
				gestionnaireMenus->afficheMenuSuivant(indexJoueur);
				break;
			default:
				{
					int useridCible = ligne->getValeur();
					edict_t * pEntity = NULL;
					try
					{
						pEntity = Api::getPEntityFromID(useridCible);
					}
					catch(const CSSMatchApiException & e)
					{
						Messages::sayTell(indexJoueur,ligne->getTexte() + " semble s'Ãªtre dÃ©connectÃ©");
						Api::reporteException(e,__FILE__,__LINE__);
					}
					joueur->setUseridCible(useridCible);
					gestionnaireMenus->afficheMenuRef(GestionMenus::admin_ban_time,indexJoueur);
				}
			}
		}
		catch(const CSSMatchMenuException & e)
		{
			Api::reporteException(e,__FILE__,__LINE__);
			gestionnaireMenus->afficheMenuRef(GestionMenus::administration,indexJoueur);
		}	
	}
	else
		gestionnaireMenus->quitteMenu(indexJoueur);
}

void admin_ban_time_choix(int indexJoueur, int choix)
{
	// Récupération de l'entité correspondant à l'index du joueur
	edict_t * pEntity = Api::engine->PEntityOfEntIndex(indexJoueur);

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
				Api::banid(5,useridCible,"Ban par un arbitre");
				Messages::sayMsg(Api::getPlayerName(Api::getPEntityFromID(useridCible)) + " est temporairement banni par \003" + Api::getPlayerName(Api::engine->PEntityOfEntIndex(indexJoueur)),indexJoueur);
				gestionnaireMenus->afficheMenuRef(GestionMenus::administration,indexJoueur);
				break;
			}
		case 2:
			{
				int useridCible = joueur->getUseridCible();
				Api::banid(60,useridCible,"Ban par un arbitre");
				Messages::sayMsg(Api::getPlayerName(Api::getPEntityFromID(useridCible)) + " est temporairement banni par \003" + Api::getPlayerName(Api::engine->PEntityOfEntIndex(indexJoueur)),indexJoueur);
				gestionnaireMenus->afficheMenuRef(GestionMenus::administration,indexJoueur);
				break;
			}
		case 3:
			{
				int useridCible = joueur->getUseridCible();
				Api::banid(0,useridCible,"Ban par un arbitre");
				Messages::sayMsg(Api::getPlayerName(Api::getPEntityFromID(useridCible)) + " est banni de faÃ§on permanente par \003" + Api::getPlayerName(Api::engine->PEntityOfEntIndex(indexJoueur)),indexJoueur);
				gestionnaireMenus->afficheMenuRef(GestionMenus::administration,indexJoueur);
				break;
			}
		case 4:
			gestionnaireMenus->afficheMenuRef(GestionMenus::admin_ban,indexJoueur);
			break;
		default:
			gestionnaireMenus->quitteMenu(indexJoueur);
		}
	}
	catch(const CSSMatchApiException & e)
	{
		Api::reporteException(e,__FILE__,__LINE__);
		gestionnaireMenus->quitteMenu(indexJoueur);
	}
}

GestionMenus::GestionMenus()
{
}

GestionMenus::~GestionMenus()
{
}

GestionMenus * GestionMenus::getMenus()
{
	if (menus == NULL)
		menus = new GestionMenus();
	return menus;
}

void GestionMenus::killMenus()
{
	if (menus != NULL)
	{
		delete menus;
		menus = NULL;
	}
}

void GestionMenus::initialiseMenuChoixCutRound()
{
	cutround_question.setCodeMenu(CODE_MENU_CUTROUND_QUESTION);
	cutround_question.setTitre("CSSMatch : Voulez-vous jouer un round au couteau ?");
	cutround_question.addLigne("Oui");
	cutround_question.addLigne("Non");
	cutround_question.addLigne("Retour");
}

void GestionMenus::initialiseMenuChoixStrats()
{
	strats_question.setCodeMenu(CODE_MENU_STRATS_QUESTION);
	strats_question.setTitre("CSSMatch : Des strats devront-ils Ãªtre jouÃ©s ?");
	strats_question.addLigne("Oui");
	strats_question.addLigne("Non");
	strats_question.addLigne("Retour");
}

void GestionMenus::initialiseMenuOff()
{
	phase_off.setCodeMenu(CODE_MENU_OFF);
	phase_off.setTitre("CSSMatch : Aucun match en cours");
	phase_off.addLigne("%s le alltalk");
	phase_off.addLigne("Lancer un match");
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
	// Corps mit à jour à chaque affichage du menu
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
	admin_swap.setTitre("CSSMatch : Choix d'un joueur");
	admin_swap.specialise(PLAYERLIST_SANS_SPEC);
	// Corps en fonction des joueurs connectés
}

void GestionMenus::initialiseMenuSpec()
{
	admin_spec.setCodeMenu(CODE_MENU_SPEC);
	admin_spec.setCodeMenuParent(CODE_MENU_ADMIN);
	admin_spec.setTitre("CSSMatch : Choix d'un joueur");
	admin_spec.specialise(PLAYERLIST);
	// Corps en fonction des joueurs connectés
}

void GestionMenus::initialiseMenuKick()
{
	admin_kick.setCodeMenu(CODE_MENU_KICK);
	admin_kick.setCodeMenuParent(CODE_MENU_ADMIN);
	admin_kick.setTitre("CSSMatch : Choix d'un joueur");
	admin_kick.specialise(PLAYERLIST);
	// Corps en fonction des joueurs connectés
}

void GestionMenus::initialiseMenuBan()
{
	admin_ban.setCodeMenu(CODE_MENU_BAN);
	admin_ban.setCodeMenuParent(CODE_MENU_ADMIN);
	admin_ban.setTitre("CSSMatch : Choix d'un joueur");
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

	strats_question.purge();	
	initialiseMenuChoixStrats();
	strats_question.setCallBack(strats_question_choix);

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
	initialiseMenuOff();
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

void GestionMenus::afficheMenuAlltalkOption(Menu & menu, int ligne, int indexJoueur)
{
	// %s de la ligne du menu concerné sera remplacé par "Activer" ou "Désactiver" le temps de l'envoi du menu
	string ligneTemp;
	string alltalkDesactiver = "DÃ©sactiver";
	string alltalkActiver= "Activer";

	string backupAllTalkOption = menu.getLigne(ligne).getTexte();
	if (ConVars::sv_alltalk->GetBool())
	{
		ligneTemp = backupAllTalkOption;
		ligneTemp.replace(
			ligneTemp.find("%s"),2,alltalkDesactiver,0,alltalkDesactiver.size());
	}
	else
	{
		ligneTemp = backupAllTalkOption;
		ligneTemp.replace(
			ligneTemp.find("%s"),2,alltalkActiver,0,alltalkActiver.size());
	}
	menu.setLigne(ligne,ligneTemp);
	GestionMenus::getMenus()->afficheMenuRef(menu,indexJoueur);
	menu.setLigne(ligne,backupAllTalkOption);
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
	catch(const CSSMatchMenuException & e)
	{
		Messages::sayTell(indexJoueur,"Impossible d'afficher cette page du menu ! Veuillez en informer l'auteur !");
		Api::debug("Impossible d'afficher une page du menu \"" + menuJoueur->getTitre() + "\" ! (" + string(e.what()) + ")");
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
	// Le menu à afficher sera différent selon la phase de match
	switch(phase)
	{
	case PHASE_OFF:
	case PHASE_FIN:
		afficheMenuAlltalkOption(phase_off,1,indexJoueur);
		break;
	case PHASE_PASSAGE_CUTROUND:
	case PHASE_CUTROUND:
		afficheMenuAlltalkOption(phase_cutround,1,indexJoueur);
		break;
	case PHASE_PASSAGE_STRATS:
	case PHASE_STRATS:
		afficheMenuAlltalkOption(phase_strats,1,indexJoueur);
		break;
	case PHASE_TRANSITION_MANCHE:
	case PHASE_PASSAGE_MANCHE:
	case PHASE_MANCHE:
		afficheMenuAlltalkOption(phase_match,1,indexJoueur);
	}
}

void GestionMenus::afficheMenuAvecAdministration(CodePhase phase, int indexJoueur)
{
	// Le menu à afficher sera différent selon la phase de match
	switch(phase)
	{
	case PHASE_OFF:
	case PHASE_FIN:
		afficheMenuAlltalkOption(phase_off,2,indexJoueur);
		break;
	case PHASE_PASSAGE_CUTROUND:
	case PHASE_CUTROUND:
		afficheMenuAlltalkOption(phase_cutround,2,indexJoueur);
		break;
	case PHASE_PASSAGE_STRATS:
	case PHASE_STRATS:
		afficheMenuAlltalkOption(phase_strats,2,indexJoueur);
		break;
	case PHASE_TRANSITION_MANCHE:
	case PHASE_PASSAGE_MANCHE:
	case PHASE_MANCHE:
		afficheMenuAlltalkOption(phase_match,2,indexJoueur);
	}
}

void GestionMenus::actionMenu(int indexJoueur, int choix)
{
	// On récupère le menu actuellement utilisé par le joueur
	Menu * menu = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur)->getMenu();

	// Si un menu du plugin est effectivement utilisé...
	if (menu->getCodeMenu() != CODE_MENU_INVALIDE)
	{
		try
		{
			Api::cexec(Api::getPEntityFromIndex(indexJoueur),"playgamesound UI/buttonrollover.wav\n");
		}
		catch(CSSMatchApiException) {}

		// Appelle de la fonction associée
		menu->callBack(indexJoueur,choix);
	}
}

void GestionMenus::afficheMenuPrecedent(int indexJoueur)
{
	Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	int pageMenu = joueur->decPageMenu();
	if (pageMenu > 0)
	{
		try
		{
			menuJoueur->affiche(indexJoueur,pageMenu);
		}
		catch(const CSSMatchMenuException & e)
		{
			Messages::sayTell(indexJoueur,"Impossible d'afficher cette page du menu, veuillez en informer l'auteur sur " CSSMATCH_SITE);
			Api::debug("Impossible d'afficher une page du menu \"" + menuJoueur->getTitre() + "\" ! (" + string(e.what()) + ")");
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
	catch(const CSSMatchMenuException & e)
	{
		Messages::sayTell(indexJoueur,"Impossible d'afficher cette page du menu ! Veuillez en informer l'auteur !");
		Api::debug("Impossible d'afficher une page du menu \"" + menuJoueur->getTitre() + "\" ! (" + string(e.what()) + ")");
	}
}

void GestionMenus::quitteMenu(int indexJoueur)
{
	// On altère la copie du menu du joueur
	GestionJoueurs::getPlayerList()->getJoueur(indexJoueur)->getMenu()->setCodeMenu(CODE_MENU_INVALIDE);
}
