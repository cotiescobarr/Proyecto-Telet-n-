int I1 = 21;
int I2 = 19;
int I3 = 18;
int I4 = 5;
int I5 = 17;
int I6 = 16;
int I7 = 4;
int I8 = 0;

void setup() {
  pinMode(I1,OUTPUT);
  pinMode(I2, OUTPUT);
  pinMode(I3, OUTPUT);
  pinMode(I4, OUTPUT);
  pinMode(I5, OUTPUT);
  pinMode(I6, OUTPUT);
  pinMode(I7, OUTPUT);
  pinMode(I8, OUTPUT);
  Serial.begin(9600);
}
  
void forward(int time){
  digitalWrite(I1, HIGH);
  digitalWrite(I2, LOW);
  digitalWrite(I3, HIGH);
  digitalWrite(I4, LOW);

  digitalWrite(I5, HIGH);
  digitalWrite(I6, LOW);
  digitalWrite(I7, HIGH);
  digitalWrite(I8, LOW);
  delay(time);
  Serial.println("foward");

}
  

void backward(int time){
  digitalWrite(I1, LOW);
  digitalWrite(I2, HIGH);
  digitalWrite(I3, LOW);
  digitalWrite(I4, HIGH);

  digitalWrite(I5, LOW);
  digitalWrite(I6, HIGH);
  digitalWrite(I7, LOW);
  digitalWrite(I8, HIGH);
  delay(time);
  Serial.println("backward");

}

void still(int time){
  digitalWrite(I1, HIGH);
  digitalWrite(I2, HIGH);
  digitalWrite(I3, HIGH);
  digitalWrite(I4, HIGH);

  digitalWrite(I5, HIGH);
  digitalWrite(I6, HIGH);
  digitalWrite(I7, HIGH);
  digitalWrite(I8, HIGH);
  delay(time);
  Serial.println("still");
}

void limp(int time){
  digitalWrite(I1, LOW);
  digitalWrite(I2, LOW);
  digitalWrite(I3, LOW);
  digitalWrite(I4, LOW);

  digitalWrite(I5, LOW);
  digitalWrite(I6, LOW);
  digitalWrite(I7, LOW);
  digitalWrite(I8, LOW);
  delay(time);
  Serial.println("limp");
}

  
void rotateright(int time){
  digitalWrite(I1, HIGH);
  digitalWrite(I2, LOW);
  digitalWrite(I3, LOW);
  digitalWrite(I4, HIGH);

  digitalWrite(I5, LOW);
  digitalWrite(I6, HIGH);
  digitalWrite(I7, HIGH);
  digitalWrite(I8, LOW);
  delay(time);
  Serial.println("rotateright"); 
}


void rotateleft(int time){
  digitalWrite(I1, LOW);
  digitalWrite(I2, HIGH);
  digitalWrite(I3, HIGH);
  digitalWrite(I4, LOW);
  
  digitalWrite(I5, HIGH);
  digitalWrite(I6, LOW);
  digitalWrite(I7, LOW);
  digitalWrite(I8, HIGH);
  delay(time);
  Serial.println("rotateleft");
}

void right(int time){
  digitalWrite(I1, HIGH);
  digitalWrite(I2, LOW);
  digitalWrite(I3, LOW);
  digitalWrite(I4, HIGH);

  digitalWrite(I5, HIGH);
  digitalWrite(I6, LOW);
  digitalWrite(I7, LOW);
  digitalWrite(I8, HIGH);
  delay(time);
  Serial.println("right");
}
  
void left(int time){
  digitalWrite(I1, LOW);
  digitalWrite(I2, HIGH);
  digitalWrite(I3, HIGH);
  digitalWrite(I4, LOW);

  digitalWrite(I5, LOW);
  digitalWrite(I6, HIGH);
  digitalWrite(I7, HIGH);
  digitalWrite(I8, LOW);
  delay(time);
  Serial.println("left");
}

void upright(int time){
  delay(time);

}

 

void upleft(int time){
  delay(time);

}
 

void downright(int time){
  delay(time);

}
 

void downleft(int time){
  delay(time);

}
 
void loop() {
  // put your main code here, to run repeatedly:

limp(1000);
forward(1000);
limp(1000);
right(1000);
limp(1000);
backward(1000);
limp(1000);
left(1000);


}

