#include <Ezo_i2c.h>                        //include the EZO I2C library from https://github.com/Atlas-Scientific/Ezo_I2c_lib
#include <Wire.h>                          // enable I2C.
#include <DHT.h>


//Define pins---------------------------------------------------------
//Relay Pins
int circulationPin = 23; // Relay 8
int HVACPin = 25; // Relay 7
int filterPin = 27; // Relay 6
int light1Pin = 29; // Relay 5
int light2Pin = 31; // Relay 4
int fan1Pin = 33; // Relay 3
int fan2Pin = 35; // Relay 2
int spare = 37; // Relay 1

// Float and Flood Pins
#define float1 A0
#define float2 A1
#define flood1 A3
#define flood2 A4

//Internal temp and humidity pin
#define boxTempHum A2

// define PWM Pins for dosing pumps
#define in2 11
#define in1 12
#define enA 13

#define in4 9
#define in3 10
#define enB 8


//Time
int hour = 7;
int min = 11;
int dayCount = 0;
int readingsInterval = 2;

// Last Sensor Reading time
int LastReadingHour = 0;
int LastReadingMin = 0;
int LastReadingDayCount = 0;

// Last Sal Dos Time
int LastSalHour = 0;
int LastSalMin = 0;
int LastSalDayCount = 0;

// Last PH Dos Time
int LastPhHour = 0;
int LastPhMin = 0;
int LastPhDayCount = 0;

// Last Nutrients Dos Day
int LastNutDosDay;

// Last Hvac Dos Time
int LastFanHour = 0;
int LastFanMin = 0;
int LastFanDayCount = 0;

// Defined element for inputing schedule
int light_on_hour = 5;
int light_on_min = 5;
int light_off_hour = 10;
int light_off_min = 20;
int nut_dos_hour = 7;
int nut_dos_min = 15;
int filt_on_hour = 15;
int filt_on_min = 15;

//Setpoints
float humSetPoint = 1.00; // good
float phSetPoint = 99.00; // good
float salSetPoint = 99.00; // good
float eTempSetPoint = 99.00; // good
float wTempSetPoint = 99.00; // good
float boxHumSet = 1; // good
float boxTempSet = 90; // good

//Define values for flood floats sensors and box temp and humidity
float humidity;
float tempF;
int saltFloat;
int freshFloat;
int moisture1;
int moisture2;


//Define sensor boards to can communicate

Ezo_board ph = Ezo_board(99, "PH");         //create a PH circuit object, who's address is 99 and name is "PH"
Ezo_board rtd = Ezo_board(102, "TEMP");     //create an RTD Temperature circuit object who's address is 102 and name is "TEMP"
Ezo_board cond = Ezo_board(100, "PH");
Ezo_board PMP_UP = Ezo_board(103, "PMP_UP");
Ezo_board PMP_UP_f = Ezo_board(104, "PMP_UP_f");
Ezo_board hum = Ezo_board(111, "HUM");

DHT dht(boxTempHum, DHT11);


// sensor levels ----------------------------------------------------------------------------------------

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
  Wire.begin();                                    // enable I2C port.
  dht.begin();


  // setup pins
  pinMode(circulationPin,OUTPUT);
  pinMode(HVACPin,OUTPUT);
  pinMode(filterPin,OUTPUT);
  pinMode(light1Pin,OUTPUT);
  pinMode(light2Pin,OUTPUT);
  pinMode(fan1Pin,OUTPUT);
  pinMode(fan2Pin,OUTPUT);
  pinMode(spare,OUTPUT);
  pinMode(enA,OUTPUT);
  pinMode(enB,OUTPUT);
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);
  pinMode(in3,OUTPUT);
  pinMode(in4,OUTPUT);
  pinMode(float1, INPUT_PULLUP);
  pinMode(float2, INPUT_PULLUP);
  pinMode(flood1, INPUT);
  pinMode(flood2, INPUT);

  // Turn on everything (water pump, lights, filter?) -- Only need circulation on will check if rest needs to be on based on time
  

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWrite(13,0);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  analogWrite(8,0);

  digitalWrite(37,LOW);
  digitalWrite(35,LOW);
  digitalWrite(33,LOW);
  digitalWrite(31,LOW);
  digitalWrite(29,LOW);
  digitalWrite(27,LOW);
  digitalWrite(25,LOW);
  digitalWrite(23,LOW);

  circulationOn();

  LastReadingDayCount = dayCount;
  LastReadingMin = min;
  LastReadingHour = hour-1;

}

