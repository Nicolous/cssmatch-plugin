#(C)2004-2006 SourceMM Development Team
# Makefile written by David "BAILOPAN" Anderson

#####################################################################
# Directories
#####################################################################

# le dossier contenant les codes sources du SDK (normalement : "mon_mod/src/") :
HL2SDK = /home/Nicolas/cssmatch/src
# dossier de destination du plugin une fois compilé
PLUGIN_DIR=Release
# si besoin, le dossier contenant les sources de MetaMod
SMM_ROOT =$(HL2SDK)/sourcemm
SMMH_ROOT =$(HL2SDK)/sourcehook
# dossier où chercher les fichiers binaires de tout à l'heure (srcds_l_binaries.zip) :
SRCDS = /home/Nicolas/valve/bin

HL2PUB = $(HL2SDK)/public
HL2SDK_TIER0 = $(HL2SDK)/public/tier0
HL2SDK_TIER1 = $(HL2SDK)/tier1

# votre projet est un projet MetaMod ?
COMPILE_MM = false


#####################################################################
# Flags
#####################################################################

ARCH_CFLAGS = -mtune=i686 -march=pentium -mmmx
USER_CFLAGS =
BASE_CFLAGS = -fno-rtti -msse -fpermissive -D_LINUX -DNDEBUG -Dstricmp=strcasecmp -D_stricmp=strcasecmp -D_strnicmp=strncasecmp -Dstrnicmp=strncasecmp -D_snprintf=snprintf -D_vsnprintf=vsnprintf -D_alloca=alloca -Dstrcmpi=strcasecmp -fPIC -Wno-deprecated -msse 
OPT_FLAGS = -O3 -fno-rtti -funroll-loops -s -pipe
DEBUG_FLAGS = -g -ggdb3 -D_DeBuG
CPP = /opt/crosstool/gcc-3.4.1-glibc-2.3.2/i686-unknown-linux-gnu/bin/i686-unknown-linux-gnu-c++

# Le type d'architecture du CPU (doit être i486 pour le moment) :
ARCH = i486
ARCH_BIN = .so


#####################################################################
# Obs/Cpps
#####################################################################

### EDIT BELOW FOR OTHER PROJECTS ###

ifeq "$(COMPILE_MM)" "false"
BINARY = cssmatch_$(ARCH)$(ARCH_BIN)
else
BINARY = cssmatch_$(ARCH)$(ARCH_BIN)
USER_CFLAGS +=-DSU_SOURCEMM
endif

# listez ici les .cpp appartenant à votre projet
OBJECTS = 	serverplugin_convar.cpp  \
			API/API.cpp \
			CSSMatch/CSSMatch.cpp \
			Authentification/Authentification.cpp \
			Configuration/Configuration.cpp \
			ConVars/ConVars.cpp \
			EnregistrementTV/EnregistrementTV.cpp \
			Joueur/Joueur.cpp \
			GestionJoueurs/GestionJoueurs.cpp \
			MRecipientFilter/MRecipientFilter.cpp \
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
			Rapport/Rapport.cpp

#memoverride.cpp IceKey.cpp
#convar.cpp KeyValues.cpp utlbuffer.cpp bitbuf.cpp strtools.cpp \

#####################################################################
# Plugin Lib Includes
#####################################################################

#LINK_SO =-lm -ldl tier0_i486.so vstdlib_i486.so
#LINK_ASDK = mathlib_i486.a choreoobjects_i486.a tier1_i486.a
#LINK_ASU = libmysqlclient.a libz.a libmysys.a

# sqlite3.a

#LINK = $(LINK_SO) $(LINK_ASDK) $(LINK_ASU)

LINK =-lm -ldl tier1_i486.a tier0_i486.so vstdlib_i486.so mathlib_i486.a choreoobjects_i486.a

#####################################################################
# .h Files Folders Includes
#####################################################################

INCLUDE = -I. -I$(HL2PUB) -I$(HL2PUB)/dlls -I$(HL2PUB)/engine -I$(HL2PUB)tier0 -I$(HL2PUB)/tier1 \
-I$(HL2PUB)/vstdlib -I$(HL2SDK)/tier1 -I$(HL2SDK)/game_shared -I$(HL2SDK)/dlls \
-I$(SMM_ROOT) -I$(SMMH_ROOT)


#####################################################################
# Compile Part (Not Touch!!!)
#####################################################################

ifeq "$(DEBUG)" "true"
BIN_DIR = Debug/Linux
CFLAGS = $(DEBUG_FLAGS)
else
BIN_DIR = Release/Linux
CFLAGS = $(OPT_FLAGS) $(USER_CFLAGS) $(BASE_CFLAGS) $(ARCH_CFLAGS)
#$(ARCH_CFLAGS)
endif

OBJ_LINUX := $(OBJECTS:%.cpp=$(BIN_DIR)/%.o)

$(BIN_DIR)/%.o: %.cpp
	$(CPP) $(INCLUDE) $(CFLAGS) -o $@ -c $<

all:
	mkdir -p $(BIN_DIR)
	$(MAKE) clean
#$(MAKE) clean
#ln -sf $(HL2SDK_TIER1)/interface.cpp interface.cpp
#ln -sf $(HL2SDK_TIER1)/convar.cpp convar.cpp
#ln -sf $(HL2SDK_TIER1)/KeyValues.cpp KeyValues.cpp
#ln -sf $(HL2SDK_TIER1)/utlbuffer.cpp utlbuffer.cpp
#ln -sf $(HL2SDK_TIER1)/bitbuf.cpp bitbuf.cpp
#ln -sf $(HL2SDK_TIER1)/strtools.cpp strtools.cpp
	ln -sf $(HL2PUB)/IceKey.cpp IceKey.cpp
	ln -sf $(SRCDS)/vstdlib_i486.so vstdlib_i486.so
	ln -sf $(SRCDS)/tier0_i486.so tier0_i486.so
	ln -sf $(HL2SDK_TIER0)/memoverride.cpp memoverride.cpp

	ln -sf $(HL2SDK)/linux_sdk/mathlib_i486.a mathlib_i486.a
	ln -sf $(HL2SDK)/linux_sdk/choreoobjects_i486.a choreoobjects_i486.a
	ln -sf $(HL2SDK)/linux_sdk/tier1_i486.a tier1_i486.a

# ln -sf $(MySQL)/libmysqlclient.a libmysqlclient.a
# ln -sf $(MySQL)/libz.a libz.a
# ln -sf $(MySQL)/libmysys.a libmysys.a
# ln -sf $(MySQL)/libmysys.a libmysys.a
#ln -sf $(PLUGIN_DIR)/library/sqlite3/sqlite3.a sqlite3.a
#ln -sf $(PLUGIN_DIR)/library/curl-7.16.1/lib/.libs/libcurl.a libcurl.a
	$(MAKE) plugin
	$(MAKE) clean
#cp $(PLUGIN_DIR)/Release/Linux/$(BINARY) $(COPY)

plugin: $(OBJ_LINUX)
	$(CPP) $(INCLUDE) $(CFLAGS) $(OBJ_LINUX) $(LINK) -shared -ldl -lm -o$(BIN_DIR)/$(BINARY)

debug:
	$(MAKE) all DEBUG=true

default: all

clean:
#rm -rf $(BIN_DIR)/*.o
	rm -rf $(BIN_DIR)/*.o
	rm -rf $(BIN_DIR)/*/*.o