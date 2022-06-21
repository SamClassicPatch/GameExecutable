/* Copyright (c) 2022 Dreamy Cecil
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

// [Cecil] Square-ish resolutions
#define CT_RES_SQUARE 24

static const PIX2D _avpix4_3[CT_RES_SQUARE] = {
  PIX2D(320,  240),
  PIX2D(400,  300),
  PIX2D(512,  384),
  PIX2D(640,  240), // Dual
  PIX2D(640,  480),
  PIX2D(720,  540),
  PIX2D(800,  300), // Dual
  PIX2D(800,  600),
  PIX2D(960,  720),
  PIX2D(1024, 384), // Dual
  PIX2D(1024, 768),
  PIX2D(1152, 864),
  PIX2D(1280, 480), // Dual
  PIX2D(1280, 960),
  PIX2D(1280, 1024), // 5:4
  PIX2D(1440, 1080),
  PIX2D(1600, 600), // Dual
  PIX2D(1600, 1200),
  PIX2D(1920, 720), // Dual
  PIX2D(1920, 1440),
  PIX2D(1920, 1920), // 1:1
  PIX2D(2048, 786),
  PIX2D(2048, 1536),
  PIX2D(2560, 2048), // 5:4
};

// [Cecil] Standard widescreen resolutions
#define CT_RES_STANDARD 13

static const PIX2D _avpix16_9[CT_RES_STANDARD] = {
  PIX2D(640, 360),
  PIX2D(854, 480),
  PIX2D(960, 540),
  PIX2D(1024, 576),
  PIX2D(1280, 720),
  PIX2D(1366, 768),
  PIX2D(1600, 900),
  PIX2D(1920, 1080),
  PIX2D(2560, 1440),
  PIX2D(3200, 1800),
  PIX2D(3840, 2160),
  PIX2D(5120, 2880),
  PIX2D(7680, 4320),
};

// [Cecil] Extra widescreen resolutions
#define CT_RES_WIDE 13

static const PIX2D _avpix16_10[CT_RES_WIDE] = {
  PIX2D(480, 320), // 3:2
  PIX2D(1152, 768), // 3:2
  PIX2D(1280, 800),
  PIX2D(1280, 854), // 3:2
  PIX2D(1440, 900),
  PIX2D(1440, 960), // 3:2
  PIX2D(1600, 1000), // Extra
  PIX2D(1680, 1050),
  PIX2D(1920, 1200),
  PIX2D(2160, 1440), // 3:2
  PIX2D(2560, 1600),
  PIX2D(2560, 1700), // 3:2
  PIX2D(3840, 2400),
};

// [Cecil] Very wide resolutions
#define CT_RES_EXTRA_WIDE 6

static const PIX2D _avpix21_9[CT_RES_EXTRA_WIDE] = {
  PIX2D(1400, 600), // Extra
  PIX2D(1680, 720), // Extra
  PIX2D(2100, 900), // Extra
  PIX2D(2560, 1080),
  PIX2D(3440, 1440),
  PIX2D(5120, 2160),
};

// [Cecil] All resolutions
#define CT_RESOLUTIONS (CT_RES_SQUARE + CT_RES_STANDARD + CT_RES_WIDE + CT_RES_EXTRA_WIDE)

// [Cecil] All aspect ratios
#define CT_ASPECTRATIOS 4

// [Cecil] Aspect ratio names
static const CTString _astrAspectRatios[CT_ASPECTRATIOS] = {
  "4:3",
  "16:9",
  "16:10",
  "21:9",
};

// [Cecil] Aspect ratio lists
static const PIX2D *_aAspectRatios[CT_ASPECTRATIOS] = {
  (PIX2D *)&_avpix4_3,
  (PIX2D *)&_avpix16_9,
  (PIX2D *)&_avpix16_10,
  (PIX2D *)&_avpix21_9,
};

// [Cecil] Aspect ratio resolutions
static const INDEX _actAspectRatioRes[CT_ASPECTRATIOS] = {
  CT_RES_SQUARE,
  CT_RES_STANDARD,
  CT_RES_WIDE,
  CT_RES_EXTRA_WIDE,
};
