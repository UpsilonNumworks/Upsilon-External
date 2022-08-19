#include <stdbool.h>
#include <stdio.h>
#include "extapp_api.h"

#define LCD_WIDTH_PX 320
#define LCD_HEIGHT_PX 222
#define TOOLBAR_HEIGHT_PX 18

#define _WHITE 0xffff
#define _BLACK 0x0000

// table periodique, d'apres https://bitbucket.org/m4x1m3/nw-atom/src/master/
// par M4x1m3 https://tiplanet.org/forum/viewtopic.php?f=102&t=23054
enum AtomType {
  ALKALI_METAL,
  ALKALI_EARTH_METAL,
  LANTHANIDE,
  ACTINIDE,
  TRANSITION_METAL,
  POST_TRANSITION_METAL,
  METALLOID,
  HALOGEN,
  REACTIVE_NONMETAL,
  NOBLE_GAS,
  UNKNOWN
};

struct AtomDef {
  uint8_t num;
  uint8_t x;
  uint8_t y;
  enum AtomType type;
  const char* name;
  const char* symbol;
  uint8_t neutrons;
  double mass;
  double electroneg;
};

const struct AtomDef atomsdefs[] = {
  {  1,  0,  0, REACTIVE_NONMETAL       , "Hydrogen"     , "H"   ,   0, 1.00784     , 2.2   },
  {  2, 17,  0, NOBLE_GAS               , "Helium"       , "He"  ,   2, 4.002602    , -1    },
  {  3,  0,  1, ALKALI_METAL            , "Lithium"      , "Li"  ,   4, 6.938       , 0.98  },
  {  4,  1,  1, ALKALI_EARTH_METAL      , "Beryllium"    , "Be"  ,   5, 9.012182    , 1.57  },
  {  5, 12,  1, METALLOID               , "Boron"        , "B"   ,   6, 10.806      , 2.04  },
  {  6, 13,  1, REACTIVE_NONMETAL       , "Carbon"       , "C"   ,   6, 12.0096     , 2.55  },
  {  7, 14,  1, REACTIVE_NONMETAL       , "Nitrogen"     , "N"   ,   7, 14.00643    , 3.04  },
  {  8, 15,  1, REACTIVE_NONMETAL       , "Oxygen"       , "O"   ,   8, 15.99903    , 3.44  },
  {  9, 16,  1, HALOGEN                 , "Fluorine"     , "F"   ,  10, 18.9984032  , 3.98  },
  { 10, 17,  1, NOBLE_GAS               , "Neon"         , "Ne"  ,  10, 20.1797     , -1    },
  { 11,  0,  2, ALKALI_METAL            , "Sodium"       , "Na"  ,  12, 22.9897693  , 0.93  },
  { 12,  1,  2, ALKALI_EARTH_METAL      , "Magnesium"    , "Mg"  ,  12, 24.3050     , 1.31  },
  { 13, 12,  2, POST_TRANSITION_METAL   , "Aluminium"    , "Al"  ,  14, 26.9815386  , 1.61  },
  { 14, 13,  2, METALLOID               , "Silicon"      , "Si"  ,  14, 28.084      , 1.9   },
  { 15, 14,  2, REACTIVE_NONMETAL       , "Phosphorus"   , "P"   ,  16, 30.973762   , 2.19  },
  { 16, 15,  2, REACTIVE_NONMETAL       , "Sulfur"       , "S"   ,  16, 32.059      , 2.58  },
  { 17, 16,  2, HALOGEN                 , "Chlorine"     , "Cl"  ,  18, 35.446      , 3.16  },
  { 18, 17,  2, NOBLE_GAS               , "Argon"        , "Ar"  ,  22, 39.948      , -1    },
  { 19,  0,  3, ALKALI_METAL            , "Potassium"    , "K"   ,  20, 39.0983     , 0.82  },
  { 20,  1,  3, ALKALI_EARTH_METAL      , "Calcium"      , "Ca"  ,  20, 40.078      , 1     },
  { 21,  2,  3, TRANSITION_METAL        , "Scandium"     , "Sc"  ,  24, 44.955912   , 1.36  },
  { 22,  3,  3, TRANSITION_METAL        , "Titanium"     , "Ti"  ,  26, 47.867      , 1.54  },
  { 23,  4,  3, TRANSITION_METAL        , "Vanadium"     , "V"   ,  28, 50.9415     , 1.63  },
  { 24,  5,  3, TRANSITION_METAL        , "Chromium"     , "Cr"  ,  28, 51.9961     , 1.66  },
  { 25,  6,  3, TRANSITION_METAL        , "Manganese"    , "Mn"  ,  30, 54.938045   , 1.55  },
  { 26,  7,  3, TRANSITION_METAL        , "Iron"         , "Fe"  ,  30, 55.845      , 1.83  },
  { 27,  8,  3, TRANSITION_METAL        , "Cobalt"       , "Co"  ,  32, 58.933195   , 1.88  },
  { 28,  9,  3, TRANSITION_METAL        , "Nickel"       , "Ni"  ,  31, 58.6934     , 1.91  },
  { 29, 10,  3, TRANSITION_METAL        , "Copper"       , "Cu"  ,  35, 63.546      , 1.9   },
  { 30, 11,  3, POST_TRANSITION_METAL   , "Zinc"         , "Zn"  ,  35, 65.38       , 1.65  },
  { 31, 12,  3, POST_TRANSITION_METAL   , "Gallium"      , "Ga"  ,  39, 69.723      , 1.81  },
  { 32, 13,  3, METALLOID               , "Germanium"    , "Ge"  ,  41, 72.63       , 2.01  },
  { 33, 14,  3, METALLOID               , "Arsenic"      , "As"  ,  42, 74.92160    , 2.18  },
  { 34, 15,  3, REACTIVE_NONMETAL       , "Selenium"     , "Se"  ,  45, 78.96       , 2.55  },
  { 35, 16,  3, HALOGEN                 , "Bromine"      , "Br"  ,  45, 79.904      , 2.96  },
  { 36, 17,  3, NOBLE_GAS               , "Krypton"      , "Kr"  ,  48, 83.798      , -1    },
  { 37,  0,  4, ALKALI_METAL            , "Rubidium"     , "Rb"  ,  20, 85.4678     , 0.82  },
  { 38,  1,  4, ALKALI_EARTH_METAL      , "Strontium"    , "Sr"  ,  20, 87.62       , 0.95  },
  { 39,  2,  4, TRANSITION_METAL        , "Yttrium"      , "Y"   ,  24, 88.90585    , 1.22  },
  { 40,  3,  4, TRANSITION_METAL        , "Zirconium"    , "Zr"  ,  26, 91.224      , 1.33  },
  { 41,  4,  4, TRANSITION_METAL        , "Niobium"      , "Nb"  ,  28, 92.90638    , 1.6   },
  { 42,  5,  4, TRANSITION_METAL        , "Molybdenum"   , "Mo"  ,  28, 95.96       , 2.16  },
  { 43,  6,  4, TRANSITION_METAL        , "Technetium"   , "Tc"  ,  30, 98          , 2.10  },
  { 44,  7,  4, TRANSITION_METAL        , "Ruthemium"    , "Ru"  ,  30, 101.07      , 2.2   },
  { 45,  8,  4, TRANSITION_METAL        , "Rhodium"      , "Rh"  ,  32, 102.90550   , 2.28  },
  { 46,  9,  4, TRANSITION_METAL        , "Palladium"    , "Pd"  ,  31, 106.42      , 2.20  },
  { 47, 10,  4, TRANSITION_METAL        , "Silver"       , "Ag"  ,  35, 107.8682    , 1.93  },
  { 48, 11,  4, POST_TRANSITION_METAL   , "Cadmium"      , "Cd"  ,  35, 112.411     , 1.69  },
  { 49, 12,  4, POST_TRANSITION_METAL   , "Indium"       , "In"  ,  39, 114.818     , 1.78  },
  { 50, 13,  4, POST_TRANSITION_METAL   , "Tin"          , "Sn"  ,  41, 118.710     , 1.96  },
  { 51, 14,  4, METALLOID               , "Antimony"     , "Sb"  ,  42, 121.760     , 2.05  },
  { 52, 15,  4, METALLOID               , "Tellurium"    , "Te"  ,  45, 127.60      , 2.1   },
  { 53, 16,  4, HALOGEN                 , "Indine"       , "I"   ,  45, 126.90447   , 2.66  },
  { 54, 17,  4, NOBLE_GAS               , "Xenon"        , "Xe"  ,  48, 131.293     , 2.60  },
  { 55,  0,  5, ALKALI_METAL            , "Caesium"      , "Cs"  ,  78, 132.905452  , 0.79  },
  { 56,  1,  5, ALKALI_EARTH_METAL      , "Barium"       , "Ba"  ,  81, 137.327     , 0.89  },
  { 57,  3,  7, LANTHANIDE              , "Lanthanum"    , "La"  ,  82, 138.90547   , 1.10  },
  { 58,  4,  7, LANTHANIDE              , "Cerium"       , "Ce"  ,  82, 140.116     , 1.12  },
  { 59,  5,  7, LANTHANIDE              , "Praseodymium" , "Pr"  ,  82, 140.90765   , 1.13  },
  { 60,  6,  7, LANTHANIDE              , "Neodymium"    , "Nd"  ,  84, 144.242     , 1.14  },
  { 61,  7,  7, LANTHANIDE              , "Promethium"   , "Pm"  ,  84, 145         , 1.13  },
  { 62,  8,  7, LANTHANIDE              , "Samarium"     , "Sm"  ,  88, 150.36      , 1.17  },
  { 63,  9,  7, LANTHANIDE              , "Europium"     , "Eu"  ,  89, 151.964     , 1.12  },
  { 64, 10,  7, LANTHANIDE              , "Gadolinium"   , "Gd"  ,  93, 157.25      , 1.20  },
  { 65, 11,  7, LANTHANIDE              , "Terbium"      , "Tb"  ,  94, 158.92535   , 1.12  },
  { 66, 12,  7, LANTHANIDE              , "Dyxprosium"   , "Dy"  ,  97, 162.500     , 1.22  },
  { 67, 13,  7, LANTHANIDE              , "Holmium"      , "Ho"  ,  98, 164.93032   , 1.23  },
  { 68, 14,  7, LANTHANIDE              , "Erbium"       , "Er"  ,  99, 167.259     , 1.24  },
  { 69, 15,  7, LANTHANIDE              , "Thulium"      , "Tm"  , 100, 168.93421   , 1.25  },
  { 70, 16,  7, LANTHANIDE              , "Ytterbium"    , "Yb"  , 103, 173.054     , 1.1   },
  { 71, 17,  7, LANTHANIDE              , "Lutetium"     , "Lu"  , 104, 174.9668    , 1.0   },
  { 72,  3,  5, TRANSITION_METAL        , "Hafnium"      , "Hf"  , 106, 178.49      , 1.3   },
  { 73,  4,  5, TRANSITION_METAL        , "Tantalum"     , "Ta"  , 108, 180.94788   , 1.5   },
  { 74,  5,  5, TRANSITION_METAL        , "Tungsten"     , "W"   , 110, 183.84      , 1.7   },
  { 75,  6,  5, TRANSITION_METAL        , "Rhenium"      , "Re"  , 111, 186.207     , 1.9   },
  { 76,  7,  5, TRANSITION_METAL        , "Osmium"       , "Os"  , 114, 190.23      , 2.2   },
  { 77,  8,  5, TRANSITION_METAL        , "Iridium"      , "Ir"  , 115, 192.217     , 2.2   },
  { 78,  9,  5, TRANSITION_METAL        , "Platinum"     , "Pt"  , 117, 195.084     , 2.2   },
  { 79, 10,  5, TRANSITION_METAL        , "Gold"         , "Au"  , 118, 196.966569  , 2.4   },
  { 80, 11,  5, POST_TRANSITION_METAL   , "Mercury"      , "Hg"  , 121, 200.59      , 1.9   },
  { 81, 12,  5, POST_TRANSITION_METAL   , "Thalium"      , "Tl"  , 123, 204.382     , 1.8   },
  { 82, 13,  5, POST_TRANSITION_METAL   , "Lead"         , "Pb"  , 125, 207.2       , 1.8   },
  { 83, 14,  5, POST_TRANSITION_METAL   , "Bismuth"      , "Bi"  , 126, 208.98040   , 1.9   },
  { 84, 15,  5, POST_TRANSITION_METAL   , "Polonium"     , "Po"  , 125, 209         , 2.0   },
  { 85, 16,  5, HALOGEN                 , "Astatine"     , "At"  , 125, 210         , 2.2   },
  { 86, 17,  5, NOBLE_GAS               , "Radon"        , "Rn"  , 136, 222         , 2.2   },
  { 87,  0,  6, ALKALI_METAL            , "Francium"     , "Fr"  , 136, 223         , 0.7   },
  { 88,  1,  6, ALKALI_EARTH_METAL      , "Radium"       , "Ra"  , 138, 226         , 0.9   },
  { 89,  3,  8, ACTINIDE                , "Actinium"     , "Ac"  , 138, 227         , 1.1   },
  { 90,  4,  8, ACTINIDE                , "Thorium"      , "Th"  , 142, 232.03806   , 1.3   },
  { 91,  5,  8, ACTINIDE                , "Protactinium" , "Pa"  , 140, 231.03588   , 1.5   },
  { 92,  6,  8, ACTINIDE                , "Uranium"      , "U"   , 146, 238.02891   , 1.38  },
  { 93,  7,  8, ACTINIDE                , "Neptunium"    , "Np"  , 144, 237         , 1.36  },
  { 94,  8,  8, ACTINIDE                , "Plutonium"    , "Pu"  , 150, 244         , 1.28  },
  { 95,  9,  8, ACTINIDE                , "Americium"    , "Am"  , 148, 243         , 1.13  },
  { 96, 10,  8, ACTINIDE                , "Curium"       , "Cm"  , 151, 247         , 1.28  },
  { 97, 11,  8, ACTINIDE                , "Berkellum"    , "Bk"  , 150, 247         , 1.3   },
  { 98, 12,  8, ACTINIDE                , "Californium"  , "Cf"  , 153, 251         , 1.3   },
  { 99, 13,  8, ACTINIDE                , "Einsteinium"  , "Es"  , 153, 252         , 1.3   },
  {100, 14,  8, ACTINIDE                , "Fermium"      , "Fm"  , 157, 257         , 1.3   },
  {101, 15,  8, ACTINIDE                , "Mendelevium"  , "Md"  , 157, 258         , 1.3   },
  {102, 16,  8, ACTINIDE                , "Nobelium"     , "No"  , 157, 259         , 1.3   },
  {103, 17,  8, ACTINIDE                , "Lawrencium"   , "Lr"  , 159, 262         , 1.3   },
  {104,  3,  6, TRANSITION_METAL        , "Rutherfordium", "Rf"  , 157, 261         , -1    },
  {105,  4,  6, TRANSITION_METAL        , "Dubnium"      , "Db"  , 157, 262         , -1    },
  {106,  5,  6, TRANSITION_METAL        , "Seaborgium"   , "Sg"  , 157, 263         , -1    },
  {107,  6,  6, TRANSITION_METAL        , "Bohrium"      , "Bh"  , 157, 264         , -1    },
  {108,  7,  6, TRANSITION_METAL        , "Hassium"      , "Hs"  , 157, 265         , -1    },
  {109,  8,  6, UNKNOWN                 , "Meitnerium"   , "Mt"  , 159, 268         , -1    },
  {110,  9,  6, UNKNOWN                 , "Damstadtium"  , "Ds"  , 171, 281         , -1    },
  {111, 10,  6, UNKNOWN                 , "Roentgenium"  , "Rg"  , 162, 273         , -1    },
  {112, 11,  6, POST_TRANSITION_METAL   , "Coppernicium" , "Cn"  , 165, 277         , -1    },
  {113, 12,  6, UNKNOWN                 , "Nihonium"     , "Nh"  , 170, 283         , -1    },
  {114, 13,  6, UNKNOWN                 , "Flerovium"    , "Fl"  , 171, 285         , -1    },
  {115, 14,  6, UNKNOWN                 , "Moscovium"    , "Mv"  , 172, 287         , -1    },
  {116, 15,  6, UNKNOWN                 , "Livermorium"  , "Lv"  , 173, 289         , -1    },
  {117, 16,  6, UNKNOWN                 , "Tennessine"   , "Ts"  , 177, 294         , -1    },
  {118, 17,  6, NOBLE_GAS               , "Oganesson"    , "Og"  , 175, 293         , -1    },
};

