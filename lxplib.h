#pragma once


/*
** See Copyright Notice in license.html
*/

#define ParserType	"Expat"

#define StartCdataKey			"StartCdataSection"
#define EndCdataKey			"EndCdataSection"
#define CharDataKey			"CharacterData"
#define CommentKey			"Comment"
#define DefaultKey			"Default"
#define DefaultExpandKey		"DefaultExpand"
#define StartElementKey			"StartElement"
#define EndElementKey			"EndElement"
#define ExternalEntityKey		"ExternalEntityRef"
#define StartNamespaceDeclKey		"StartNamespaceDecl"
#define EndNamespaceDeclKey		"EndNamespaceDecl"
#define NotationDeclKey			"NotationDecl"
#define NotStandaloneKey		"NotStandalone"
#define ProcessingInstructionKey	"ProcessingInstruction"
#define UnparsedEntityDeclKey		"UnparsedEntityDecl"
#define StartDoctypeDeclKey		"StartDoctypeDecl"
#define XmlDeclKey			"XmlDecl"

#ifndef LUAEXPAT_API
//#define LUASOCKET_API extern
#define LUAEXPAT_API __declspec(dllexport)
#endif

LUAEXPAT_API int luaopen_lxp (lua_State *L);
