master
======

Master repository for ENGR3199 includes C and Python source code and supporting files



Scons
======
A build tool that is used to compile the C script inside of each folder. Make sure the Scontruct file is configured correctly. 

To use, run scons inside the folder containing the Scontruct file and the C script. 

http://www.scons.org/download.php

bootloader
======
Loads the operating system into the pic microcontroller. Make sure the pic is in debug model by holding down sw1 when plugging the pic in. In order to upload a script:

bootloadercmd.py -i [hex-filename] to import the hex file into the flash buffer memory 
bootloadercmd.py -w to Erase the program memory of the connected PIC24FJ USB bootloader device, write the contents of the flash memory buffer, and verify the device if verify on write is enabled. 


blink.py
======
A modified version of the blink script from the original master repository. sw1 now toggles between the different colors. sw2 doubles the period of the blink and sw3 halves the period. 