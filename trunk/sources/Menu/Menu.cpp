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

#include "Menu.h"
#include "../Messages/Messages.h"

using std::string;
using std::ostringstream;
using std::ifstream;

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
	if (fonction != NULL)
		fonction(indexJoueur,choix);
}

int Menu::getNbrLignes() const
{
	return lignes.size();
}

void Menu::addLigne(const string & ligne, int valeurAssociee)
{
	// Association d'une valeur � la ligne
	Ligne aAjouter(ligne,valeurAssociee);

	// R�cup�ration du nombre de ligne contenues dans le menu
	int nbrLignes = lignes.size();

	// Si le menu n'a pas de menu parent le traitement est plus compliqu� : pas de "retour" d�s la premi�re page
	if (codeMenuParent == CODE_MENU_INVALIDE)
	{
		// Si le menu ne fait toujours qu'une page ET si aucun menu parent n'existe
		if (nbrLignes < maxOptions)
		{
			// Aucun probl�me, on ajoute simplement la ligne
			lignes.push_back(aAjouter);
			return;
		}
		// Le sinon est tout � la fin de la fonction ;)

		// Si l'�l�ment � ins�rer ne tient plus sur le premi�re page
		if (nbrLignes == maxOptions) 
		{
			// Notre liste va compter 3 �l�ments en plus : "suivant", l'�l�ment � ins�rer, et "retour"
			// maxOptions + 3 �l�ment en plus

			// On sauvegarde l'�l�ment qui va �tre d�cal� (remplac� par un "suivant")
			Ligne dernierElement = lignes[maxOptions-1]; // les �l�ments sont ind�x�s � partir de 0

			// On remplace le dernier �l�ment par un "suivant"
			lignes[maxOptions-1] = suivant;

			// On place l'�l�ment d�cal� en d�but de seconde page
			lignes.push_back(dernierElement);

			// On ins�rer la nouvelle ligne
			lignes.push_back(aAjouter);

			// On place un "retour" apr�s le nouvel �l�ment
			lignes.push_back(retour);

			return;
		}
	}
	else if (nbrLignes == 0)
	{
		// On ajoute la ligne
		lignes.push_back(aAjouter);
		// On place un "retour" apr�s la ligne
		lignes.push_back(retour);

		return;
	}

	// Si la taille actuelle du menu est multiple de 9, on doit ajouter une page
	if (nbrLignes % maxOptions == 0)
	{
		// Notre liste va compter 4 �l�ments en plus : "suivant", l'�l�ment � ins�rer, "retour" et "suivant"
		// lignes.resize(nbrLigne+4);

		// On sauvegarde l'�l�ment qui va �tre d�cal� (remplac� par le "retour")
		Ligne elementDecale = lignes[nbrLignes-2]; // les �l�ments sont ind�x�s � partir de 0

		// On place un "retour" sur � la place de l'�l�ment que l'on vient de sauvegarder
		lignes[nbrLignes-2] = retour;

		// L'ancienne place du "retour" doit maintenant contenir un "suivant"
		lignes[nbrLignes-1] = suivant;

		// La seconde page commance avec l'�l�ment que l'on a sauvegard�
		lignes.push_back(elementDecale);

		// Insertion de la nouvelle ligne
		lignes.push_back(aAjouter);

		// Insertion d'un "retour" apr�s cette nouvelle ligne
		lignes.push_back(retour);

		return;
	}

	// Sinon, on continue de remplir l'actuelle derni�re page (qui n'est pas pleine)

	// On remplace le "retour" par la nouvelle ligne
	lignes[nbrLignes-1] = aAjouter;

	// On replace un "retour" apr�s cette nouvelle ligne
	lignes.push_back(retour);
}

void Menu::setLigne(int indexLigne, const string & nouvTexte, int valeurAssociee)
{
	Ligne ligne = Ligne(nouvTexte,valeurAssociee);

	indexLigne--;

	if (indexLigne>=0 && indexLigne<(int)lignes.size())
		lignes[indexLigne] = ligne; // Modification de la ligne
	// Aucune exception jett�e car c'est un bug facilement d�tectable pendant la phase de test O_O
}

const Ligne & Menu::getLigne(int indexLigne) const
{
	indexLigne--;

	// Si l'index est valide on retourne la ligne
	if (indexLigne>=0 && indexLigne<(int)lignes.size())
		return lignes[indexLigne];

	// Sinon on jette une exception
	throw CSSMatchMenuException("L'index de la ligne du menu \"" + titre + "\" est invalide");
}

