#include <Adafruit_NeoPixel.h>

#define PIN_LED 12
#define MODE_NUMBER 2
#define MAX_COLOR 5
#define LED_NUMBER 88
#define STRIP_PIN 6

String cmd = "";

enum ModeEnum{
  STATIC = 1, WAVE = 2
};

struct Color {
  byte red, green, blue;
};

/// PARAMS
ModeEnum mode = STATIC;
float duration = 1;
Color* colors;
byte colorNumber = 1;
float intensity = 1;
byte on = 0;


// vars
float timeVar = 0;
int prevTime = 0;
// strip colors;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(150, STRIP_PIN, NEO_GRB + NEO_KHZ800);


void setup() {
  strip.begin();
  strip.show(); // Turn off the leds 

  
  colors = new Color[MAX_COLOR]();
  
  pinMode( PIN_LED , OUTPUT);
  Serial.begin( 9600 );
  Serial.println("Hello Computer");
}

void setPixelColor(int i, byte r, byte g, byte b){
  strip.setPixelColor(i, r,g,b);
}

/*                         2    
 *                ------------------
 *                |                |
 *              1 |                | 3
 *                |                |
 *                ------------------
 *                        4
 *            ---------------------------
 *            |      8      |      5    |
 *            |             |           |
 *            ---------------------------
 *            |      7      |      6    |
 *            |             |           |
 *            ---------------------------
 */
void setSideColor(int i, byte r, byte g, byte b ){
  int from, to;
  switch( i ){
    case 0: from = 0; to = 8; break;
    case 1: from = 8; to = 24; break;
    case 2: from = 24; to = 32; break;
    case 3: from = 32; to = 48; break;
    case 4: from = 48; to = 58; break;
    case 5: from = 58; to = 68; break;
    case 6: from = 68; to = 78; break;
    case 7: from = 78; to = 88; break;
  }
  for ( i = from; i < to ; i ++ ){
    setPixelColor(i, r,g,b );
  }
}

void loop() {

  timeVar += (float)(prevTime - millis()) / 1000.0;
  if ( timeVar > duration ){
    timeVar -= (int)(timeVar / duration) * duration;
  }
  prevTime = millis();
  

  /*for (int i = 0; i < LED_NUMBER; i++){
    setPixelColor(i, stripColors[i]);
  }*/
  for (int i = 0; i < LED_NUMBER; i ++){
    setPixelColor(i, 255, 0 , 0);
    setPixelColor((i-1+LED_NUMBER) % LED_NUMBER, 255, 255 , 255);
    updatePixels();
    delay(50);
  }
}


/* Update the pixels 
 */
void updatePixels(){
  strip.show(); // on affiche
}

/* Receive the cmd
 */

void serialEvent() {
  while (Serial.available()) {
    char c = Serial.read();
    if ( c != '\n'){
      cmd += c;
    } else {
      parseCmd( cmd );
      cmd = "";
    }
    
  }
}

/* Parse the cmd and execute the cmd
 * 
 */
void parseCmd( String cmd ) {
  int splitIndex = cmd.indexOf(':');
  if ( splitIndex > 0 ){
    String cmdName = cmd.substring(0, splitIndex); 
    String cmdParam = cmd.substring(splitIndex + 1);
    
    Serial.print( "Commande : ");
    Serial.println( cmdName );
    Serial.print( "Param : ");
    Serial.println( cmdParam ); 

    if ( cmdName.equals("setMode")){
      int tmp = cmdParam.toInt();
      if ( tmp > 0 && tmp <= MODE_NUMBER){
        mode = (ModeEnum)tmp;
        Serial.print("Mode set to " );
        Serial.println( mode );
      } else {
        Serial.println("unknown mode");
      }
    } else if ( cmdName.equals("setColors")){
      int l = cmdParam.length();
      Serial.println(l);
      if (l > 0 && l % 9 == 0 ){
        colorNumber = l / 9;
        for ( int i = 0; i < colorNumber ; i ++){
          colors[i].red =   (byte)atoi(cmdParam.substring(i * 9        , i * 9 + 1 * 3).c_str());
          colors[i].green = (byte)atoi(cmdParam.substring(i * 9 + 1 * 3, i * 9 + 2 * 3).c_str());
          colors[i].blue =  (byte)atoi(cmdParam.substring(i * 9 + 2 * 3, i * 9 + 3 * 3).c_str());
          Serial.print("Color set to ");
        }
      }
    } else if ( cmdName.equals("setDuration")){
      float tmp = cmdParam.toFloat();
      if ( tmp > 0 ){
        duration = tmp;
        Serial.print("Duration set to " );
        Serial.println( duration );
      } else {
        Serial.println("wrong duration");
      }
    } else if ( cmdName.equals("setIntensity")){
      float tmp = cmdParam.toFloat();
      if ( tmp >= 0 && tmp <= 1){
        intensity = tmp;
        Serial.print("Intensity set to " );
        Serial.println( intensity );
      } else {
        Serial.println("wrong intensity");
      }
    } else if ( cmdName.equals("setOnOff")){
      float tmp = cmdParam.toInt();
      if ( tmp >= 0 && tmp < 4 ){
        on = tmp;
        Serial.print("Set to" );
        Serial.println( tmp );
      }
    } else {
      Serial.println("unknown cmd");
    }
  } else { // no params
    if ( cmd.equals("getInfo")){
      sendInfoJSON();
    }
  }
}


void sendInfoJSON(){
  Serial.print("{\"colors\":{" );
    for ( int i = 0; i < colorNumber ; i ++){
      Serial.print( "\"" );
      Serial.print( i );
      Serial.print( "\":{\"r\":" );
      Serial.print( colors[i].red );
      Serial.print( ",\"g\":" );
      Serial.print( colors[i].green );
      Serial.print( ",\"b\":" );
      Serial.print( colors[i].blue );
      Serial.print( "}" );
      if ( i + 1 < colorNumber ){
        Serial.print( "," );
      }
    }
    Serial.print("},");
    Serial.print( "\"duration\":" );
    Serial.print( duration );
    Serial.print( ", \"instensity\":");
    Serial.print( intensity );
    Serial.print( ", \"mode\":");
    Serial.print( mode );
    Serial.println("}");
}

