/*
  Esplora Tinkerkit Input Read

 This sketch shows you how to read the Tinkerkit Inputs on the Arduino Esplora.
 Most likely this program will become obsolete when the Arduino team updates the IDE beyond 1.04

 Created on 2013-03-24 by Mike Barela

 This example is in the public domain, please attribute
 */
#ifndef TINKERKIT_H
#define TINKERKIT_H

#include "Arduino.h"

const byte CH_TINKERKIT_INA = 8;   // Add values missing from Esplora.h
const byte CH_TINKERKIT_INB = 9;
const byte INPUT_A          = 0;
const byte INPUT_B          = 1;

unsigned int readTinkerkitInput(byte whichInput);  
unsigned int readChannel(byte channel);

#endif

