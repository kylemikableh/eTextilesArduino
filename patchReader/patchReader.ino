/*

2 pins control enable the east and west side of the board, 
and the following 4 pins enable one of 4 chips in each quadrant. 
using E/W_SEL and SEL, you can control which of 8 lines you can 
get data from. I can explain in more depth later, but this will be 
how each tile is read.
*/
#define D6 6
#define D7 7
#define D13 13 //invert: low = on
#define D12 12
#define D11 11//invert
#define D10 10//invert
#define D9 9//invert

int pins [8];
void setup() {
  
  pinMode(D6, INPUT);  //east
  pinMode(D7, INPUT);  //west
  pinMode(A2, INPUT);  //tile 1
  pinMode(A3, INPUT);  //tile 2
  pinMode(A4, INPUT);  //tile 3
  pinMode(A1, INPUT);  //tile 4

  //pins to control  for reading
  pinMode(D13, OUTPUT);  //sel
  pinMode(D11, OUTPUT);  //e/w_sel
  pinMode(D10, OUTPUT);  //top sel
  pinMode(D9, OUTPUT);   //bot sel

  pinMode(D12, INPUT);  //read input data from this pin (a or b input)
}
void loop() {
  tile_sel(1, 1);  //east, bottom right
//  Serial.println(pins[0]);
//  Serial.println(pins[1]);
//  Serial.println(pins[2]);
//  Serial.println(pins[3]);
//  Serial.println(pins[4]);
//  Serial.println(pins[5]);
//  Serial.println(pins[6]);
//  Serial.println(pins[7]);
    Serial.println(digitalRead(A2));
  Serial.println("===");
  delay(1000);
}

void tile_sel(int is_east, int cardinal) {
  if (is_east) {
    pins[0] = 1;             //east?
    pins[1] = 0;             //west?
    digitalWrite(D7, LOW);   //set west to low
    digitalWrite(D6, HIGH);  //set west to low
  } else {
    pins[0] = 0;             //east?
    pins[1] = 1;             //west?
    digitalWrite(D7, HIGH);  //set west to low
    digitalWrite(D6, HIGH);  //set west to low
  }

  switch (cardinal) {
    case 1:  //top left
      pins[2] = 1;
      pins[3] = 0;
      pins[4] = 0;
      pins[5] = 0;
      digitalWrite(D10, HIGH);   //top and bottom sel
      digitalWrite(D9, LOW);
      break;

    case 2:  //top right
      pins[2] = 0;
      pins[3] = 1;
      pins[4] = 0;
      pins[5] = 0;
      digitalWrite(D10, !HIGH);   //top and bottom sel
      digitalWrite(D9, !LOW);
      break;

    case 3:  //bottom left
      pins[2] = 0;
      pins[3] = 0;
      pins[4] = 1;
      pins[5] = 0;
      digitalWrite(D10, !LOW);   //top and bottom sel
      digitalWrite(D9, !HIGH);
      break;

    case 4:  //bottom right
      pins[2] = 0;
      pins[3] = 0;
      pins[4] = 0;
      pins[5] = 1;
      digitalWrite(D10, !LOW);   //top and bottom sel
      digitalWrite(D9, !HIGH);
      break;
    default: Serial.println("invalid input!");
  }
  digitalWrite(A2, pins[2]);
  digitalWrite(A3, pins[3]);
  digitalWrite(A4, pins[4]);
  digitalWrite(A5, pins[5]);

  digitalWrite(D13, (cardinal % 2 == 0));
}
