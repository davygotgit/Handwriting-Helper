//
//  This sketch uses an M5Stack Core2 (or CoreS3) to give children
//  handwriting practice.
//
//  In summary, the Core2 is used to display BMP files from an SD
//  Card. The images can be traced with a finger, which leaves
//  a color trail. This allows handwriting practice.
//
//  License:  MIT. See the LICENSE file in the project root for
//            more details.
//

//  These files must be included before M5Unified.h and M5GFX.h
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

//  M5Stack headers
#include <M5Unified.h>
#include <M5GFX.h>

//  Used to initialize the SPI interface  
#define SD_SPI_CS_PIN   4
#define SD_SPI_SCK_PIN  18
#define SD_SPI_MISO_PIN 38
#define SD_SPI_MOSI_PIN 23

//  Define this to a value of 1 to only run the demo once
#define RUN_DEMO_ONCE   0

//  Button sizing
const int buttonSize           = 50;
const int halfBtnSize          = buttonSize / 2;

//  Max letter in the alphabet (Z)
const int maxLetter             = 26;

//  Max seconds to wait for the speaker to become free
const int speakerWait          = 5000;

//  Max seconds to wait for some demo stages to complete
const int stageWait             = 2000;

//  The BMP display X position 
const int BMPX                  = 20;

//  Size of the line tracing the letter
const int drawingSize           = 4;

//  Border pixels around the drawing area
const int borderX               = 20;
const int borderY               = 4;

//  Screen dimensions
int       screenWidth;
int       screenHeight;

//  Define color buttons
LGFX_Button colorButtons    [4];
const int buttonColors      []  = {RED, GREEN, BLUE, YELLOW};
const int numColors             = sizeof(colorButtons) / sizeof (colorButtons [0]);
int       currentColor          = buttonColors [0];
int       lastButton            = 0;

//  Action buttons
LGFX_Button actionButtons   [3];
const char *actionText      []  = {"<", "X", ">"};
const int   numActions          = sizeof(actionButtons) / sizeof (actionButtons [0]);
enum actions : int {backButton, deleteButton, nextButton};

//  Alphabet counter. Start at A, end at Z
int       currentLetter         = 0;

//  State values for the application
enum states : int {demoWelcome, demoPractice, demoColor, demoLast, demoDelete, demoNext, demoTrace, demoDone, drawingAllowed};
states     currentState         = demoWelcome;

//  WAV files for the demo
const char *demoWAVNames []     = { 
                                    "/welcome.wav",     "/practice.wav",   
                                    "/selectcolor.wav", "/lastletter.wav", 
                                    "/resetletter.wav", "/nextletter.wav", 
                                    "/trace.wav",       "/ready.wav"
                                  };

#if (RUN_DEMO_ONCE)
const char  *demoDoneFile       = "/demodone";
#endif  // RUN_DEMO_ONCE

//  WAV data buffer
uint8_t     *WAVBuffer          = nullptr;


//  Clear the drawing area
void ClearDrawingArea (void)
{
  M5.Display.fillRect(0, 0, screenWidth - buttonSize, screenHeight - buttonSize, BLACK);
}


//  Clear stray drawing pixels
//
//  There are some pixels to the left of the main drawing area. We
//  could clear the entire drawing area, but that causes to much
//  flicker
//
void ClearStrayPixels (void)
{
    M5.Display.fillRect(0, 0, BMPX, screenHeight - buttonSize, BLACK);
}


