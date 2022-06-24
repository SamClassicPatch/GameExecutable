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

#include "StdH.h"

#include <Core/Base/Shell.h>
#include <Core/Base/Console.h>
#include <Core/Base/CTString.h>

#include <Engine/Engine.h>
#include <Engine/CurrentVersion.h>
#include <Engine/Entities/Entity.h>
#include <Engine/Network/Server.h>
#include <Engine/Network/Network.h>
#include <Engine/Network/SessionState.h>

#include <Engine/Query/QueryMgr.h>

#define MSPORT      28900
#define BUFFSZ      8192
#define BUFFSZSTR   4096

#define SERIOUSSAMKEY       "AKbna4\0"
#define SERIOUSSAMSTR       "serioussamse"

extern unsigned char *gsseckey(u_char *secure, u_char *key, int enctype);

#define PCKQUERY    "\\gamename\\%s" \
                    "\\gamever\\%s" \
                    "\\location\\%s" \
                    "\\hostname\\%s" \
                    "\\hostport\\%hu" \
                    "\\mapname\\%s" \
                    "\\gametype\\%s" \
                    "\\activemod\\" \
                    "\\numplayers\\%d" \
                    "\\maxplayers\\%d" \
                    "\\gamemode\\openplaying" \
                    "\\difficulty\\Normal" \
                    "\\friendlyfire\\%d" \
                    "\\weaponsstay\\%d" \
                    "\\ammosstay\\%d" \
                    "\\healthandarmorstays\\%d" \
                    "\\allowhealth\\%d" \
                    "\\allowarmor\\%d" \
                    "\\infiniteammo\\%d" \
                    "\\respawninplace\\%d" \
                    "\\password\\0" \
                    "\\vipplayers\\1"

#define PCKINFO     "\\hostname\\%s" \
                    "\\hostport\\%hu" \
                    "\\mapname\\%s" \
                    "\\gametype\\%s" \
                    "\\numplayers\\%d" \
                    "\\maxplayers\\%d" \
                    "\\gamemode\\openplaying" \
                    "\\final\\" \
                    "\\queryid\\8.1"

#define PCKBASIC    "\\gamename\\%s" \
                    "\\gamever\\%s" \
                    "\\location\\EU" \
                    "\\final\\" \
                    "\\queryid\\1.1"

extern CTString _getCurrentGameTypeName();

extern sockaddr_in _sinFrom;
extern CHAR* _szBuffer;
extern CHAR* _szIPPortBuffer;
extern INT   _iIPPortBufferLen;
extern CHAR* _szIPPortBufferLocal;
extern INT   _iIPPortBufferLocalLen;

extern BOOL _bServer;
extern BOOL _bInitialized;
extern BOOL _bActivated;
extern BOOL _bActivatedLocal;

extern void _initializeWinsock(void);
extern void _uninitWinsock();
extern void _sendPacket(const char* szBuffer);
extern void _sendPacket(const char* pubBuffer, INDEX iLen);
extern void _sendPacketTo(const char* szBuffer, sockaddr_in* addsin);
extern void _sendPacketTo(const char* pubBuffer, INDEX iLen, sockaddr_in* sin);
extern void _setStatus(const CTString &strStatus);

extern CDynamicStackArray<CServerRequest> ga_asrRequests;

// Builds hearthbeat packet.
void CLegacyQuery::BuildHearthbeatPacket(CTString &strPacket)
{
  strPacket.PrintF("\\heartbeat\\%hu\\gamename\\%s", (_pShell->GetINDEX("net_iPort") + 1), SERIOUSSAMSTR);
}

