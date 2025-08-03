# Handwriting-Helper
Helping children practice handwriting.

## Overview

This project uses an ESP32 based microcontroller with a touch screen and an Arduino Sketch to provide children with a way to practice handwriting. Letters are displayed on the touch screen which the child can trace with their finger. In addition to practicing handwriting, the child also improves their image recognition and motor skills. 

## Background

It takes a considerable amount of time and resources (pens, pencils, paper, coloring books) to learn to write. This method obviously works, and most people reading this information likely learned to write this way. Although I think effective methods don’t need to be replaced, I also wonder if technology could provide a more engaging learning environment.

There are handwriting practice Apps available for tablets and smartphones. However, young children and portable devices don’t always mix well. Even though the touch interface of such devices is intuitive, nuances of the interface are lost on young children. It is far too easy for a child to swipe up or down and end up in configuration screens, and your tablet is no longer in your native language, or worse. As intuitive as mobile device interfaces can be, error messages and confirmation windows are lost on users of this age group, leading to deeper issues.

## Project Goals

The main goals of this project were:

1. Have fun.
2. Use a small form factor device, ideal for small hands.
3. Implement an intuitive, but dedicated interface.
4. Create an engaging learning environment.

## What does it look like?

This is what the project looks like when it’s running:
<img width="1545" height="2000" alt="image" src="https://github.com/user-attachments/assets/ebdc850a-3cc2-4529-b3e7-a3de63bbaa8d" />
This shows a letter A that the user can trace over using their finger. There is a choice of 4 different drawing colors. There are 3 action buttons that will move to the last (previous) letter, reset the drawing area and move to the next letter.

Here’s what the letter looks like after it has been traced:
<img width="1545" height="2000" alt="image" src="https://github.com/user-attachments/assets/05672d6d-c1f7-443f-8883-cb2e76917dd8" />
Enhanced versions of the letters can be display that contain additional information. Here’s an enhanced A:
<img width="1545" height="2000" alt="image" src="https://github.com/user-attachments/assets/17f4ed53-bfaf-4446-9236-9eb4ba5ab74a" />
This reinforces an object (apple), the letter of the alphabet and its associated word. The child could also color the apple graphic as well as trace the letter, adding an element of fun. Here’s an enhanced B:
<img width="1545" height="2000" alt="image" src="https://github.com/user-attachments/assets/710564d7-1de0-410b-9ec2-71ddda9496e4" />
Color graphics can also be included, as well as object outlines. Here’s an enhanced G:
<img width="1545" height="2000" alt="image" src="https://github.com/user-attachments/assets/39643266-ca9f-4364-a394-83e899ac9d45" />
## Technical Overview