//  Display the current letter by reading a BMP the SD card
void DisplayLetter (void)
{
  //  Generate the filename for the BMP which will be
  //  A.bmp through Z.bmp
  //
  //  There might be some enhanced BMP files with extra
  //  graphics which will be A-enh.bmp through Z-enh.bmp

  //  Check for the enhanced version first - it's okay if
  //  the file is not there as we will check for the
  //  regular BMP file
  char letter = 'A' + currentLetter;
  char enhName [16];
  snprintf(enhName, sizeof(enhName), "/%c-enh.bmp", letter);
  if (SD.exists(enhName))
  {
    //  Display the enhanced file
    ClearStrayPixels();
    M5.Display.drawBmpFile(SD, enhName, BMPX, 0);
    return;
  }

  //  Check for the regular BMP
  char BMPName [8];
  snprintf(BMPName, sizeof(BMPName), "/%c.bmp", letter);

  //  Make sure the file exists
  if (!SD.exists(BMPName))
  {
    //  Output a visual indicator that something went wrong
    M5.Lcd.fillCircle(screenWidth / 2, screenHeight / 2, buttonSize, RED);
    return;
  }

  //  Display the file
  ClearStrayPixels();
  M5.Display.drawBmpFile(SD, BMPName, BMPX, 0);
}


//  Wait for the speaker to finish playing the
//  current WAV file
bool WaitForSpeaker (void)
{
  //  Give the current WAV file a few seconds to complete
  unsigned long speakerTMO = millis() + speakerWait;

  while (M5.Speaker.isPlaying())
  {
    if (millis() > speakerTMO)
    {
      //  Don't wait forever
      return false;
    }

    delay(100);
  }

  return true;
}


//  Play a WAV file from the SD Card
bool PlayWav (const char *WAVFile)
{
  if (!WaitForSpeaker())
  {
    //  Something went wrong - let the caller deal with this
    M5.Speaker.stop();
    return false;
  }
 
  //  Don't create a hard error if the WAV file is missing. This
  //  is a nice to have and the application will still work
  if (SD.exists(WAVFile))
  {
    //  We need to open and read the contents of the WAV file
    if (WAVBuffer)
    {
      //  Free any old buffer first
      free(WAVBuffer);

      //  Prevent double free
      WAVBuffer = nullptr;
    }

    //  Open the file
    File WAVHandle = SD.open(WAVFile);
 
    //  We need to allocate a buffer for the entire WAV file
    //
    //  Note: This buffer will be freed when the WAV file stops
    //        playing
    size_t WAVSize = WAVHandle.size();
    WAVBuffer = (uint8_t *) malloc(WAVSize);

    //  Did we get the buffer?
    if (!WAVBuffer)
    {
      //  No - bail without a hard error
      return false;
    }

    //  Read the WAV file
    size_t bytesRead = WAVHandle.read(WAVBuffer, WAVSize);
    if (bytesRead != WAVSize)
    {
      //  Did not get the correct number of bytes
      free(WAVBuffer);
      WAVBuffer = nullptr;
      return false;
    }

    //  Close the file
    WAVHandle.close();
    
    //  Play the WAV file
    M5.Speaker.playWav(WAVBuffer);
  }

  return true;
}


//  Highlight the current demo action button
void HighlightAction (void)
{
  //  Remove any old indicators
  ClearDrawingArea();

  //  Calculate where the action button indicators should be
  const int buttonsOnX  = (screenWidth - buttonSize) / numActions;
  const int actionY     = screenHeight - buttonSize - halfBtnSize - borderY;
  const int actionX     = halfBtnSize + (buttonsOnX * (currentState - demoLast));

  //  Draw a circle above the action button
  M5.Display.fillCircle(actionX, actionY, halfBtnSize, WHITE);
}


