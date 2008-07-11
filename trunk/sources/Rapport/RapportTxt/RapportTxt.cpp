/* 
 * Copyright 2007, 2008 Nicolas Maingot
 * 
 * This file is part of CSSmatch->
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

#include "RapportTxt.h"

using std::vector;
using std::string;

RapportTxt::RapportTxt(Match * match) : Rapport(match)
{
}

void RapportTxt::ecritEntete(FileHandle_t & fh) const
{
	Api::filesystem->FPrintf(fh,"CSSMatch Plugin version %s\n",CSSMATCH_VERSION_LIGHT);
	Api::filesystem->FPrintf(fh,"Nicolous\n%s\n",CSSMATCH_SITE);
}

void RapportTxt::ecritCorps(FileHandle_t & fh) const
{
	Team * team1 = match->getTeam1();
	Team * team2 = match->getTeam2();

	ecritSituationMatch(fh,team1, team2);
	ecritScores(fh,team1, team2);
	ecritLignups(fh,team1, team2);
	ecritEnregistrements(fh);
}

void RapportTxt::ecritPied(FileHandle_t & fh) const
{
}

void RapportTxt::ecritSituationMatch(FileHandle_t & fh, const Team * team1, const Team * team2) const
{
	char datebuf[96];
	strftime(datebuf,sizeof(datebuf),"Le %d/%m/%Y, a %Hh%M (%Z)",dateLocale);
	Api::filesystem->FPrintf(fh,"%s\n\n",datebuf);

	Api::filesystem->FPrintf(fh,"Nom du match : %s versus %s\n",team1->getTag().c_str(),team2->getTag().c_str());
	Api::filesystem->FPrintf(fh,"Map du match : %s\n\n",Api::gpGlobals->mapname.ToCStr());

	const string * tagGagnantCutRound = &match->getTagGagnantCutRound();
	if (*tagGagnantCutRound != "")	
		Api::filesystem->FPrintf(fh,"Gagnant du CutRound : %s\n\n",tagGagnantCutRound->c_str());

}

void RapportTxt::ecritScores(FileHandle_t & fh, const Team * team1, const Team * team2) const
{
	const string * tagT1 = &team1->getTag();
	const string * tagT2 = &team2->getTag();

	Api::filesystem->FPrintf(fh,"Scores :\n","");
	Api::filesystem->FPrintf(fh,"\t%s : %i\n",tagT1->c_str(),team1->getScore());
	Api::filesystem->FPrintf(fh,"\t%s : %i\n\n",tagT2->c_str(),team2->getScore());

	Api::filesystem->FPrintf(fh,"Score des %s par camp :\n",tagT1->c_str());
	Api::filesystem->FPrintf(fh,"\tEn terroriste : %i\n",team1->getScoreT());
	Api::filesystem->FPrintf(fh,"\tEn anti-terroriste : %i\n\n",team1->getScoreCT());

	Api::filesystem->FPrintf(fh,"Score des %s par camp :\n",tagT2->c_str());
	Api::filesystem->FPrintf(fh,"\tEn terroriste : %i\n",team2->getScoreT());
	Api::filesystem->FPrintf(fh,"\tEn anti-terroriste : %i\n\n",team2->getScoreCT());
}

void RapportTxt::ecritLignup(FileHandle_t & fh, const string * tag, const vector<int> & lignup) const
{
	Api::filesystem->FPrintf(fh,"lign-up des %s\n",tag->c_str());

	// Cas où il n'y a pas de joueurs dans la team
	if (lignup.size() > 0)
	{
		vector<int>::const_iterator indexJoueur = lignup.begin();
		while(indexJoueur != lignup.end())
		{
			try
			{
				IPlayerInfo * player = Api::getIPlayerInfoFromEntity(Api::engine->PEntityOfEntIndex(*indexJoueur));
			
				Joueur * joueur = GestionJoueurs::getPlayerList()->getJoueur(*indexJoueur);
				Api::filesystem->FPrintf(fh,
										"\t%s (%s) :\n\t\tkills = %i\n\t\tdeaths = %i\n\t\tratio = %.2f\n",
										player->GetName(),
										player->GetNetworkIDString(),
										joueur->getKills(),
										joueur->getDeaths(),
										joueur->getRatio());
			}
			catch(const CSSMatchApiException & e){}

			indexJoueur++;
		}
	}
	else
		Api::filesystem->FPrintf(fh,"\tPas de joueur\n");
}

void RapportTxt::ecritLignups(FileHandle_t & fh, const Team * team1, const Team * team2) const
{
	const string * tagT;
	const string * tagCT;
	//if (match->getMancheCourante() == 1)
	if (match->getMancheCourante()%2 == 1)
	{
		tagT = &team1->getTag();
		tagCT = &team2->getTag();
	}
	else
	{
		tagT = &team2->getTag();
		tagCT = &team1->getTag();
	}
	RapportTxt::ecritLignup(fh,tagT,lignupT);
	RapportTxt::ecritLignup(fh,tagCT,lignupCT);
	RapportTxt::ecritLignup(fh,&string("spectateurs"),lignupSpec);
}

void RapportTxt::ecritEnregistrements(FileHandle_t & fh) const
{
	/*if (ConVars::cssmatch_sourcetv.GetBool())
	{
		const EnregistrementTV * tvManche1 = match->getTvManche1();
		const EnregistrementTV * tvManche2 = match->getTvManche2();

		Api::filesystem->FPrintf(fh,"\n\nEnregistrements SourceTV :\n");
		if (tvManche1!=NULL)
			Api::filesystem->FPrintf(fh,"- %s\n",tvManche1->getNom().c_str());
		else
			Api::filesystem->FPrintf(fh,"- pas d'enregistrement de la manche 1\n");
		if (tvManche2!=NULL)
			Api::filesystem->FPrintf(fh,"- %s\n",tvManche2->getNom().c_str());
		else
			Api::filesystem->FPrintf(fh,"- pas d'enregistrement de la manche 2\n");
	}*/
	// Récupération de la liste des enregistrements
	const vector<EnregistrementTV> * enregistrements = match->getEnregistrementsTV();

	// Ecriture du nom des enregistrements
	if (enregistrements->empty())
		Api::filesystem->FPrintf(fh,"Pas d'enregistrement du match\n");
	else
	{
		vector<EnregistrementTV>::const_iterator enregistrement = enregistrements->begin();

		while(enregistrement != enregistrements->end())
		{
			Api::filesystem->FPrintf(fh,"- %s\n",enregistrement->getNom().c_str());
			enregistrement++;
		}
	}
}

void RapportTxt::ecrit()
{
	// Ouverture/création du fichier
	FileHandle_t fh = Api::filesystem->Open((nom + ".txt").c_str(),"w","MOD");

	ecritEntete(fh);
	ecritCorps(fh);
	ecritPied(fh);

	// Fermeture du fichier
	Api::filesystem->Close(fh);
}

