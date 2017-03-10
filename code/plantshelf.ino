// This #include statement was automatically added by the Particle IDE.
#include <OneWire.h>

// This #include statement was automatically added by the Particle IDE.
#include <LiquidCrystal_I2C_Spark.h>
#include "DS18.h"

// This #include statement was automatically added by the Particle IDE.
//#include "lib1.h"
//#include "Serial/Serial.h"


LiquidCrystal_I2C *lcd;
OneWire tempsensor(A4);

/* Function prototypes -------------------------------------------------------*/
int tinkerDigitalRead(String pin);
int tinkerDigitalWrite(String command);
int tinkerAnalogRead(String pin);
int tinkerAnalogWrite(String command);
int ledstatus;

int ScreenNumber=0;
int lastSecond=0;
int lastHour=0;
int iDelay=0;


TCPClient myTCP;
int sentOnce;
int moisture1=0;
int moisture2=0;
int moisture3=0;
int moisture4=0;
int light1State=HIGH;
int light2State=HIGH;
double temperature;
String LowerLight="";
String UpperLight="";




/* This function is called once at start up ----------------------------------*/
/*******************************************************************************
 *******************************************************************************/
void setup()
{
	//Setup the Tinker application here

    Serial.begin(9600);
    
  lcd = new LiquidCrystal_I2C(0x3F, 16, 2);
  lcd->init();
  lcd->backlight();
  lcd->clear();
  //lcd->print("***Spark Time***");
  Time.zone(-7);
  
	//Register all the Tinker functions
	Particle.function("digitalread", tinkerDigitalRead);
	Particle.function("digitalwrite", tinkerDigitalWrite);
	Particle.function("analogread", tinkerAnalogRead);
	Particle.function("analogwrite", tinkerAnalogWrite);

	Particle.publish("plant shelf","online with plant v1 + tinker");

    pinMode(D2,OUTPUT);
    pinMode(D3,OUTPUT);
    pinMode(D4,OUTPUT);
    pinMode(D5,OUTPUT);
    pinMode(D6,OUTPUT);
  
    tempsensor.reset();
    
    
    digitalWrite(D2,HIGH);
    digitalWrite(D3,HIGH);
    light1State=HIGH;
    light2State=HIGH;
    
    readMoisture();
    
    Particle.variable("Moisture1",moisture1);
    Particle.variable("Moisture2",moisture2);
    Particle.variable("Moisture3",moisture3);
    Particle.variable("Moisture4",moisture4);
    Particle.variable("Temperature",temperature);
    Particle.variable("LowerLight",LowerLight);
    Particle.variable("UpperLight",UpperLight);
    
    Particle.function("ReadMoisture",readMoist);
    Particle.function("ReadTemp",readTemperature);
    Particle.function("LightsOn",lightsOn);
    Particle.function("LightsOff",lightsOff);
    
	//Serial.begin(9600);
	Serial.println("done with init...");
}


/*******************************************************************************
 * Function Name  : sendToProwl()
 * Description    : Sends a text notification to Prowl
 * Input          : none
 * Output         : None.
 * Return         : none
                    
 *******************************************************************************/
void sendToProwl()
{  
  //int TimeOfNotification;
  
  Serial.println("about to connect to prowl");
   if (myTCP.connect("api.prowlapp.com", 80))
   {
        //Serial.println(F("Outgoing Notification"));
        myTCP.println("POST /publicapi/add?apikey=8a89ee8b8ad79c9f938a5e119a7a7f4ab1388d1a&application=PlantShelf&event=WaterAlert&priority=1&description=You%20Should%20Water%20Your%20Plants&url=google.com HTTP/1.1\r\nHost: api.prowlapp.com:80\r\n");
        //delay(1000);
        myTCP.stop();
        Particle.publish("Prowl","Sending Prowl Notification");
        Serial.println("Connected and posted https to Prowl");
    } else {
        Particle.publish("Prowl","unable to connect");
        Serial.println("unable to conenct to prowl");
    }
    //TimeOfNotification = (millis()/1000);
    sentOnce = 1;
   
}



// read from the DS1821 and display on the LCD.
void displayTemperature(){
    float temp;
     temp=ReadTemp();
     temperature=temp;
        // Do something cool with the temperature
        //Serial.printf("Temperature %.2f C %.2f F ", sensor.celsius(), sensor.fahrenheit());
       // String df;
       // df=sprintf("%.2f",tempsensor.fahrenheit());
        
        lcd->clear();
        lcd->setCursor(0,0);
        lcd->print("Temperature");
        lcd->setCursor(1,5);
        lcd->print( temp);
        // Additional info useful while debugging
        //printDebugInfo();
     
}

/*******************************************************************************
 * Function Name  : displayMoisture
 * Description    : displays the moisture value of a given pin
 * Input          : string to display, and the pin to read a value from
 * Output         : None.
 * Return         : puts the moisture value on the lcd
                    
 *******************************************************************************/
void displayMoisture(String tag, int value)
{

    int val = value;
    
    lcd->clear();
    lcd->setCursor(0,0);
    lcd->print(tag);
    lcd->setCursor(1,5);
    lcd->print(val);
}



