/* Copyright (c) 2021-2022 by ZCaliptium.

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

#include "StdH.h"

// [Cecil] Compatibility with SE1.05
#if SE1_VER == 105
  #include <Engine/Network/Comm.h>
  typedef Communication CCommunicationInterface;
  #define _cmiComm comm
#else
  #include <Engine/Network/CommunicationInterface.h>
#endif

#pragma comment(lib, "wsock32.lib")

// [Cecil] Put under the namespace
namespace QueryData {
  WSADATA *_wsaData = NULL;
  SOCKET _socket = NULL;

  sockaddr_in* _sin = NULL;
  sockaddr_in* _sinLocal = NULL;
  sockaddr_in _sinFrom;

  CHAR *_szBuffer = NULL;
  CHAR *_szIPPortBuffer = NULL;
  INT   _iIPPortBufferLen = 0;
  CHAR *_szIPPortBufferLocal = NULL;
  INT   _iIPPortBufferLocalLen = 0;

  BOOL _bServer = FALSE;
  BOOL _bInitialized = FALSE;
  BOOL _bActivated = FALSE;
  BOOL _bActivatedLocal = FALSE;

  CDynamicStackArray<CServerRequest> ga_asrRequests;
};

// [Cecil] Use query data here
using namespace QueryData;

// [Cecil] Made static
static TIME _tmLastHeartbeat = -1.0f;

extern CTString ms_strGameAgentMS = "master.333networks.com";
extern CTString ms_strMSLegacy = "master.333networks.com";
extern CTString ms_strDarkPlacesMS = "192.168.1.4";

// [Cecil] Current master server protocol
extern INDEX ms_iProtocol = E_MS_LEGACY;

// [Cecil] Debug output for query
extern INDEX ms_bDebugOutput = FALSE;

// [Cecil] Get amount of server clients
INDEX GetClientCount(void) {
  CServer &srv = _pNetwork->ga_srvServer;
  INDEX ctClients = 0;

  for (INDEX iSession = 0; iSession < srv.srv_assoSessions.Count(); iSession++) {
    CSessionSocket &sso = srv.srv_assoSessions[iSession];

    if (iSession > 0 && !sso.sso_bActive) {
      continue;
    }

    ctClients++;
  }

  return ctClients;
};

// [Cecil] Get number of active server players
INDEX GetPlayerCount(void) {
  CServer &srv = _pNetwork->ga_srvServer;
  INDEX ctPlayers = 0;

  FOREACHINSTATICARRAY(srv.srv_aplbPlayers, CPlayerBuffer, itplb) {
    if (itplb->IsActive()) {
      ctPlayers++;
    }
  }

  return ctPlayers;
};

void _uninitWinsock();

void _initializeWinsock(void)
{
  if (_wsaData != NULL && _socket != NULL) {
    return;
  }

  _wsaData = new WSADATA;
  _socket = NULL;

  // make the buffer that we'll use for packet reading
  if (_szBuffer != NULL) {
    delete[] _szBuffer;
  }
  _szBuffer = new char[2050];

  // start WSA
  if (WSAStartup(MAKEWORD(2, 2), _wsaData) != 0) {
    CPrintF("Error initializing winsock!\n");
    _uninitWinsock();
    return;
  }

  // [Cecil] Arranged in an array
  static const CTString astrIPs[E_MS_MAX] = {
    ms_strMSLegacy,
    ms_strDarkPlacesMS,
    ms_strGameAgentMS,
  };

  // [Cecil] Select IP first
  const CTString &strMasterServerIP = astrIPs[GetProtocol()];

  // get the host IP
  hostent* phe = gethostbyname(strMasterServerIP); // [Cecil] Get host

  // if we couldn't resolve the hostname
  if (phe == NULL) {
    // report and stop
    CPrintF("Couldn't resolve the host server '%s'\n", strMasterServerIP);
    _uninitWinsock();
    return;
  }

  // create the socket destination address
  _sin = new sockaddr_in;
  _sin->sin_family = AF_INET;
  _sin->sin_addr.s_addr = *(ULONG*)phe->h_addr_list[0];

  // [Cecil] Arranged in an array
  static const UWORD aiPorts[E_MS_MAX] = {
    27900,
    27950,
    9005,
  };
  
  // [SSE]
  _sin->sin_port = htons(aiPorts[GetProtocol()]);

  // create the socket
  _socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  // if we're a server
  if (_bServer) {
    // create the local socket source address
    _sinLocal = new sockaddr_in;
    _sinLocal->sin_family = AF_INET;
    _sinLocal->sin_addr.s_addr = inet_addr("0.0.0.0");
    _sinLocal->sin_port = htons(_pShell->GetINDEX("net_iPort") + 1);

    // bind the socket
    bind(_socket, (sockaddr*)_sinLocal, sizeof(*_sinLocal));
  }

  // set the socket to be nonblocking
  DWORD dwNonBlocking = 1;
  if (ioctlsocket(_socket, FIONBIO, &dwNonBlocking) != 0) {
    CPrintF("Error setting socket to nonblocking!\n");
    _uninitWinsock();
    return;
  }
}

void _uninitWinsock()
{
  if (_wsaData != NULL) {
    closesocket(_socket);
    delete _wsaData;
    _wsaData = NULL;
  }
  _socket = NULL;
}

void _sendPacketTo(const char* pubBuffer, INDEX iLen, sockaddr_in* sin)
{
  sendto(_socket, pubBuffer, iLen, 0, (sockaddr*)sin, sizeof(sockaddr_in));
}

void _sendPacketTo(const char* szBuffer, sockaddr_in* addsin)
{
  sendto(_socket, szBuffer, strlen(szBuffer), 0, (sockaddr*)addsin, sizeof(sockaddr_in));
}

void _sendPacket(const char* pubBuffer, INDEX iLen)
{
  _initializeWinsock();
  _sendPacketTo(pubBuffer, iLen, _sin);
}

void _sendPacket(const char* szBuffer)
{
  _initializeWinsock();
  _sendPacketTo(szBuffer, _sin);
}

int _recvPacket()
{
  int fromLength = sizeof(_sinFrom);
  return recvfrom(_socket, _szBuffer, 2048, 0, (sockaddr*)&_sinFrom, &fromLength);
}

CTString _getGameModeName(INDEX iGameMode)
{
  // get function that will provide us the info about gametype
  CShellSymbol *pss = _pShell->GetSymbol("GetGameTypeName", /*bDeclaredOnly=*/ TRUE);

  if (pss == NULL) {
    return "";
  }

  CTString (*pFunc)(INDEX) = (CTString (*)(INDEX))pss->ss_pvValue;
  return pFunc(iGameMode);
}

