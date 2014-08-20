#include <Wire.h>
#include <SoftwareSerial.h>
int sensorPin = A0;    
int newButtonPin = 4;
int led = 8; 
int buzzer = 5;
SoftwareSerial portTwo(6,7);
SoftwareSerial WiFlySerial(2,3); //RX  TX
const int Bin1 = 12; //White
 const int Bin2 = 8; //Yellow
 const int Bin3 = 9; //Orange
 const int Bin4 = 11; //;Blue
int sensorValue = 0;  // variable to store the value coming from the sensor
int buttonState = 0;

float timearray[11];
int x = 0;
long curtime = 0;          // used for counter
int total_time;
int score = 0;

unsigned long start_time;  // time of trial beginning
bool flag_start = false;  // flag to start trial
bool flag_rfid = false;  //flag to check rfid value
int a = 0;
int standing = 200; 
int sitting = 350;
 byte i = 0;   //for rfid function
byte val = 0;
char code[6];
byte checksum = 0;
byte bytesread = 0;
byte tempbyte = 0;

boolean dataReadyFlag = false;
boolean flag_sit = true;
boolean flag_reset = false;
int RFIDResetPin = 13;


void setup() {
  Serial.begin(9600);
 // irrecv.enableIRIn();
  pinMode(led, OUTPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(newButtonPin, INPUT);
  WiFlySerial.begin(9600);  // set the bardrate of the SoftwareSerial port for Wifly Module
  portTwo.begin(9600);
  Wire.begin();  // initialize I2C stuff
  pinMode(Bin1, OUTPUT); // sets up binary output one as a digital output
   pinMode(Bin2, OUTPUT); //and so on...
   pinMode(Bin3, OUTPUT);
   pinMode(Bin4, OUTPUT);
      digitalWrite(Bin1, B1); 
   digitalWrite(Bin2, B1);
   digitalWrite(Bin3, B1);
   digitalWrite(Bin4, B1);
   pinMode(RFIDResetPin, OUTPUT);
  digitalWrite(RFIDResetPin, HIGH);
}

void changeFlag(){
  dataReadyFlag = true;
}


void WiFiSend(String data_send)
{
  WiFlySerial.print(data_send);
  WiFlySerial.println("");
  WiFlySerial.println("");
}

void WiFiSend(unsigned int data_send)
{ 
  WiFlySerial.print(data_send);
  WiFlySerial.println("");
  WiFlySerial.println("");
}


void button_flag()
{  
  buttonState = digitalRead(newButtonPin);
  if (buttonState != 0)
 {
   delay(20);
    buttonState = digitalRead(newButtonPin);
   if (buttonState != 0)
   {
     delay(20);
      buttonState = digitalRead(newButtonPin);
    if (buttonState != 0)
    {
       digitalWrite(Bin1, B1); //Write "3" to the display
   digitalWrite(Bin2, B1);
   digitalWrite(Bin3, B0);
   digitalWrite(Bin4, B0);
   delay(1000);
    digitalWrite(Bin1, B0); //Write "2" to the display
   digitalWrite(Bin2, B1);
   digitalWrite(Bin3, B0);
   digitalWrite(Bin4, B0);
   delay(1000); //Wait for half of a second
      digitalWrite(Bin1, B1); //Write "1" to the display
   digitalWrite(Bin2, B0);
   digitalWrite(Bin3, B0);
   digitalWrite(Bin4, B0);
   delay(700);
         digitalWrite(Bin1, B1); 
   digitalWrite(Bin2, B1);
   digitalWrite(Bin3, B1);
   digitalWrite(Bin4, B1);
   digitalWrite(buzzer,HIGH);
   delay(300);
   digitalWrite(buzzer,LOW);
  WiFiSend("SitStandstart");
  flag_start = true;
  buttonState = 0;
    }
   } 
 }
}

void button2()
{  
  buttonState = digitalRead(newButtonPin);
  if (buttonState != 0)
 {
   delay(20);
    buttonState = digitalRead(newButtonPin);
   if (buttonState != 0)
   {
     delay(20);
      buttonState = digitalRead(newButtonPin);
    if (buttonState != 0)
    {
       flag_reset = true;
      buttonState = 0;
    }
   } 
 }
}

void display_num()
{
 if (score == 4)
 {
   digitalWrite(Bin1, B0); //Write "4" to the display
   digitalWrite(Bin2, B0);
   digitalWrite(Bin3, B1);
   digitalWrite(Bin4, B0);
   delay(5000);
   digitalWrite(Bin1, B1); 
   digitalWrite(Bin2, B1);
   digitalWrite(Bin3, B1);
   digitalWrite(Bin4, B1);
 }
 if (score == 3)
 { 
   digitalWrite(Bin1, B1); //Write "3" to the display
   digitalWrite(Bin2, B1);
   digitalWrite(Bin3, B0);
   digitalWrite(Bin4, B0);
   delay(5000); 
      digitalWrite(Bin1, B1); 
   digitalWrite(Bin2, B1);
   digitalWrite(Bin3, B1);
   digitalWrite(Bin4, B1);
 }
  if (score == 2)
 { 
   digitalWrite(Bin1, B0); //Write "2" to the display
   digitalWrite(Bin2, B1);
   digitalWrite(Bin3, B0);
   digitalWrite(Bin4, B0);
   delay(5000); 
      digitalWrite(Bin1, B1); 
   digitalWrite(Bin2, B1);
   digitalWrite(Bin3, B1);
   digitalWrite(Bin4, B1);
 }
  if (score == 1)
 { 
   digitalWrite(Bin1, B1); //Write "1" to the display
   digitalWrite(Bin2, B0);
   digitalWrite(Bin3, B0);
   digitalWrite(Bin4, B0);
   delay(5000); 
      digitalWrite(Bin1, B1); 
   digitalWrite(Bin2, B1);
   digitalWrite(Bin3, B1);
   digitalWrite(Bin4, B1);
 }
  if (score == 0)
 { 
   digitalWrite(Bin1, B0); //Write "0" to the display
   digitalWrite(Bin2, B0);
   digitalWrite(Bin3, B0);
   digitalWrite(Bin4, B0);
   delay(5000); 
      digitalWrite(Bin1, B1); 
   digitalWrite(Bin2, B1);
   digitalWrite(Bin3, B1);
   digitalWrite(Bin4, B1);
 }
  
}

void check_rfid()
{
char tagString[15];
  int index = 0;
  boolean reading = false;
  while(portTwo.available()){
    int readByte = portTwo.read(); //read next available byte

    if(readByte == 2) reading = true; //begining of tag
    if(readByte == 3) reading = false; //end of tag

    if(reading && readByte != 2 && readByte != 10 && readByte != 13){
      //store the tag
      tagString[index] = readByte;
      index ++;
       if(index ==12){
        digitalWrite(buzzer,HIGH);
        delay(500);
        digitalWrite(buzzer,LOW);
        Serial.println(tagString);
        WiFiSend("RFID_Tag");
        delay(1000);
        tagString[12] = 'S';
        tagString[13] = 'T';
        tagString[14] = '\0';
        WiFiSend(tagString);       
        delay(400);
        WiFlySerial.listen();
        flag_rfid = true;
        }
     }
  }
  clearTag(tagString);
  resetReader(); //reset the RFID reader
}

void clearTag(char one[]){
///////////////////////////////////
//clear the char array by filling with null - ASCII 0
//Will think same tag has been read otherwise
///////////////////////////////////
  for(int i = 0; i < strlen(one); i++){
    one[i] = 0;
  }
}
void resetReader(){
///////////////////////////////////
//Reset the RFID reader to read again.
///////////////////////////////////
  digitalWrite(RFIDResetPin, LOW);
  digitalWrite(RFIDResetPin, HIGH);
  delay(150);
}



void loop() {
  
 while (flag_rfid == false)
  {
    check_rfid(); 
  }

  while (flag_start == false)
  {
  button_flag();
    if(flag_start == true)
    {
      start_time = millis();
      x = 0;
    }
  }
  
  if (flag_start == true)
  {
   button2();
   if (flag_reset == true)
   {
     score = 0; 
    Serial.println("Finished");
     String Title="SitScore";
    String myScore = String(score);
    String stringScore = String(Title + myScore);
       WiFiSend(stringScore);
       delay(500);

    digitalWrite(buzzer, HIGH);  
    delay(500);        
    digitalWrite(buzzer, LOW); 
    display_num();
    flag_start = false;  
    flag_rfid = false;
    flag_reset = false;
        portTwo.listen();

   }
    
  }
 
  sensorValue = analogRead(sensorPin); 
  Serial.println(sensorValue);
 // WiFiSend(sensorValue);
  
  curtime = millis();   
  
  if (flag_sit == false)
  {
   if (sensorValue >= sitting)
   {
     flag_sit = true;
     timearray[x]=(curtime-start_time);
     x=x+1;
   }
  }
  
  if (flag_sit == true)
  {
   if (sensorValue <= standing)
   {
     flag_sit = false;
     timearray[x]=(curtime-start_time);
     x=x+1;
   } 
  }

  if (curtime-start_time>60000 && flag_start == true)  // if trial extends over 60 sec, ends trial 
  {
      score = 0; 
    Serial.println("Finished");
      String Title="SitScore";
    String myScore = String(score);
    String stringScore = String(Title + myScore);
       WiFiSend(stringScore);
       delay(500);
    digitalWrite(buzzer, HIGH);  
    delay(500);        
    digitalWrite(buzzer, LOW); 
    display_num();
    flag_start = false;  
    flag_rfid = false;
    portTwo.listen();
  }

  if (x==10)    // when the trial is over, this ends the program and writes the data
  {
    Serial.println("Finished");
    total_time = (timearray[9]);
    digitalWrite(buzzer,HIGH);
    delay(500);
    digitalWrite(buzzer,LOW);
     Serial.println(total_time);
     if (total_time <= 11199)
     {  
       score = 4;
     }
     else if (total_time > 11200 && total_time <= 13699)
     {
       score = 3;
     }
     else if (total_time > 13700 && total_time <= 16699)
     {
       score = 2;
     }
     else if (total_time > 16700)
     {
       score = 1;
     }
     else
     {
       score = 0;
     }
     
     
//     WiFiSend("RFID_Tag");
//     char sendCode[9];
//     sendCode[0] = 'S';
//     sendCode[1] = 'T';
//     sendCode[8] = '\0';
//     int i = 0;
//     for (i = 0; i < 6; i++) {
//       sendCode[i+2] = code[i];
//     }
//     WiFiSend(String(sendCode));
//    // WiFiSend(total_time);
//     delay(500);
    String Title="SitScore";
    String myScore = String(score);
    String stringScore = String(Title + myScore);
       WiFiSend(stringScore);
       delay(500);
       String Time = "SitTime";
       String Sendtime = (Time + total_time);
       WiFiSend(Sendtime);
       delay(500);
       
       
      display_num();     
      
      
     for (int i = 0; i<10; i++)
     {
       Serial.println(timearray[i]/1000);
     }
     Serial.println(score);
    portTwo.listen();
     flag_start = false;
     flag_rfid = false;
   }
   
}