void draw_rectangle(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color) {
  extapp_pushRectUniform(x, y + TOOLBAR_HEIGHT_PX, w, h, color);
}

void stroke_rectangle(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color) {
  extapp_pushRectUniform(x, y + TOOLBAR_HEIGHT_PX, w, 1, color);
  extapp_pushRectUniform(x, y + h - 1 + TOOLBAR_HEIGHT_PX, w, 1, color);
  extapp_pushRectUniform(x, y + TOOLBAR_HEIGHT_PX, 1, h, color);
  extapp_pushRectUniform(x + w - 1, y + TOOLBAR_HEIGHT_PX, 1, h, color);
}

void draw_string(int16_t x, int16_t y, const char *text) {
  extapp_drawTextLarge(text, x, y + TOOLBAR_HEIGHT_PX, _BLACK, _WHITE, false);
}

void draw_string_small(int16_t x, int16_t y, uint16_t fg, uint16_t bg, const char *text) {
  extapp_drawTextSmall(text, x, y + TOOLBAR_HEIGHT_PX, fg, bg, false);
}

int rgb24to16(int c) {
  int r=(c>>16)&0xff,g=(c>>8)&0xff,b=c&0xff;
  return (((r*32)/256)<<11) | (((g*64)/256)<<5) | (b*32/256);
}

