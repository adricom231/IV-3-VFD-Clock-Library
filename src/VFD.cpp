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
    _VfdTable['!'] = 0b01100000;

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

void VFD::begin(){
    pinMode(_dataPin, OUTPUT);
    pinMode(_clockPin, OUTPUT);
    pinMode(_latchPin, OUTPUT);
    digitalWrite(_latchPin, HIGH);
    digitalWrite(_clockPin, LOW);
    digitalWrite(_dataPin, LOW);
    offAll();
}

void VFD::print(const String& text, unsigned int Interval, bool looping, ScrollDirection direction){
    if(text.length() <= _numberOfTubes){
        printDisplay(text);
    }else{
        printScrolling(text, Interval, looping, direction);
    }
}

void VFD::printDisplay(const String& textCenter){
    if(isScrollingActive()){
        pauseScrolling();
    }
    String _text = centerText(textCenter);

    // Print
    printFrame(_text);
}


void VFD::printDisplayNC(const String& textNC){
    if(isScrollingActive()){
        pauseScrolling();
    }
    printFrame(textNC);
}


void VFD::printDisplayRaw(byte patterns[]){
    if(isScrollingActive()){
        pauseScrolling();
    }
    digitalWrite(_latchPin, LOW);

    // Print raw patterns 
    for(int i = _numberOfTubes - 1 ; i >= 0 ; i--){
        shiftOut(_dataPin, _clockPin, MSBFIRST, patterns[i]);
    }

    digitalWrite(_latchPin, HIGH);
}

bool VFD::isScrollingActive(){
    return _scrollActive;
}

int VFD::scrollingIndex(){
    return _scrollIndex;
}

ScrollDirection VFD::scrollDirection(){
    return _scrollDirection;
}

void VFD::printFrame(const String& text){
    char CurrentChar;

    // Print, allign left, no centering
    digitalWrite(_latchPin, LOW);
    for(int i = _numberOfTubes - 1 ; i >= 0 ; i--){
        if(i >= (int)text.length()){
            CurrentChar = ' ';
        }else{
            CurrentChar = text[i];
        }
        shiftOut(_dataPin, _clockPin, MSBFIRST, _VfdTable[CurrentChar & 0x7F]);
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

void VFD::printScrollIn(const String& textScroll, unsigned int Interval, ScrollDirection direction){
    //centerText Does the centering for NumberOfTubes 
    _textScroll = centerText(textScroll);
    _unmodifiedTextScroll = textScroll;
    _scrollDirection = direction;

    // Add  padding (numberOfTubes) depending on the direction
    if (direction == SCROLL_LEFT){
        for(int i = 0 ; i < _numberOfTubes ; i++){
            _textScroll = ' ' + _textScroll;
        }
    }else{
        for(int i = 0 ; i < _numberOfTubes ; i++){
            _textScroll = _textScroll + ' ';
        }
    }
    


    _scrollInterval = Interval;
    _scrollLoop = false;
    _scrollActive = true;
    _scrollDirection = direction;
    _scrollIndex = 0;
    _lastScrollTime = millis();
    updateScrollingText();
}


void VFD::printScrollOut(unsigned int Interval, ScrollDirection direction, const String& textScroll){
    if(!textScroll.isEmpty()){
        _textScroll = centerText(textScroll);
    }else{
        _textScroll = centerText(_unmodifiedTextScroll);
    }


    _scrollDirection = direction;
    

    // Add  padding (numberOfTubes) depending on the direction
    if (direction == SCROLL_RIGHT){
        for(int i = 0 ; i < _numberOfTubes ; i++){
            _textScroll = ' ' + _textScroll;
        }
    }else{
        for(int i = 0 ; i < _numberOfTubes ; i++){
            _textScroll = _textScroll + ' ';
        }
    }
    _scrollInterval = Interval;
    _scrollLoop = false;
    _scrollActive = true;
    _scrollDirection = direction;
    _scrollIndex = 0;
    _lastScrollTime = millis();
    updateScrollingText();
}


void VFD::printScrolling(const String& textScroll, unsigned int Interval, bool looping, ScrollDirection direction){
    _textScroll = centerText(textScroll);
    _scrollDirection = direction;

    for(int i = 0 ; i < _numberOfTubes ; i++){
        _textScroll = ' ' + _textScroll + ' ';
    }

    _scrollInterval = Interval;
    _scrollLoop = looping;
    _scrollActive = true;
    _scrollDirection = direction;
    _scrollIndex = 0;
    _lastScrollTime = millis();
    updateScrollingText();
}

void VFD::stopScrolling(bool clearDisplay){
    _scrollActive = false;
    _scrollIndex = 0;
    if(clearDisplay){
        offAll();
    }
}

void VFD::pauseScrolling(){
    _scrollActive = false;
}

void VFD::resumeScrolling(){
    if(!_scrollActive){
        _scrollActive = true;
        _lastScrollTime = millis();
    }
}

void VFD::updateScrollingText(){
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

    if(_scrollDirection == SCROLL_LEFT){
        int mPos = _scrollIndex + _numberOfTubes - 1;
        int nPos = _scrollIndex + _numberOfTubes;
        // Check if the current frame is about to display 'm' followed by 'n', if so, skip the 'm' to display 'nn' together
        if (nPos < _textScroll.length() && _textScroll[mPos] == 'm' && _textScroll[nPos] == 'n') {
            _scrollIndex++;
        }
    
        // Take (numberOfTubes) characters from the input starting at _scrollIndex to get current frame
        for(int FrameIndex = _scrollIndex; FrameIndex < _scrollIndex+_numberOfTubes ; FrameIndex++){
            FrameText += _textScroll[FrameIndex];
        }
    }
    else if(_scrollDirection == SCROLL_RIGHT){

        int nPos = _textScroll.length() -_scrollIndex - _numberOfTubes;
        int mPos = _textScroll.length() -_scrollIndex - _numberOfTubes - 1;
        // Check if the current frame is about to display 'm' followed by 'n', if so, skip the 'm' to display 'nn' together
        if (mPos >= 0 && _textScroll[mPos] == 'm' && _textScroll[nPos] == 'n') {
            _scrollIndex++;
        }
        
        for(int FrameIndex = ( _textScroll.length() -_scrollIndex - _numberOfTubes); FrameIndex < ( _textScroll.length() -_scrollIndex ); FrameIndex++){
            FrameText += _textScroll[FrameIndex];

        }
    }
    // Display the current frame
    printFrame(FrameText);

    //Reset Loop if loop active and reached the end of the text, else end scrolling if not looping
    if(_scrollIndex >= _textScroll.length() - _numberOfTubes){
        if(!_scrollLoop){
            _scrollActive = false;
        }
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



String VFD::centerText(const String& text){
    String _ctrText = text;
    if(_ctrText.length() < _numberOfTubes){
        // Make the text lenght even
        int blankCount = _numberOfTubes - _ctrText.length();
        if((blankCount % 2) == 1){
            blankCount = blankCount -1;
            _ctrText = _ctrText + ' ';
        }
        
        // Add padding so that it fills (numberOfTubes)
        blankCount = blankCount / 2;
        for(int i = 0 ; i < blankCount ; i++){ _ctrText = ' ' + _ctrText + ' ';}  
    }
    return _ctrText;
}