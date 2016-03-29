/*
    Analog Panel Meters For Arduino & PC
    HackaDay.io page:
    
    https://hackaday.io/project/10629-pc-analog-panel-meters-w-arduino
    
    Based on :
    Drives PC Meter device.
    http://www.lungstruck.com/projects/pc-meter

    Uses this fork for stats serial data:
    https://github.com/cody82/open-hardware-monitor

*/

int DEBUG_METER = 0;//1+ = serial write meter # pos//0 = off //can use arduino serial monitor to send eg C45 = CPU 45%

int METERS_POS[] = {0, 0, 0};//current meter positin//0-255
int METERS_DATA[] = {0, 0, 0};//current meter reading//0-100
int METERS_PIN[] = {9, 6, 3};//meter arduino pins//pwm pins

int NUM_METERS = 3;

int UPDATE_EVERY = 100;//increment meter pos ever ms. - longer = slower smoother movements

const long SERIAL_TIMEOUT = 10000;  //How long to wait until serial "times out"

//Variables
unsigned long lastSerialRecd = 0;     //Time last serial recd
unsigned long lastUpdate = 0;//last meter change

void setup()
{
 Serial.begin(9600);


    for(int i = 0; i < NUM_METERS; i++){
      pinMode(METERS_PIN[i], OUTPUT);
      METERS_DATA[i] = 100;
    }

  for(int i = 0; i <= 510; i++){
    if(i == 255){memset(METERS_DATA, 0, sizeof(METERS_DATA));delay(2000);}
    updateMeters();
    delay(1);
  }
 
 //Give meter some time to start receiving data
 lastSerialRecd = millis();
}


void loop()
{
  char buffer[5];                //buffer
    
  while (Serial.available() > 0)
  {
   Serial.readBytesUntil('\r', buffer, 5);

   switch (buffer[0])
   {
    case 'C':
     METERS_DATA[0] = min(atoi(&buffer[1]), 100);
     break;
    case 'M':
     //Memory
     METERS_DATA[1] = min(atoi(&buffer[1]), 100);
     break;
    case 'T':
     //Temp
     int temp = min(atoi(&buffer[1]), 100);
     METERS_DATA[2] = map(temp,40,70,0,100);//scale temperature to percentage
     break;
   }
   
   //Reset for next measurement
   memset(buffer, ' ', 5);
   
   //Update last serial received
   lastSerialRecd = millis();
  }
  

  if (millis() - lastSerialRecd > SERIAL_TIMEOUT)memset(METERS_DATA, 0, sizeof(METERS_DATA));
    
  if(millis() - lastUpdate > UPDATE_EVERY)updateMeters();
    
}


//Set Meter position and LED color.
void updateMeters()
{
  
  for(int i = 0; i < NUM_METERS; i++){
    
      int pwm = map(METERS_DATA[i], 0, 100, 0, 255);//convert % to pwm
      
      if(pwm != METERS_POS[i]){      
        
          if(pwm > METERS_POS[i])METERS_POS[i]++;
          else METERS_POS[i]--;  
          
          //Set meter
          analogWrite(METERS_PIN[i], METERS_POS[i]);
          
          if(DEBUG_METER && i == (DEBUG_METER-1))Serial.println(METERS_POS[i]);//debug select meter   

      }
  }
  
  lastUpdate = millis();
}