void drawAtom(uint8_t id) {
  int fill = rgb24to16(0xeeeeee);

  switch(atomsdefs[id].type) {
    case ALKALI_METAL:
      fill = rgb24to16(0xffaa00);
      break;
    case ALKALI_EARTH_METAL:
      fill = rgb24to16(0xf6f200);
      break;
    case LANTHANIDE:
      fill = rgb24to16(0xffaa8b);
      break;
    case ACTINIDE:
      fill = rgb24to16(0xdeaacd);
      break;
    case TRANSITION_METAL:
      fill = rgb24to16(0xde999c);
      break;
    case POST_TRANSITION_METAL:
      fill = rgb24to16(0x9cbaac);
      break;
    case METALLOID:
      fill = rgb24to16(0x52ce8b);
      break;
    case REACTIVE_NONMETAL:
      fill = rgb24to16(0x00ee00);
      break;
    case NOBLE_GAS:
      fill = rgb24to16(0x8baaff);
      break;
    case HALOGEN:
      fill = rgb24to16(0x00debd);
      break;
    default:
      break;
  }

  if (atomsdefs[id].y >= 7) {
    draw_rectangle(6 + atomsdefs[id].x * 17, 15 + atomsdefs[id].y * 17, 18, 18, fill);
    stroke_rectangle(6 + atomsdefs[id].x * 17, 15 + atomsdefs[id].y * 17, 18, 18, rgb24to16(0x525552));
    draw_string_small(8 + atomsdefs[id].x * 17, 17 + atomsdefs[id].y * 17, _BLACK, fill, atomsdefs[id].symbol);
  } else {
    draw_rectangle(6 + atomsdefs[id].x * 17, 6 + atomsdefs[id].y * 17, 18, 18, fill);
    stroke_rectangle(6 + atomsdefs[id].x * 17, 6 + atomsdefs[id].y * 17, 18, 18, rgb24to16(0x525552));
    draw_string_small(8 + atomsdefs[id].x * 17, 8 + atomsdefs[id].y * 17, _BLACK, fill, atomsdefs[id].symbol);
  }
}

