/*
  Esplora Tinkerkit Input Read

 This sketch shows you how to read the Tinkerkit Inputs on the Arduino Esplora.
 Most likely this program will become obsolete when the Arduino team updates the IDE beyond 1.04

 Created on 2013-03-24 by Mike Barela

 This example is in the public domain, please attribute
 */
#include "Arduino.h"
#include "tinkerkit.h"
#include <Esplora.h>

unsigned int readTinkerkitInput(byte whichInput) {      // return 0-1023 from Tinkerkit Input A or B
   return readChannel(whichInput+CH_TINKERKIT_INA); }   //   as defined above
  
unsigned int readChannel(byte channel) {                // as Esplora.readChannel is a private function
     digitalWrite(A0, (channel & 1) ? HIGH : LOW);      //  we declare our own as a hack
     digitalWrite(A1, (channel & 2) ? HIGH : LOW);      //
     digitalWrite(A2, (channel & 4) ? HIGH : LOW);      // digitalWrite sets address lines for the input
     digitalWrite(A3, (channel & 8) ? HIGH : LOW);
     return analogRead(A4);               // analogRead gets value from MUX chip
}

