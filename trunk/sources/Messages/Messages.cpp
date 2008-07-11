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

#include "Messages.h"

using std::string;
using std::ostringstream;

void Messages::sayMsg(const string & message, int indexJoueur)
{
	// Chaque message du TCHAT sera préfixé par [CSSMatch] :
	string format("\004[" CSSMATCH_NOM "] :\001 " + message + "\n");
	const char * msg = format.c_str();

	MRecipientFilter filter;
	filter.addAllPlayers();
	bf_write * pBitBuf = Api::engine->UserMessageBegin( &filter, MESSAGE_SAYTEXT );

	if (pBitBuf != NULL)
	{
		// Ici l'index d'un joueur permet de colorer une partie du message précédée par un \003
		//		de la couleur de la team du joueur
		// CT <=> bleu
		// T <=> rouge
		// SPEC/UN <=> blanc
		pBitBuf->WriteByte(indexJoueur);
		pBitBuf->WriteString(msg);
		Api::engine->LogPrint(msg);
		pBitBuf->WriteByte(1);
		Api::engine->MessageEnd();
	}
	else
		Api::debug("Type d'UserMessage invalide");
}

void Messages::sayColor(const string & message)
{
	string format("\004[" CSSMATCH_NOM "] :\003 " + message + "\n");
	const char * msg = format.c_str();

	MRecipientFilter filter; 
	filter.addAllPlayers();
	bf_write * pBitBuf = Api::engine->UserMessageBegin( &filter, MESSAGE_SAYTEXT );

	if (pBitBuf != NULL)
	{
		pBitBuf->WriteByte(0x02);
		pBitBuf->WriteString(msg);
		Api::engine->LogPrint(msg);
		pBitBuf->WriteByte(0x01);
		pBitBuf->WriteByte(1);
		Api::engine->MessageEnd();
	}
	else
		Api::debug("Type d'UserMessage invalide");
}

void Messages::sayTell(int indexJoueur, const string & message)
{
	string format("\004[" CSSMATCH_NOM "] :\001 " + message + "\n");
	const char * msg = format.c_str();

	MRecipientFilter filter;
	filter.addRecipient(indexJoueur);
	bf_write * pBitBuf = Api::engine->UserMessageBegin( &filter, MESSAGE_SAYTEXT );

	if (pBitBuf != NULL)
	{
		pBitBuf->WriteByte(NULL);
		pBitBuf->WriteString(msg);
		pBitBuf->WriteByte(NULL);
		Api::engine->MessageEnd();
	}
	else
		Api::debug("Type d'UserMessage invalide");
}

void Messages::sayAMX(const string & message, int vie)
{
	for (int i=1;i<=Api::maxplayers;i++)
	{
		tellAMX(i,message,vie);
	}
}

void Messages::tellAMX(int indexJoueur, const string & message, int vie)
{
	// Si le message fait plus de 1024 caractères le message est invalide
	if (message.size() <= MENU_TAILLE_LIMITE)
	{
		const char * msg = message.c_str();
		// Nous devons éviter que le joueur choisisse par accident une option dans un menu du plugin caché par une fenêtre AMX
		GestionJoueurs::getPlayerList()->getJoueur(indexJoueur)->getMenu()->setCodeMenu(CODE_MENU_INVALIDE);
		
		MRecipientFilter filter;
		filter.addRecipient(indexJoueur);
		bf_write * pBuffer = Api::engine->UserMessageBegin( &filter, MESSAGE_SHOWMENU );
		
		if (pBuffer != NULL)
		{
			// Toutes les touches permetteront d'effacer le menu
			pBuffer->WriteShort(OPTION_ALL);
			pBuffer->WriteChar(vie);
			pBuffer->WriteByte(false); 
			pBuffer->WriteString(msg);
			Api::engine->MessageEnd();
		}
		else
			Api::debug("Type d'UserMessage invalide");
	}
	else
		Api::debug("Taille de la page > 1024 ! (tellAMX)");
}

void Messages::hintMsg(const string & message)
{
	for (int i=1;i<=Api::maxplayers;i++)
	{
		hintTell(i,message);
	}
}