void copy(const char * text) {
  extapp_clipboardStore(text);
  draw_string(130, 100, "Copie !");
  extapp_msleep(500);
}

void extapp_main(int argc, char * argv[]) {
  char buf[128];
  bool partial_draw = false, redraw = true;
  int cursor_pos = 0;
  const int ATOM_NUMS = sizeof(atomsdefs) / sizeof(struct AtomDef);
  for (;;) {
    if (redraw) {
      if (partial_draw) {
        for(int i = 0; i < ATOM_NUMS; i++) {
          drawAtom(i);
        }
        partial_draw = false;
        draw_rectangle(41, 0, 169, 57, _WHITE);
        draw_rectangle(0, 180, LCD_WIDTH_PX, 14, _WHITE);
      } else {
        draw_rectangle(0, 0, LCD_WIDTH_PX, LCD_HEIGHT_PX, _WHITE);
        for(int i = 0; i < ATOM_NUMS; i++) {
          drawAtom(i);
        }
      }
      draw_string_small(0, 198, _BLACK, _WHITE, "Copier dans le presse papier");
      draw_string_small(0, 210, _BLACK, _WHITE, "OK: tout, P:protons, N:nucleons, M:mass, E:khi");
      if (atomsdefs[cursor_pos].y >= 7) {
        stroke_rectangle(6 + atomsdefs[cursor_pos].x * 17, 15 + atomsdefs[cursor_pos].y * 17, 18, 18, 0x000000);
        stroke_rectangle(7 + atomsdefs[cursor_pos].x * 17, 16 + atomsdefs[cursor_pos].y * 17, 16, 16, 0x000000);
      } else {
        stroke_rectangle(6 + atomsdefs[cursor_pos].x * 17, 6 + atomsdefs[cursor_pos].y * 17, 18, 18, 0x000000);
        stroke_rectangle(7 + atomsdefs[cursor_pos].x * 17, 7 + atomsdefs[cursor_pos].y * 17, 16, 16, 0x000000);
      }

      draw_rectangle(48,  99, 2, 61, rgb24to16(0x525552));
      draw_rectangle(48, 141, 9,  2, rgb24to16(0x525552));
      draw_rectangle(48, 158, 9,  2, rgb24to16(0x525552));

      draw_string(73, 23, atomsdefs[cursor_pos].symbol);
      draw_string_small(110, 27, _BLACK, _WHITE, atomsdefs[cursor_pos].name);
      sprintf(buf, "%d", atomsdefs[cursor_pos].neutrons + atomsdefs[cursor_pos].num);
      draw_string_small(50, 18, _BLACK, _WHITE, buf);
      sprintf(buf, "%d", atomsdefs[cursor_pos].num);
      draw_string_small(50, 31, _BLACK, _WHITE, buf);
      sprintf(buf, "M : %f", atomsdefs[cursor_pos].mass);
      draw_string_small(0, 180, _BLACK, _WHITE, buf);
      sprintf(buf, "khi : %f", atomsdefs[cursor_pos].electroneg);
      draw_string_small(160, 180, _BLACK, _WHITE, buf);
    }
    redraw = false;
    int key = extapp_getKey(true, NULL);
    if(key == KEY_CTRL_EXIT || key == KEY_CTRL_MENU) {
      return;
    } else if (key == KEY_CTRL_LEFT && cursor_pos > 0) {
      cursor_pos--;
      redraw = partial_draw = true;
    } else if (key == KEY_CTRL_RIGHT && cursor_pos < ATOM_NUMS - 1) {
      cursor_pos++;
      redraw = partial_draw = true;
    } else if (key == KEY_CTRL_UP) {
      uint8_t curr_x = atomsdefs[cursor_pos].x;
      uint8_t curr_y = atomsdefs[cursor_pos].y;
      bool updated = false;

      if (curr_y > 0 && curr_y <= 9) {
        for(uint8_t i = 0; i < ATOM_NUMS; i++) {
          if (atomsdefs[i].x == curr_x && atomsdefs[i].y == curr_y - 1) {
            cursor_pos = i;
            redraw = partial_draw = true;
          }
        }
      }
    } else if (key == KEY_CTRL_DOWN) {
      uint8_t curr_x = atomsdefs[cursor_pos].x;
      uint8_t curr_y = atomsdefs[cursor_pos].y;
      bool updated = false;

      if (curr_y >= 0 && curr_y < 9) {
        for (uint8_t i = 0; i < ATOM_NUMS; i++) {
          if (atomsdefs[i].x == curr_x && atomsdefs[i].y == curr_y + 1) {
            cursor_pos = i;
            redraw = partial_draw = true;
            break;
          }
        }
      }
    } else if (key == KEY_CTRL_OK || key == KEY_CTRL_EXE) {
      sprintf(buf, "%s,%d,%d,%f,%f", atomsdefs[cursor_pos].name, atomsdefs[cursor_pos].num, atomsdefs[cursor_pos].neutrons + atomsdefs[cursor_pos].num, atomsdefs[cursor_pos].mass, atomsdefs[cursor_pos].electroneg);
      copy(buf);
      redraw = partial_draw = true;
    } else if (key == '(') {
      sprintf(buf, "%d", atomsdefs[cursor_pos].num);
      copy(buf);
      redraw = partial_draw = true;
    } else if (key == '8') {
      sprintf(buf, "%d", atomsdefs[cursor_pos].neutrons + atomsdefs[cursor_pos].num);
      copy(buf);
      redraw = partial_draw = true;
    } else if (key == '7') {
      sprintf(buf, "%f", atomsdefs[cursor_pos].mass);
      copy(buf);
      redraw = partial_draw = true;
    } else if (key == ',') {
      sprintf(buf, "%f", atomsdefs[cursor_pos].electroneg);
      copy(buf);
      redraw = partial_draw = true;
    } else if (key == KEY_PRGM_ACON) {
      redraw = partial_draw = true;
    }
  }
}
