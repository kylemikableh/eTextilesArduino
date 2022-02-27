#define data 2

#define eastEnable 3  //(low in non use)
#define topEnable 4   //(low in non use)
#define eastSelect 5  //(default = a)
#define topSelect 6   //(default = a)

#define westEnable 7  //(low in non use)
#define botEnable 8   //(low in non use)

char dataOut[4];

void setup() {
  // put yoursetup code here, to run once:
  pinMode(data, INPUT);
  
  pinMode(eastSelect, OUTPUT);
  pinMode(eastEnable, OUTPUT);
  pinMode(topSelect, OUTPUT);
  pinMode(topEnable, OUTPUT);

  pinMode(westEnable, OUTPUT);
  pinMode(botEnable, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  
  //for (int i = 0; i < 4; i++) { //do this 4 times (for each magnet)

    //EAST TOP
    mux_setup(1, 1, 1, 1);  //CONTROLLS WHAT QUADRANT YOU'RE GETTING DATA FOR
    dataOut[0] = digitalRead(data);//tile 1

     mux_setup(1, 1, 0, 0);
    dataOut[1] = digitalRead(data);//tile 2

    mux_setup(1, 1, 1, 0);
    dataOut[2] = digitalRead(data);//tile 3

    mux_setup(1, 1, 0, 1);
    dataOut[3] = digitalRead(data);//tile 4
    printTile();



    //WEST TOP
    mux_setup(0, 1, 1, 1);  //CONTROLLS WHAT QUADRANT YOU'RE GETTING DATA FOR
    dataOut[0] = digitalRead(data);//tile 1

     mux_setup(0, 1, 0, 0);
    dataOut[1] = digitalRead(data);//tile 2

    mux_setup(0, 1, 1, 0);
    dataOut[2] = digitalRead(data);//tile 3

    mux_setup(0, 1, 0, 1);
    dataOut[3] = digitalRead(data);//tile 4
    printTile();




    //EAST BOTTOM
    mux_setup(1, 0, 1, 1);  //CONTROLLS WHAT QUADRANT YOU'RE GETTING DATA FOR
    dataOut[0] = digitalRead(data);//tile 1

     mux_setup(1, 0, 0, 0);
    dataOut[1] = digitalRead(data);//tile 2

    mux_setup(1, 0, 1, 0);
    dataOut[2] = digitalRead(data);//tile 3

    mux_setup(1, 0, 0, 1);
    dataOut[3] = digitalRead(data);//tile 4
    printTile();





    //WEST BOTTOM
    mux_setup(0, 0, 1, 1);  //CONTROLLS WHAT QUADRANT YOU'RE GETTING DATA FOR
    dataOut[0] = digitalRead(data);//tile 1

     mux_setup(0, 0, 0, 0);
    dataOut[1] = digitalRead(data);//tile 2

    mux_setup(0, 0, 1, 0);
    dataOut[2] = digitalRead(data);//tile 3

    mux_setup(0, 0, 0, 1);
    dataOut[3] = digitalRead(data);//tile 4
    printTile();
  //}

  Serial.println("=====");

  delay(250);
}

void printTile(){
  //PRINT DATA: FOR DEBUG
  char buf[40], *pos = buf;
    for (int i = 0 ; i <= 3 ; i++) {
    if (i) {
      pos += sprintf(pos, ", ");
    }
    pos += sprintf(pos, "%d", dataOut[i]);
  }
  Serial.println(buf);
}

void mux_setup(bool eastEn, bool topEn, bool eastAB, bool topAB) {
  //eastEnable
  digitalWrite(eastEnable, !eastEn);
  digitalWrite(westEnable, eastEn);
  //eastSelect a or b
  digitalWrite(eastSelect, !eastAB);

  //topEnable
  digitalWrite(topEnable, !topEn);
  digitalWrite(botEnable, topEn);
  //eastSelect a or b
  digitalWrite(topSelect, !topAB);
}
