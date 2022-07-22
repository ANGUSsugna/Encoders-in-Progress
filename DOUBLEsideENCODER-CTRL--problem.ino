// MAC Address of responder - edit as required
// uint8_t broadcastAddress[] = {0xC8, 0xC9, 0xA3, 0xF9, 0x0E, 0xF4}; 0x0E, 0x28}
 
//ESP32
 
#include <esp_now.h>
#include <WiFi.h>

// MAC Address of responder - edit as required
uint8_t broadcastAddress[] = {0xC8, 0xC9, 0xA3, 0xF9, 0x0E, 0xF4};
 
//Displays
// ST7789 libraries//
#include <SPI.h>            // for use with ST7789 using TFT_eSPI library
#include <TFT_eSPI.h>       // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();  // Invoke custom library

// This CODE has been updated to use 2 encoders with the built-in switch and 
// posts to the serial monitor. 

#define TFT_BBGREY 0x39C7   // this is custom colour using an online RGB colour picker; hexadecimal 16 bit

int x = 0;
int y = 0;

int w =  0; //water is value 0
int m = 10; //miss is value 10
int s = 50; //ship is value 50
int h = 60; //hit is value 60

int q = 14; // OWN square indicators
int k = 8; // OPP filled-square indicators

int z ;

int OWNgfxARRAY[5][8]={
                      {h, m, s, h, s, w, w, w,},
                      {w, w, w, w, w, w, w, w,},
                      {w, w, s, s, s, w, w, w,},
                      {m, w, w, w, w, w, w, w,},
                      {s, w, w, w, w, w, w, w,}
                      };

#include <rotary.h>

// Rotary encoder is wired with the common to ground and the three
// outputs to pins A0, A1 and A2.

Rotary rotary1 = Rotary(34,35, 32);

Rotary rotary2 = Rotary(33,25, 26);
 
// Counter that will be incremented or decremented by rotation.
int OWNcounter1 = 0;
int OWNcounter2 = 0;

bool OWNSW1 = false; //boolean value for switches- false is off; true is pressed
bool OWNSW2 = false;

//Encoder values for cursor

int OWNcounter1y = 0; // y value for cursor 
int OWNcounter2x = 0; // x value for cursor

int OPPcounter1y = 0; // y value for OPP cursor
int OPPcounter2x = 0; // x value for OPP cursor

// DATA STRUCTURE TRANSMIT // 

typedef struct TxStruct     // Define a data structure
{
  int OWNcounter1;
  int OWNcounter2;

  int OWNSW1;
  int OWNSW2;
  
 } TxStruct;

// Create a structured object
TxStruct sentData;

// Peer info
esp_now_peer_info_t peerInfo;

// Callback function called when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
//  Serial.print("\r\nLast Packet Send Status:\t");
//  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
// DATA STRUCTURE RECEIVE // 

typedef struct RxStruct
{
  int OWNcounter1;
  int OWNcounter2;

// bool OWNSW1;
// bool OWNSW2;
  
}RxStruct;
RxStruct receivedData;

//-------------------------------------------------------------------------------------
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len)
{
  memcpy(&receivedData, incomingData, sizeof(receivedData));
}
//======================================================================================

 void setup() { 

 // Setup Serial Monitor and encoders
  Serial.begin(250000);
  Serial.println("Start...");

// SET-UP CODE for ST7789
 tft.init();
 tft.setRotation(1);
 tft.fillScreen(TFT_BLACK);
  
  tft.setCursor(0, 80, 4);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.println("val1: ");  

  tft.setCursor(120, 80, 4);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.println("sw1: ");  

  tft.setCursor(0, 100, 4);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.println("val2: ");  

  tft.setCursor(120, 100, 4);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.println("sw2: ");  


// SET-UP CODE for ESP32 wifi
 WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);

  // Register the send callback
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  
 // INITIALIZE OWNgfxARRAY    
