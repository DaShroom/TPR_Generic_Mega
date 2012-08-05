/* Copyright 2012 George Ainscough. Released under CC-BY SA until my nice new license is finished, then it can be used under both  :-D
This is currently V0.01, ALPHA (ie untested)
This is made for TPR-Engineering (RCJ-Soc Team), by George Ainscough, and is hosted at dashroom66.com and at my github repo

The code is clumsy and inefficient, but that's what you get for using the Arduino IDE, without proper C. Though it is easily ported, and modifiable.
For reasons of ease of modification by beginners I have put housekeeping as called in loop dependent on a millis count, though you could put it in a timer interupt very easily.*/
#include <Wire.h>

#define HEAD_THRESHOLD 50 //how wobbly the robot can be
#define HEAD_CORRECT 10  //how much shall we correct the heading
#define HK_TIME 3000  //wait time between housekeeping

//Pin Names
byte startPin = 22;
byte kickPin = 24;  //not used here as hard wired in out bot, but probably will be on an interupt soon.. (can't comment out as pinMode set)
byte ballPin = 26;
byte M1_En = 2;
byte M2_En = 3;
byte M3_En = 4;
byte M4_En = 5;
byte M1_Fw = 26;
byte M2_Fw = 28;
byte M3_Fw = 30;
byte M4_Fw = 32;
byte M1_Bw = 34;
byte M2_Bw = 36;
byte M3_Bw = 38;
byte M4_Bw = 40;

byte motspeed = 255;  //speed of motors
int compaddr_slave = 0x42;  //some I2C addresses
int diraddr = 0x49;
int sigaddr = 0x4C;
float heading;  //The current heading from the compass sensor
int starthead;  //The heading at setup
unsigned long time = 0;  //set to current time every housekeeping interval, used to compare times with to find 3000ms
byte dir;  //Where is the ball
byte sig;  //How well can we see it
byte start;  //can we play now, PPPPLLLLEEEEEEEEAAAAAAASSSSSSSSSSSEEEEEEEEEEEEEEEE :-P

void setup()
{
  compaddr_slave = compaddr_slave >> 1; // significant bit loss compensation
//  diraddr = diraddr >> 1;
//  sigaddr = sigaddr >> 1;
  Wire.begin();
  get_head();  //Where am I pointed
  starthead = heading;  //Where is pitch north
  
  pinMode(startPin, INPUT); pinMode(kickPin, OUTPUT); pinMode(ballPin, INPUT);  //What are the pins used for
  pinMode(M1_En, OUTPUT); pinMode(M2_En, OUTPUT); pinMode(M3_En, OUTPUT); pinMode(M4_En, OUTPUT);
  pinMode(M1_Fw, OUTPUT); pinMode(M2_Fw, OUTPUT); pinMode(M3_Fw, OUTPUT); pinMode(M4_Fw, OUTPUT);
  pinMode(M1_Bw, OUTPUT); pinMode(M2_Bw, OUTPUT); pinMode(M3_Bw, OUTPUT); pinMode(M4_Bw, OUTPUT);
  
  while (start = 0)  //Wait until start flag raised
  {
    start =  digitalRead(startPin); //Keep checking start pin, and raise flag when high
  }
  housekeeping();  // Just a bit of maintainance
  time = millis();  //Write down ewhen we had this clean up
}


void loop()  //Keeps going over, and over, and over...
{
  if ((millis()) - time >= HK_TIME) //is it time for a bit of housekeeping, are we more than the interval past the last clean up
  {
    time = millis();  //Note down the time of the clean up
    housekeeping();//Have the clean uo
  }
  
  switch (dir) {  //Lots of options, depending on where the ball is, as given in dir by the HTIR Seeker, 1-9
  case 1:                 //can't be arsed doing them all, but go to the ball, until it goes into a different bit of vision. Keep checking for that.
   halt();
   S_go();
   while (dir == 1) {
     get_dir(); }
   break;
   case 2:
   halt();
   SW_go();
   while (dir == 2) {
     get_dir(); }
   break;
   case 3:
   halt();
   W_go();
   while (dir == 3) {
     get_dir(); }
   break;
   case 4:
   halt();
   NW_go();
   while (dir == 4) {
     get_dir(); }
   break;
   case 5:
   halt();
   N_go();
   while (dir == 5) {
     get_dir(); }
   break;
   case 6:
   halt();
   NE_go();
   while (dir == 6) {
     get_dir(); }
   break;
   case 7:
   halt();
   E_go();
   while (dir == 7) {
     get_dir(); }
   break;
   case 8:
   halt();
   SE_go();
   while (dir == 8) {
     get_dir(); }
   break;
   case 9:
   halt();
   S_go();
   while (dir == 9) {
     get_dir(); }
   break;
   default:
   halt();
   S_go();
   while (dir >= 9) {
     get_dir(); }
   break;
 }
}


void housekeeping()  //clean up eny mess
{
  get_head();  //where am I pointing
//  get_dir();
//  get_sig();
  if(heading <= starthead - HEAD_THRESHOLD || heading >= starthead + HEAD_THRESHOLD)  //Am I a bit off track
  {
    correct_north(); //If yes, do sommet about it
  }
}


