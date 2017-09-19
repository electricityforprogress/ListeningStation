// Specifically for use with the Adafruit Feather, the pins are pre-set here!

// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <SD.h>
#include <Adafruit_VS1053.h>
#include <Bounce2.h>
#include <LEDFader.h>

// These are the pins used
#define VS1053_RESET   -1     // VS1053 reset pin (not used!)

// Feather M0 or 32u4
#if defined(__AVR__) || defined(ARDUINO_SAMD_FEATHER_M0)
  #define VS1053_CS       6     // VS1053 chip select pin (output)
  #define VS1053_DCS     10     // VS1053 Data/command select pin (output)
  #define CARDCS          5     // Card chip select pin
  // DREQ should be an Int pin *if possible* (not possible on 32u4)
  #define VS1053_DREQ     9     // VS1053 Data request, ideally an Interrupt pin
#endif

Adafruit_VS1053_FilePlayer musicPlayer = 
  Adafruit_VS1053_FilePlayer(VS1053_RESET, VS1053_CS, VS1053_DCS, VS1053_DREQ, CARDCS);

Bounce button = Bounce();
int knobPin = A0;
int buttonPin = A3;
int knobHighPin = A1;
int buttonLowPin = A5;
int inputPin = 12; //13;
int ledPin = 13; //11;
LEDFader led = LEDFader(ledPin);

void setup() {
 // Serial.begin(9600);
//setup knob and button supporting pins
  pinMode(knobHighPin, OUTPUT);
  digitalWrite(knobHighPin, HIGH);
  pinMode(buttonLowPin, OUTPUT);
  digitalWrite(buttonLowPin, LOW);
  pinMode(buttonPin, INPUT_PULLUP);
  button.attach(buttonPin);
  button.interval(5);
  pinMode(inputPin, INPUT);

  musicPlayer.softReset();
  
 while (! musicPlayer.begin()) { // initialise the music player
      digitalWrite(ledPin, HIGH); delay(50);
      digitalWrite(ledPin, LOW); delay(50);
}

//musicPlayer.begin();  delay(150); //wait for it ...

  //Serial.println(F("VS1053 Found"));
   // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(25, 25);

//  if (!SD.begin(CARDCS)) {
//   // Serial.println(F("SD failed, or not present"));
//    while (1);  // don't do anything more
//  }
  //Serial.println("SD OK!");

  //SD.begin(CARDCS); delay(150); //cook!
  while(!SD.begin(CARDCS)) {
      digitalWrite(ledPin, HIGH); delay(150);
      digitalWrite(ledPin, LOW); delay(150);
  }
  
  // list files
//  printDirectory(SD.open("/"), 0);

  musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int


//start playing the music!!! 
//  musicPlayer.startPlayingFile("BIODAT01.mp3");
  
}

void loop() {

  int change = button.update();
  int buttonPress =  button.read();
  int knob = analogRead(knobPin);

  int volume = map(knob, 0, 1023, 100, 10); //scale the volume
  //musicPlayer.setVolume(volume, volume);
  


//  if(change && !buttonPress) { //changed state and pushed down
//    if(!musicPlayer.stopped()) musicPlayer.stopPlaying();
//     musicPlayer.softReset();
//    for(byte i=0;i<4;i++) { //mini lightshow!
//      digitalWrite(ledPin, HIGH); delay(50);
//      digitalWrite(ledPin, LOW); delay(50);
//    }
//Start Playing
  if(musicPlayer.stopped()){ //keep it playing if it ends
    musicPlayer.startPlayingFile("BIODAT01.mp3");
   }
   
  pulse(ledPin, 200, 650);
  led.update();
}

void pulse(int ledPin, int maxValue, int time) {
 //check on the state of the LED and force it to pulse
 if(led.is_fading() == false) {  //if not fading
   if(led.get_value() > 0) { //and is illuminated
     led.fade(0, time); //fade down
   } else led.fade(maxValue, time); //fade up
 }
}

/// File listing helper
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}


