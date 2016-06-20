# MOVIArduinoAPI
This is the official repository of the Arduino API for MOVI.
For more information check out http://www.audeme.com/MOVI

#VERSION 1.01
Fixed uppercase bug for stream.h in Linux.

#Version 1.02
Fixed stability issue with serial connection
Added NestedDialog example

#Version 1.03
Added 7 more examples

#Version 1.04
Added changes from Dylan, as of forum post: http://www.audeme.com/forum.html#/20160525/pull-request-for-moviarduinoapi-5233546/
to allow the use of HardwareSerial as well as speeding up the ask() method.

#Version 1.05
Fixed bug in ask(F(<string>)) method that would copy the flashed string back to String memory just for checking if it's greater than zero. Added unparametered ask() function.
