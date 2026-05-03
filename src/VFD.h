/*
    Simple Driver Library for IV-3 VFD Clock By Adricom
    Created by Adricom 25.08.2025
    Released on GitHub
*/


#ifndef VFD_h
#define VFD_h

#include <Arduino.h>

enum ScrollDirection{
    SCROLL_LEFT,
    SCROLL_RIGHT
};


class VFD
{
    public:
        VFD(int dataPin, int clockPin, int latchPin, int numberOfTubes);
        void begin();
        void print(const String& text, unsigned int Interval, bool looping = false, ScrollDirection direction = SCROLL_LEFT);
        void printDisplay(const String& textCenter);
        void printDisplayNC(const String& textNC);
        void printDisplayRaw(byte patterns[]);

        void stopScrolling(bool clearDisplay = false);
        void pauseScrolling();
        void resumeScrolling();
        void printScrolling(const String& textScroll, unsigned int Interval = 750, bool looping = false, ScrollDirection direction = SCROLL_LEFT);
        void printScrollIn(const String& textScroll, unsigned int Interval = 750, ScrollDirection direction = SCROLL_LEFT);
        void printScrollOut(unsigned int Interval = 750, ScrollDirection direction = SCROLL_LEFT, const String& textScroll = "");
        void update();

        void onAll();
        void offAll();
        bool isScrollingActive();
        int scrollingIndex();
        ScrollDirection scrollDirection();

        void setCustomChar(char c , byte pattern);
        template<size_t N>
        void setCustomTranslationTable(const uint8_t (&map)[N]){
            static_assert(N % 2 == 0, "mapping must contain pairs of (char, pattern)");
            setCustomTable(map, N/2);
        }
    

    private:
        String centerText(const String& text);
        void updateScrollingText();
        void printFrame(const String& text);
        void setCustomTable(const uint8_t* map, size_t count);
        int _dataPin;
        int _clockPin;
        int _latchPin;
        int _numberOfTubes;
        
        uint8_t _VfdTable[128];

        String _textScroll;
        String _unmodifiedTextScroll;
        int _scrollIndex = 0;
        unsigned long _scrollInterval = 750;
        unsigned long _lastScrollTime = 0;
        bool _scrollActive = false;
        bool _scrollLoop = false;
        ScrollDirection _scrollDirection = SCROLL_LEFT; // SCROLL_LEFT for left, SCROLL_RIGHT for right



};


#endif