// internet function to read the moisture sensors.
int readMoist(String fred){
    readMoisture();
    return 0;
}

// read from the moisture sensors...
// don't display... just read and store
// moisture doesn't change all the much, so only a periodic read is necessary.
void readMoisture(){
    Particle.publish("plant shelf","read Moisture..");
    lcd->clear();
    lcd->print("Read Moisture");
    // first turn them on
    digitalWrite(D5,HIGH);
    delay(1000);
    moisture1=tinkerAnalogRead("A0");
    moisture2=tinkerAnalogRead("A1");
    moisture3=tinkerAnalogRead("A2");
    moisture4=tinkerAnalogRead("A3");
    delay(1000);
    // turn them off
    digitalWrite(D5,LOW);
    
}



/* This function loops forever --------------------------------------------*/
void loop()
{
  
  if (Time.second() != lastSecond )
  {
    lastSecond = Time.second();
     // lcd->clear();
     // lcd->print(lastSecond);
    //  displayTime();
      iDelay = iDelay + 1;
    processState();
//    readMoisture();
  }
  
  if(Time.hour() != lastHour){
      lastHour=Time.hour();
      readMoisture();
  }
  
  if (iDelay == 2){
      
      if(ScreenNumber == 0) {
          //readMoisture();
          displayTime();
      } else if (ScreenNumber == 1){
          displayTemperature();
      } else if (ScreenNumber == 2){
          displayMoisture("Sensor 1",moisture1);
      } else if (ScreenNumber == 3){
          displayMoisture("Sensor 2",moisture2);
      } else if (ScreenNumber ==4) {
          displayMoisture("Sensor 3",moisture3);
      } else if (ScreenNumber == 5){
          displayMoisture("Sensor 4",moisture4);
      } else if (ScreenNumber == 6){
         // readMoisture();
          displayTime();
      
      }
    ScreenNumber = ScreenNumber + 1;
    iDelay = 0;
  }
  
  if (ScreenNumber == 6){
      ScreenNumber = 0;
  }
}

unsigned int readBytes(int count)
{
    unsigned int val = 0;
    for (int i = 0; i < count; i++)
    {
        val |= (unsigned int)(tempsensor.read() << i * 8);
    }
    return val;
}


// internet function for reading the temperature
int readTemperature(String fred){
    float temp;
    Particle.publish("plant shelf","internet read temperature");
    temp=ReadTemp();
    temperature=temp;
    return (int)temp;
}

float ReadTemp(void) {
  byte temp_read = 0;
  int temperature = 0;
  unsigned int count_remain = 0;
  unsigned int count_per_c = 0;
  byte configuration_register = 0;

  tempsensor.reset();
  tempsensor.write(0xEE); //Start Converting the temperatures  
  
   do {
        delay(1);
        configuration_register = 0;
        tempsensor.reset();
        tempsensor.write(0xAC);

        // Read the configuration Register from the DS1821
        configuration_register = readBytes(1);
    } while ((configuration_register & (1 << 7)) == 0); // If Bit #8 is 1 then we are finished converting the temp

    // Get Temp
    tempsensor.reset();
    tempsensor.write(0xAA);
    temp_read = readBytes(1); ;

    // Get Count Remaining
    tempsensor.reset();
    tempsensor.write(0xA0);
    count_remain = readBytes(2);
    // Load The Counter to populate the slope accumulator
    tempsensor.reset();
    tempsensor.write(0x41);
    // Read Count Per Deg
    tempsensor.reset();
    tempsensor.write(0xA0);
    count_per_c = readBytes(2);

    
    
    //jcw convert 8 bit temperature to an int..
    temperature = temp_read;
    // > 128 means the sign bit is set in the 8 bit temp register
    if (temperature >= 128) temperature = temperature - 256;

    //jcw original code was this.  but i don't see how dropping 256 from a byte makes any sense at all
    // so i'll use the above to convert it to an int, then drop 256-- which could make sense.   
    // see the ds1821 data sheet at https://datasheets.maximintegrated.com/en/ds/DS1821.pdf
    
    // If we are reading above the 200 mark then we are below 0 and need to compensate the calculation
    //if (temp_read >= 200) temp_read -= 256;


    // Convert to F from C.
    float highResTemp = (float)temperature - .5 + (((float)count_per_c - (float)count_remain) / (float)count_per_c);

    highResTemp = (float)((1.80 * highResTemp) + 32.00);
    return highResTemp;
}


// this should be called every second to see when to turn lights on and off and such.
int processState(){
    int onTime=6;
    int offTime=20;
    
    // on at 6
    if(Time.hour()==onTime){
        changeLight(D2,LOW);
        changeLight(D3,LOW);
    }
    if(Time.hour()==offTime){
        changeLight(D2,HIGH);
        changeLight(D3,HIGH);
    }
   // if(Time.hour()<onTime){
   //     changeLight(D2,HIGH);
   //     changeLight(D3,HIGH);
   // }
   return 0;
}

