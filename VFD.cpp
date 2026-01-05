#include <Arduino.h>
#include <VFD.h>


VFD::VFD(int dataPin, int clockPin, int latchPin, int numberOfTubes)
{
    _dataPin = dataPin;
    _clockPin = clockPin;
    _latchPin = latchPin;
    _numberOfTubes = numberOfTubes;


    memset(_VfdTable, 0, sizeof(_VfdTable));
    _VfdTable['_'] = 0b00000001;
    _VfdTable['^'] = 0b00110100;
    _VfdTable['-'] = 0b00001000;
    _VfdTable['['] = 0b00010111;
    _VfdTable[']'] = 0b10110001;
    _VfdTable['0'] = 0b10110111;
    _VfdTable['1'] = 0b10100000;
    _VfdTable['2'] = 0b00111011;
    _VfdTable['3'] = 0b10111001;
    _VfdTable['4'] = 0b10101100;
    _VfdTable['5'] = 0b10011101;
    _VfdTable['6'] = 0b10011111;
    _VfdTable['7'] = 0b10110000;
    _VfdTable['8'] = 0b10111111;
    _VfdTable['9'] = 0b10111101;
    _VfdTable['A'] = 0b10111110;
    _VfdTable['B'] = 0b10001111;
    _VfdTable['C'] = 0b00010111;
    _VfdTable['c'] = 0b00001011;
    _VfdTable['D'] = 0b10101011;
    _VfdTable['E'] = 0b00011111;
    _VfdTable['F'] = 0b00011110;
    _VfdTable['G'] = 0b10010111;
    _VfdTable['H'] = 0b10101110;
    _VfdTable['I'] = 0b10100000;
    _VfdTable['i'] = 0b10010000;
    _VfdTable['J'] = 0b10100011;
    _VfdTable['L'] = 0b00000111;
    _VfdTable['N'] = 0b10110110;
    _VfdTable['n'] = 0b10001010;
    _VfdTable['m'] = 0b11001010;
    _VfdTable['O'] = 0b10110111;
    _VfdTable['o'] = 0b10001011;
    _VfdTable['P'] = 0b00111110;
    _VfdTable['Q'] = 0b10111100;
    _VfdTable['R'] = 0b00001010;
    _VfdTable['r'] = 0b00001010;
    _VfdTable['S'] = 0b10011101;
    _VfdTable['T'] = 0b00001111; 
    _VfdTable['U'] = 0b10100111;
    _VfdTable['Y'] = 0b10101101;
    _VfdTable['Z'] = 0b00111011;
    _VfdTable[' '] = 0b00000000;


}

void VFD::setCustomTable(const uint8_t* map, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        char character = (char)map[i * 2];
        uint8_t pattern = map[i * 2 + 1];

        setCustomChar(character, pattern);
    }
}



void VFD::setCustomChar(char c, byte pattern) {
    _VfdTable[c & 0x7F] = pattern;
}

void VFD::begin()
{
    pinMode(_dataPin, OUTPUT);
    pinMode(_clockPin, OUTPUT);
    pinMode(_latchPin, OUTPUT);
    digitalWrite(_latchPin, HIGH);
    digitalWrite(_clockPin, LOW);
    digitalWrite(_dataPin, LOW);

}

void VFD::printDisplay(const String& text){
    String _text = text;
    digitalWrite(_latchPin, LOW);
    char CurrentChar = ' ';
    if(_text.length() < _numberOfTubes){
        int blankCount = _numberOfTubes - _text.length();
        if((blankCount % 2) == 1){
            blankCount = blankCount -1;
            _text = _text + ' ';
        }
        blankCount = blankCount / 2;
        for(int i = 0 ; i < blankCount ; i++){ _text = _text + ' '; }
        for(int i = 0 ; i < blankCount ; i++){ _text = ' ' + _text; }     
    }
    for(int i = _numberOfTubes - 1 ; i >= 0 ; i--){
        CurrentChar = _text[i];
        shiftOut(_dataPin, _clockPin, MSBFIRST, _VfdTable[CurrentChar & 0x7F]);
    }
  digitalWrite(_latchPin, HIGH);
}


void VFD::printDisplayNC(const String& textNC){
    String _text = textNC;
    digitalWrite(_latchPin, LOW);
    char CurrentChar = ' ';
    for(int i = _numberOfTubes - 1 ; i >= 0 ; i--){
        if(i >= (int)_text.length()){
            CurrentChar = ' ';
        }else{
            CurrentChar = _text[i];
        }
        shiftOut(_dataPin, _clockPin, MSBFIRST, _VfdTable[CurrentChar & 0x7F]);
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

void VFD::update(){
    if(_scrollActive){
        unsigned long currentTime = millis();
        if(currentTime - _lastScrollTime >= _scrollInterval){
            _lastScrollTime = currentTime;
            updateScrollingText();
        }
    }
}

void VFD::printScrolling(const String& textScroll, unsigned int Interval){
    _textScroll = textScroll;
    for(int i = 0 ; i < _numberOfTubes ; i++){
        _textScroll = ' ' + _textScroll;
        _textScroll = _textScroll + ' ';
    }
    _scrollInterval = Interval;
    _scrollActive = true;
    _scrollIndex = 0;
    _lastScrollTime = millis();
    updateScrollingText();
}

void VFD::updateScrollingText(){
    String input = _textScroll;
    String FrameText = "";
    // When its about to pass mn it jumps a character to display the nn together at the same time, 
    // Example
    // Input text - Adricomn
    // Displayed text - Adriconn (It kinda looks like a m)
    // Display Sequence : 
    // -----A
    // ----Ad
    // ---Adr
    // --Adri
    // -Adric
    // Adrico
    // ricom ( nn )
    // icom-
    // com--
    // om---
    // m----
    int mPos = _scrollIndex + _numberOfTubes - 1;
    int nPos = _scrollIndex + _numberOfTubes;

    if (nPos < input.length() && input[mPos] == 'm' && input[nPos] == 'n') {
        _scrollIndex++;
    }
    //


    for(int FrameIndex = _scrollIndex; FrameIndex < _scrollIndex+_numberOfTubes ; FrameIndex++){
      FrameText += input[FrameIndex];
    }
    printDisplayNC(FrameText);

    // Stop once the entire text has scrolled completely off-screen
    if(_scrollIndex >= input.length() - _numberOfTubes){
        _scrollActive = false;
        _scrollIndex = 0;
        return;
    }
    _scrollIndex++;
}



void VFD::onAll(){
    byte _data = 0b11111111;
    digitalWrite(_latchPin, LOW);
    for(int i = _numberOfTubes - 1 ; i >= 0 ; i--){
        shiftOut(_dataPin, _clockPin, MSBFIRST, _data);
    }
    digitalWrite(_latchPin, HIGH);
}

void VFD::offAll(){
    byte _dataz = 0b00000000;
    digitalWrite(_latchPin, LOW);
    for(int i = _numberOfTubes - 1 ; i >= 0 ; i--){
        shiftOut(_dataPin, _clockPin, MSBFIRST, _dataz);
    }
    digitalWrite(_latchPin, HIGH);
} 