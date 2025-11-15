#include <Arduino.h>
#include <VFD.h>


VFD::VFD(int dataPin, int clockPin, int latchPin, int numberOfTubes)
{
    _dataPin = dataPin;
    _clockPin = clockPin;
    _latchPin = latchPin;
    _numberOfTubes = numberOfTubes;
    memset(_VfdTable, 0, sizeof(_VfdTable));
    _VfdTable['-'] = B00001000;
    _VfdTable['['] = B00010111;
    _VfdTable[']'] = B10110001;
    _VfdTable['0'] = B10110111;
    _VfdTable['1'] = B10100000;
    _VfdTable['2'] = B00111011;
    _VfdTable['3'] = B10111001;
    _VfdTable['4'] = B10101100;
    _VfdTable['5'] = B10011101;
    _VfdTable['6'] = B10011111;
    _VfdTable['7'] = B10110000;
    _VfdTable['8'] = B10111111;
    _VfdTable['9'] = B10111101;
    _VfdTable['A'] = B10111110;
    _VfdTable['B'] = B10111111;
    _VfdTable['C'] = B00010111;
    _VfdTable['D'] = B10110111;
    _VfdTable['E'] = B00011111;
    _VfdTable['F'] = B00011110;
}

void VFD::begin()
{
    pinMode(_dataPin, OUTPUT);
    pinMode(_clockPin, OUTPUT);
    pinMode(_latchPin, OUTPUT);

}

void VFD::printDisplay(String text){
    String _text = text;
    digitalWrite(_latchPin, LOW);
    char SelInput = ' ';
    if(_text.length() < _numberOfTubes){
        int NChar = _numberOfTubes - _text.length();
        if((NChar % 2) == 1){
            NChar = NChar -1;
            _text = _text + ' ';}
        NChar = NChar / 2;
        for(int i = 0 ; i < NChar ; i++){ _text = _text + ' '; }
        for(int i = 0 ; i < NChar ; i++){ _text = ' ' + _text; }     
  }
  for(int i = _numberOfTubes - 1 ; i >= 0 ; i--){
    SelInput = _text[i];
    shiftOut(_dataPin, _clockPin, MSBFIRST, _VfdTable[SelInput]);
  }
  digitalWrite(_latchPin, HIGH);
}


void VFD::printDisplayNC(String textNC){
    String _text = textNC;
    digitalWrite(_latchPin, LOW);
    char SelInput = ' ';
    for(int i = _numberOfTubes - 1 ; i >= 0 ; i--){
        SelInput = _text[i];
        shiftOut(_dataPin, _clockPin, MSBFIRST, _VfdTable[SelInput]);
     }
    digitalWrite(_latchPin, HIGH);
}


void VFD::printDisplayRaw(byte patterns[]){
    digitalWrite(_latchPin, LOW);
    for(int i = _numberOfTubes - 1 ; i >= 0 ; i--){
        shiftOut(_dataPin, _clockPin, MSBFIRST, patterns[i]);
    }
    digitalWrite(_latchPin, HIGH);
}



void VFD::onAll(){
    byte _data = B11111111;
    digitalWrite(_latchPin, LOW);
    for(int i = _numberOfTubes - 1 ; i >= 0 ; i--){
        shiftOut(_dataPin, _clockPin, MSBFIRST, _data);
    }
    digitalWrite(_latchPin, HIGH);
}

void VFD::offAll(){
    byte _dataz = B00000000;
    digitalWrite(_latchPin, LOW);
    for(int i = _numberOfTubes - 1 ; i >= 0 ; i--){
        shiftOut(_dataPin, _clockPin, MSBFIRST, _dataz);
    }
    digitalWrite(_latchPin, HIGH);
} 