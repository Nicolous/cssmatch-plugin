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

#include "Rapport.h"

using std::vector;
using std::out_of_range;
using std::string;

void Rapport::ecritEntete(FileHandle_t & fh)
{
	API::filesystem->FPrintf(fh,"CSSMatch Plugin version %s\n",CSSMATCH_VERSION_LIGHT);
	API::filesystem->FPrintf(fh,"Nicolous\n%s\n",CSSMATCH_SITE);
}

void Rapport::ecritSituationMatch(FileHandle_t & fh, Match & match)
{
	time_t date;
	time(&date);
	struct tm * locale = localtime(&date);
	char datebuf[96];
	strftime(datebuf,sizeof(datebuf),"Le %d/%m/%Y, a %Hh%M (%Z)",locale);
	API::filesystem->FPrintf(fh,"%s\n\n",datebuf);

	API::filesystem->FPrintf(fh,"Nom du match : %s versus %s\n",match.getTeam1()->getTag().c_str(),match.getTeam2()->getTag().c_str());
	API::filesystem->FPrintf(fh,"Map du match : %s\n\n",API::gpGlobals->mapname.ToCStr());

	const string * tagGagnantCutRound = &match.getTagGagnantCutRound();
	if (*tagGagnantCutRound != "")	
		API::filesystem->FPrintf(fh,"Gagnant du CutRound : %s\n\n",tagGagnantCutRound->c_str());

}

void Rapport::ecritScores(FileHandle_t & fh, Match & match)
{
	Team * team1 = match.getTeam1();
	Team * team2 = match.getTeam2();

	const char * tagT1 = team1->getTag().c_str();
	const char * tagT2 = team2->getTag().c_str();

	API::filesystem->FPrintf(fh,"Scores :\n","");
	API::filesystem->FPrintf(fh,"\t%s : %i\n",tagT1,team1->getScore());
	API::filesystem->FPrintf(fh,"\t%s : %i\n\n",tagT2,team2->getScore());

	API::filesystem->FPrintf(fh,"Score des %s par camp :\n",tagT1);
	API::filesystem->FPrintf(fh,"\tEn terroriste : %i\n",team1->getScoreT());
	API::filesystem->FPrintf(fh,"\tEn anti-terroriste : %i\n\n",team1->getScoreCT());

	API::filesystem->FPrintf(fh,"Score des %s par camp :\n",tagT2);
	API::filesystem->FPrintf(fh,"\tEn terroriste : %i\n",team2->getScoreT());
	API::filesystem->FPrintf(fh,"\tEn anti-terroriste : %i\n\n",team2->getScoreCT());
}

void Rapport::ecritLignupT(FileHandle_t & fh, Match & match, const vector<int> & lignupT, const string * tagT)
{
	// Cas où il n'y a pas de team adverse
	if (lignupT.size() == 0)
	{
		API::filesystem->FPrintf(fh,"Pas d'adversaire","");
		return;
	}

	API::filesystem->FPrintf(fh,"lign-up des %s\n",tagT->c_str());

	vector<int>::const_iterator indexJoueur = lignupT.begin();
	while(indexJoueur != lignupT.end())
	{
		try
		{
			IPlayerInfo * player = API::getIPlayerInfoFromEntity(API::engine->PEntityOfEntIndex(*indexJoueur));
		
			Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(*indexJoueur);
			const char * pseudo = player->GetName();
			const char * steamid = player->GetNetworkIDString();
			API::filesystem->FPrintf(fh,"\t%s (%s) :\n\t\tkills = %i\n\t\tdeaths = %i\n\t\tratio = %.2f\n",pseudo,steamid,joueur->getKills(),joueur->getDeaths(),joueur->getRatio());
		}
		catch(const out_of_range & e){}

		indexJoueur++;
	}
}

void Rapport::ecritLignupCT(FileHandle_t & fh, Match & match, const vector<int> & lignupCT, const string * tagCT)
{
	// Cas où il n'y a pas de team adverse
	if (lignupCT.size() == 0)
	{
		API::filesystem->FPrintf(fh,"Pas d'adversaire","");
		return;
	}

	API::filesystem->FPrintf(fh,"lign-up des %s\n",tagCT->c_str());

	vector<int>::const_iterator indexJoueur = lignupCT.begin();
	while(indexJoueur != lignupCT.end())
	{
		try
		{
			IPlayerInfo * player = API::getIPlayerInfoFromEntity(API::engine->PEntityOfEntIndex(*indexJoueur));
		
			Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(*indexJoueur);
			const char * pseudo = player->GetName();
			const char * steamid = player->GetNetworkIDString();
			API::filesystem->FPrintf(fh,"\t%s (%s) :\n\t\tkills = %i\n\t\tdeaths = %i\n\t\tratio = %.2f\n",pseudo,steamid,joueur->getKills(),joueur->getDeaths(),joueur->getRatio());
		}
		catch(const out_of_range & e){}

		indexJoueur++;
	}
}

