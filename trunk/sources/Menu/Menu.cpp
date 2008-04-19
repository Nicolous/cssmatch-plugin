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

#include "Menu.h"

using std::string;
using std::out_of_range;

const Ligne Menu::retour = Ligne("Retour",MENU_CODE_RETOUR);
const Ligne Menu::suivant = Ligne("Suivant",MENU_CODE_SUIVANT);
const int Menu::maxOptions = 9;

Menu::Menu()
{
	titre = "CSSMatch";
	codeMenu = CODE_MENU_INVALIDE;
	codeMenuParent = CODE_MENU_INVALIDE;
	type = NOLIST;
	fonction = NULL;
}

Menu::Menu(const Menu & menu)
{
	titre = menu.titre;
	codeMenu = menu.codeMenu;
	codeMenuParent = menu.codeMenuParent;
	lignes = menu.lignes;
	type = menu.type;
	fonction = menu.fonction;
}

void Menu::setTitre(const string & titre)
{
	this->titre = titre;
}

const string & Menu::getTitre() const
{
	return titre;
}

void Menu::setCodeMenu(CodeMenu codeMenu)
{
	this->codeMenu = codeMenu;
}

CodeMenu Menu::getCodeMenu() const
{
	return codeMenu;
}

void Menu::setCodeMenuParent(CodeMenu codeMenuParent)
{
	this->codeMenuParent = codeMenuParent;
}

CodeMenu Menu::getCodeMenuParent() const
{
	return codeMenuParent;
}

void Menu::specialise(TypeSpeMenu type)
{
	this->type = type;
}

TypeSpeMenu Menu::getSpecialisation() const
{
	return type;
}

void Menu::setCallBack(void (* fonction)(int indexJoueur, int choix))
{
	this->fonction = fonction;
}

void Menu::callBack(int indexJoueur, int choix)
{
	if (fonction)
		fonction(indexJoueur,choix);
}

int Menu::getNbrLignes() const
{
	return lignes.size();
}

void Menu::addLigne(const string & ligne, int valeurAssociee)
{
	// Ajout d'un "challenge" à la fin de la liste
	Ligne aAjouter(ligne,valeurAssociee);
	// Ce challenge permet d'éviter d'avoir des conflits dans les listes de joueurs ou de fichier

	// Récupération du nombre de ligne contenues dans le menu
	int nbrLignes = lignes.size();

	// Si le menu n'a pas de menu parent le traitement est plus compliqué : pas de "retour" dès la première page
	if (codeMenuParent == CODE_MENU_INVALIDE)
	{
		// Si le menu ne fait toujours qu'une page ET si aucun menu parent n'existe
		if (nbrLignes < maxOptions)
		{
			// Aucun problème, on ajoute simplement la ligne
			lignes.push_back(aAjouter);
			return;
		}
		// Le sinon est tout à la fin de la fonction ;)

		// Si l'élément à insérer ne tient plus sur le première page
		if (nbrLignes == maxOptions) 
		{
			// Notre liste va compter 3 éléments en plus : "suivant", l'élément à insérer, et "retour"
			// maxOptions + 3 élément en plus

			// On sauvegarde l'élément qui va être décalé (remplacé par un "suivant")
			Ligne dernierElement = lignes[maxOptions-1]; // les éléments sont indéxés à partir de 0

			// On remplace le dernier élément par un "suivant"
			lignes[maxOptions-1] = suivant;

			// On place l'élément décalé en début de seconde page
			lignes.push_back(dernierElement);

			// On insérer la nouvelle ligne
			lignes.push_back(aAjouter);

			// On place un "retour" après le nouvelle élément
			lignes.push_back(retour);

			return;
		}
	}
	else if (nbrLignes == 0)
	{
		// On ajoute la ligne
		lignes.push_back(aAjouter);
		// On place un "retour" après la ligne
		lignes.push_back(retour);

		return;
	}

	// Si la taille actuelle du menu est multiple de 9, on doit ajouter une page
	if (nbrLignes % maxOptions == 0)
	{
		// Notre liste va compter 4 éléments en plus : "suivant", l'élément à insérer, "retour" et "suivant"
		// lignes.resize(nbrLigne+4);

		// On sauvegarde l'élément qui va être décalé (remplacé par le "retour")
		Ligne elementDecale = lignes[nbrLignes-2]; // les éléments sont indéxés à partir de 0

		// On place un "retour" sur à la place de l'élément que l'on vient de sauvegarder
		lignes[nbrLignes-2] = retour;

		// L'ancienne place du "retour" doit maintenant contenir un "suivant"
		lignes[nbrLignes-1] = suivant;

		// La seconde page commance avec l'élément que l'on a sauvegardé
		lignes.push_back(elementDecale);

		// Insertion de la nouvelle ligne
		lignes.push_back(aAjouter);

		// Insertion d'un "retour" après cette nouvelle ligne
		lignes.push_back(retour);

		return;
	}

	// Sinon, on continue de remplir l'actuelle dernière page (qui n'est pas pleine)

	// On remplace le "retour" par la nouvelle ligne
	lignes[nbrLignes-1] = aAjouter;

	// On replace un "retour" après cette nouvelle ligne
	lignes.push_back(retour);
}

