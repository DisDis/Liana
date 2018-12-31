#include "palette.h"
#include "anim.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!! Be sure to set up your WiFi network name and password in the wifi.ino file !!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#define ANIMS 7 //number of animations (not including start one) to cycle randomly
#define PALS 8 //number of palettes
#define INTERVAL 30000 //change interval, msec

//#define USE_START_ANIMATION //start animation is used in cycling as well as other animations

Palette * pals[PALS] = {&PalRgb, &PalRainbow, &PalRainbowStripe, &PalParty, &PalHeat, &PalFire, &PalIceBlue, &PalXMas};

Anim anim = Anim();


unsigned long ms = 10000;//startup animation duration, 10000 for "release" AnimStart

int paletteInd = random(PALS);
int animInd = 0;


void setup() { 
  Serial.begin(115200);
  Serial.println("Entering setup");

  wifiSetUp();
  webSetup();
  wsSetup();
  otaSetUp();

  randomSeed(analogRead(0)*analogRead(1));
  anim.setAnim(animInd);
  anim.setPeriod(20);
  anim.setPalette(pals[0]);
  anim.doSetUp();


  Serial.println("Setup done");
}


void loop() {
  yield();
  
  /* this piece of code checks for looping while trying to find different colors
  for (int pi=0;pi<PALS;pi++) {
    int c = 0;
    
    Serial.print(F("pi="));Serial.print(pi);
    Color c1 = pals[pi]->getPalColor((float)rngb()/256);
    Color c2 = c1;
    while (c1.isCloseTo(c2)) {
      c = c + 1;
      c2 = pals[pi]->getPalColor((float)rngb()/256);
    }
    Serial.print(F(" c="));Serial.println(c);
  }
  /**/

  anim.run();
  
  if (millis() > ms && animInd != 255) {// animind == 255 is for turned off strip - it never ends automatically
    ms = millis() + INTERVAL; 
    switch ( (animInd < 0) ? 0 : random(1)) {
      case 0: 
      {
        Serial.print(F("anim->"));
        int prevAnimInd = animInd;
#ifdef USE_START_ANIMATION
        while (prevAnimInd == animInd) animInd = random(ANIMS+1);
        if (animInd == 0) ms = millis() + 10000;//startup animation has fixed 10 seconds length
#else
        while (prevAnimInd == animInd) animInd = random(ANIMS) + 1;
#endif        
        setAnimPal();
        wsNotify(-1);
        break;
      }
      case 1:
      {
        Serial.print(F("pal->"));
        int prevPalInd = paletteInd;
        while (prevPalInd == paletteInd) paletteInd = random(PALS);
        anim.setPalette(pals[paletteInd]);
        Serial.print(paletteInd);
        break;
      }
    }
    Serial.println();
  }
  /**/

  webRun();

  wsRun();

  otaRun();

}

//sets animation and palette to values specified in animInd and paletteInd, 
//and notifies all WebSocket connections of the change
void setAnimPal() {
  anim.setAnim(animInd);
  anim.setPeriod(random(20, 40));
  anim.setPalette(pals[paletteInd]);
  anim.doSetUp();
}
