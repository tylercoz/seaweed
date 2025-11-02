
#define in2 11
#define in1 12
#define enA 13

#define in4 9
#define in3 10
#define enB 8


int i = 0;
int j = 0;




void setup() {
  // put your setup code here, to run once:

  
  pinMode(enA,OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enB,OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:

  if (j < 5)  {

      
       digitalWrite(in1, HIGH);
       digitalWrite(in2, LOW);
       analogWrite(13,255);

       delay(30000);

       digitalWrite(in1, LOW);
       digitalWrite(in2, LOW);
       analogWrite(13,0); 
       
       j = j + 1;
      
    }


    if (i < 5)  {

      
       digitalWrite(in3, HIGH);
       digitalWrite(in4, LOW);
       analogWrite(8,255);

       delay(30000);

       digitalWrite(in3, LOW);
       digitalWrite(in4, LOW);
       analogWrite(8,0); 

       i = i + 1;
      
    }



  

}
