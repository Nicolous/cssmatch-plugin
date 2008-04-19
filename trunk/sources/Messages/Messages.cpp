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
	string format = "\004[CSSMatch] :\001 " + message + "\n";
	const char * msg = format.c_str();

	// Préparation du message
	MRecipientFilter filter;
	filter.AddAllPlayers();
	bf_write * pBitBuf = API::engine->UserMessageBegin( &filter, MESSAGE_SAYTEXT );
	// Ici l'index d'un joueur permet de colorer une partie du message précédée par un \003 de la couleur de la team du joueur
	// CT <=> bleu
	// T <=> rouge
	// SPEC/UN <=> blanc
	pBitBuf->WriteByte(indexJoueur);
	pBitBuf->WriteString(msg);
	// API::engine->ServerCommand(("say " + message + "\n").str());
	API::engine->LogPrint(msg);
	pBitBuf->WriteByte(1);
	API::engine->MessageEnd();
}

void Messages::sayColor(const string & message)
{
	string format = "\004[CSSMatch] :\003 " + message + "\n";
	const char * msg = format.c_str();

	MRecipientFilter filter; 
	filter.AddAllPlayers();
	bf_write * pBitBuf = API::engine->UserMessageBegin( &filter, MESSAGE_SAYTEXT );
	pBitBuf->WriteByte(0x02); // <=> "\002"
	pBitBuf->WriteString(msg);
	// API::engine->ServerCommand(("say " + message + "\n").str());
	API::engine->LogPrint(msg);
	pBitBuf->WriteByte(0x01); // <=> "\001"
	pBitBuf->WriteByte(1);
	API::engine->MessageEnd();
}

void Messages::sayTell(int indexJoueur, const string & message)
{
	string format = "\004[CSSMatch] :\001 " + message + "\n";
	const char * msg = format.c_str();

	MRecipientFilter filter;
	filter.AddRecipient(indexJoueur);
	bf_write * pBitBuf = API::engine->UserMessageBegin( &filter, MESSAGE_SAYTEXT );
    pBitBuf->WriteByte(NULL);
    pBitBuf->WriteString(msg);
	// API::engine->ServerCommand(("say " + message + "\n").str());
	pBitBuf->WriteByte(NULL);
    API::engine->MessageEnd();
}

void Messages::sayAMX(const string & message, int vie)
{
	const char * msg = message.c_str();
	// Envoi d'une fenêtre AMX à tous les joueurs
	for (int i=1;i<=API::maxplayers;i++)
	{
		edict_t *pPlayer = API::engine->PEntityOfEntIndex(i);
		if (pPlayer && !pPlayer->IsFree())
			if (API::engine->GetPlayerUserId(pPlayer)!=-1)
				tellAMX(i,msg,vie);
	}
}

void Messages::tellAMX(int indexJoueur, const string & message, int vie)
{
	const char * msg = message.c_str();
	// Nous devons éviter que le joueur choisisse par accident une option dans un menu du plugin caché par une fenêtre AMX
	GestionJoueurs::getPlayerList()->getJoueur(indexJoueur)->getMenu()->setCodeMenu(CODE_MENU_INVALIDE);
	//API::cexec(API::engine->PEntityOfEntIndex(indexJoueur),"slot10\n");
	MRecipientFilter filter;
	filter.AddRecipient(indexJoueur);
	bf_write *pBuffer = API::engine->UserMessageBegin( &filter, MESSAGE_SHOWMENU );
	// Toutes les touches permetteront d'effacer le menu
	pBuffer->WriteShort(OPTION_ALL);
	pBuffer->WriteChar(vie);
	pBuffer->WriteByte(false); 
	pBuffer->WriteString(msg);
	API::engine->MessageEnd();
}

void Messages::hintMsg(const string & message)
{
	const char * msg = message.c_str();
	// API::engine->ServerCommand(("say " + message + "\n").str());
	MRecipientFilter filter; 
	filter.AddAllPlayers(); 
	bf_write *pWrite; 
	pWrite = API::engine->UserMessageBegin(&filter, MESSAGE_HINTTEXT); 
	pWrite->WriteByte(1); 
	pWrite->WriteString(msg);
	pWrite->WriteByte(NULL); 
	API::engine->MessageEnd();
}

void Messages::hintTell(int indexJoueur, const string & message)
{
	const char * msg = message.c_str();
	MRecipientFilter filter;
	filter.AddRecipient(indexJoueur);
	bf_write *pWrite;
	pWrite = API::engine->UserMessageBegin(&filter, MESSAGE_HINTTEXT);
	pWrite->WriteByte(1);
	pWrite->WriteString(msg);
	pWrite->WriteByte(NULL);
	API::engine->MessageEnd();
}

void Messages::centerMsg(const string & message)
{
	const char * msg = message.c_str();
	MRecipientFilter filter; 
	filter.AddAllPlayers(); 
	bf_write *pWrite; 
	pWrite = API::engine->UserMessageBegin(&filter, MESSAGE_TEXTMSG); 
	pWrite->WriteByte(4); 
	pWrite->WriteString(msg); 
	pWrite->WriteByte(NULL); 
	API::engine->MessageEnd();
}

void Messages::centerTell(const int indexJoueur, const string & message)
{
	const char * msg = message.c_str();
	MRecipientFilter filter; 
	filter.AddRecipient(indexJoueur); 
	bf_write *pWrite; 
	pWrite = API::engine->UserMessageBegin(&filter, MESSAGE_TEXTMSG); 
	pWrite->WriteByte(4); 
	pWrite->WriteString(msg); 
	pWrite->WriteByte(NULL); 
	API::engine->MessageEnd();
}

