# LuaSmartCardLibrary
Smart Card Library for LUA

This is a quick hack for development/scripting smartcard based on lua, user friendly debugging with ZeroBrane or others

Based on:
1. CardPeek

2. Smart Card Digging Utilities,PC/SC Wrappers (Ilya O. Levin, http://www.literatecode.com)

3. Others


# Installations
1. Download all the files
2. Open with visual studio (VS Community 2015)
3. Compile dynamically against Lua V5.3 and openssl. (Compile lua5.3 and openSSL as DLL first) to avoid multiple VM Errors!
4. Copy the DLL to ZeroBrane\bin, the same folder with Lua53.dll
5. enjoy

# SCRIPTS
Scripts can be found in folder scripts
1. 02.DualCard.lua --> showing transactions between two cards and logging