//  Run the demo. This happens the first time the application
bool RunDemo (void)
{
  //  We need some stages to be able to display
  //  information for a few seconds
  static unsigned long  stateTMO    = 0;

  //  Time to check the state?
  if (stateTMO > 0
  &&  stateTMO < millis())
  {
    //  No, exit and re-try
    return true;
  }

  //  Play the WAV file associated with this stage
  if (!PlayWav(demoWAVNames [currentState]))
  {
    //  Something went wrong - let the caller handle this
    return false;
  }

  switch (currentState)
  {
    case demoWelcome:
      //  Next stage
      currentState = demoPractice;
      break;

    case demoPractice:
      //  Next stage
      currentState = demoColor;
      break;

    //  The color demo needs an indicator to highlight the buttons
    case demoColor:
    {
      //  Set Y position for markers next to the color buttons
      const int buttonsOnY  = (screenHeight - buttonSize) / numColors;
      const int colorX      = screenWidth - buttonSize - halfBtnSize - borderX;
      int       colorY      = halfBtnSize;
 
      //  Draw a circle next to each of the color buttons
      for (int i = 0; i < numColors; i ++)
      {
        //  Draw the initial circle
        M5.Display.fillCircle(colorX, colorY, halfBtnSize, WHITE);

        //  Let it sink in
        delay(1000);

        //  Remove the circle
        M5.Display.fillCircle(colorX, colorY, halfBtnSize, BLACK);

        //  Next Y position
        colorY += buttonsOnY;
      }

      //  Leave the indicators up for a few seconds
      stateTMO = millis() + stageWait;

      //  Next stage
      currentState = demoLast;
      break;
    }

    case demoLast:
      //  Bring attention to this action button
      HighlightAction();

      //  Leave the indicators up for a few seconds
      stateTMO = millis() + stageWait;

      //  Next stage
      currentState = demoDelete;
      break;

    case demoDelete:
      //  Bring attention to this action button
      HighlightAction();

      //  Leave the indicators up for a few seconds
      stateTMO = millis() + stageWait;

      //  Next stage
      currentState = demoNext;
      break;

    case demoNext:
      //  Bring attention to this action button
      HighlightAction();

      //  Leave the indicators up for a few seconds
      stateTMO = millis() + stageWait;

      //  Next stage
      currentState = demoTrace;
      break;

    //  Last stage of the actual demo
    case demoTrace:
      //  Output the BMP for the current letter (A)
      DisplayLetter();

      //  Draw a rectangle around the BMP
      M5.Display.drawRect(0, 0, screenWidth - buttonSize - 1, screenHeight - buttonSize - 1, WHITE);

      //  Leave the rectangle up for a few seconds
      stateTMO = millis() + stageWait;

      //  Next stage
      currentState = demoDone;
      break;

    //  Demo complete
    case demoDone:
    {
      //  Clear the rectangle around the BMP
      M5.Display.drawRect(0, 0, screenWidth - buttonSize - 1, screenHeight - buttonSize - 1, BLACK);

      //  Wait for the last WAV file to play
      WaitForSpeaker();

      //  Reset the drawing area
      DisplayLetter();

      //  Free off any WAV buffer
      if (WAVBuffer)
      {
        free(WAVBuffer);
        WAVBuffer = nullptr;
      }

#if (RUN_DEMO_ONCE)
      //  Create the file that tells us not to run the
      //  demo the next time the Core2 is powered on
      //
      //  Note: We fail silently here as the worst thing
      //        that will happen is the demo runs again
      //
      File demoHandle = SD.open(demoDoneFile, FILE_WRITE);
      if (demoHandle)
      {
        demoHandle.println("Done!");
        demoHandle.close();
      }
#endif  // RUN_DEMO_ONCE

      //  Play the read to start WAV
      currentState = drawingAllowed;
      break;
    }
  }

  return true;
}


