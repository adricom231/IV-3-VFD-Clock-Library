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
        void printDisplay(String text);
        void printDisplayNC(String textNC);
        void printDisplayRaw(byte patterns[]);
        void onAll();
        void offAll();

        void setCustomChar(char c , byte pattern);

        template<size_t N>
        void setCustomTranslationTable(const char (&map)[N]){
            setCustomTable(map, N/2);
        }
    
    private:
        void setCustomTable(const char* map, size_t count);
        int _dataPin;
        int _clockPin;
        int _latchPin;
        int _numberOfTubes;

        byte _VfdTable[128];


};


#endif