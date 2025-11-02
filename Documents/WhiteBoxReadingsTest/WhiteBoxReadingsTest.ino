
#include <Ezo_i2c.h>                        //include the EZO I2C library from https://github.com/Atlas-Scientific/Ezo_I2c_lib
#include <Wire.h>  

// define PWM Pins for dosing pumps
#define in2 11
#define in1 12
#define enA 13

#define in4 9
#define in3 10
#define enB 8


Ezo_board ph = Ezo_board(99, "PH");         //create a PH circuit object, who's address is 99 and name is "PH"
Ezo_board rtd = Ezo_board(102, "TEMP");     //create an RTD Temperature circuit object who's address is 102 and name is "TEMP"
Ezo_board cond = Ezo_board(100, "PH");
Ezo_board PMP_UP = Ezo_board(103, "PMP_UP");
Ezo_board PMP_UP_f = Ezo_board(104, "PMP_UP_f");
Ezo_board hum = Ezo_board(111, "HUM");

// defined info for taking Conductivity mesurements

char cond_data[32];          //we make a 32-byte character array to hold incoming data from the EC sensor.
char *cond_str;                     //char pointer used in string parsing.
char *TDS;                       //char pointer used in string parsing.
char *SAL;                       //char pointer used in string parsing (the sensor outputs some text that we don't need).
char *SG;                       //char pointer used in string parsing.

char hum_data[32];
char *hum_str;
char *TE;

// floats for sensor measurements
float EC_float;               //float var used to hold the float value of the conductivity.
float TDS_float;                 //float var used to hold the float value of the total dissolved solids.
float SAL_float;                 //float var used to hold the float value of the salinity.
float SG_float;                 //float var used to hold the float value of the specific gravity.
float HUM_float;                 
float TE_float;
float phLevel;
float waterTempLevel;
float eTempLevel;
float humLevel;
float salLevel;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600, SERIAL_8N1);                         // Set the hardware serial port.
  Wire.begin();

  pinMode(enA,OUTPUT);
  pinMode(enB,OUTPUT);
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);
  pinMode(in3,OUTPUT);
  pinMode(in4,OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
    Serial.println("Start");
  //read sensors
    readWaterTemp();
    Serial.println("Temp Done"); 
    readConductivity();
    readPH();
    readHumidity();
    Serial.println("Read"); 

    Serial.print("r");
    Serial.print(phLevel);
    Serial.print(",");
    Serial.print(EC_float);
    Serial.print(",");
    Serial.print(TDS_float);
    Serial.print(",");
    Serial.print(salLevel);
    Serial.print(",");
    Serial.print(SG_float);
    Serial.print(",");
    Serial.print(waterTempLevel);
    Serial.print(",");
    Serial.print(eTempLevel);
    Serial.print(",");
    Serial.println(humLevel);


    //H-Bridge peristaltic dosing
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    analogWrite(enB, 255);

    delay(10000); // change delay for amount dosed

    //Stop Dosing
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    analogWrite(enB, 0);

    //H-Bridge peristaltic dosing
    digitalWrite(in2, LOW);
    digitalWrite(in1, HIGH);
    analogWrite(enA, 255);

    delay(10000); // change delay for amount dosed

    //Stop Dosing
    digitalWrite(in2, LOW);
    digitalWrite(in1, LOW);
    analogWrite(enA, 0);

  

}


void readWaterTemp() {

  rtd.send_read_cmd();
  delay(1500);
  rtd.receive_read_cmd();
  waterTempLevel =  rtd.get_last_received_reading();
}

void readConductivity() {

  //cond.send_cmd("r"); // old

  cond.send_read_with_temp_comp(waterTempLevel);

  delay(1500);

  cond.receive_cmd(cond_data, 32);       //put the response into the buffer

  cond_str = strtok(cond_data, ",");       //let's parse the string at each comma.
  TDS = strtok(NULL, ",");                  //let's parse the string at each comma.
  SAL = strtok(NULL, ",");                  //let's parse the string at each comma 
  SG = strtok(NULL, ",");

  EC_float = atof(cond_str);
  salLevel = atof(SAL);
  SG_float = atof(SG);
  TDS_float = atof(TDS);
  
  delay(1500);
}


// Take humidity reading
void readHumidity() {

  hum.send_cmd("r");

  delay(1500);

  hum.receive_cmd(hum_data, 32);       

  hum_str = strtok(hum_data, ",");       
  TE = strtok(NULL, ",");

  // HUM_float = toFloat(hum_str);
  // TE_float = toFloat(TE);

  humLevel = atof(hum_str);
  eTempLevel = atof(TE);

  // humLevel = HUM_float;
  // eTempLevel = TE_float;
}


//Take PH reading
void readPH() {

  //ph.send_read_cmd(); // old
  ph.send_read_with_temp_comp(waterTempLevel);

  delay(1500);
  ph.receive_read_cmd();
  phLevel =  ph.get_last_received_reading();
}