for (int i = 0; i < 5; ++i) {
  for (int j = 0; j < 8; ++j) {
    OWNgfxARRAY[i][j];
  }
 }
     
} 
 
 void loop() { 
 //******************//************* TRANSMIT SECTION 3
// sentData.OWNcounter1 = OWNcounter1;  // OPPcounter1 ?
//  sentData.OWNcounter2 = OWNcounter2;  // 

 sentData.OWNSW1 = OWNSW1;
  sentData.OWNSW2 = OWNSW2;
  
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sentData, sizeof(sentData));
   
 /* if (result == ESP_OK) {
    Serial.println("Sending confirmed");
  }
  else {
    Serial.println("Sending error");
  }
  delay(200  );   /// HAHAHAHAHA THIS STUPID DELAY SCREWED THE ENCODERS UP!!
  */
  //******************//************* End TRANSMIT SECTION 3

//**** ENCODER1 and ENCODER2****
 unsigned char result1 = rotary1.process();
  if (result1 == DIR_CW) {
    OWNcounter1++;
    
    Serial.println(OWNcounter1);
    
  } else if (result1 == DIR_CCW) {
    OWNcounter1--;
        Serial.println(OWNcounter1);
  }
   // Check to see if the button has been pressed.
  // Passes in a debounce delay of 20 milliseconds
  
   if (rotary1.buttonPressedReleased(20)) {
    Serial.println("Push button1");
    OWNSW1 = true;
   }
  //endif buttonPressedReleased

//// Encoder2
    unsigned char result2 = rotary2.process();
  if (result2 == DIR_CW) {
    OWNcounter2++;
    Serial.println(OWNcounter2);
  } else if (result2 == DIR_CCW) {
    OWNcounter2--;
    Serial.println(OWNcounter2);
  }
 
   if (rotary2.buttonPressedReleased(20)) {
    Serial.println("Push button2");
    OWNSW2 = true;
   } 
  //endif buttonPressedReleased

//******* Active code ST7789 display*********//
//OWN 
  
 tft.setCursor(60, 80, 4); 
 tft.setTextColor(TFT_RED, TFT_BLACK);
 tft.println(OWNcounter1);

 tft.setCursor(180, 80, 4); 
 tft.setTextColor(TFT_RED, TFT_BLACK);
 tft.println(OWNSW1);

tft.setCursor(60, 100, 4); 
 tft.setTextColor(TFT_GREEN, TFT_BLACK);
 tft.println(OWNcounter2);

 tft.setCursor(180, 100, 4); 
 tft.setTextColor(TFT_GREEN, TFT_BLACK);
 tft.println(OWNSW2);

//void display_OWNgfxARRAY (int [5][8])
  for (int i = 0; i < 5; ++i){
  for (int j = 0; j < 8; ++j){
    int y = i * 14;                   //remember that [5] is for rows - therefore y axis
    int x = j * 14;                   //[8] is the number of columns - therefore x axis
    
     if (OWNgfxARRAY[i][j] == w){
      tft.drawRect(x,y, q,q, TFT_BLUE);
     }
      else if (OWNgfxARRAY[i][j] == m){
        tft.drawRect(x,y, q,q, TFT_WHITE);
      }
      else if (OWNgfxARRAY [i][j] == s){
        tft.drawRect(x,y, q,q, TFT_BBGREY);
      }
      else /*(OWNgfxARRAY [i][j] == h)*/{
        tft.drawRect(x,y, q,q, TFT_RED);    
        }
      }
    } 

  
// tft.fillCircle((OWNcounter1+q), (OWNcounter2+q), 10, TFT_GREEN); // THIS line of code simply advances the cursor by value of encoder. 


/*
for (int i = 0; i < 5; ++i){
for (int j = 0; j < 8; ++j){
    int y = i * 14;
    int x = j * 14;
*/
// CODE to draw OWN CURSOR   
if (OWNcounter1 <= 2) {
    OWNcounter1y = 0;
    } else if (OWNcounter1 <= 4){
      OWNcounter1y = 14;
      } else if (OWNcounter1 <= 6){
       OWNcounter1y = 28;
        } else if (OWNcounter1 <= 8){
         OWNcounter1y = 42;
          } else if(OWNcounter1 <= 10){
           OWNcounter1y = 56;
            } else //if (OWNcounter1 <= 12){
              OWNcounter1y = 70;
          /*    } else if (OWNcounter1 <= 14){
                OWNcounter1y = 84;
                } else {//(OWNcounter1 <= 16){
                  OWNcounter1y = 98;
                  } */
