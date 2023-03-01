
#include <stdlib.h>
#include <string.h>
#include <stdio.h> 
#include "neslib.h"
#include <nes.h>
//#link "chr_generic.s"
#include "bcd.h"
//#link "bcd.c"
#include "vrambuf.h"
//#link "vrambuf.c"

#define COLS 32
#define ROWS 27

byte X,Y; // coordenadas en X y Y
byte parpadeo; // 1 para ocultarlo, 0 para mostrarlo
int tempo = 0;
byte tile_num = 25;  
byte tile_0=0;
char str[32];


//-----
byte revisar_coordenadas()
{
  byte resultado;
  word coordenada = NTADR_A(X,Y);
  // Tenemos que esperar primero a la VBLANK
  ppu_wait_nmi();
  //Establece dirección de vram y lee un byte en "resultados"
  vram_adr(coordenada);
  vram_read(&resultado, 1);
  // scroll registers are corrupt
  // fix by setting vram address
  vram_adr(0x0);
  return resultado;
}
//-----
void limpia_pantalla() {
  vrambuf_clear();
  ppu_off();
  vram_adr(0x2000);
  vram_fill(0, 32*28);
  vram_adr(0x0);
  ppu_on_bg();
}
//-----
void dibuja_puntero(oam_id)
{
  
  tempo++;
    if ((tempo)>2)
    {
      oam_spr(X*28, Y*5, 0x00,0, oam_id);
      if((tempo)>2)
      	tempo=0;
    }
    else
      oam_spr(X*8, Y*8,tile_num,0, oam_id); 
}
//-----
void dibuja_rastro()
{
  byte r;
  r = revisar_coordenadas();
  
  if(r!=0)
    vrambuf_put(NTADR_A(X,Y), &tile_0, 1);
  else
    vrambuf_put(NTADR_A(X,Y), &tile_num, 1);
    
  sprintf(str," %6d", r);
  vrambuf_put(NTADR_A(23,27), str, 32);

  vrambuf_flush();
}
//-----
void captura_botones()
{
  char pad;
  pad = pad_poll(0); 
  
  if(pad & PAD_LEFT && X>1)
      X--;
  if(pad & PAD_RIGHT && X<30)
    X++;
  if(pad & PAD_UP && Y>1)
    Y--;
  if(pad & PAD_DOWN && Y<27)
    Y++;
  if(pad & PAD_START)
    dibuja_rastro();
  if(pad & PAD_A)
    tile_num++;
  if(pad & PAD_B)
    tile_num--;
  
}
//-----

/*{pal:"nes",layout:"nes"}*/
const char PALETTE[32] = { 
  0x03,			// screen color

  0x11,0x30,0x27,0x0,	// background palette 0
  0x1c,0x20,0x2c,0x0,	// background palette 1
  0x00,0x10,0x20,0x0,	// background palette 2
  0x06,0x16,0x26,0x0,   // background palette 3

  0x16,0x35,0x24,0x0,	// sprite palette 0
  0x00,0x37,0x25,0x0,	// sprite palette 1
  0x0d,0x2d,0x3a,0x0,	// sprite palette 2
  0x0d,0x27,0x2a	// sprite palette 3
};

// setup PPU and tables
void setup_graphics() {
  // clear sprites
  oam_clear();
  // set palette colors
  pal_all(PALETTE);
}

void main(void)
{
  char oam_id=0;
  
  X=21;
  Y=9;
  
  setup_graphics();
  vrambuf_clear();
  set_vram_update(updbuf);
  
  vram_adr(NAMETABLE_A);
  vram_fill(0x00, 32*30);
  
  vram_adr(NTADR_A(2,2));
  vram_write("PRESIONA ENTER PARA AGREGAR", 27);
  vram_adr(NTADR_A(2,3));
  vram_write("UN TILE", 7);
  vram_adr(NTADR_A(2,5));
  vram_write("PRESIONA OTRA VEZ PARA", 22);
  vram_adr(NTADR_A(2,6));
  vram_write("ELIMINAR", 8);
  vram_adr(NTADR_A(2,8));
  vram_write("USA LA TECLA ESPACIO O SHIFT", 28);
  vram_adr(NTADR_A(2,9));
  vram_write("PARA CAMBIAR DE TILE", 20);
  vram_adr(NTADR_A(2,27));
  vram_write("EL TILE ERA EL NUMERO", 21);
  
  ppu_on_all();
  //memset(str, 0, sizeof(str));

  while(1) {
    
    dibuja_puntero(oam_id); 
    captura_botones();
    
    delay(8);
  }
}
