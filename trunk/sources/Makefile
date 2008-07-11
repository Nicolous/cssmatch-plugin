# 
# Copyright 2007, 2008 Nicolas Maingot
# 
# This file is part of CSSMatch.
# 
# CSSMatch is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
# 
# CSSMatch is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with CSSMatch; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
# Portions of this code are also Copyright © 2004-2006 SourceMM Development Team, All rights reserved
#


############
# Dossiers du projet
############

# Code source du SDK de VALVE
SDK_SRC_DIR = /home/Nicolas/cssmatch/src
SDK_PUBLIC_DIR = $(SDK_SRC_DIR)/public
SDK_TIER0_DIR = $(SDK_SRC_DIR)/public/tier0
SDK_TIER1_DIR = $(SDK_SRC_DIR)/tier1

# Dossier de sortie
RELEASE_DIR = Release/linux
DEBUG_DIR = Debug/linux

# Dossier contenant les binaires provenant de l'archive SRCDS_BIN_DIR_l_binaries.zip :
SRCDS_BIN_DIR = /home/Nicolas/valve/bin

###############
# Options de compilation
###############

# Architecture du processeur
ARCH_CFLAGS = 	-mtune=i686 \
				-march=pentium \
				-mmmx
ARCH = i486
ARCH_BIN = .so

# Options du compilateur
USER_CFLAGS =
BASE_CFLAGS = 	-fno-rtti \
				-msse \
				-fpermissive \
				-D_LINUX \
				-DNDEBUG \
				-Dstricmp=strcasecmp \
				-D_stricmp=strcasecmp \
				-D_strnicmp=strncasecmp \
				-Dstrnicmp=strncasecmp \
				-D_snprintf=snprintf \
				-D_vsnprintf=vsnprintf \
				-D_alloca=alloca \
				-Dstrcmpi=strcasecmp \
				-fPIC \
				-Wno-deprecated \
				-msse 
OPT_FLAGS = -O3 \
			-fno-rtti \
			-funroll-loops \
			-s \
			-pipe
DEBUG_FLAGS = 	-g \
				-ggdb3 \
				-D_DeBuG

# Compilateur
CXX = /opt/crosstool/gcc-3.4.1-glibc-2.3.2/i686-unknown-linux-gnu/bin/i686-unknown-linux-gnu-c++


############
# Objets à compiler
############

OBJECTS = 	PlayerProp/PlayerProp.cpp \
			Hooks/SayHook/SayHook.cpp \
			Hooks/SayTeamHook/SayTeamHook.cpp \
			Authentification/Authentification.cpp \
			Configuration/Configuration.cpp \
			ConVars/ConVars.cpp \
			EnregistrementTV/EnregistrementTV.cpp \
			Joueur/Joueur.cpp \
			GestionJoueurs/GestionJoueurs.cpp \
			Messages/Messages.cpp \
			Team/Team.cpp \
			Timer/Timer.cpp \
			Menu/Menu.cpp \
			Menu/Ligne.cpp \
			GestionTimers/GestionTimers.cpp \
			Decompte/Decompte.cpp \
			Match/Match.cpp \
			ConCommands/ConCommands.cpp \
			GestionMenus/GestionMenus.cpp \
			Rapport/RapportXml/RapportXml.cpp \
			Rapport/Rapport.cpp \
			MRecipientFilter/MRecipientFilter.cpp \
			CSSMatch/CSSMatch.cpp \
			EventListener/EventListener.cpp \
			serverplugin_convar.cpp  \
			API/API.cpp
#I18n/I18n.cpp \			
#Rapport/RapportTxt/RapportTxt.cpp \			

###########
# Librairies à lier			
###########

#LINK_SO =	$(SRCDS_BIN_DIR)/tier0_i486.so \
#			$(SRCDS_BIN_DIR)/vstdlib_i486.so
#LINK_A = 	$(SDK_SRC_DIR)/linux_sdk/mathlib_i486.a \
#			$(SDK_SRC_DIR)/linux_sdk/choreoobjects_i486.a \
#			$(SDK_SRC_DIR)/linux_sdk/tier1_i486.a
LINK_SO =	tier0_i486.so \
			vstdlib_i486.so
LINK_A = 	mathlib_i486.a \
			choreoobjects_i486.a \
			tier1_i486.a

LINK = -lm -ldl $(LINK_SO) $(LINK_A)

# #############
# Bibliothèques à inclure 
##############

INCLUDE = 	-I. \
			-I$(SDK_PUBLIC_DIR) \
			-I$(SDK_PUBLIC_DIR)/dlls \
			-I$(SDK_PUBLIC_DIR)/engine \
			-I$(SDK_PUBLIC_DIR)tier0 \
			-I$(SDK_PUBLIC_DIR)/tier1 \
			-I$(SDK_PUBLIC_DIR)/vstdlib \
			-I$(SDK_SRC_DIR)/tier1 \
			-I$(SDK_SRC_DIR)/game_shared \
			-I$(SDK_SRC_DIR)/dlls

###################
# Cibles et règles de compilation
###################

# Nom du fichier binaire de sortie
BINARY_NAME = cssmatch_$(ARCH)$(ARCH_BIN)

# Détermination du dossier de sortie et rassemblement des options de compilation
ifeq "$(DEBUG)" "true"
BIN_DIR = $(DEBUG_DIR)
CFLAGS = $(DEBUG_FLAGS)
else
BIN_DIR = $(RELEASE_DIR)
CFLAGS = $(OPT_FLAGS) $(USER_CFLAGS) $(BASE_CFLAGS) $(ARCH_CFLAGS)
endif

# Règle de compilation des fichiers .cpp
OBJ_LINUX := $(OBJECTS:%.cpp=$(BIN_DIR)/%.o)
$(BIN_DIR)/%.o: %.cpp
	$(CXX) $(INCLUDE) $(CFLAGS) -o $@ -c $<

all:
	mkdir -p $(BIN_DIR)
	$(MAKE) clean
	
	ln -sf $(SRCDS_BIN_DIR)/tier0_i486.so tier0_i486.so
	ln -sf $(SRCDS_BIN_DIR)/vstdlib_i486.so vstdlib_i486.so
	ln -sf $(SDK_SRC_DIR)/linux_sdk/mathlib_i486.a mathlib_i486.a
	ln -sf $(SDK_SRC_DIR)/linux_sdk/choreoobjects_i486.a choreoobjects_i486.a
	ln -sf $(SDK_SRC_DIR)/linux_sdk/tier1_i486.a tier1_i486.a
	
	$(MAKE) release
	$(MAKE) clean

release: $(OBJ_LINUX)
	$(CXX) $(INCLUDE) $(CFLAGS) $(OBJ_LINUX) $(LINK) -shared -ldl -lm -o $(BIN_DIR)/$(BINARY_NAME)
	
debug:
	$(MAKE) all DEBUG=true

clean:
	rm -rf $(BIN_DIR)/*.o
	rm -rf $(BIN_DIR)/*/*.o
	rm -rf $(BIN_DIR)/*/*/*.o
	rm -rf *.lnk