void get_head()  //get heading
{
  Wire.beginTransmission(compaddr_slave);
  Wire.write("A");   //A is the get data command
  Wire.endTransmission();
  delay(1);  // Aparently you need it, but I HATE DELAYS, they SUCK
  Wire.requestFrom(compaddr_slave, 2);
  byte MSB = Wire.read();  //Get the 2 bytes, and berge them together to get a word
  byte LSB = Wire.read();
  float heading = (MSB << 8) + LSB;
}

void get_dir() //get ball direction
{
//  Wire.beginTransmission(diraddr);
  Wire.requestFrom(diraddr, 1);
  dir = Wire.read();
}

void get_sig() //get ball signal level
{
//  Wire.beginTransmission(sigaddr);
  Wire.requestFrom(sigaddr, 1);
  sig = Wire.read();
}

void correct_north()  //get us facing the right way
{
  if(heading <= starthead - HEAD_THRESHOLD)
  {
   halt();
   turn_right();
   while (heading <= starthead - HEAD_CORRECT)
   {
     get_head();
   }
   halt();
  }
  else if(heading >= starthead + HEAD_THRESHOLD)
  {
    halt();
    turn_left();
    while (heading >= starthead + HEAD_CORRECT)
    {
      get_head();
    }
    halt();
  }
}

//Lots of motor outputs for different directions

void N_go(){analogWrite(M1_En,motspeed);analogWrite(M2_En,motspeed);digitalWrite(M1_Fw, HIGH);digitalWrite(M1_Bw, LOW);digitalWrite(M2_Fw, LOW);digitalWrite(M2_Bw, HIGH);analogWrite(M4_En,motspeed);analogWrite(M3_En,motspeed);digitalWrite(M4_Fw, HIGH);digitalWrite(M4_Bw, LOW);digitalWrite(M3_Fw, LOW);digitalWrite(M3_Bw, HIGH);} //Go north
//void N_stop(){} //stop going north
void NE_go(){analogWrite(M1_En,motspeed);analogWrite(M3_En,motspeed);digitalWrite(M1_Fw, HIGH);digitalWrite(M1_Bw, LOW);digitalWrite(M3_Fw, LOW);digitalWrite(M3_Bw, HIGH);} //Go north east
//void NE_stop(){} //stop going north easr
void E_go(){analogWrite(M1_En,motspeed);analogWrite(M4_En,motspeed);digitalWrite(M1_Fw, HIGH);digitalWrite(M1_Bw, LOW);digitalWrite(M4_Fw, LOW);digitalWrite(M4_Bw, HIGH);analogWrite(M2_En,motspeed);analogWrite(M3_En,motspeed);digitalWrite(M2_Fw, HIGH);digitalWrite(M2_Bw, LOW);digitalWrite(M3_Fw, LOW);digitalWrite(M3_Bw, HIGH);} //getting the idea ??  :-D
//void E_stop(){}
void SE_go(){analogWrite(M2_En,motspeed);analogWrite(M4_En,motspeed);digitalWrite(M2_Fw, HIGH);digitalWrite(M2_Bw, LOW);digitalWrite(M4_Fw, LOW);digitalWrite(M4_Bw, HIGH);}
//void SE_stop(){}
void S_go(){analogWrite(M2_En,motspeed);analogWrite(M1_En,motspeed);digitalWrite(M2_Fw, HIGH);digitalWrite(M2_Bw, LOW);digitalWrite(M1_Fw, LOW);digitalWrite(M1_Bw, HIGH);analogWrite(M3_En,motspeed);analogWrite(M4_En,motspeed);digitalWrite(M3_Fw, HIGH);digitalWrite(M3_Bw, LOW);digitalWrite(M4_Fw, LOW);digitalWrite(M4_Bw, HIGH);}
//void S_stop(){}
void SW_go(){analogWrite(M3_En,motspeed);analogWrite(M1_En,motspeed);digitalWrite(M3_Fw, HIGH);digitalWrite(M3_Bw, LOW);digitalWrite(M1_Fw, LOW);digitalWrite(M1_Bw, HIGH);}
//void SW_stop(){}
void W_go(){analogWrite(M4_En,motspeed);analogWrite(M1_En,motspeed);digitalWrite(M4_Fw, HIGH);digitalWrite(M4_Bw, LOW);digitalWrite(M1_Fw, LOW);digitalWrite(M1_Bw, HIGH);analogWrite(M3_En,motspeed);analogWrite(M2_En,motspeed);digitalWrite(M3_Fw, HIGH);digitalWrite(M3_Bw, LOW);digitalWrite(M2_Fw, LOW);digitalWrite(M2_Bw, HIGH);}
//void W_stop(){}
void NW_go(){analogWrite(M4_En,motspeed);analogWrite(M2_En,motspeed);digitalWrite(M4_Fw, HIGH);digitalWrite(M4_Bw, LOW);digitalWrite(M2_Fw, LOW);digitalWrite(M2_Bw, HIGH);}
//void NW_stop(){}
void halt(){}  //Stop moving everything, freeze still
void turn_left(){} //Rotate left on spot
//void stop_left(){} //stop rotating left
void turn_right(){} //ditto for right
//void stop_right(){}