void loop() {
  // put your main code here, to run repeatedly:

  //1. give a chance for time to update, setpoints to update, schedule to update
  
 // Read time and Setpoints and update them -------------------------------------------------------------------------------------------------------------------------------------------
  
  // read serial data from Raspberry Pi
  String message = get_message();

  if ( message.charAt(0) == 's') { // Check if message is not empty and starts with 's'
    message = message.substring(1);
    Serial.println("Saving Setpoints");

    // Extract data from read string 
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
    min = min_now;


    // Extract setpoints
    int light_hour = light_on.substring(0,2).toInt();
    int light_min = light_on.substring(3).toInt();
    int light_hourO = light_off.substring(0,2).toInt();
    int light_minO = light_off.substring(3).toInt();
    int nut_hour = nut_on.substring(0,2).toInt();
    int nut_min = nut_on.substring(3).toInt();;
    int filt_hour = filt_on.substring(0,2).toInt();
    int filt_min = filt_on.substring(3).toInt();

    // Save On/Off Setpoints
    light_on_hour = light_hour;
    light_on_min = light_min;
    light_off_hour = light_hourO;
    light_off_min = light_minO;
    nut_dos_hour = nut_hour;
    nut_dos_min = nut_min;
    filt_on_hour = filt_hour;
    filt_on_min = filt_min;

    // Convert Sensor setpoints to float
    float wtempf = wtemps.toFloat();
    float etempf = etemps.toFloat();
    float phf = phs.toFloat();
    float salf = sals.toFloat();
    float humf = hums.toFloat();

    // save sensor setpoints
    wTempSetPoint = wtempf;
    eTempSetPoint = etempf;
    phSetPoint = phf;
    salSetPoint = salf;
    humSetPoint = humf;

  }


  // 2. read sensors (sal, hum, etemp, wtemp, ph, water level sensors) -----------------------------------------------------


  int timeDiff = (hour*60 + min) - (LastReadingHour*60 + LastReadingMin);
  

  if ( timeDiff >= readingsInterval || LastReadingDayCount > dayCount) {
    
    // save last time sensors were read
    LastReadingDayCount = dayCount;
    LastReadingMin = min;
    LastReadingHour = hour;


    Serial.print("Time Diff: ");
    Serial.println(timeDiff);
    
//    Serial.print("Day Count: ");
//    Serial.println(dayCount);
//    Serial.print("Hour: ");
//    Serial.println(hour);
//    Serial.print("Min: ");
//    Serial.println(min);
//    Serial.print(" Last Day Count: ");
//    Serial.println(LastReadingDayCount);
//    Serial.print("Last Hour: ");
//    Serial.println(LastReadingHour);
//    Serial.print("Min: ");
//    Serial.println(LastReadingMin);
    

    //read sensors
    readWaterTemp();
    readConductivity();
    readPH();
    readHumidity();
    //readEmergencySensors(); //---------------------------------------------------------need to test
    

    // 3. Print Data -----------------------------------------
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

    //4. Check readings with dosing methods and emergency codes ------------------------------------------ 
    
    if ( (hour*60 + min) - (LastSalHour*60 + LastSalMin) >= (readingsInterval) || LastSalDayCount > dayCount) {
      
      // save last time sensors were read
      LastSalDayCount = dayCount;
      LastSalMin = min;
      LastSalHour = hour;

      salDosing(salLevel); // -- check salinity
    }

    if ( (hour*60 + min) - (LastPhHour*60 + LastPhMin) >= (readingsInterval) || LastPhDayCount > dayCount) {

      LastPhDayCount = dayCount;
      LastPhMin = min;
      LastPhHour = hour;

      phDosing(phLevel);   // -- check PH

    }

    // Dosing Nutrients
    if (dayCount != LastNutDosDay ) {

      if ((hour > nut_dos_hour) || (hour == nut_dos_hour && nut_dos_min >= nut_dos_min)) {

        nutDosing();

        LastNutDosDay = dayCount;
      }

    }

    // Dosing Check Fan/etemp
    if ( (hour*60 + min) - (LastFanHour*60 + LastFanMin) >= (readingsInterval) || LastFanDayCount > dayCount) {

      LastFanDayCount = dayCount;
      LastFanMin = min;
      LastFanHour = hour;

      etempCorrect(eTempLevel); // -- check envoronmental temp

    }


    //Setpoints check with no dosing methods only alarms
    waterTempCorrect(waterTempLevel); // method to check water temp
    humidityCorrect(humLevel); // ---- check humidity level
  
  }
  

  // 5. Check to see if lights need to still be on
  if ((hour > light_on_hour || (hour == light_on_hour && min >= light_on_min)) && 
    (hour < light_off_hour || (hour == light_off_hour && min < light_off_min))) {

    lightOn(light1Pin,fan1Pin);
    lightOn(light2Pin,fan2Pin);
    
    } else { 
        
      lightOff(light1Pin,fan1Pin);
      lightOff(light2Pin,fan2Pin);
      }

  
  // 6. Delay untill next reading
  delay(1000);
  

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

// PH Dosing method
void phDosing( float phLevel){

  if (phLevel > phSetPoint) {

    Serial.print("ephs");
    Serial.print("PH above Setpoint, PH Reading:" );   
    Serial.println(phLevel); 


  }

  if (phLevel > 0.1*phSetPoint + phSetPoint) {

    //insert pump dispersing here from EZO libray
    // PMP_UP.send_cmd_with_num("d,",15); // need to figure out how to volumetrically dispense

    //H-Bridge peristaltic dosing
    digitalWrite(in2, LOW);
    digitalWrite(in1, HIGH);
    analogWrite(enA, 255);

    delay(30000); // change delay for amount dosed

    //Stop Dosing
    digitalWrite(in2, LOW);
    digitalWrite(in1, LOW);
    analogWrite(enA, 0);
  }
}


void salDosing( float salLevel) {

  if (salLevel > salSetPoint) {

    // Print Error Message To Email On Rasp pi 
    Serial.print("esal");
    Serial.print("Salinity above Setpoint, Salinity Reading:" );
    Serial.println(salLevel);


  }


  
  if (salLevel > 0.2*salSetPoint + salSetPoint) {

    
    //H-Bridge peristaltic dosing
    digitalWrite(in4, LOW);
    digitalWrite(in3, HIGH);
    analogWrite(enB, 177);

    delay(10000); // change delay for amount dosed

    //Stop Dosing
    digitalWrite(in4, LOW);
    digitalWrite(in3, LOW);
    analogWrite(enB, 0);
  }
}

void nutDosing() {

  PMP_UP.send_cmd_with_num("d,",15); // or can use volumetric dosing
  delay(5000);
}



void etempCorrect(float eTempLevel) {

  if (eTempLevel > eTempSetPoint) {

    Serial.print("etem");
    Serial.print("Environment Temp above Setpoint, Temp Reading:" );
    Serial.println(eTempLevel);


  }


  // if (eTempLevel > eTempSetPoint - (0.2)*eTempSetPoint) {

  //   // Print Error Message To Email On Rasp pi 
  //   Serial.print("etem");
  //   Serial.print("Environment Temp above Setpoint, Temp Reading:" );
  //   Serial.println(eTempLevel);

  //   //insert Hvac fan code

  //   hvacOn(HVACPin); //---- Put in while loop if we are indeed in temp controlled room
  //   delay(5000); //--------------------------------------------------------------------------------------make more scientifc (poll the temp again after the delay and check that reading is good)
  //   hvacOff(HVACPin);
    
  // }
}

void waterTempCorrect( float waterTempLevel) {

  if (waterTempLevel > (wTempSetPoint - wTempSetPoint*0.1)) {

    Serial.print("ewtp");
    Serial.print("Water Temp above Setpoint, Temp Reading:" );
    Serial.println(waterTempLevel);
  }
}

void humidityCorrect( float humLevel) {

  if (humLevel < (humSetPoint)) {

    Serial.print("ehum");
    Serial.print("Humidity Below Setpoint, Humidity Reading:" );
    Serial.println(humLevel);
  }
}

//Read fload Swtich method with flaot pin and pin to shut off with shutoff call if triggered
void readEmergencySensors() {

  humidity = dht.readHumidity();
  tempF = dht.readTemperature(true);
  saltFloat = digitalRead(float1);
  freshFloat = digitalRead(float2);
  moisture1 = digitalRead(flood1);
  moisture2 = digitalRead(flood2);

  if (saltFloat == 1) {

    Serial.println("efts Emergency Water Shutoff To Much Water In Sump");
  } 
  
  else if (freshFloat == 1) {

    Serial.println("efts Emergency Not Enough Fresh Water");
  }

  else if ((moisture1 == 1) || (moisture2 == 1)) {

    Serial.println("efld Emergency Water Shutoff: Leak Detected");
    circulationOff();
  }

  else if ((humidity >= boxHumSet) || (tempF >= boxTempSet)) {

    Serial.print("ebth Emergency: Box Temp or Humidity Is To High,  Humidity: ");
    Serial.print(humidity);
    Serial.print(", Temp(F)");
    Serial.println(tempF);
  }

}


// Turn on light and fan Relay on and off
void lightOn(int lightPin, int fanPin){

  digitalWrite(lightPin, HIGH);
  digitalWrite(fanPin, HIGH);
}

void lightOff( int lightPin, int fanPin){

  digitalWrite(lightPin, LOW);
  digitalWrite(fanPin, LOW);
}

// Turn on and off filter pump relay
void filterOn(int filterPin){

  digitalWrite(filterPin, HIGH);
}

void filterOff(int filterPin){

  digitalWrite(filterPin, LOW);
}

// turn on and off hvac relay
void hvacOn(int HVACPin){

  digitalWrite(HVACPin, HIGH);
}

void hvacOff(int HVACPin){

  digitalWrite(HVACPin, LOW);
}

// turn on and off circulation pump and control the speed or PWM 0-255 range
void circulationOn() {

  digitalWrite(circulationPin, HIGH);
}

void circulationOff() {

  digitalWrite(circulationPin, LOW);
}

// Method to read setpoints and time 
String get_message() {

  if (Serial.available() > 0) {
    char firstChar = Serial.peek(); // Peek at the first character in the buffer

    if (firstChar == 's'|| firstChar == 't') { // Check if the first character is 's'
      //char incomingChar = Serial.read(); // Consume the first character

      String message = Serial.readStringUntil('\n'); // Read until newline
      Serial.println("Message Recieved");

      return message;

    }  else {
      // Skip to the next message by reading until the end of the line

      Serial.println("Invalid message");

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
