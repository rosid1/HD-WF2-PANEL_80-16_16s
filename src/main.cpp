#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <Adafruit_GFX.h>
#include "ESP32-VirtualMatrixPanel-I2S-DMA.h"


// WF2 GPIO Configuration
#define WF2_X1_R1_PIN 2
#define WF2_X1_R2_PIN 3
#define WF2_X1_G1_PIN 6
#define WF2_X1_G2_PIN 7
#define WF2_X1_B1_PIN 10
#define WF2_X1_B2_PIN 11
#define WF2_X1_E_PIN 21

#define WF2_A_PIN 39
#define WF2_B_PIN 38
#define WF2_C_PIN 37
#define WF2_D_PIN 36
#define WF2_OE_PIN 35
#define WF2_CLK_PIN 34
#define WF2_LAT_PIN 33

#define WF2_BUTTON_TEST 17 // Test key button on PCB, 1=normal, 0=pressed
#define WF2_LED_RUN_PIN 40 // Status LED on PCB
#define WF2_BM8563_I2C_SDA 41 // RTC BM8563 I2C port
#define WF2_BM8563_I2C_SCL 42
#define WF2_USB_DM_PIN 19
#define WF2_USB_DP_PIN 20

#define PANEL_RES_X 80 // Number of pixels wide of each INDIVIDUAL panel module.
#define PANEL_RES_Y 32 // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1 // Total number of panels chained one to another

#define NUM_ROWS 1 // Number of rows of chained INDIVIDUAL PANELS
#define NUM_COLS 1 // Number of INDIVIDUAL PANELS per ROW

// placeholder for the matrix object
MatrixPanel_I2S_DMA *dma_display = nullptr;

// placeholder for the virtual display object
VirtualMatrixPanel   *FourScanPanel = nullptr;

void led_scroll_right(const char *text, int speed);
void led_scroll_left(const char *text, int speed);
void led_scroll_up(const char *text,int speed);
void led_scroll_down(const char *text,int speed);
void display_switching_texts();

void setup () {
Serial.begin(115200);
HUB75_I2S_CFG::i2s_pins _pins_x1 = {WF2_X1_R1_PIN, WF2_X1_G1_PIN, WF2_X1_B1_PIN, WF2_X1_R2_PIN, WF2_X1_G2_PIN, WF2_X1_B2_PIN, WF2_A_PIN, WF2_B_PIN, WF2_C_PIN, WF2_D_PIN, WF2_X1_E_PIN, WF2_LAT_PIN, WF2_OE_PIN, WF2_CLK_PIN};

HUB75_I2S_CFG mxconfig(
PANEL_RES_X, // module width
PANEL_RES_Y, // module height
PANEL_CHAIN, // Chain length
_pins_x1 // pin mapping for port X1
);

// OK, now we can create our matrix object
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);

  // let's adjust default brightness to about 75%
  // dma_display->setBrightness8(255);    // range is 0-255, 0 - 0%, 255 - 100%

  // Allocate memory and start DMA display
  if ( not dma_display->begin() )
    Serial.println("****** !KABOOM! I2S memory allocation failed ***********");


  dma_display->clearScreen();
 
  // create FourScanPanellay object based on our newly created dma_display object
  FourScanPanel = new VirtualMatrixPanel ((*dma_display), NUM_ROWS, NUM_COLS, PANEL_RES_X, PANEL_RES_Y);

}

void loop (){
  led_scroll_right("HELLO,",150);
  delay(1000);
  led_scroll_left("WHAT",150);
  delay(1000);
  led_scroll_up("1234",150);
  delay(1000);
  led_scroll_down("ENED",150);
  delay(1000);
  display_switching_texts();
  delay(1000);
}

void led_scroll_right(const char *text,int speed){
    int charWidth = 12; // Change based on your font's character width
    int textWidth = strlen(text) * charWidth; // Calculate total text width
    int position = -textWidth; // Start from the left side of the display

    //set text color
    FourScanPanel->setTextColor(FourScanPanel->color565(0, 0, 255)); // Set text color to blue
    FourScanPanel->setTextSize(2);

    while (position < FourScanPanel->width()) { // Continue until the text is completely off-screen
        FourScanPanel->clearScreen();
        FourScanPanel->fillScreenRGB888(255, 255, 255);
        FourScanPanel->setCursor(position, 1); // Set cursor position
        FourScanPanel->print(text); // Print the text
        position += 2; // Move text right
        delay(speed); // Control speed
    }
    FourScanPanel->clearScreen();
}

