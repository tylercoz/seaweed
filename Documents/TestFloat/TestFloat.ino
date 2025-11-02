

#define float1 A0
#define float2 A1
#define pump 37
//int pump = 37; // Relay 1 37
int circulationPin = 23; // Relay 8
int HVACPin = 25; // Relay 7
int filterPin = 27; // Relay 6
int light1Pin = 29; // Relay 5
int light2Pin = 31; // Relay 4
int fan1Pin = 33; // Relay 3
int fan2Pin = 35; // Relay 2


int saltFloat;
int freshFloat;


void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600);
  pinMode(float1, INPUT_PULLUP);
  pinMode(float2, INPUT_PULLUP);
  pinMode(37,OUTPUT);
  pinMode(23,OUTPUT);
  pinMode(25,OUTPUT);
  pinMode(27,OUTPUT);
  pinMode(31,OUTPUT);
  pinMode(33,OUTPUT);
  pinMode(35,OUTPUT);
  
  digitalWrite(37,LOW);
  digitalWrite(35,LOW);
  digitalWrite(33,LOW);
  digitalWrite(31,LOW);
  digitalWrite(29,LOW);
  digitalWrite(27,LOW);
  digitalWrite(25,LOW);
  digitalWrite(23,LOW);
  digitalWrite(7, LOW);


}

void loop() {
  // put your main code here, to run repeatedly:

  delay(30000);

  
  int floatState1 = digitalRead(float1);
  int floatState2 = digitalRead(float2);

  Serial.print("Float1: ");
  Serial.print(floatState1);
  Serial.print(" Float2: ");
  Serial.println(floatState2);

  if (floatState1 ) {

      digitalWrite(37,HIGH);
      digitalWrite(35,HIGH);
      //digitalWrite(33,HIGH);
      //digitalWrite(31,HIGH);
      digitalWrite(7,HIGH);
      //digitalWrite(27,HIGH);
      //digitalWrite(25,HIGH);
      //digitalWrite(23,HIGH);

    
  }
  else {
    
    
  digitalWrite(37,LOW);
  digitalWrite(35,LOW);
  digitalWrite(29,LOW);
//  digitalWrite(33,LOW);
//  digitalWrite(31,LOW);
//  digitalWrite(29,LOW);
//  digitalWrite(27,LOW);
//  digitalWrite(25,LOW);
//  digitalWrite(23,LOW);
    
    
    
    
    }

  
  delay(4000);

}
