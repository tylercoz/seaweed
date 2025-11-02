#include <Ezo_i2c.h>                        //include the EZO I2C library from https://github.com/Atlas-Scientific/Ezo_I2c_lib
#include <Wire.h>  

#define in2 11
#define in1 12
#define enA 13

#define in4 9
#define in3 10
#define enB 8

float humSetPoint;
float phSetPoint;
float salSetPoint;
float eTempSetPoint;
float wTempSetPoint;
int hour = 4;
int mine = 35;
int light_on_hour = 5;
int light_on_min = 5;
int light_off_hour = 10;
int light_off_min = 20;
int nut_dos_hour = 7;
int nut_dos_min = 15;
int filt_on_hour = 15;
int filt_on_min = 15;

int lightPin = 23;

int phDosHour = 0;
int phDosMin = 0;

int salDosHour = 0;
int salDosMin = 0;

//Define EZO devices

Ezo_board ph = Ezo_board(99, "PH");         //create a PH circuit object, who's address is 99 and name is "PH"
Ezo_board rtd = Ezo_board(102, "TEMP");     //create an RTD Temperature circuit object who's address is 102 and name is "TEMP"
Ezo_board cond = Ezo_board(100, "PH");
Ezo_board hum = Ezo_board(111, "HUM");

// define data storage for sensor readings
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

int dayCount = 0;


void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600, SERIAL_8N1);
  Wire.begin();

  pinMode(enA,OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enB,OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  // analogWrite(enA,0);

  


}

void loop() {
  // put your main code here, to run repeatedly:

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWrite(13,0);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  analogWrite(8,0);

  String message = get_message();

  if ( message.charAt(0) == 's') { // Check if message is not empty and starts with 's'

    message = message.substring(1);
    
    char charArray[message.length() + 1];
    message.toCharArray(charArray, sizeof(charArray));

    String time = strtok(charArray, ",");
    String phs =  strtok(NULL, ",");
    String sals =  strtok(NULL, ",");
    String wtemps =  strtok(NULL, ",");
    String etemps =  strtok(NULL, ",");
    String hums =  strtok(NULL, ",");
    String nut_on =  strtok(NULL, ",");
    String filt_on =  strtok(NULL, ",");
    String light_on =  strtok(NULL, ",");
    String light_off =  strtok(NULL, ",");

    // extract time
    int hour_now = time.substring(0,2).toInt();
    int min_now = time.substring(3,6).toInt();

    // if new hour is lower then last hour then new day increment day counter
    if (hour_now < hour) {

      dayCount = dayCount + 1;

    }

    // Save time
    hour = hour_now;
    mine = min_now;
    

    int light_hour = light_on.substring(0,2).toInt();
    int light_min = light_on.substring(3).toInt();
    int light_hourO = light_off.substring(0,2).toInt();
    int light_minO = light_off.substring(3).toInt();
    int nut_hour = nut_on.substring(0,2).toInt();
    int nut_min = nut_on.substring(3).toInt();;
    int filt_hour = filt_on.substring(0,2).toInt();
    int filt_min = filt_on.substring(3).toInt();

    light_on_hour = light_hour;
    light_on_min = light_min;
    light_off_hour = light_hourO;
    light_off_min = light_minO;
    nut_dos_hour = nut_hour;
    nut_dos_min = nut_min;
    filt_on_hour = filt_hour;
    filt_on_min = filt_min;

    float wtempf = wtemps.toFloat();
    float etempf = etemps.toFloat();
    float phf = phs.toFloat();
    float salf = sals.toFloat();
    float humf = hums.toFloat();

    wTempSetPoint = wtempf;
    eTempSetPoint = etempf;
    phSetPoint = phf;
    salSetPoint = salf;
    humSetPoint = humf;

    // Serial.println(wTempSetPoint); // 50
    // Serial.println(eTempSetPoint); 
    // Serial.println(phSetPoint);
    // Serial.println(salSetPoint);
    // Serial.println(humSetPoint);
    // Serial.println(hour);
    // Serial.println(min);

        

  }

  // 2. Read Sensors -----------------------------------------------------------

    readWaterTemp();
    readConductivity();
    readPH();
    readHumidity();

  // 3. Send Data to Computer
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

    if (phLevel > (phSetPoint - 0.1)) { //((phLevel > phSetPoint - 0.1) && ((phDosMin - min) >= 10)) 

      phDosHour = hour;
      phDosMin = mine;


       digitalWrite(in1, HIGH);
       digitalWrite(in2, LOW);
       analogWrite(13,255);

       delay(30000);

       digitalWrite(in1, LOW);
       digitalWrite(in2, LOW);
       analogWrite(13,0); 
      
    }


    if (salLevel > (salSetPoint - 0.1)) { //((salLevel > salSetPoint - 0.1) && ((salDosMin - min) >= 10))

        salDosHour = hour;
        salDosMin = mine;

       digitalWrite(in3, HIGH);
       digitalWrite(in4, LOW);
       analogWrite(8,255);

       delay(30000);

       digitalWrite(in3, LOW);
       digitalWrite(in4, LOW);
       analogWrite(8,0); 
      
    }

  delay(15000);
}


String get_message() {
  if (Serial.available() > 0) {
    char firstChar = Serial.peek(); // Peek at the first character in the buffer

    if (firstChar == 's'|| firstChar == 't') { // Check if the first character is 's'
      //char incomingChar = Serial.read(); // Consume the first character

      String message = Serial.readStringUntil('\n'); // Read until newline
      // Serial.println("message Recieved");

      return message;

    }  else {
      // Skip to the next message by reading until the end of the line

      // Serial.println("Invalid message");

      while (Serial.available() > 0 && Serial.peek() != '\n') {
        Serial.read(); // Discard the character
      }

      // Consume the newline character
      if (Serial.peek() == '\n') {
        Serial.read(); // Discard the newline character
      }
    }
  }
  return ""; // Return an empty string if no valid message is received
}

void readWaterTemp() {

  rtd.send_read_cmd();
  delay(1500);
  rtd.receive_read_cmd();
  waterTempLevel =  rtd.get_last_received_reading();
}

void readConductivity() {

  cond.send_cmd("r");

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

  ph.send_read_cmd();
  delay(1500);
  ph.receive_read_cmd();
  phLevel =  ph.get_last_received_reading();
}
