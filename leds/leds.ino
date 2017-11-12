#define PIN_LED 12
#define MODE_NUMBER 2
#define MAX_COLOR 5
#define LED_NUMBER 60

String cmd = "";

enum ModeEnum{
  STATIC = 1, WAVE = 2
};

class Color{
  byte red, green, blue;

  public:
    Color(){
      this->red = 255;
      this->green = 255;
      this->blue = 255;
    }
    Color(char r, char g, char b){
      this->red = r;
      this->green = g;
      this->blue = b;
    }
    Color(String str){
      if ( str.length() >= 3 ){
        this->red = str[0];
        this->green = str[1];
        this->blue = str[2];
      } else {
        this->red = 255;
        this->green = 255;
        this->blue = 255;
      } 
    }
    byte getRed(){
      return red;
    }
    byte getGreen(){
      return green;
    }
    byte getBlue(){
      return blue;
    }
};

/// PARAMS
ModeEnum mode = STATIC;
float duration = 1;
Color* colors;
byte colorNumber = 1;
float intensity = 1;

// strip colors;
Color* stripColors ;

void setup() {
  colors = new Color[MAX_COLOR]();
  stripColors = new Color[MAX_COLOR]();

  
  pinMode( PIN_LED , OUTPUT);
  Serial.begin( 9600 );
  Serial.println("Hello Computer");
}

void loop() {
  
}



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
      if (l > 0 && l % 3 == 0 ){
        colorNumber = l / 3;
        for ( int i = 0; i < colorNumber ; i ++){
          colors[i] = cmdParam.substring(i * 3, (i + 1) * 3);
          Serial.print("Color set to ");
          Serial.print(colors[i].getRed() );
          Serial.print(colors[i].getGreen() );
          Serial.print(colors[i].getBlue() );
        }
      }
    } else if ( cmdName.equals("setDuration")){
      float tmp = cmdParam.toFloat();
      if ( duration > 0 ){
        duration = tmp;
        Serial.print("Duration set to " );
        Serial.println( duration );
      } else {
        Serial.println("wrong duration");
      }
    } else if ( cmdName.equals("setIntensity")){
      float tmp = cmdParam.toFloat();
      if ( duration >= 0 && duration <= 1){
        intensity = tmp;
        Serial.print("Intensity set to " );
        Serial.println( intensity );
      } else {
        Serial.println("wrong intensity");
      }
    } else {
      Serial.println("unknown cmd");
    }
  } else { // no params
    if ( cmd.equals("getInfo")){
      Serial.print("{\"colors\":{" );
      for ( int i = 0; i < colorNumber ; i ++){
        Serial.print( "\"" );
        Serial.print( i );
        Serial.print( "\":{\"r\":" );
        Serial.print( colors[i].getRed() );
        Serial.print( ",\"g\":" );
        Serial.print( colors[i].getGreen() );
        Serial.print( ",\"b\":" );
        Serial.print( colors[i].getBlue() );
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
  }
  
}