void Menu::setLigne(int indexLigne, const string & nouvTexte, int valeurAssociee)
{
	Ligne ligne = Ligne(nouvTexte,valeurAssociee);

	indexLigne--;

	// L'index donné est invalide...
	// Aucune exception jettée car c'est un bug facilement détectable pendant la phase de test O_O
	if (indexLigne<0 || indexLigne>=(int)lignes.size())
		return;

	// Modification de la ligne
	lignes[indexLigne] = ligne;
}

const Ligne & Menu::getLigne(int indexLigne) const
{
	indexLigne--;

	// Si l'index est valide on retourne la ligne
	if (indexLigne>=0 && indexLigne<(int)lignes.size())
		return lignes[indexLigne];

	// Sinon on jette une exception
	throw out_of_range("L'index de la ligne du menu \"" + titre + "\" est invalide");
}

const Ligne & Menu::getChoix(int choix, int pageMenu) const
{
	// Les vectors étant indéxés à partie de 0, on décrément de 1 le choix dans le menu
	choix--;
	// Calcul de l'index de la lige sélectionnée par le joueur
	int indexLigne = (pageMenu-1)*9+choix;

	// Si l'index de la ligne est valide, on peut retourner la ligne
	if (indexLigne>=0 && indexLigne<(int)lignes.size())
		return lignes[indexLigne];

	// Sinon on jette une exception
	throw out_of_range("L'index du choix dans le menu \"" + titre + "\" est invalide");
}

void Menu::affiche(int indexJoueur, int pageMenu)
{
	// Si l'index du joueur est invalide on ne jettera pas d'exception
	if (!API::isValideIndex(indexJoueur))
		return;

	// Variable contenant le nombre de lignes du menu
	int nbrLignes = lignes.size();

	// Construction du menu AMX
	MRecipientFilter filter;
	filter.AddRecipient(indexJoueur);
	bf_write *pBuffer = API::engine->UserMessageBegin( &filter, MESSAGE_SHOWMENU );
	
	// Il faut décrémenter pageMenu car lignes est indéxé à partir de 0
	pageMenu--;

	// On calcul l'index de la première ligne de la page
	int depart = pageMenu*9;
	// Si on obtient une valeur abérante, on sort sans rien afficher
	if (pageMenu<0 || depart>nbrLignes) // depart==nbrLignes==0 traité plus base
	{
		// Il faut rendre la main au prochain message
		API::engine->MessageEnd();
		throw out_of_range("Page du menu \"" + titre + "\" invalide");
	}

	// On calcul l'index de la dernière ligne de la page
	int arrivee = depart+9;
	
	// Cette variable contiendra les options senssibles à la sélection du joueur dans le menu
	int flagsSensibilite = OPTION_ANNULER;

	// Variable contenant le nombre d'options senssibles à la sélection
	int sensibilite = nbrLignes-depart;
	// Si toutes les options sont senssibles on évite une boucle
	if (sensibilite>=9)
		flagsSensibilite |= OPTION_ALL;
	// Sinon on ajoute dynamiquement les options sensibles
	else
	{
		// Les options senssibles commencent à (1<<0)
		sensibilite--;
		// Tant qu'il reste une option sensible à positionner...
		while(sensibilite>=0)
		{
			flagsSensibilite |= (1<<sensibilite);
			sensibilite--;
		}
	}

	// On détermine les options sensibles du menu
	pBuffer->WriteShort(flagsSensibilite);
	// Le menu n'a pas de durée de vie limite
	pBuffer->WriteChar(-1);

	pBuffer->WriteByte(false); 

	// Buffer contenant le corps du menu (titre comprit)
	char menu[1024];

	// On place le titre du menu en début de menu
	V_snprintf(menu,sizeof(menu),"%s\n",titre.c_str());

	// Si le menu ne contient aucune ligne
	if (nbrLignes==0)
	{
		// On le dit dans le menu
		V_strcat(menu," \nCette liste est vide :-(\n \n0. Fermer ce menu",sizeof(menu));
		pBuffer->WriteString(menu);
		// Et on finalise l'affichage
		API::engine->MessageEnd();
		// On envoi un message dans le log du serveur
		API::debug("Le menu \"" + titre + "\" est vide");
		return;
	}

	// On ajoute dynamiquement les lignes du menu au corps du menu
	int indexMenu = 1;
	int i=depart;
	//API::debug(titre + "\n");
	while(i<nbrLignes && i<arrivee)
	{
		char buf[64];
		V_snprintf(buf,sizeof(buf),"->%i. %s\n",indexMenu,lignes[i].getTexte().c_str());
		//API::debug(lignes[i] + "\n");
		V_strcat(menu,buf,sizeof(menu));

		indexMenu++;
		i++;
	}
	//API::debug("0. Fermer ce menu");

	// Ajout de la dernière option (annuler)
	V_strcat(menu,"0. Fermer ce menu",sizeof(menu));

	// On écrit le corps du menu dans le message
	pBuffer->WriteString(menu);

	// Envoi du message
	API::engine->MessageEnd();

	// Si la taille du menu excède MENU_TAILLE_LIMITE on lance une exception out_of_range
	if (V_strlen(menu)>MENU_TAILLE_LIMITE)
		throw out_of_range("Taille de la page > 1024 octets !");
}