The project uses a Core2 AWS (https://shop.m5stack.com/products/m5stack-core2-esp32-iot-development-kit-for-aws-iot-edukit) Microcontroller from M5Stack (https://m5stack.com/). I just happen to have a few of these microcontrollers from another project I worked on.

The Core2 is 2.12 inches (54mm) square, and 0.6 inches (16mm) thick. This is an ideal form factor for small hands. The Core2 has a 2 inch (50.8mm) capacitive touch screen with a resolution of 240 x 320. This is not a large amount of screen real estate, but enough for this project.

An Arduino Sketch is loaded onto the Core2 that creates and reacts to buttons, displays the letters and tracks where the user touches the screen.

The Core2 has an SD Card reader. The project uses an SD Card which contains a some WAV files and a series of BMP files. The WAV files are played when the application is used for the first time. This provides instructions, in spoken English, on how to use the device. There are also some onscreen cues.

The WAV files are 16bit, 16KHz mono which makes them fairly small. Compared to other microcontrollers, the Core2 has a generous amount of memory. But, WAV files have to be loaded into memory to be played which can put a lot of pressure on memory.

The BMP files contain the letters and any additional words and graphics. The BMP files were scaled to 170 x 270 pixels to fit into the drawing area. The BMP files contain the plain alphabet are named A.bmp through Z.bmp. The enhanced versions, with additional graphics, are named A-enh.bmp through Z-enh.bmp. As the user presses the last or next buttons, a counter into the alphabet is decremented or incremented and added to an ASCII “A”. This gives the start letter for the file and then full filenames are created. If “A-enh.bmp” exists, it will be loaded and displayed, otherwise an attempt to load “A.bmp” will be made. The application will display a large red circle if no suitable BMP files can be found. This is a more meaning error mechanism for the target audience.

## What do I need?

You will need:

1. An M5Stack Core2 AWS to run the application. M5Stack have other 
   microcontrollers with a similar form factor, which are the regular 
   Core2 and a few versions of the CoreS3 series. The current application 
   uses a number of M5Stack APIs, but it’s possible the application could 
   be ported to another microcontroller with a touchscreen, SD Card reader 
   and speaker.
       
2. The application only uses the standard M5Stack and Core2 libraries. 
       
3. An 8GB or 16GB SD Card to hold the WAV and BMP files. The SD Card must
   be formatted for FAT32. The Core2 is only supposed to support up to 16GB
   SD Cards. I was able to use a 32GB card formatted with FAT32. The WAV 
   (9 files) and BMP (26 regular, 26 enhanced) example files are only ~12MB 
   of data. I am not sure what would happen if I tried to read past 16GB. 
   So, it would be better use a lower capacity SD Card.
       
4. A PC with Windows, Linux, or a Mac to install the Arduino IDE which can
   be downloaded here https://www.arduino.cc/en/software/. 
       
5. A USB A to USB C cable to connect the PC or Mac to the Core2.
       
6. The git utility to access the GIT repository 
   (git clone https://github.com/davygotgit/Handwriting-Helper.git) or 
   visit https://github.com/davygotgit/Handwriting-Helper and download
   a ZIP file.
       
7. The GIT repository contains example BMP files. If you want to replace 
   these files, you will need a suitable graphs program that can create 
   BMP files that scaled to 170 x 270 pixels.

## How do I install and configure the tools?

Here are some instructions for downloading and installing GIT https://github.com/git-guides/install-git.

Here are some instructions on how to download and install the Arduino IDE https://docs.arduino.cc/software/ide-v2/tutorials/getting-started/ide-v2-downloading-and-installing/.

M5Stack have an excellent quick start guide here https://docs.m5stack.com/en/arduino/m5core2/program. I don’t recall having to install any driver on my version of Ubuntu 24.04.2 LTS. It’s possible this is already included in the kernel. On Linux you must add your account to the dialout group by running the following bash command:

	sudo usermod -a -G dialout <your_account>

For example, if your user account is fantasticfred:

	sudo usermod -a -G dialout  fantasticfred

You must log out your current session and log back in again for this change to become active.

For Linux systems, I would first see if your Arduino IDE can see the Core2 device before attempting to install any drivers.

You know you are connected to the Core2 if you see something similar to the following status (bottom right) in the Arduino IDE:

<img width="519" height="67" alt="image" src="https://github.com/user-attachments/assets/52449c2d-199f-4a75-8ae6-63964bf853a8" />

## How do I build and install the application?

You need to download the code from the GIT repository. This can be done by visiting https://github.com/davygotgit/Handwriting-Helper and downloading, and then extracting, a ZIP file or by running the following terminal command from bash, a Windows Command Prompt or any suitable GIT access tool:

	git clone https://github.com/davygotgit/Handwriting-Helper.git

When you have the repository downloaded, copy all the files from the data subdirectory (folder) to your SD Card. The files must be copied to the root of the SD Card, do not place the files in any subdirectories. You can and then insert the SD Card into the Core2. The application will start reading from the SD Card as soon as the Core2 is powered on.

There are a couple of options to build the application for the first time. Option 1 is:

1. Start the Arduino IDE.
2. Create a new project using the File -> New Sketch menu option.
3. Save the project using the name writinghelper by using the
   File -> Save menu option.
5. Open the src/writinghelper.ino file, from repository, using
   another editor, and copy/paste the contents over the skeleton project.
       
Option 2 is:
       
1. Start the Arduino IDE.
2. Create a new project using the File -> New Sketch menu option.
3. Save the project using the name writinghelper by using the
   File -> Save menu option.
5. Use the Sketch -> Show Sketch Folder menu option to get the
   location of the project (Sketch location). This will be similar
   to Home/Arduino/writinghelper on Linux.
7. Close the IDE.
8. Copy the writinghelper.ino file from the src subdirectory of the
   repository to the Sketch location.
10. Start the Arduino IDE and load the writinghelper project.
       
Once you have the initial project saved, you can just load it from File -> Open Recent menu option.

With the Sketch loaded, connect the Core2 using the USB A to USB C cable,. Ensure the M5Core2 board is selected and the USB port shows a connected status. Press the Upload button on the toolbar. The Sketch will be compiled and transferred to the Core2. The application will start after the transfer completes.

## Were there any challenges creating this project?

I have used Arduino IDE and M5Stack for several years, and I am very comfortable with their microcontrollers and development environment. However, I haven’t done much with M5Stack and Arduino IDE from mid 2023 to mid 2025 for this project. A number of API calls changed in the M5Stack library. It took me a while to find the correct code to initialize the SD Card reader.

In this time frame, it seems there has been a change to the order in which certain header files must be included. This also took a little research and is documented in the Sketch.

When playing any sounds through the Core2’s speaker, it is important to make sure the speaker is available for use. Submitting a WAV file will tie up the speaker for a few seconds. You should not attempt to play any other sounds until the speaker is ready, or you intentionally stop the current playback to submit another sound.

## Generating the demo files

The GIT repository contains enough files to get a good sense of how to use the application. However, you can create your own versions if you like.

The WAV files were created with the espeak utility on Linux using the following command line options:

	espeak -s1 -v mb-en1 <text> -w <file>

Unless you change the source code, the WAV files must use the same names in the repository. Here’s a table that describes the content of each WAV file:


|File (\<file\>)|Content (\<text\>)|
|-------------|----------------|
|Welcome.wav|Hello and welcome.|
|Practice.wav|Use me to practice writing.|
|Selectcolor.wav|You can select a color.|
|Lastletter.wav|Move to the last letter.|
|Resetletter.wav|Reset the letter.|
|Nextletter.wav|Move to the next letter.|
|Trace.wav|Trace the letter with your finger.|
|Ready.wav|Ready to start|

The regular BMP files were created with a pair of bash scripts. The following script creates a series of .TXT files (A.txt through Z.txt) that just contain the letter:

```bash
alpha="ABCDEFGHIJKLMNOPQRSTUVWXYZ"
for (( i=0; i<${#alpha}; i++ )); do
  ch=${alpha:$i:1}
  echo "$ch" > $ch.txt
done
```

Once the text files are created, the following bash script was used to create the BMP files:

```bash
for i in `ls *.txt`
do
  ch=`cat $i`
  convert -negate -size 170x270 -font DejaVu-Sans-Mono -type TrueColor -pointsize 200 caption:"$ch" $ch.bmp
done
```
The convert utility is part of the ImageMagick package on Linux.

The files A-enh.bmp through Z-enh.bmp were manually created in a graphical editing tool.

## Next Steps

There are few things I would like to try in the future:

1. The WAV files used for the demo phase are very robotic. I would
   like to generate WAV files that sound better.
   
2. When using the enhanced BMP files, create WAV files that say things
   like “A is for apple” to reinforce the learning experience.
       
3. There is enough room to add an extra action button. This would be
   used to replay the demo and turn the speaker on or off. I suspect
   that hearing “A is for apple” etc., after a while, would become annoying.
       
4. Even an 8GB SD Card could hold a lot of BMP files. The current BMP
   files were easy to create and use plain fonts. Children, sometimes,
   have issues identifying letters when written in different fonts. This
   is especially true with lowercase letters. I might create an alternative
   set of BMP files with different fonts that present the child with a
   different challenge.
        
5. The current flow is logical e.g. start at A and progress to Z. I’m not
   entirely sure about this, but I wonder if some randomization in the
   display of the letters would be engaging or just confusing.
       
6. When the last or next buttons are pressed, I would like to validate
   the traced letter to see how close the child got to the displayed letter.
   This would require a K-Nearest Neighbors (KNN) implementation. However,
   I’m not sure how this will go on the Core2 until I try it. I think
   it would be an interesting feature. There may be something in tracking
   progress like this.



