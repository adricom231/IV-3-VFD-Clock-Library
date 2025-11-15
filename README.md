# IV-3-VFD-Clock-Library
Library for my IV-3 Clock
It allows to display text and custom data very easly

To display text with centering use .printDisplay()

To display text without centering use .printDisplayNC()

To display custom data use .printDisplayRaw() With predefined data in bytes

EXAMPLE

vfd.printDisplay("123456")

  
byte pattern[6] = {
    B10110111,
    B00001000,
    B00010111,
    B10110001,
    B00001000,
    B10110111
 };
 
 vfd.printDisplayRaw(pattern);
