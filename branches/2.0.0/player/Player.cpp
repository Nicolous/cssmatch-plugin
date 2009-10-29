/* 
 * Copyright 2008, 2009 Nicolas Maingot
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

#include "Player.h"
#include "../plugin/ServerPlugin.h"

using namespace cssmatch;

Player::Player(int index) throw (PlayerException)
	: cashHandler("CCSPlayer","m_iAccount"), lifeStateHandler("CBasePlayer","m_lifeState")
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	identity.index = index;

	identity.pEntity = interfaces->engine->PEntityOfEntIndex(index);
	if (! isValidEntity(identity.pEntity))
		throw PlayerException("The plugin was unable to construct a Player instance => player entity not found");

	identity.userid = interfaces->engine->GetPlayerUserId(identity.pEntity);
	if (! isValidPlayerUserid(identity.userid))
		throw PlayerException("The plugin was unable to construct a Player instance => player userid not found");

	const char * tempSteamid = interfaces->engine->GetPlayerNetworkIDString(identity.pEntity);
	if (tempSteamid == NULL)
		throw PlayerException("The plugin was unable to construct a Player instance => player steamid not found");
	else
		identity.steamid = tempSteamid;
}

PlayerIdentity * Player::getIdentity()
{
	return &identity;
}

TeamCode Player::getMyTeam() const
{
	TeamCode team = INVALID_TEAM;
	IPlayerInfo * pInfo = getPlayerInfo();

	if (isValidPlayer(pInfo))
		team = (TeamCode)pInfo->GetTeamIndex();

	if (team == INVALID_TEAM)
		print(__FILE__,__LINE__,"The plugin was unable to find the team of a Player");

	return team;
}

CBasePlayer * Player::getBasePlayer() const
{
	CBasePlayer * bPlayer = NULL;
	IServerUnknown * sUnknown = getServerUnknow(identity.pEntity);

	if (isValidServerUnknown(sUnknown))
		bPlayer = reinterpret_cast<CBasePlayer *>(sUnknown->GetBaseEntity());

	if (! isValidBasePlayer(bPlayer))
	{
		print(__FILE__,__LINE__,"The plugin was unable to find the base player pointer of a Player");
		bPlayer = NULL;
	}

	return bPlayer;
}

CBaseCombatCharacter * Player::getBaseCombatCharacter() const
{
	CBaseCombatCharacter * bCombatCharacter = NULL;
	CBaseEntity * bEntity = getBaseEntity(identity.pEntity);

	if (isValidBaseEntity(bEntity))
		bCombatCharacter = bEntity->MyCombatCharacterPointer();

	if (! isValidBaseCombatCharacter(bCombatCharacter))
	{
		print(__FILE__,__LINE__,"The plugin was unable to find the base combat character of a Player");
		bCombatCharacter = NULL;
	}

	return bCombatCharacter;
}

IPlayerInfo * Player::getPlayerInfo() const
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	IPlayerInfo * pInfo = interfaces->playerinfomanager->GetPlayerInfo(identity.pEntity);

	if (! isValidPlayer(pInfo))
	{
		print(__FILE__,__LINE__,"The plugin was unable to find the player's infos of a Player");
		pInfo = NULL;
	}

	return pInfo;
}

CBaseCombatWeapon * Player::getWeaponFromWeaponSlot(WeaponSlotCode slot) const
{
	CBaseCombatWeapon * bCombatWeapon = NULL;
	CBaseCombatCharacter * bCombatCharacter = getBaseCombatCharacter();

	if (isValidBaseCombatCharacter(bCombatCharacter))
		bCombatWeapon = bCombatCharacter->Weapon_GetSlot((int)slot);

	/*if (bCombatWeapon == NULL)
		print("");*/
	// NULL is not abnormal here

	return bCombatWeapon;
}

void Player::removeWeapon(WeaponSlotCode slot)
{
	CBaseCombatWeapon * weapon = getWeaponFromWeaponSlot(slot);
	if (weapon != NULL)
		weapon->Kill();
}

void Player::setCash(unsigned int newCash)
{
	try
	{
		cashHandler.getProp<unsigned int>(identity.pEntity) = newCash;
	}
	catch(const EntityPropException & e)
	{
		printException(e,__FILE__,__LINE__);
	}
}

void Player::setLifeState(unsigned int newState)
{
	try
	{
		lifeStateHandler.getProp<unsigned int>(identity.pEntity) = newState;
	}
	catch(const EntityPropException & e)
	{
		printException(e,__FILE__,__LINE__);
	}
}