const Ligne & Menu::getChoix(int choix, int pageMenu) const
{
	// Les vectors �tant ind�x�s � partie de 0, on d�cr�ment de 1 le choix dans le menu
	choix--;
	// Calcul de l'index de la lige s�lectionn�e par le joueur
	int indexLigne = (pageMenu-1)*9+choix;

	// Si l'index de la ligne est valide, on peut retourner la ligne
	if (indexLigne>=0 && indexLigne<(int)lignes.size())
		return lignes[indexLigne];

	// Sinon on jette une exception
	throw CSSMatchMenuException("L'index du choix dans le menu \"" + titre + "\" est invalide");
}

void Menu::affiche(int indexJoueur, int pageMenu)
{
	// Si l'index du joueur est invalide on ne jettera pas d'exception
	if (Api::isValideIndex(indexJoueur))
	{
		// Variable contenant le nombre de lignes du menu
		int nbrLignes = lignes.size();

		// Construction du menu AMX
		MRecipientFilter filter;
		filter.addRecipient(indexJoueur);
		bf_write * pBuffer = Api::engine->UserMessageBegin(&filter,MESSAGE_SHOWMENU);
		
		// Il faut d�cr�menter pageMenu car lignes est ind�x� � partir de 0
		pageMenu--;

		// On calcul l'index de la premi�re ligne de la page
		int depart = pageMenu*9;
		// Si on obtient une valeur ab�rante, on sort sans rien afficher
		if (pageMenu<0 || depart>nbrLignes) // depart==nbrLignes==0 trait� plus bas
		{
			Api::engine->MessageEnd(); // Il faut rendre la main au prochain message
			throw CSSMatchMenuException("Page du menu \"" + titre + "\" invalide");
		}

		// On calcul l'index de la derni�re ligne de la page
		int arrivee = depart+9;
		
		// Cette variable contiendra les options senssibles � la s�lection du joueur dans le menu
		int flagsSensibilite = OPTION_ANNULER;

		// Variable contenant le nombre d'options senssibles � la s�lection
		int sensibilite = nbrLignes-depart;
		// Si toutes les options sont senssibles on �vite une boucle
		if (sensibilite>=9)
			flagsSensibilite |= OPTION_ALL;
		// Sinon on ajoute dynamiquement les options sensibles
		else
		{
			// Les options senssibles commencent � (1<<0)
			sensibilite--;
			// Tant qu'il reste une option sensible � positionner...
			while(sensibilite>=0)
			{
				flagsSensibilite |= (1<<sensibilite);
				sensibilite--;
			}
		}

		// On d�termine les options sensibles du menu
		pBuffer->WriteShort(flagsSensibilite);
		// Le menu n'a pas de dur�e de vie limite
		pBuffer->WriteChar(-1);

		pBuffer->WriteByte(false); 

		// Corps du menu (titre comprit)
		ostringstream menu;

		// On place le titre du menu en d�but de menu
		menu << titre << "\n";

		// Si le menu ne contient aucune ligne
		if (nbrLignes==0)
		{
			// On le dit dans le menu
			menu << " \n";
			menu << "Cette liste est vide :-(\n \n";
			menu << "0. Fermer ce menu";
			//pBuffer->WriteString(menu);
			pBuffer->WriteString(menu.str().c_str());
			// Et on finalise l'affichage
			Api::engine->MessageEnd();
			// On envoi un message dans le log du serveur
			Api::debug("Le menu \"" + titre + "\" est vide");
		}
		else
		{
			// On ajoute dynamiquement les lignes du menu au corps du menu
			int indexMenu = 1;
			int i=depart;
			//Api::debug(titre + "\n");
			while(i<nbrLignes && i<arrivee)
			{
				//Api::debug(lignes[i] + "\n");
				menu << "->" << indexMenu << ". " << lignes[i].getTexte() << "\n";

				indexMenu++;
				i++;
			}
			//Api::debug("0. Fermer ce menu");

			// Ajout de la derni�re option (annuler)
			menu << "0. Fermer ce menu";

			// On �crit le corps du menu dans le message
			pBuffer->WriteString(menu.str().c_str());

			// Envoi du message
			Api::engine->MessageEnd();

			// Si la taille du menu exc�de MENU_TAILLE_LIMITE on lance une exception CSSMatchMenuException
			int tailleMenu = menu.str().size();
			if (tailleMenu > MENU_TAILLE_LIMITE)
			{
				ostringstream message;
				message << "Taille de la page (" << tailleMenu << ") > 255 octets !";
				throw CSSMatchMenuException(message.str());
			}
		}
	}
}

