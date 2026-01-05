/*
    Simple Driver Library for IV-3 VFD Clock By Adricom
    Created by Adricom 25.08.2025
    Released on GitHub
*/


#ifndef VFD_h
#define VFD_h

#include <Arduino.h>

class VFD
{
    public:
        VFD(int dataPin, int clockPin, int latchPin, int numberOfTubes);
        void begin();
        void printDisplay(const String& text);
        void printDisplayNC(const String& textNC);
        void printDisplayRaw(byte patterns[]);
        void printScrolling(const String& textScroll, unsigned int Interval);
        void update();

        void onAll();
        void offAll();

        void setCustomChar(char c , byte pattern);
        template<size_t N>
        void setCustomTranslationTable(const uint8_t (&map)[N]){
            static_assert(N % 2 == 0, "mapping must contain pairs of (char, pattern)");
            setCustomTable(map, N/2);
        }
    

        private:
        void updateScrollingText();
        void setCustomTable(const uint8_t* map, size_t count);
        int _dataPin;
        int _clockPin;
        int _latchPin;
        int _numberOfTubes;
        byte _VfdTable[128];

        String _textScroll;
        int _scrollIndex = 0;
        unsigned long _scrollInterval;
        unsigned long _lastScrollTime = 0;
        bool _scrollActive = false;

};


#endif