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

#ifndef __TRANSLATION_FILE_H__
#define __TRANSLATION_FILE_H__

#include "ConfigurationFile.h"

#include "../exceptions/BaseException.h"
#include "../features/BaseSingleton.h"

#include <map>

namespace cssmatch
{
	class TranslationException : public ConfigurationFileException
	{
	public:
		TranslationException(const std::string & message) : ConfigurationFileException(message){};
	};

	/** Translation file <br>
	 * Rules : <br>
	 *	- // marks a commentary statement <br>
	 *	- A [header] must be placed before any translation, e.g. [French] <br>
	 *	- Each translation line must takes place on its proper line <br>
	 *	- Each translation line must be under this format :  <br>
	 *		keyword = translation <br>
	 *	- Both keyword and translation can be between quotation marks, to delimit where the data ends <br>
	 *  Support the ANSI and UTF-8 encoding <br>
	 * Note: \n are interpreted
	 */
	class TranslationFile : public ConfigurationFile
	{
	private:
		/** [header] (e.g. translation name)*/
		std::string header;

		/** {keyword => translation} map */
		std::map<std::string,std::string> translations;

		/** Search and store the header and the translations 
		 * @throws TranslationException if not header was found
		 */
		void parse() throw(TranslationException);

		/** Search the header 
		 * @param lines The lines to parse
		 */
		void parseHeader(std::list<std::string> & lines);

		/** Search the translations
		 * @param lines The remaining lines to parse
		 */
		void parseTranslations(std::list<std::string> & lines);
	public:
		/** Parse a new translation file
		 * @param filePath The path of the file to parse
		 * @throws TranslationException if not header was found (incorrect translations are just ignored)
		 */
		TranslationFile(const std::string & filePath) throw(ConfigurationFileException,TranslationException);
		virtual ~TranslationFile();

		/** Get the header content */
		std::string getHeader() const;

		/** In-memory header modification */
		void setHeader(const std::string & header);

		/** Add a translation 
		 * @param keyword Identifier for the translation
		 * @param translation The translation to add
		 */
		void addTranslation(const std::string & keyword, const std::string & translation);

		/** Check if a translation exists
		 * @param keyword The keyword corresponding to the translation
		 * @return <code>true</code> if the translation was found, <code>false</code> otherwise
		 */
		bool keywordExists(const std::string & keyword) const;

		/** Get the translation corresponding to a keyword
		 * @param keyword The keyword corresponding to the translation
		 * @return The translation
		 * @throws TranslationException if the translation does not exist
		 */
		std::string operator [](const std::string & keyword) throw (TranslationException);
	};
}

#endif // __TRANSLATION_FILE_H__