extern CTString _getCurrentGameTypeName()
{
  CShellSymbol *pss = _pShell->GetSymbol("GetCurrentGameTypeName", /*bDeclaredOnly=*/ TRUE);

  if (pss == NULL) {
    return "";
  }

  CTString (*pFunc)(void) = (CTString (*)(void))pss->ss_pvValue;
  return pFunc();
}

extern void MS_SendHeartbeat(INDEX iChallenge)
{
  CTString strPacket;
  
  // [SSE]
  switch (GetProtocol()) {
    case E_MS_LEGACY:
      CLegacyQuery::BuildHearthbeatPacket(strPacket);
      break;

    case E_MS_DARKPLACES:
      CDarkPlacesQuery::BuildHearthbeatPacket(strPacket);
      break;

    case E_MS_GAMEAGENT:
      CGameAgentQuery::BuildHearthbeatPacket(strPacket, iChallenge);
      break;
  }

  _sendPacket(strPacket);
  _tmLastHeartbeat = _pTimer->GetRealTimeTick();

  if (ms_bDebugOutput) {
    CPrintF("Sending heartbeat:\n%s\n", strPacket);
  }
}

extern void _setStatus(const CTString &strStatus)
{
  _pNetwork->ga_bEnumerationChange = TRUE;
  _pNetwork->ga_strEnumerationStatus = strStatus;
}

CServerRequest::CServerRequest(void)
{
  Clear();
}

CServerRequest::~CServerRequest(void) {}

void CServerRequest::Clear(void)
{
  sr_ulAddress = 0;
  sr_iPort = 0;
  sr_tmRequestTime = 0;
}

// Called on every network server startup.
extern void MS_OnServerStart(void)
{
  // join
  _bServer = TRUE;
  _bInitialized = TRUE;
  
  // [SSE]
  switch (GetProtocol()) {
    // [Cecil] Unused in SSE
    /*case E_MS_LEGACY: {
      CTString strPacket;
      strPacket.PrintF("\\heartbeat\\%hu\\gamename\\%s", (_pShell->GetINDEX("net_iPort") + 1), SERIOUSSAMSTR);

      _sendPacket(strPacket);

      if (ms_bDebugOutput) {
        CPrintF("Server start:\n%s\n", strPacket);
      }
    } break;*/

    case E_MS_DARKPLACES: {
      CTString strPacket;
      strPacket.PrintF("\xFF\xFF\xFF\xFFheartbeat DarkPlaces\x0A");
    
      _sendPacket(strPacket);
    } break;

    case E_MS_GAMEAGENT: {
      _sendPacket("q");
    } break;
  }
}