void led_scroll_left(const char *text, int speed) {
    int charWidth = 12;  // Change based on your font's character width
    int textWidth = strlen(text) * charWidth; // Calculate total text width
    int position = FourScanPanel->width(); // Start from the right side of the display

    // Set text color
    FourScanPanel->setTextColor(FourScanPanel->color565(0, 128, 0)); // Set text color to red
    FourScanPanel->setTextSize(2);

    // Continue until the text is completely off-screen
    while (position > -textWidth) {
        FourScanPanel->clearScreen();
        FourScanPanel->setCursor(position, 1); // Set cursor position (x=position, y=1)
        FourScanPanel->print(text); // Print the text
        position -= 2; // Move text left
        delay(speed); // Control speed
    }
    FourScanPanel->clearScreen(); // Clear screen after scrolling
}

void led_scroll_up(const char *text, int speed) {
    int charHeight = 16; // Change based on your font's character height
    int charWidth = 12;  // Change based on your font's character width
    int textWidth = strlen(text) * charWidth; // Calculate total text width
    int textHeight = charHeight; // Assuming single line text
    int position = FourScanPanel->height(); // Start from the bottom of the display

    // Calculate centered x position
    int xCenter = (FourScanPanel->width() - textWidth) / 2;

    // Set text color
    FourScanPanel->setTextColor(FourScanPanel->color565(255, 255, 0)); // Set text color to red
    FourScanPanel->setTextSize(2);

    // Continue until the text is completely off-screen
    while (position > -textHeight) {
        FourScanPanel->clearScreen();
        FourScanPanel->fillScreenRGB888(255, 255, 255);
        FourScanPanel->setCursor(xCenter, position); // Set cursor position (centered x, y=position)
        FourScanPanel->print(text); // Print the text
        position -= 2; // Move text up
        delay(speed); // Control speed
    }
    FourScanPanel->clearScreen(); // Clear screen after scrolling
}

void led_scroll_down(const char *text, int speed) {
    int charHeight = 10; // Change based on your font's character height
    int charWidth = 8;  // Change based on your font's character width
    int textWidth = strlen(text) * charWidth; // Calculate total text width
    int textHeight = charHeight; // Assuming single line text
    int position = -textHeight; // Start from above the top of the display

    // Calculate centered x position
    int xCenter = (FourScanPanel->width() - textWidth) / 2;

    // Set text color
    FourScanPanel->setTextColor(FourScanPanel->color565(255, 0, 0)); // Set text color to red
    FourScanPanel->setTextSize(2);

    // Continue until the text is completely off-screen
    while (position < FourScanPanel->height()) {
        FourScanPanel->clearScreen();
        FourScanPanel->setCursor(xCenter, position); // Set cursor position (centered x, y=position)
        FourScanPanel->print(text); // Print the text
        position += 2; // Move text down
        delay(speed); // Control speed
    }
    FourScanPanel->clearScreen(); // Clear screen after scrolling
}

void display_switching_texts() {
    // Clear the screen
    FourScanPanel->clearScreen(); 

    // Set text color to blue
    FourScanPanel->setTextColor(FourScanPanel->color565(0, 0, 255)); // RGB for blue
    FourScanPanel->setTextSize(2); // Set the text size
   

    // Array of texts to display
    const char *texts[] = {
        "WHAT",
        "PAY",
        "890"
    };

    // Loop to display each text for 3 seconds
    for (int i = 0; i < 3; i++) {
        // Clear the screen for each new text
        FourScanPanel->clearScreen(); 

        // Set cursor position (you can adjust y as needed)
        FourScanPanel->setCursor(1, 1); 
        FourScanPanel->print(texts[i]); // Print the current text

        // Wait for 3 seconds
        delay(3000); 
    }

    // Optionally clear the screen after switching texts
    FourScanPanel->clearScreen(); 
}
