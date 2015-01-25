elecanisms_teamAWESOME
======

Master repository for ENGR3199. Includes C and Python source code and supporting files
Maintained by Derek Redfern and Louis Yi
Olin College - Elecanisms - Spring 2015

Scons
======
A build tool that is used to compile the C script inside of each folder. Make sure the Scontruct file is configured correctly. 

To use, run `scons` inside the folder containing the Scontruct file and the C script. 

http://www.scons.org/download.php

bootloader
======
Loads the compiled binary into the PIC microcontroller. Make sure the PIC is in debug mode by holding down sw1 when plugging the board in. In order to upload a script:

`python bootloadercmd.py -i [hex-filename] -w`

-i imports the hex file into the flash buffer memory 

-w erases the program memory of the connected PIC24FJ USB bootloader device, write the contents of the flash memory buffer, and verify the device if verify on write is enabled. 


blink.py
======
A modified version of the blink script from the original master repository. sw1 now toggles between the different LED colors. sw2 doubles the period of the blink and sw3 halves the period. 