// Called if server has been stopped.
extern void MS_OnServerEnd(void)
{
  if (!_bInitialized) {
    return;
  }

  const INDEX iProtocol = GetProtocol();

  if (iProtocol == E_MS_DARKPLACES) {
    MS_SendHeartbeat(0);
    MS_SendHeartbeat(0);

  // [Cecil] Anything but GameAgent
  } else if (iProtocol != E_MS_GAMEAGENT) {
    CTString strPacket;
    strPacket.PrintF("\\heartbeat\\%hu\\gamename\\%s\\statechanged", (_pShell->GetINDEX("net_iPort") + 1), SERIOUSSAMSTR);
    _sendPacket(strPacket);

    if (ms_bDebugOutput) {
      CPrintF("Server end:\n%s\n", strPacket);
    }
  }

  _uninitWinsock();
  _bInitialized = FALSE;
}

// Regular network server update.
// Responds to enumeration pings and sends pings to masterserver.
extern void MS_OnServerUpdate(void)
{
  if ((_socket == NULL) || (!_bInitialized)) {
    return;
  }

  INDEX iLength = _recvPacket();

  if (iLength > 0)
  {
    if (ms_bDebugOutput) {
      CPrintF("Received packet, length: %d\n", iLength);
    }

    // [Cecil] Arranged in an array
    static void (*apParsePacket[E_MS_MAX])(INDEX) = {
      &CLegacyQuery::ServerParsePacket,
      &CDarkPlacesQuery::ServerParsePacket,
      &CGameAgentQuery::ServerParsePacket,
    };

    // [SSE]
    (*apParsePacket[GetProtocol()])(iLength);
  }

  // send a heartbeat every 150 seconds
  if (_pTimer->GetRealTimeTick() - _tmLastHeartbeat >= 150.0f) {
    MS_SendHeartbeat(0);
  }
};

// Notify master server that the server state has changed.
extern void MS_OnServerStateChanged(void)
{
  if (!_bInitialized) {
    return;
  }

  // [Cecil] Arranged in a switch-case
  switch (GetProtocol()) {
    case E_MS_LEGACY: {
      CTString strPacket;
      strPacket.PrintF("\\heartbeat\\%hu\\gamename\\%s\\statechanged", (_pShell->GetINDEX("net_iPort") + 1), SERIOUSSAMSTR);

      _sendPacket(strPacket);

      if (ms_bDebugOutput) {
        CPrintF("Sending state change:\n%s\n", strPacket);
      }
    } break;

    // Nothing for E_MS_DARKPLACES

    case E_MS_GAMEAGENT: {
      _sendPacket("u");
    } break;
  }
}

// Request serverlist enumeration. Sends request packet.
extern void MS_EnumTrigger(BOOL bInternet)
{
  if (_pNetwork->ga_bEnumerationChange) {
    return;
  }
  
  // [Cecil] Arranged in an array
  static void (*apEnumTrigger[E_MS_MAX])(BOOL) = {
    &CLegacyQuery::EnumTrigger,
    &CDarkPlacesQuery::EnumTrigger,
    &CGameAgentQuery::EnumTrigger,
  };

  (*apEnumTrigger[GetProtocol()])(bInternet);
}

// Client update for enumerations.
extern void MS_EnumUpdate(void)
{
  if ((_socket == NULL) || (!_bInitialized)) {
    return;
  }
  
  // [Cecil] Arranged in an array
  static void (*apEnumUpdate[E_MS_MAX])(void) = {
    &CLegacyQuery::EnumUpdate,
    &CDarkPlacesQuery::EnumUpdate,
    &CGameAgentQuery::EnumUpdate,
  };

  // [SSE]
  (*apEnumUpdate[GetProtocol()])();
}

// Cancel the master server serverlist enumeration.
extern void MS_EnumCancel(void)
{
  if (_bInitialized) {
    ga_asrRequests.Clear();
    _uninitWinsock();
  }
}

// [Cecil] Replacement for CNetworkLibrary::EnumSessions
void MS_EnumSessions(BOOL bInternet)
{
  // Clear old sessions
  FORDELETELIST(CNetworkSession, ns_lnNode, _pNetwork->ga_lhEnumeratedSessions, itns) {
    delete &*itns;
  }

  if (!_cmiComm.IsNetworkEnabled()) {
    // Have to enumerate as server
    _cmiComm.PrepareForUse(TRUE, FALSE);
  }

  // Enumerate sessions using new query manager
  MS_EnumTrigger(bInternet);
};
