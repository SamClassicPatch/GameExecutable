/* Copyright (c) 2002-2012 Croteam Ltd. 
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#ifndef SE_INCL_GAMEAGENT_H
#define SE_INCL_GAMEAGENT_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

extern CTString ms_strGameAgentMS;
extern CTString ms_strMSLegacy;
extern CTString ms_strDarkPlacesMS;

// [Cecil] Master server protocols
enum EMasterServerProtocols {
  E_MS_LEGACY     = 0, // Default
  E_MS_DARKPLACES = 1, // Dark Places
  E_MS_GAMEAGENT  = 2, // SE1.10

  E_MS_MAX,
};

// [Cecil] Current master server protocol
extern INDEX ms_iProtocol;

// [Cecil] Get current protocol
inline INDEX GetProtocol(void) {
  if (ms_iProtocol < E_MS_LEGACY || ms_iProtocol >= E_MS_MAX) {
    return E_MS_LEGACY;
  }
  return ms_iProtocol;
};

// [Cecil] Debug output for query
extern INDEX ms_bDebugOutput;

// [Cecil] Get amount of server clients
INDEX GetClientCount(void);

// [Cecil] Get number of active server players
INDEX GetPlayerCount(void);

extern void MS_OnServerStart(void);
extern void MS_OnServerEnd(void);
extern void MS_OnServerUpdate(void);
extern void MS_OnServerStateChanged(void);

// Common Serverlist Enumeration
extern void MS_SendHeartbeat(INDEX iChallenge);

extern void MS_EnumTrigger(BOOL bInternet);
extern void MS_EnumUpdate(void);
extern void MS_EnumCancel(void);

// [Cecil] Replacement for CNetworkLibrary::EnumSessions
extern void MS_EnumSessions(BOOL bInternet);

//

// GameAgent Master Server
class CGameAgentQuery
{
  public:
    static void BuildHearthbeatPacket(CTString &strPacket, INDEX iChallenge);
    static void EnumTrigger(BOOL bInternet);
    static void EnumUpdate(void);
    static void ServerParsePacket(INDEX iLength);
};

// Legacy Master Server
class CLegacyQuery
{
  public:
    static void BuildHearthbeatPacket(CTString &strPacket);
    static void EnumTrigger(BOOL bInternet);
    static void EnumUpdate(void);
    static void ServerParsePacket(INDEX iLength);
};

// DarkPlaces Master Server
class CDarkPlacesQuery
{
  public:  
    static void BuildHearthbeatPacket(CTString &strPacket);
    static void EnumTrigger(BOOL bInternet);
    static void EnumUpdate(void);
    static void ServerParsePacket(INDEX iLength);
};

DWORD WINAPI _MS_Thread(LPVOID lpParam);
DWORD WINAPI _LocalNet_Thread(LPVOID lpParam);

//! Server request structure. Primarily used for getting server pings.
class CServerRequest
{
  public:
    ULONG sr_ulAddress;
    USHORT sr_iPort;
    __int64 sr_tmRequestTime; // [Cecil] 'long long' -> '__int64'

  public:
    CServerRequest(void);
    ~CServerRequest(void);

    //! Destroy all objects, and reset the array to initial (empty) state.
    void Clear(void);
};

// [Cecil] Made query data available from anywhere
namespace QueryData {
  // Used by all
  extern sockaddr_in _sinFrom;
  extern CHAR* _szBuffer;

  extern BOOL _bServer;
  extern BOOL _bInitialized;
  extern BOOL _bActivated;
  extern BOOL _bActivatedLocal;

  extern CDynamicStackArray<CServerRequest> ga_asrRequests;

  // Used by MSLegacy
  extern CHAR *_szIPPortBuffer;
  extern INT   _iIPPortBufferLen;
  extern CHAR *_szIPPortBufferLocal;
  extern INT   _iIPPortBufferLocalLen;
};

// [Cecil] Moved out of Legacy query sources
#define SERIOUSSAMKEY "AKbna4\0"
#ifdef SE1_TFE
  #define SERIOUSSAMSTR "serioussam"
#else
  #define SERIOUSSAMSTR "serioussamse"
#endif

#endif // include once check