void setup (void) 
{
  M5.begin();

  //  Configure display
  M5.Display.setRotation(0);
  M5.Lcd.fillScreen(BLACK);

  //  Get screen dimensions
  screenWidth   = M5.Lcd.width();
  screenHeight  = M5.Lcd.height();

  //  Enable the speaker and set max volume
  M5.Speaker.begin();
  M5.Speaker.setVolume(255);
  
  // Initialize SD card
  SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);
  if (!SD.begin(SD_SPI_CS_PIN, SPI, 25000000))
  {
    //  Visual indicator that something is wrong
    M5.Lcd.fillScreen(RED);
    while (true);
  }

  //  Set Y position for color buttons. These are on the RHS of the screen
  const int buttonsOnY  = (screenHeight - buttonSize) / numColors;
  const int colorX      = screenWidth - halfBtnSize;
  int       colorY      = halfBtnSize;
 
  //  Add color buttons
  for (int i = 0; i < numColors; i ++)
  {
    //  The default color is the red button, which we highlight
    int outlineColor = (i == 0) ? WHITE : buttonColors [i];

    //  Get the other button colors
    int color = buttonColors [i];

    //  Create the button
    colorButtons [i].initButton(&M5.Lcd, colorX, colorY, buttonSize, buttonSize, outlineColor, color, color, " ", 4, 4);
    colorButtons [i].drawButton();

    //  Next Y position
    colorY += buttonsOnY;
  }

  //  Set X position for action buttons. These are at the bottom of the screen
  const int buttonsOnX  = (screenWidth - buttonSize) / numActions;
  const int actionY     = screenHeight - halfBtnSize;
  int       actionX     = halfBtnSize;

  //  Add action buttons
  for (int i = 0; i < numActions; i ++)
  {
    actionButtons [i].initButton(&M5.Lcd, actionX, actionY, buttonSize, buttonSize, BLACK, WHITE, BLACK, actionText [i], 4, 4);
    actionButtons [i].drawButton();

    //  Next X positon
    actionX += buttonsOnX;
  }

#if (RUN_DEMO_ONCE)
  if (SD.exists(demoDoneFile))
  {
    //  Don't run the demo again, if the is not the
    //  first use of the application
    currentState = drawingAllowed;
  }
#endif  // RUN_DEMO_ONCE
}


void loop (void) 
{
  M5.update();

  //  Are we allowed to draw?
  if (currentState != drawingAllowed)
  {
    //  No - demo or welcome WAV
    if (!RunDemo())
    {
      //  Something went wrong in the demo, so let the user
      //  start drawing
      currentState = drawingAllowed;

      //  Display the initial letter (A)
      DisplayLetter();

      //  Clean up any dangling WAV buffers
      if (WAVBuffer)
      {
        free(WAVBuffer);
        WAVBuffer = nullptr;
      }
    }

    //  Done for now
    return;
  }

  //  Get information about where the user touched the screen
  auto touchDetail = M5.Touch.getDetail();

  if (touchDetail.isPressed())
  {
    //  Check if the press is in the drawing or button
    //  areas the screen
    if (touchDetail.x < screenWidth - buttonSize - borderX
    &&  touchDetail.y < screenHeight - buttonSize - borderY)
    {
        M5.Display.fillCircle(touchDetail.x, touchDetail.y, drawingSize, currentColor);
    }
    else
    {
      //  Handle color buttons first
      for (int i = 0; i < numColors; i ++)
      {
        if (colorButtons [i].contains(touchDetail.x, touchDetail.y))
        {
          //  Ignore multiple presses on the same button
          if (i == lastButton)
          {
            return;
          }

          //  De-select the last button
          colorButtons [lastButton].setOutlineColor(buttonColors [lastButton]);
          colorButtons [lastButton].drawButton();

          //  Select this button
          colorButtons [i].setOutlineColor(WHITE);
          colorButtons [i].drawButton();

          //  Set the color and last button pressed
          lastButton    = i;
          currentColor  = buttonColors [i];

          //  We are done
          return;
        }
      }

      //  Now action buttons 
      for (int i = 0; i < numActions; i ++)
      {
        if (actionButtons [i].contains(touchDetail.x, touchDetail.y))
        {
          //  See what action the user wants
          if (i == backButton)
          {
              //  Move to the previous letter
              currentLetter --;
              if (currentLetter < 0)
              {
                //  Stop at A
                currentLetter = 0;
              }
          }
          else
          if (i == nextButton)
          {
            //  Move to the next letter
            currentLetter ++;
            if (currentLetter >= maxLetter)
            {
              //  Stop at Z
              currentLetter = maxLetter - 1;
            }
          }

          //  Delete or moving to another letter. Display the
          //  current letter  
          DisplayLetter();

          //  We are done
          return;
        }
      }
    }
  }
} 