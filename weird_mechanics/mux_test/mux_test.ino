#define data 12
#define enable 11
#define select 13

int dataOut;

void setup() {
  // put your setup code here, to run once:
  pinMode(data, INPUT);
  pinMode(select, OUTPUT);
  pinMode(enable,OUTPUT);
  Serial.begin(9600); 
}

void loop() {
  // put your main code here, to run repeatedly:
  mux_setup(1,1);//on, a
  dataOut = digitalRead(data);
  Serial.println(dataOut);
  delay(500);
}

void mux_setup(bool en, bool ab){
  //enable
  digitalWrite(enable, !en);
  //select a or b
  digitalWrite(select, ab);
}
