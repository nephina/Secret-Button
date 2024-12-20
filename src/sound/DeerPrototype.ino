

#include <MultiStepper.h>
#include <AccelStepper.h>
#include <Servo.h>
#include "pitches.h"

/*
  Use the Qwiic Scale to read load cells and scales
  By: Nathan Seidle @ SparkFun Electronics
  Date: March 3rd, 2019
  License: This code is public domain but you buy me a beer if you use this 
  and we meet someday (Beerware license).

  The Qwiic Scale is an I2C device that converts analog signals to a 24-bit
  digital signal. This makes it possible to create your own digital scale
  either by hacking an off-the-shelf bathroom scale or by creating your
  own scale using a load cell.

  This example merely outputs the raw data from a load cell. For example, the
  output may be 25776 and change to 43122 when a cup of tea is set on the scale.
  These values are unitless - they are not grams or ounces. Instead, it is a
  linear relationship that must be calculated. Remeber y = mx + b?
  If 25776 is the 'zero' or tare state, and 43122 when I put 15.2oz of tea on the
  scale, then what is a reading of 57683 in oz?

  (43122 - 25776) = 17346/15.2 = 1141.2 per oz
  (57683 - 25776) = 31907/1141.2 = 27.96oz is on the scale
  
  SparkFun labored with love to create this code. Feel like supporting open
  source? Buy a board from SparkFun!
  https://www.sparkfun.com/products/15242

  Hardware Connections:
  Plug a Qwiic cable into the Qwiic Scale and a RedBoard Qwiic
  If you don't have a platform with a Qwiic connection use the SparkFun Qwiic Breadboard Jumper (https://www.sparkfun.com/products/14425)
  Open the serial monitor at 115200 baud to see the output
*/
 
#include <Wire.h>

#include "SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h" // Click here to get the library: http://librarymanager/All#SparkFun_NAU7802

NAU7802 myScale; //Create instance of the NAU7802 class
Servo Ear;

int Eyes = 2;
float normalized_load = 0;

// notes of the melody followed by the duration.
// a quarter note is 250 ms, eighth note is 125 ms, etc.
int melody[] = {
  NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4,
  NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4
};

int noteDurations[] = {
  4, 4, 4, 4, 4, 4, 2,
  4, 4, 4, 4, 4, 4, 2
};

int noteIterator = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("Qwiic Scale Example");
  
  Wire.begin();

  if (myScale.begin() == false)
  {
    Serial.println("Scale not detected. Please check wiring. Freezing...");
    while (1);
  }
  Serial.println("Scale detected!");
  
  pinMode(4,OUTPUT);
  digitalWrite(4,LOW);
  pinMode(Eyes,OUTPUT);      
  Ear.attach(9);
  // initialize digital pin 5 as an output for the piezo speaker
  pinMode(5, OUTPUT);
}

void loop()
{
  if(myScale.available() == true){
    int32_t currentReading = myScale.getReading();
    Serial.print("Reading: ");
    Serial.print(currentReading);
    Serial.print(" Normalized Load: ");
    normalized_load = (float(currentReading) - 120000) / float(20000);   
    if (normalized_load < 0){  
      normalized_load = 0.0;
    }
    if (normalized_load > 1.0){
      normalized_load = 1.0;
    }
    Serial.println(normalized_load);
    
    for (int i = 0; i < int(300*normalized_load); i++){
      digitalWrite(Eyes, HIGH);
    }
    for (int i = 0; i < int(300 * (1 - normalized_load)); i++){
      digitalWrite(Eyes, LOW);
    }
    if (random(0,100) == 42){
      Ear.write(random(0,30));
    
      // calculate the note duration
      int noteDuration = 1000 / noteDurations[noteIterator];
      tone(5, melody[noteIterator], noteDuration);
  
      // wait for the note to play
      delay(noteDuration);
      noteIterator++;
      if (noteIterator == 14){
        noteIterator = 0;
      }
    }
  }
}