void Rapport::ecritLignupSpec(FileHandle_t & fh, Match & match, const vector<int> & lignupSpec)
{
	// Cas où il n'y a pas de team adverse
	if (lignupSpec.size() == 0)
	{
		API::filesystem->FPrintf(fh,"\n\nPas de spectateur","");
		return;
	}

	API::filesystem->FPrintf(fh,"\n\nEn spectateur :\n","");

	vector<int>::const_iterator indexJoueur = lignupSpec.begin();
	while(indexJoueur != lignupSpec.end())
	{
		try
		{
			IPlayerInfo * player = API::getIPlayerInfoFromEntity(API::engine->PEntityOfEntIndex(*indexJoueur));
		
			Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(*indexJoueur);
			const char * pseudo = player->GetName();
			const char * steamid = player->GetNetworkIDString();
			API::filesystem->FPrintf(fh,"\t%s (%s) :\n\t\tkills = %i\n\t\tdeaths = %i\n\t\tratio = %.2f\n",pseudo,steamid,joueur->getKills(),joueur->getDeaths(),joueur->getRatio());
		}
		catch(const out_of_range & e){}

		indexJoueur++;
	}
}

void Rapport::ecritLignups(FileHandle_t & fh, Match & match)
{
	Team * team1 = match.getTeam1();
	Team * team2 = match.getTeam2();

	const string * tagT;
	const string * tagCT;
	if (match.getMancheCourante() == 1)
	{
		tagT = &team1->getTag();
		tagCT = &team2->getTag();
	}
	else
	{
		tagT = &team2->getTag();
		tagCT = &team1->getTag();
	}

	vector<int> lignupT = vector<int>();
	vector<int> lignupCT = vector<int>();
	vector<int> lignupSpec = vector<int>();

	for (int i=1;i<=API::maxplayers;i++)
	{
		edict_t * pEntity = API::engine->PEntityOfEntIndex(i);
		try
		{
			IPlayerInfo * player = API::getIPlayerInfoFromEntity(pEntity);
			switch(player->GetTeamIndex())
			{
			case TEAM_T:
				lignupT.push_back(i);
				break;
			case TEAM_CT:
				lignupCT.push_back(i);
				break;
			default:
				lignupSpec.push_back(i);
			}
		}
		catch(const out_of_range & e){}
	}

	Rapport::ecritLignupT(fh,match,lignupT,tagT);
	Rapport::ecritLignupCT(fh,match,lignupCT,tagCT);
	Rapport::ecritLignupSpec(fh,match,lignupSpec);
}

void Rapport::ecritEnregistrements(FileHandle_t & fh, Match & match)
{
	if (ConVars::cssmatch_sourcetv.GetBool())
	{
		const EnregistrementTV * tvManche1 = match.getTvManche1();
		const EnregistrementTV * tvManche2 = match.getTvManche2();

		API::filesystem->FPrintf(fh,"\n\nEnregistrements SourceTV :\n","");
		if (tvManche1!=NULL)
			API::filesystem->FPrintf(fh,"- %s\n",tvManche1->getNom().c_str());
		else
			API::filesystem->FPrintf(fh,"- pas d'enregistrement de la manche 1\n","");
		if (tvManche2!=NULL)
			API::filesystem->FPrintf(fh,"- %s\n",tvManche2->getNom().c_str());
		else
			API::filesystem->FPrintf(fh,"- pas d'enregistrement de la manche 2\n","");
	}
}

void Rapport::genereRapport(Match & match)
{
	Team * team1 = match.getTeam1();
	Team * team2 = match.getTeam2();

	const string * tagT1 = &match.getTeam1()->getTag();
	const string * tagT2 = &match.getTeam2()->getTag();

	// Construction du nom du fichier
	string nomFichier = *tagT1 + "_vs_" + *tagT2 + "_" + API::gpGlobals->mapname.ToCStr();
	API::valideNomDeFichier(nomFichier);

	// Construction du nom final du fichier avec son arborescence
	string path = "cfg/cssmatch/rapports/" + nomFichier + ".txt";

	// Ouverture/création du fichier
	FileHandle_t fh = API::filesystem->Open(path.c_str(),"w","MOD");

	Rapport::ecritEntete(fh);
	Rapport::ecritSituationMatch(fh,match);
	Rapport::ecritScores(fh,match);
	Rapport::ecritLignups(fh,match);
	Rapport::ecritEnregistrements(fh,match);

	// Fermeture du fichier
	API::filesystem->Close(fh);
}
