#include <Adafruit_NeoPixel.h>
#include "I2Cdev.h"
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN           7

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      54


const int SW_pin = 3; // digital pin connected to switch output
const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output
const int MPU_addr=0x68;
double x; double y; double z;
float  AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
int minVal=265; int maxVal=402;
// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayval = 100; // delay for half a second

int JoyX;
int JoyY;
int JoyButt;
typedef struct _Ball {
   int y ;
   int x ;
};

typedef struct _Led {
  int ID;
  int Red ;
  int Blue  ;
  int Green ;
  bool Player;
};

_Led LedBoads[3][18];
_Ball myPlayer ;

void setup() {

  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(115200);

  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH);

  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  myPlayer.x = 0;
  myPlayer.y = 0;

  initLedBoard();
  pixels.begin(); // This initializes the NeoPixel library.
}

void loop() {

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers

 AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)

  JoyX = analogRead(X_pin);
  JoyY = analogRead(Y_pin);
  JoyButt = digitalRead(SW_pin);
  Wire.beginTransmission(MPU_addr); Wire.write(0x3B); Wire.endTransmission(false); Wire.requestFrom(MPU_addr,14,true);
  AcX=Wire.read()<<8|Wire.read();
  AcY=Wire.read()<<8|Wire.read();
  AcZ=Wire.read()<<8|Wire.read();
  int xAng = map(AcX,minVal,maxVal,-90,90);
  int yAng = map(AcY,minVal,maxVal,-90,90);
  int zAng = map(AcZ,minVal,maxVal,-90,90);

 x= RAD_TO_DEG * (atan2(-yAng, -zAng)+PI); y= RAD_TO_DEG * (atan2(-xAng, -zAng)+PI); z= RAD_TO_DEG * (atan2(-yAng, -xAng)+PI);

 //Serial.print("AngleX= "); Serial.println(x);

//Serial.print("AngleY= "); Serial.println(y);

 //Serial.print("AngleZ= "); Serial.println(z); Serial.println("-----------------------------------------"); delay(400);
//  Serial.println(AcX);

  if(x < 177)
  {
    moveUp();
  }
  if(x > 183)
  {
    moveDown();
  }

  if(y < 177)
  {
    moveLeft();
  }

  if(y > 183)
  {
    moveRight();
  }

  if(!JoyButt)
    initLedBoard();


  UpdatePlayer();
  UpdatePixels();


}

void moveUp()
{
  if(myPlayer.x <= 16)
  {
        LedBoads[myPlayer.y][myPlayer.x].Red = 0;
        //LedBoads[myPlayer.y][myPlayer.x].Green += 1;
        delay( abs (delayval -( abs(x - 180) * 5)));
        //Serial.println(delayval -( abs(x - 180) * 5));
        myPlayer.x++;
        myPlayer.y = myPlayer.y;
  }
}

void moveDown()
{
  if(myPlayer.x >= 1)
  {
        LedBoads[myPlayer.y][myPlayer.x].Red = 0;
        //LedBoads[myPlayer.y][myPlayer.x].Blue += 1;
        delay(abs(delayval - ( abs(x - 180))  * 5));
        myPlayer.x--;
        myPlayer.y = myPlayer.y;
  }
}

void moveLeft()
{
  if(myPlayer.y < 2)
  {
        LedBoads[myPlayer.y][myPlayer.x].Red = 0;
      //  LedBoads[myPlayer.y][myPlayer.x].Green += 1;
        delay(abs(delayval -( abs(y - 180)) * 5));
        myPlayer.y++;
        myPlayer.x = myPlayer.x;
  }
}

void moveRight()
{
  if(myPlayer.y >= 1)
  {
        LedBoads[myPlayer.y][myPlayer.x].Red -= 50;
        //LedBoads[myPlayer.y][myPlayer.x].Blue += 1;
        delay(abs(delayval -( abs(y - 180))  * 5));
        myPlayer.y--;
        myPlayer.x = myPlayer.x;
  }
}

void UpdatePixels()
{
  for( int i = 0; i < 3; i++)
       {
         for (int j = 0; j< 18; j++)
         {
            pixels.setPixelColor(LedBoads[i][j].ID, pixels.Color(LedBoads[i][j].Red, LedBoads[i][j].Green, LedBoads[i][j].Blue));
         }
       }
       pixels.show();
}


void UpdatePlayer()
 {
    LedBoads[myPlayer.y][myPlayer.x].Red += 50;
 }

void initLedBoard()
{
  int idCounter = 0;
  for( int i = 0; i < 3; i++)
  {
    if ( (i % 2) == 0)
    {
      for (int j = 0; j < 18; j++)
      {
        _Led myLed = {idCounter, 0,0,0,false};
        LedBoads[i][j] = myLed;
        idCounter++;
      }
    }
    else
    {
      for (int j = 17; j >= 0; j--)
      {
        _Led myLed = {idCounter, 0,0,0,false};
        LedBoads[i][j] = myLed;
        idCounter++;
      }
    }
  }
}
