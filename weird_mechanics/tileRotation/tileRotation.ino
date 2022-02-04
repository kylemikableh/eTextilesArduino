#include <stdint.h>

uint8_t tile_orientation [2];

int tile_sel_left;
int tile_sel_right;
int tile_sel_hor;
int tile_sel_vir;

int tile_out_pin;

int grid_ne_pin;
int grid_nw_pin;
int grid_se_pin;
int grid_sw_pin;

const unsigned long event_1 = 1000;

bool determine_orientation = false;

void setup() 
{
  //=== ATTACH INTERRUPT TO CHECK TILE ORIENTATION EVERY SO OFTEN
  // generate a 'TIMER0_COMPA' interrupt whenever the counter value passes 0xAF
  OCR0A = 0xFA;
  TIMSK0 |= _BV(OCIE0A);  
}

// Interrupt is called once a millisecond, 
SIGNAL(TIMER0_COMPA_vect) 
{
  determine_orientation = true;
}

int tile_read(int left, int right){
  int i = 0;
  int out;

  //read tile pin
  digitalWrite(tile_sel_left, left); //a
  digitalWrite(tile_sel_right, right); //b
  out = digitalRead(tile_out_pin); //z
  i++;
  return out;
}

int grid_space_read(int quarter){
  //returns teh status of 
  int i = 0;
  int out;
  int grid_pin;
  int hor;
  int vir;

  switch(quarter){
    case 1://NE
      grid_pin = grid_ne_pin;
      hor = 0;
      vir = 0;      
      break;
    case 2://NW
      grid_pin = grid_nw_pin;
      hor = 1;
      vir = 0;
      break;
    case 3://SE
      grid_pin = grid_se_pin;
      hor = 0;
      vir = 1;
      break;
    case 4://SW
      grid_pin = grid_sw_pin;
      hor = 1;
      vir = 1;
      break;        
    }
  
  //read grid pin
  digitalWrite(tile_sel_hor, hor); //a
  digitalWrite(tile_sel_vir, vir); //b
  out = digitalRead(grid_pin); //z
  i++;
  return out;
}

void status(int quadrant, int tile_orientation){
  char buffer[4];
  snprintf(buffer,sizeof(buffer), "quadrant: %d, tile orientation: %d", quadrant, tile_orientation);
  Serial.println(buffer);
}

void loop() 
{
  if(determine_orientation){
        
    for(int quadrant = 0; quadrant < 4; quadrant++){
        int tile_orientation;
        bool unfinished = true;
        for(int tile = 0; tile < 4; tile++){
          if ( millis() > event_1  && unfinished) {
          grid_space_read(quadrant);          
           tile_orientation = tile_read(0,0);
          status( quadrant,  tile_orientation);
           tile_orientation = tile_read(0,1);
          status( quadrant,  tile_orientation);
           tile_orientation = tile_read(1,0);
          status( quadrant,  tile_orientation);
           tile_orientation = tile_read(1,1);
          status( quadrant,  tile_orientation);
          unfinished = false;
          }
        }
        quadrant ++;
      }

    }
      determine_orientation= false;    
  }