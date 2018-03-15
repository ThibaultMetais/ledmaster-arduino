#include <Adafruit_NeoPixel.h>

#define PIN_LED 12
#define MODE_NUMBER 3
#define MAX_COLOR 5
#define LED_NUMBER 88
#define STRIP_PIN 6

String cmd = "";

enum ModeEnum{
  STATIC = 1, WAVE = 2, MOSAIC = 3
};
enum Side{
  SCREEN_LEFT = 1, SCREEN_TOP, SCREEN_RIGHT, SCREEN_BOTTOM, N_E, S_E, S_O, N_O 
};

struct Color {
  byte red, green, blue;
};

/// PARAMS
ModeEnum mode = MOSAIC;
float duration = 10;
Color* colors;
byte colorNumber = 1;
byte intensity = 255;
byte on = 1;


// vars
float timeVar = 0;
long prevTime = 0;
// strip colors;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(150, STRIP_PIN, NEO_GRB + NEO_KHZ800);


void setup() {
  strip.begin();
  strip.show(); // Turn off the leds 

  
  colors = new Color[MAX_COLOR]();

  colors[0].red= 255;
  
  pinMode( PIN_LED , OUTPUT);
  Serial1.begin( 9600 );
  Serial1.println("Hello Computer");
}

void setPixelColor(int i, byte r, byte g, byte b){
  setPixelColor(i, r, g, b, 1);
}