//THIS is for X axis             
if (OWNcounter2 <= 2){
   OWNcounter2x = 0;
    } else if (OWNcounter2 <= 4){
      OWNcounter2x= 14;
      } else if (OWNcounter2 <= 6){
        OWNcounter2x = 28;
        } else if (OWNcounter2 <= 8){
          OWNcounter2x = 42;
          } else if (OWNcounter2 <= 10){
            OWNcounter2x = 56;
           }  else if (OWNcounter2 <= 12){
              OWNcounter2x = 70;
              } else if (OWNcounter2 <= 14){
                OWNcounter2x = 84;
                } else //(OWNcounter2 <= 16){
                  OWNcounter2x = 98;
                   
                  
tft.drawRect(OWNcounter2x, OWNcounter1y, q,q, TFT_RED); //draw cursor x and y axes are inverted. correct later.
               
tft.drawRect(OWNcounter2x, OWNcounter1y, q,q, TFT_BLACK); // erase cursor 

//END of OWN CURSOR 
//SWITCH SELECTION REGISTRATION - crosshairs - 

if (rotary1.buttonPressedReleased(20)) { 
    tft.drawCircle((OWNcounter2x+7),(OWNcounter1y+7), 4, TFT_RED);  // axes switched but why?? 
    tft.drawLine((OWNcounter2x+7), OWNcounter1y, (OWNcounter2x+7), (OWNcounter1y+14), TFT_RED);//[solved it: remember the rows are first,  
    tft.drawLine(OWNcounter2x, (OWNcounter1y+7), (OWNcounter2x+14), (OWNcounter1y+7), TFT_RED);//columns second in array syntax : array[r][c].
   }                                                              
// #### OPP cursor and button #### XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

if (receivedData.OWNcounter1 <= 2) {
    OPPcounter1y = 0;
    } else if (receivedData.OWNcounter1 <= 4){
      OPPcounter1y = 14;
      } else if (receivedData.OWNcounter1 <= 6){
       OPPcounter1y = 28;
        } else if (receivedData.OWNcounter1 <= 8){
         OPPcounter1y = 42;
          } else if(receivedData.OWNcounter1 <= 10){
           OPPcounter1y = 56;
            } else //if (receivedData.OWNcounter1 <= 12){
              OPPcounter1y = 70;
          /*    } else if (receivedData.OWNcounter1 <= 14){
                OPPcounter1y = 84;
                } else {//(receivedData.OWNcounter1 <= 16){
                  OPPcounter1y = 98;
                  } */
//THIS is for X axis             
if (receivedData.OWNcounter2 <= 2){
   OPPcounter2x = 0;
    } else if (receivedData.OWNcounter2 <= 4){
      OPPcounter2x= 14;
      } else if (receivedData.OWNcounter2 <= 6){
        OPPcounter2x = 28;
        } else if (receivedData.OWNcounter2 <= 8){
          OPPcounter2x = 42;
          } else if (receivedData.OWNcounter2 <= 10){
            OPPcounter2x = 56;
           }  else if (receivedData.OWNcounter2 <= 12){
              OPPcounter2x = 70;
              } else if (receivedData.OWNcounter2 <= 14){
                OPPcounter2x = 84;
                } else //(receivedData.OWNcounter2 <= 16){
                  OPPcounter2x = 98;
                   
                  
tft.drawRect(OPPcounter2x, OPPcounter1y, q,q, TFT_RED); //draw cursor x and y axes are inverted. correct later.
               
tft.drawRect(OPPcounter2x, OPPcounter1y, q,q, TFT_BLACK); // erase cursor 

//END of OWN CURSOR

//SWITCH SELECTION REGISTRATION - crosshairs

//if (rotary1.buttonPressedReleased(20)) {

   
    tft.drawCircle((OPPcounter2x+7),(OPPcounter1y+7), 4, TFT_GREEN);  // axes switched but why?? [solved it: remember the rows are first,  
    tft.drawLine((OPPcounter2x+7), OPPcounter1y, (OPPcounter2x+7), (OPPcounter1y+14), TFT_GREEN);
    tft.drawLine(OPPcounter2x, (OPPcounter1y+7), (OPPcounter2x+14), (OPPcounter1y+7), TFT_GREEN);
   } 