void Menu::purge()
{
	lignes.clear();
}

void Menu::playerlist(bool avecSpec)
{
	// On fera correspondre à chaque joueur son userid unique via la classe Ligne pour pouvoir le retrouver
	// de manière efficace lors du choix de l'arbitre dans le menu

	// On efface le contenu précédent du menu
	purge();

	// Si on doit prendre en compte les spectateurs et les non-assignés
	if (avecSpec)
	{
		// On ajoute tous les joueurs
		for (int i=1;i<=API::maxplayers;i++)
		{
			edict_t * pEntity = API::engine->PEntityOfEntIndex(i);
			try
			{
				IPlayerInfo * player = API::getIPlayerInfoFromEntity(pEntity);
				const char * pseudo = player->GetName();
				if (pseudo)
					addLigne(pseudo,player->GetUserID());
			}
			catch(const out_of_range & e){}
		}
		return;
	}

	// Sinon on ajoute que les joueurs non spectateurs et non-assignés
	for (int i=1;i<=API::maxplayers;i++)
	{
		edict_t * pEntity = API::engine->PEntityOfEntIndex(i);
		try
		{
			IPlayerInfo * player = API::getIPlayerInfoFromEntity(pEntity);
			if (player->GetTeamIndex()>=TEAM_T)
			{
				const char * pseudo = player->GetName();
				if (pseudo)
					addLigne(pseudo,player->GetUserID());
			}
		}
		catch(const out_of_range & e){}
	}
	// 2 boucles séparées éviter un test indépendant dans la boucle
}

void Menu::cfglist(const string & repertoire)
{
	// On forme le motif de recherche et on le place en char* pour interagir avec l'interface de VALVE
	string dossierRecherche = repertoire + "/*.cfg";

	// Récupération du pointeur sur le premier nom de fichier du dossier
	FileFindHandle_t findHandleRenomme;
	const char * cfg = API::filesystem->FindFirstEx(dossierRecherche.c_str(),"MOD",&findHandleRenomme);
	// Tant que l'on trouve un fichier
	while(cfg)
	{
		// Les espaces ne sont pas souhaitables pour une utilisation avec la ConCommand exec
		if (string(cfg).find_first_of(' ') != string::npos)
		{
			char nomFinal[256];
			// On les remplace par un _
			V_StrSubst(cfg," ","_",nomFinal,sizeof(nomFinal));
			// On les renomme sur le FTP
			API::filesystem->RenameFile((repertoire + "/" + cfg).c_str(),(repertoire + "/" + nomFinal).c_str(),"MOD");
			API::debug("Fichier de configuration " + string(cfg) + "a Ã©tÃ© renommÃ© en  " + nomFinal);
		}
		// Récupération du fichier suivant
		cfg = API::filesystem->FindNext(findHandleRenomme);
	}
	// Nous faisons une boucle séparée par filesystem->FindNext peut renvoyer une nouvelle fois le nom fichier dans sa version renommée

	FileFindHandle_t findHandleListe;
	const char * fichier = API::filesystem->FindFirstEx(dossierRecherche.c_str(),"MOD",&findHandleListe);
	// Tant que l'on trouve un fichier
	while(fichier)
	{
		// On ajoute le fichier à la liste des configurations
		addLigne(fichier);

		//API::debug("Fichier de configuration trouvÃ© : \"" + string(fichier) + "\"");

		// Récupération du fichier suivant
		fichier = API::filesystem->FindNext(findHandleListe);
	}
}

void Menu::filelist(const std::string & pathFichier)
{
	// On repasse en char* pour utilise les méthodes de VALVE
	const char * nomFichier = pathFichier.c_str();

	// si le fichier n'existe pas on ne fait rien, le joueur sera avertit à l'affichage de la liste
	if (!API::filesystem->FileExists(nomFichier,"MOD"))
		return;

	// On ouvre le fichier
	FileHandle_t fh = API::filesystem->Open(nomFichier,"r","MOD");
	// Tant qu'on a pas atteint la fin du fichier
	while(!API::filesystem->EndOfFile(fh))
	{
		// Vous avez dit "écrit en C++" ?
		char texte[1028];
		string ligne = API::filesystem->ReadLine(texte,sizeof(texte),fh);
		// Si la ligne n'est pas un commentaire ou une ligne vide
		if (!API::estNulleOuCommentee(ligne))
			// On extrait correctement le nom de la map et on l'ajoute à la liste
			addLigne(API::extraitChaineFormelle(ligne));
	}
	// Fermeture du fichier
	API::filesystem->Close(fh);
}

Menu & Menu::operator = (const Menu & menu)
{
	titre = menu.titre;
	codeMenu = menu.codeMenu;
	codeMenuParent = menu.codeMenuParent;
	lignes = menu.lignes;
	type = menu.type;
	fonction = menu.fonction;

	return *this;
}
