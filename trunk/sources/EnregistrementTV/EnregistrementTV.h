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

#include "../API/API.h"
#include "../ConVars/ConVars.h"

#ifndef __ENREGISTREMENTTV_H__
#define __ENREGISTREMENTTV_H__

/** Exception sp�cifique aux enregistrements SourceTV du plugin */
class CSSMatchTVException : public std::exception
{
private:
    std::string msg;
public:
	CSSMatchTVException(const std::string & msg) : std::exception()
    {
        this->msg = msg;
    }

    virtual ~CSSMatchTVException() throw() {}

    virtual const char * what() const throw()
    {
		return this->msg.c_str();
    }
};

/** Un enregistrement SourceTV */
class EnregistrementTV
{
private:
	/** Nom de l'enregistrement (arborescence comprise) */
	std::string nom;

	/** Est-ce que l'enregistrement est en cours ? */
	bool enregistrant;

public:
	/** Constructeur par d�faut pour permettre l'initialisation de conteneur d'enregistrements <br/>
	 * Note : un tel enregistrement ne peut pas �tre lanc� !
	 *	
	 */
	EnregistrementTV();

	/** Pr�pare un enregistrement avec un nom donn� 
	 *
	 * @param fichierSansPathSansExtension Nom de l'enregistrement
	 *
	 */
	EnregistrementTV(const std::string & fichierSansPathSansExtension) throw(CSSMatchTVException);

	/** Lance un enregistrement sourceTV
	 *
	 * @throws CSSMatchTVException si il est impossible de lancer l'enregistrement
	 *
	 */
	void enregistre() throw (CSSMatchTVException);

	/** Stoppe l'enregistrement en quelques secondes (~2sec) */
	void coupe();

	/** Accesseur sur le nom de l'enregistrement 
	 *
	 * @return Le nom de l'enregistrement
	 *
	 */
	const std::string & getNom() const;

	/** Accesseur sur l'indicateur d'�tat de l'enregistrement
	 *
	 * @return true si l'enregistrement est en cours
	 *
	 */
	bool getEnregistrant() const;
};

#endif // __ENREGISTREMENTTV_H__