void setPixelColor(int i, byte r, byte g, byte b, float brightCoef){
  float bright = (float)intensity/255.0 * brightCoef;
  if ( i > 68 ){ // ignore a dead led.
    strip.setPixelColor(i-1, (byte)((float)r*bright),(byte)((float)g*bright),(byte)((float)b*bright));
  } else {
    strip.setPixelColor(i, (byte)((float)r*bright),(byte)((float)g*bright),(byte)((float)b*bright));
  }
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
void setSideColor(Side side, byte r, byte g, byte b ){
  int from, to;
  switch( side ){
    case SCREEN_LEFT: from = 0; to = 8; break;
    case SCREEN_TOP: from = 8; to = 24; break;
    case SCREEN_RIGHT: from = 24; to = 32; break;
    case SCREEN_BOTTOM: from = 32; to = 48; break;
    case N_E: from = 48; to = 58; break;
    case S_E: from = 58; to = 68; break;
    case S_O: from = 68; to = 78; break;
    case N_O: from = 78; to = 88; break;
  }
  for ( int i = from; i < to ; i ++ ){
    setPixelColor(i, r,g,b );
  }
}

void loop() {

  timeVar += (float)(millis() - prevTime) / 1000.0;
  while ( timeVar > duration ){
    timeVar -= duration;
  }
  prevTime = millis();

  float progress = timeVar / duration ;
  
  /*int val = analogRead(0) / 4; 
  for (int i = 0; i < LED_NUMBER; i ++){
    setPixelColor(i, val,val, val);
  }*/

  if ( on == 1 ) {
    if ( mode == STATIC){
      int val = 100; 
      for (int i = 0; i < LED_NUMBER; i ++){
        setPixelColor(i, colors[0].red, colors[0].green, colors[0].blue);
      }
      delay(10);
    } else if ( mode == WAVE){
      float brightness = (sin(progress * 2 * PI) + 1)/2.0;
      for (int i = 0; i < LED_NUMBER; i ++){
        setPixelColor(i, colors[0].red, colors[0].green, colors[0].blue, brightness);
      }
      delay(10);
    } else {
        for (int i = 0; i < LED_NUMBER; i ++){
          float red = 0.0;
          float blue = 150.0;
          float green = 0.0;
          float a = (float)i / 48.0;
          if ( sin((a- 0.35) * 4 * PI) > 0){
             red = 255 * sin((a- 0.35) * 4 * PI);
          }
          green = 255 * abs(sin((-a) * 2 * PI));
          if ( sin((-a + 0.5) * 2 * PI) < 0 && a > 0.5 && a < 1){
              blue = 0;
              red = 255;
          }
          setPixelColor(i, int(red),int(blue), int(green));
        }
        setSideColor( N_E, 255, 255, 0);
        setSideColor( S_E, 255, 0, 255);
        setSideColor( S_O, 255, 255, 0);
        setSideColor( N_O, 0, 255, 255);
    }
    updatePixels();
  } else {
      for (int i = 0; i < LED_NUMBER; i ++){
        setPixelColor(i, 0, 0, 0);
      }
      delay(10);
    updatePixels();
  }

  while (Serial1.available()) {
    char c = Serial1.read();
    if ( c != '\n'){
      cmd += c;
      
    } else {
      Serial1.println("#recieved cmd : " + cmd);
      parseCmd( cmd );
      cmd = "";
    }
    
  }
}


/* Update the pixels 
 */
void updatePixels(){
  strip.show(); // on affiche
}

/* Receive the cmd
 */

void SerialEvent1() {

  
}

/* Parse the cmd and execute the cmd
 * 
 */
int parseCmd( String cmd ) {
  int splitIndex = cmd.indexOf(':');
  int sumIndex = cmd.indexOf('?');

  String cmdName = ""; 
  String cmdParam = "";

  // CHECK SUM
  int sum = -1;
  if ( sumIndex > 0){
    sum = cmd[sumIndex+1];
    cmdName = cmd.substring(0, sumIndex); 
  } else {
    cmdName = cmd;
  }
  int s = 0;
  for ( int i = 0; i < cmdName.length(); i ++){
    s += cmd[i];
  }
  s%=256;

  Serial1.print( "#thSum : ");
  Serial1.println( (int)sum); 
  Serial1.print( "#Sum : ");
  Serial1.println( (int)s ); 

  if ( sum != -1 && s!=sum){
    Serial1.println("csError");
    return 0;
  }

  // PARAMS
  if ( splitIndex > 0 ){
    cmdName = cmd.substring(0, splitIndex); 
    cmdParam = cmd.substring(splitIndex + 1, sumIndex);
  }

  // ALIAS
  
  if ( cmdName.equals("sM")){
    cmdName = "setMode";
  } else if ( cmdName.equals("sC")){
    cmdName = "setColors";
  } else if ( cmdName.equals("sO")){
    cmdName = "setOnOff";
  } else if ( cmdName.equals("sD")){
    cmdName = "setDuration";
  } else if ( cmdName.equals("sI")){
    cmdName = "setIntensity";
  } else if ( cmdName.equals("gI")){
    cmdName = "getInfo";
  }
  
  Serial1.print( "#Commande : ");
  Serial1.println( cmdName );
  Serial1.print( "#Param : ");
  Serial1.println( cmdParam ); 

  if ( !cmdParam.equals("")){
    if ( cmdName.equals("setMode")){
      int tmp = cmdParam.toInt();
      if ( tmp > 0 && tmp <= MODE_NUMBER){
        mode = (ModeEnum)tmp;
        Serial1.print("Mode set to " );
        Serial1.println( mode );
      } else {
        Serial1.println("unknown mode");
      }
    } else if ( cmdName.equals("setColors")){
      int l = cmdParam.length();
      Serial1.println(l);
      if (l > 0 && l % 9 == 0 ){
        colorNumber = l / 9;
        for ( int i = 0; i < colorNumber ; i ++){
          colors[i].red =   (byte)atoi(cmdParam.substring(i * 9        , i * 9 + 1 * 3).c_str());
          colors[i].green = (byte)atoi(cmdParam.substring(i * 9 + 1 * 3, i * 9 + 2 * 3).c_str());
          colors[i].blue =  (byte)atoi(cmdParam.substring(i * 9 + 2 * 3, i * 9 + 3 * 3).c_str());
          Serial1.print("Color set to ");
        }
      }
    } else if ( cmdName.equals("setDuration")){
      float tmp = cmdParam.toFloat();
      if ( tmp > 0 ){
        duration = tmp;
        Serial1.print("Duration set to " );
        Serial1.println( duration );
      } else {
        Serial1.println("wrong duration");
      }
    } else if ( cmdName.equals("setIntensity")){
      byte tmp = (byte)cmdParam.toInt();
      if ( tmp >= 0 && tmp <= 255){
        intensity = tmp;
        Serial1.print("Intensity set to " );
        Serial1.println( intensity );
      } else {
        Serial1.println("wrong intensity");
      }
    } else if ( cmdName.equals("setOnOff")){
      float tmp = cmdParam.toInt();
      if ( tmp >= 0 && tmp < 4 ){
        on = tmp;
        Serial1.print("Set to" );
        Serial1.println( tmp );
      }
    } else {
      Serial1.println("unknown cmd");
    }
  } else { //noParam
    if ( cmd.equals("getInfo")){
        sendInfoJSON();
    }
  }
  
  return 1;
}


void sendInfoJSON(){
  Serial1.print("{\"colors\":{" );
    for ( int i = 0; i < colorNumber ; i ++){
      Serial1.print( "\"" );
      Serial1.print( i );
      Serial1.print( "\":{\"r\":" );
      Serial1.print( colors[i].red );
      Serial1.print( ",\"g\":" );
      Serial1.print( colors[i].green );
      Serial1.print( ",\"b\":" );
      Serial1.print( colors[i].blue );
      Serial1.print( "}" );
      if ( i + 1 < colorNumber ){
        Serial1.print( "," );
      }
    }
    Serial1.print("},");
    Serial1.print( "\"duration\":" );
    Serial1.print( duration );
    Serial1.print( ", \"instensity\":");
    Serial1.print( intensity );
    Serial1.print( ", \"mode\":");
    Serial1.print( mode );
    Serial1.println("}");
}