// internet function for lights.
int lightsOn(String fred){
    changeLight(D2,LOW);
    changeLight(D3,LOW);
    return 0;    
}

int lightsOff(String fred){
    changeLight(D2,HIGH);
    changeLight(D3,HIGH);
    return 0;
}

void changeLight(int light,int state){
    
    if (state == LOW){
        // turn it off
        
           if (light==D2){
               if(light1State!=state){
                    Particle.publish("plant shelf","Turn Lower light on..");
                    digitalWrite(light,LOW);
               }
               light1State=state;
               LowerLight="ON";
           } else {
               if(light2State!=state){
                    Particle.publish("plant shelf","Turn Upper light on..");
                    digitalWrite(light,LOW);
               }
               light2State=state;
               UpperLight="ON";
           }
        
    } else if (state == HIGH){
        // turn it on
       
          if (light==D2){
               if (light1State!=state){
                  Particle.publish("plant shelf","Turn Lower light off..");
                  digitalWrite(light,HIGH);
               }
              light1State=state;
              LowerLight="OFF";
          } else {
              if (light2State!=state){
                  Particle.publish("plant shelf","Turn Upper Light off..");
                  digitalWrite(light,HIGH);
               }
              light2State=state;
              UpperLight="OFF";
          }
        
    }
}


void displayTime(){
    lcd->clear();
    
    Serial.print(Time.timeStr());
    lcd->setCursor(0,0);
    lcd->print("   ");
    lcd->print(Time.year());
    lcd->print("/");
    lcd->print(Time.month());
    lcd->print("/");
    lcd->print(Time.day());
    lcd->print(" ");
    lcd->setCursor(1,5);
    lcd->print(Time.hour() < 10? "   0" : "    ");
    lcd->print(Time.hour());
    lcd->print(Time.minute() < 10? ":0": ":");
    lcd->print(Time.minute());
    lcd->print(Time.second() < 10? ":0": ":");
    lcd->print(Time.second());

}

/*******************************************************************************
 * Function Name  : tinkerDigitalRead
 * Description    : Reads the digital value of a given pin
 * Input          : Pin
 * Output         : None.
 * Return         : Value of the pin (0 or 1) in INT type
                    Returns a negative number on failure
 *******************************************************************************/
int tinkerDigitalRead(String pin)
{
	//convert ascii to integer
	int pinNumber = pin.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber >7) return -1;

	if(pin.startsWith("D"))
	{
		pinMode(pinNumber, INPUT_PULLDOWN);
		return digitalRead(pinNumber);
	}
	else if (pin.startsWith("A"))
	{
		pinMode(pinNumber+10, INPUT_PULLDOWN);
		return digitalRead(pinNumber+10);
	}
	return -2;
}

/*******************************************************************************
 * Function Name  : tinkerDigitalWrite
 * Description    : Sets the specified pin HIGH or LOW
 * Input          : Pin and value
 * Output         : None.
 * Return         : 1 on success and a negative number on failure
 *******************************************************************************/
int tinkerDigitalWrite(String command)
{
	bool value = 0;
	//convert ascii to integer
	int pinNumber = command.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber >7) return -1;

	if(command.substring(3,7) == "HIGH") value = 1;
	else if(command.substring(3,6) == "LOW") value = 0;
	else return -2;

	if(command.startsWith("D"))
	{
		pinMode(pinNumber, OUTPUT);
		digitalWrite(pinNumber, value);
		return 1;
	}
	else if(command.startsWith("A"))
	{
		pinMode(pinNumber+10, OUTPUT);
		digitalWrite(pinNumber+10, value);
		return 1;
	}
	else return -3;
}

/*******************************************************************************
 * Function Name  : tinkerAnalogRead
 * Description    : Reads the analog value of a pin
 * Input          : Pin
 * Output         : None.
 * Return         : Returns the analog value in INT type (0 to 4095)
                    Returns a negative number on failure
 *******************************************************************************/
int tinkerAnalogRead(String pin)
{
	//convert ascii to integer
	int pinNumber = pin.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber >7) return -1;

	if(pin.startsWith("D"))
	{
		return -3;
	}
	else if (pin.startsWith("A"))
	{
		return analogRead(pinNumber+10);
	}
	return -2;
}

/*******************************************************************************
 * Function Name  : tinkerAnalogWrite
 * Description    : Writes an analog value (PWM) to the specified pin
 * Input          : Pin and Value (0 to 255)
 * Output         : None.
 * Return         : 1 on success and a negative number on failure
 *******************************************************************************/
int tinkerAnalogWrite(String command)
{
	//convert ascii to integer
	int pinNumber = command.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber >7) return -1;

	String value = command.substring(3);

	if(command.startsWith("D"))
	{
		pinMode(pinNumber, OUTPUT);
		analogWrite(pinNumber, value.toInt());
		return 1;
	}
	else if(command.startsWith("A"))
	{
		pinMode(pinNumber+10, OUTPUT);
		analogWrite(pinNumber+10, value.toInt());
		return 1;
	}
	else return -2;
}