void CLegacyQuery::ServerParsePacket(INDEX iLength)
{
  unsigned char *data = (unsigned char*)&_szBuffer[0];

  char *sPch1 = NULL, *sPch2 = NULL, *sPch3 = NULL, *sPch4 = NULL, *sPch5;

  sPch1 = strstr(_szBuffer, "\\status\\");
  sPch2 = strstr(_szBuffer, "\\info\\");
  sPch3 = strstr(_szBuffer, "\\basic\\");
  sPch4 = strstr(_szBuffer, "\\players\\");

  sPch5 = strstr(_szBuffer, "\\secure\\"); // [SSE] [ZCaliptium] Validation Fix.
  
  //CDebugF("Data[%d]: %s\n", iLength, _szBuffer);

  // status request
  if (sPch1) {
    CTString strPacket;
    CTString strLocation;
    strLocation = _pShell->GetString("net_strLocalHost");

    if (strLocation == ""){
      strLocation = "Heartland";
    }

    strPacket.PrintF( PCKQUERY,
      _pShell->GetString("sam_strGameName"),
      _SE_VER_STRING,
      //_pShell->GetString("net_strLocalHost"),
      strLocation,
      _pShell->GetString("gam_strSessionName"),
      _pShell->GetINDEX("net_iPort"),
      _pNetwork->ga_World.wo_strName,
      _getCurrentGameTypeName(),
      _pNetwork->ga_srvServer.GetPlayerCount(),
      _pNetwork->ga_sesSessionState.ses_ctMaxPlayers,
      _pShell->GetINDEX("gam_bFriendlyFire"),
      _pShell->GetINDEX("gam_bWeaponsStay"),
      _pShell->GetINDEX("gam_bAmmoStays"),
      _pShell->GetINDEX("gam_bHealthArmorStays"),
      _pShell->GetINDEX("gam_bAllowHealth"),
      _pShell->GetINDEX("gam_bAllowArmor"),
      _pShell->GetINDEX("gam_bInfiniteAmmo"),
      _pShell->GetINDEX("gam_bRespawnInPlace"));

      for (INDEX i=0; i<_pNetwork->ga_srvServer.GetPlayerCount(); i++)
      {
        CPlayerBuffer &plb = _pNetwork->ga_srvServer.srv_aplbPlayers[i];
        CPlayerTarget &plt = _pNetwork->ga_sesSessionState.ses_apltPlayers[i];
        if (plt.plt_bActive) {
          CTString strPlayer;
          plt.GetMSLegacyPlayerInf(plb.plb_Index, strPlayer);

          // if we don't have enough space left for the next player
          if (strlen(strPacket) + strlen(strPlayer) > 2048) {
            // send the packet
            _sendPacketTo(strPacket, &_sinFrom);
            strPacket = "";
          }
          strPacket += strPlayer;
        }
      }

    strPacket += "\\final\\\\queryid\\333.1";
    _sendPacketTo(strPacket, &_sinFrom);

  // info request
  } else if (sPch2){

    CTString strPacket;
    strPacket.PrintF( PCKINFO,
      _pShell->GetString("gam_strSessionName"),
      _pShell->GetINDEX("net_iPort"),
      _pNetwork->ga_World.wo_strName,
      _getCurrentGameTypeName(),
      _pNetwork->ga_srvServer.GetPlayerCount(),
      _pNetwork->ga_sesSessionState.ses_ctMaxPlayers);
    _sendPacketTo(strPacket, &_sinFrom);

  // basis request
  } else if (sPch3){

    CTString strPacket;
    CTString strLocation;
    strLocation = _pShell->GetString("net_strLocalHost");
    if (strLocation == ""){
      strLocation = "Heartland";
    }
    strPacket.PrintF( PCKBASIC,
      _pShell->GetString("sam_strGameName"),
      _SE_VER_STRING,
      //_pShell->GetString("net_strLocalHost"));
      strLocation);
    _sendPacketTo(strPacket, &_sinFrom);

  // players request
  } else if (sPch4){

    // send the player status response
    CTString strPacket;
    strPacket = "";

    for (INDEX i=0; i<_pNetwork->ga_srvServer.GetPlayerCount(); i++) {
      CPlayerBuffer &plb = _pNetwork->ga_srvServer.srv_aplbPlayers[i];
      CPlayerTarget &plt = _pNetwork->ga_sesSessionState.ses_apltPlayers[i];
      if (plt.plt_bActive) {
        CTString strPlayer;
        plt.GetMSLegacyPlayerInf(plb.plb_Index, strPlayer);

        // if we don't have enough space left for the next player
        if (strlen(strPacket) + strlen(strPlayer) > 2048) {
          // send the packet
          _sendPacketTo(strPacket, &_sinFrom);
          strPacket = "";
        }

        strPacket += strPlayer;
      }
    }

    strPacket += "\\final\\\\queryid\\6.1";
    _sendPacketTo(strPacket, &_sinFrom);
  
  // [SSE] [ZCaliptium] '/validate/' - Validation request.
  } else if (sPch5) {
    //CDebugF("Received 'validate' request from MS.\n");
    data += 8;
    
    //CDebugF("SecureKey: %s\n", data);
    
    u_char  ucGamekey[]          = {SERIOUSSAMKEY};
    //u_char  ucReckey[]          = {"XUCXHC"};
    //CDebugF("SecureKey: %s\n", ucReckey);
    unsigned char *pValidateKey = NULL;
    pValidateKey = gsseckey((u_char*)data, ucGamekey, 0);
    
    CTString strPacket;
    strPacket.PrintF("\\validate\\%s\\final\\%s\\queryid\\2.1", pValidateKey, "");
    _sendPacketTo(strPacket, &_sinFrom);

  } else {
    CErrorF("Unknown query server command!\n");
    CErrorF("%s\n", _szBuffer);
    return;
  }
}