void Menu::purge()
{
	lignes.clear();
}

void Menu::playerlist(bool avecSpec)
{
	// On fera correspondre � chaque joueur son userid unique via la classe Ligne pour pouvoir le retrouver
	// de mani�re efficace lors du choix de l'arbitre dans le menu

	// On efface le contenu pr�c�dent du menu
	purge();

	// Si on doit prendre en compte les spectateurs et les non-assign�s
	if (avecSpec)
	{
		// On ajoute tous les joueurs
		for (int i=1;i<=Api::maxplayers;i++)
		{
			edict_t * pEntity = Api::engine->PEntityOfEntIndex(i);
			try
			{
				IPlayerInfo * player = Api::getIPlayerInfoFromEntity(pEntity);
				const char * pseudo = player->GetName();
				if (pseudo != NULL)
				{
					string sPseudo(pseudo) ;
					if (sPseudo.size() > MENU_TAILLE_PSEUDO_LIMITE+1)
						sPseudo = sPseudo.substr(0,MENU_TAILLE_PSEUDO_LIMITE) + ".";
					addLigne(sPseudo,player->GetUserID());
				}
			}
			catch(const CSSMatchApiException & e){}
		}
	}
	else
	{
		// Sinon on ajoute que les joueurs non spectateurs et non-assign�s
		for (int i=1;i<=Api::maxplayers;i++)
		{
			edict_t * pEntity = Api::engine->PEntityOfEntIndex(i);
			try
			{
				IPlayerInfo * player = Api::getIPlayerInfoFromEntity(pEntity);
				if (player->GetTeamIndex() >= TEAM_T)
				{
					const char * pseudo = player->GetName();
					if (pseudo != NULL)
					{
						string sPseudo(pseudo) ;
						if (sPseudo.size() > MENU_TAILLE_PSEUDO_LIMITE+1)
							sPseudo = sPseudo.substr(0,MENU_TAILLE_PSEUDO_LIMITE) + ".";
						addLigne(sPseudo,player->GetUserID());
					}
				}
			}
			catch(const CSSMatchApiException & e){}
		}
		// 2 boucles s�par�es �viter un test ind�pendant dans la boucle
	}
}

void Menu::cfglist(const string & repertoire)
{
	// On forme le motif de recherche et on le place en char* pour interagir avec l'interface de VALVE
	string dossierRecherche(repertoire + "/*.cfg");

	// R�cup�ration du pointeur sur le premier nom de fichier du dossier
	FileFindHandle_t findHandleRenomme;
	const char * cfg = Api::filesystem->FindFirstEx(dossierRecherche.c_str(),"MOD",&findHandleRenomme);
	// Tant que l'on trouve un fichier
	while(cfg != NULL)
	{
		string nomFinal(cfg);

		// Les espaces ne sont pas souhaitables pour une utilisation avec la ConCommand exec
		size_t posEspace;
		bool renomme = false;
		while((posEspace = nomFinal.find(' ')) != string::npos)
		{
			// On les remplace par un _
			nomFinal.replace(posEspace,1,"_",0,1);

			renomme = true;
		}
		if (renomme)
		{
			// On les renomme sur le FTP
			Api::filesystem->RenameFile((repertoire + "/" + cfg).c_str(),(repertoire + "/" + nomFinal).c_str(),"MOD");
			Api::debug("Le fichier de configuration \"" + string(cfg) + "\" a ete renomme en \"" + nomFinal + "\"");
		}

		addLigne(nomFinal);

		// R�cup�ration du fichier suivant
		cfg = Api::filesystem->FindNext(findHandleRenomme);
	}
}

void Menu::filelist(const std::string & pathFichier)
{
	ifstream fichier(pathFichier.c_str());

	// Si le fichier n'existe pas on ne fait rien, le joueur sera avertit � l'affichage de la liste
	if (! fichier.fail())
	{
		string ligne;
		while(std::getline(fichier,ligne))
		{
			// Si la ligne n'est pas un commentaire ou une ligne vide
			if (! Api::estNulleOuCommentee(ligne))
				// On extrait correctement le nom de la map et on l'ajoute � la liste
				addLigne(Api::extraitChaineFormelle(ligne));
		}
	}
	else
		Api::debug("Le fichier \"" + pathFichier + "\" est inaccessible en lecture");
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
