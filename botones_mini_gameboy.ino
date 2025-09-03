int boton1= 2;
int boton2 = 3;
int boton3 = 4;
bool bandera1 = false;
bool bandera2 = false;
bool bandera3 = false;

 void setup() {
  pinMode(boton1,INPUT_PULLUP);
   Serial.begin(9600);
  pinMode(boton2,INPUT_PULLUP);
   Serial.begin(9600);
  pinMode(boton3,INPUT_PULLUP);
   Serial.begin(9600);
}

void loop() {
  int btn1 = digitalRead(boton1);
  //Serial.println(digitalRead(boton1));
  delay(10);
  int btn2 = digitalRead(boton2);
  //Serial.println(digitalRead(boton1));
  delay(10);
  int btn3 = digitalRead(boton3);
  //Serial.println(digitalRead(boton1));
  delay(10);
  

  if (bandera1 && btn1 == 0) {
    Serial.println("Hola profe Rodney (1)");
  }

    
  if (bandera2 && btn2 == 0) {
    Serial.println("Hola profe Rodney (2)");
  }

    
  if (bandera3 && btn3 == 0 ) {
    Serial.println("Hola profe Rodney (3)");
  }
  bandera1 = btn1;
  bandera2 = btn2;
  bandera3 = btn3;


}