void Messages::hintTell(int indexJoueur, const string & message)
{
	const char * msg = message.c_str();
	MRecipientFilter filter;
	filter.addRecipient(indexJoueur);
	bf_write * pWrite = Api::engine->UserMessageBegin(&filter, MESSAGE_HINTTEXT);
	
	if (pWrite != NULL)
	{
		pWrite->WriteByte(1);
		pWrite->WriteString(msg);
		pWrite->WriteByte(NULL);
		Api::engine->MessageEnd();
	}
	else
		Api::debug("Type d'UserMessage invalide");
}

void Messages::centerMsg(const string & message)
{
	for (int i=1;i<=Api::maxplayers;i++)
	{
		centerTell(i,message);
	}
}

void Messages::centerTell(const int indexJoueur, const string & message)
{
	const char * msg = message.c_str();
	MRecipientFilter filter; 
	filter.addRecipient(indexJoueur); 
	bf_write * pWrite = Api::engine->UserMessageBegin(&filter, MESSAGE_TEXTMSG);
	
	if (pWrite != NULL)
	{
		pWrite->WriteByte(4); 
		pWrite->WriteString(msg); 
		pWrite->WriteByte(NULL); 
		Api::engine->MessageEnd();
	}
	else
		Api::debug("Type d'UserMessage invalide");
}

void Messages::clientPrintMsg(const string & message)
{
	for (int i=1;i<=Api::maxplayers;i++)
	{
		try
		{
			edict_t * pEntity = Api::getPEntityFromIndex(i);
			clientPrintTell(pEntity,message);
		}
		catch(const CSSMatchApiException & e){}
	}
}

void Messages::clientPrintTell(edict_t * pEntity,const string & message)
{
	// On vérifie que le joueur n'est pas un bot
	//		Il parait que ClientPrintf plante avec les bots
	try
	{
		IPlayerInfo * pInfo = Api::getIPlayerInfoFromEntity(pEntity);
		if (! pInfo->IsFakeClient())
			Api::engine->ClientPrintf(pEntity,message.c_str());
	}
	catch(const CSSMatchApiException & e){}
}

void Messages::getRatesTab(edict_t * pEntity)
{
	if (Api::isValidePEntity(pEntity))
	{
		ostringstream entete;
		entete	<< "+ " << std::setfill('=') << std::setw(85) << "=" << std::setfill(' ') << " +\n"
				<< "| " << std::setw(30) << std::left << "Pseudo du joueur"
				<< " | " << std::setw(13) << std::left << "cl_updaterate"
				<< " | " << std::setw(10) << std::left << "cl_cmdrate"
				<< " | " << std::setw(14) << std::left << "cl_interpolate"
				<< " | " << std::setw(6) << std::left << "rate"	<< " |" << "\n"
				<< "+ " << std::setfill('=') << std::setw(85) << "=" << std::setfill(' ') << " +\n";
		clientPrintTell(pEntity,entete.str().c_str());

		for (int i=1;i<=Api::maxplayers;i++)
		{
			try
			{
				edict_t * joueur = Api::getPEntityFromIndex(i);
				IPlayerInfo * pInfo = Api::getIPlayerInfoFromEntity(joueur);
				
				if (pInfo->IsConnected() && (! pInfo->IsFakeClient()))
				{
					ostringstream ligne;
					ligne	<< "| " << std::setw(30) << std::left << string(Api::engine->GetClientConVarValue(i,"name")).substr(0,30)
							<< " | " << std::setw(13) << std::right << Api::engine->GetClientConVarValue(i,"cl_updaterate")
							<< " | " << std::setw(10) << std::right << Api::engine->GetClientConVarValue(i,"cl_cmdrate")
							<< " | " << std::setw(14) << std::right << Api::engine->GetClientConVarValue(i,"cl_interpolate")
							<< " | " << std::setw(6) << std::right << Api::engine->GetClientConVarValue(i,"rate") << " |" << "\n"
							<< "+ " << std::setfill('-') << std::setw(85) << "-" << std::setfill(' ') << " +\n";;
					clientPrintTell(pEntity,ligne.str().c_str());
				}
			}
			catch(const CSSMatchApiException & e){}
		}
	}
}

void Messages::timerSayMsg(const string & message)
{
	Messages::sayMsg(message);
}

void Messages::timerSayAMX(const string & message)
{
	Messages::sayAMX(message,5);
}
