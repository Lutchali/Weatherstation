/*Code by github.com/Lutchali*/

/*Disclaimer*/
/*Tis is still under active devellopment.
 * For updates check out my github.*/
/*Disclaimer*/

/*You're free to use any part of my code,
 * but if you want to publish it on the internet
 * and use bigger parts of it,
 * just put my name in somewhere.
 */
/*  Verdrahtung NodeMCU:
 *   Touch:
 *      T_IRQ         D1
 *      T_DO          D6
 *      T_DIN         D7
 *      T_CS          D2
 *      T_CLK         D5
 *   Display:
 *      SODI(miso)    D6
 *      LED           3V
 *      SCK           D5
 *      SDA / MOSI    D7
 *      A0 / DC       D3
 *      RESET         3V
 *      CS            D8
 *      GND           GND
 *      VCC           3V
 *   BME280:
 *      Vcc           3V
 *      Gnd           G
 *      BME_SDA       RX
 *      BME_SCL       TX
*/
using namespace std;
#include <SPI.h>                     
#include <TFT_eSPI.h>               
#include <BME280I2C.h>
#include <Wire.h>
#include <deque>

int touchValue = 250;
int touchPower = 0;
bool touch = false;
float Pres, Temp, Hum;
int presPercent = 0;
int tempPercent = 0;
int humPercent = 0;
int menuState = 0;
int oldMenuState = 0;
int i = 0;  //All valiables with one letter are counters
int j = 0;
int k = 0;
int m = 240;
int drawTemp = 0;
int prevTemp = 0;
int drawHum = 0;
int prevHum = 0;
int drawPres = 0;
int prevPres = 0;
int forecast = 0; 
bool indication = true;
bool indicationChange = true;
bool debug = false;
bool debugChange = true;
deque<float> tempdeque;   //Deque is the same as an array, but if the first value gets deleted the others move up
deque<float> presdeque;
deque<float> humdeque;
uint16_t x=0;                        
uint16_t y=0;


BME280I2C bme;    //defining sensor
TFT_eSPI tft = TFT_eSPI();   //defining display                                                        

void setup(void) {
for(k = 0; k<240; k++){   //defining the length of a deque
  tempdeque.emplace_back(0);
}
for(k = 0; k<240; k++){
  presdeque.emplace_back(0);
}
for(k = 0; k<240; k++){
  humdeque.emplace_back(0);
}
    
  Serial.begin(115200);
  Wire.begin(3,1);    
  tft.init();                     //setting up the display and sensor
  tft.setRotation(2);             
  tft.fillScreen(ILI9341_BLACK);   
  tft.setCursor(0, 0, 2);          
  tft.setTextColor(TFT_WHITE,TFT_BLACK);  
  tft.setTextSize(1);                    
  while(!bme.begin())   
  {
    tft.println("BME280 nicht gefunden!");
    delay(1000);
  }
  tft.fillScreen(ILI9341_BLACK);  

}

void loop() {   
  while(i<600){
    GetData();
    TouchInput(); 
    Options();
    DatenAusgeben();
    i++;
    delay(100);
  }
  GetHistory();   //Is executed every 3-5 minutes, for uploading to a server later
  i = 0;
} 

void GetData()   
{
   bme.read(Pres, Temp, Hum);  //Get data for sensor
   Pres = Pres/100;
}

void TouchInput(){
  touchPower = tft.getTouchRawZ();    //get touchinput, but only if the touch power is high enough
  if (touchPower > touchValue){
    touch = true;
  }
  else{
    touch = false;
  }  
  tft.setRotation(2);     
  tft.getTouchRaw(&x, &y);
  
}

void GetHistory(){
  tempdeque.pop_front();    //uses deques to make value history
  tempdeque.emplace_back(Temp);

  presdeque.pop_front();
  presdeque.emplace_back(Pres);

  humdeque.pop_front();
  humdeque.emplace_back(Hum);
}

void Options(){
  
  if (touch){                                         //when touch is true this looks what to do
    if ((x<850) and (y<850) and (menuState != 1)){    //for every menu and saves it
      oldMenuState = menuState;
      menuState = 1;
      tft.fillScreen(ILI9341_BLACK);
    }
    else if((y<851) and (menuState ==1)){
      menuState = 0;
      tft.fillScreen(ILI9341_BLACK);
    }
    else if((y>851) and (y<1200) and (menuState ==1)){
      menuState = 2;
      tft.fillScreen(ILI9341_BLACK);
    }
    else if((y>1201) and (y<1500) and (menuState ==1)){
      menuState = 3;
      tft.fillScreen(ILI9341_BLACK);
    }
    else if((y>1501) and (y<1900) and (menuState ==1)){
      menuState = 4;
      tft.fillScreen(ILI9341_BLACK);
    }
    else if((y>1901) and (y<2250) and (menuState ==1)){
      menuState = 5;
      tft.fillScreen(ILI9341_BLACK);
    }
    else if((y>2251) and (y<2500) and (menuState ==1)){
      menuState = 6;
      m = 240;
      tft.fillScreen(ILI9341_BLACK);
    }
    else if((y>2501) and (y<2800) and (menuState ==1)){
      menuState = 7;
      indicationChange = true;
      debugChange = true;
      tft.fillScreen(ILI9341_BLACK);
    }
    else if((y>2801) and (y<3140) and (menuState ==1) and (debug == true)){
      menuState = 8;
      tft.fillScreen(ILI9341_BLACK);
    }
    else if((x>3300) and (x<3650) and (y>450) and (y<750) and (menuState ==7)) {
      if (indication == true){
        indication = false;
        indicationChange = true;
      }
      else{
        indication = true;  
        indicationChange = true;
      }
    }  
    else if((x>3300) and (x<3650) and (y>950) and (y<1250) and (menuState ==7)) {
      if (debug == true){
        debug = false;
        debugChange = true;
      }
      else{
        debug = true;  
        debugChange = true;
      }
    }
    else{
      if (menuState == 1){
        menuState = oldMenuState;
        if (oldMenuState == 7){
          indicationChange = true;
          debugChange = true;
        }
        tft.fillScreen(ILI9341_BLACK);
      }
    }
    delay(50);
  }
}

void DatenAusgeben(){   //executes the menuState
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE,TFT_BLACK);
  if (menuState == 0){
    MainMenu();
  }
  else if(menuState == 1){
    Menu();
  }
  else if(menuState == 2){
    Forecast();
  }
  else if(menuState == 3){
    Temperature();
  }
  else if(menuState == 4){
    Humidity();
  }
  else if(menuState == 5){
    Pressure();
  }
  else if(menuState == 6){
    History();
  }
  else if(menuState == 7){
    Settings();
  }
  else if(menuState == 8){
    DebugMenu();
  }
}

void MainMenu(){
  tft.setCursor(60, 10, 2);
  tft.setTextSize(2);
  if (indication == true){
    if (tempdeque[239] > tempdeque[238]){   //looks how values have changed and weather colorindication is turned on or off
      tft.setTextColor(TFT_GREEN,TFT_BLACK);
    }
    else if(tempdeque[239] == tempdeque[238]){
      tft.setTextColor(TFT_WHITE,TFT_BLACK);
    }
    else{
      tft.setTextColor(TFT_RED,TFT_BLACK);
    }
  }
  else{
    tft.setCursor(200, 10, 2);
    if (tempdeque[239] > tempdeque[238]){
      tft.print("+");
    }
    else if(tempdeque[239] == tempdeque[238]){
      tft.print("|");
    }
    else{
      tft.print("-");
    }
  }
  tft.setCursor(70, 10, 2);
  tft.print(Temp);
  tft.print("`C");
  tempPercent = Temp*2.5;
  tempPercent = tempPercent + 100;
  tft.fillRect(tempPercent + 5, 50, 20, 25, TFT_BLACK);   //fill space behind current value black
  tft.drawRect(5, 50, 230, 25, TFT_RED);                  //redraw border
  tft.fillRect(5, 50, tempPercent, 25, TFT_RED);          //draw new value

  tft.setCursor(70, 110, 2);
  tft.setTextSize(2);
  if (indication == true){
    if (humdeque[239] > humdeque[238]){
      tft.setTextColor(TFT_GREEN,TFT_BLACK);
    }
    else if(humdeque[239] == humdeque[238]){
      tft.setTextColor(TFT_WHITE,TFT_BLACK);
    }
    else{
      tft.setTextColor(TFT_RED,TFT_BLACK);
    }
  }
  else{
    tft.setCursor(200, 110, 2);
    if (humdeque[239] > humdeque[238]){
      tft.print("+");
    }
    else if(humdeque[239] == humdeque[238]){
      tft.print("|");
    }
    else{
      tft.print("-");
    }
  }
  tft.setCursor(72, 110, 2);
  tft.print(Hum);
  tft.print("%");
  humPercent = Hum*2.29;
  tft.fillRect(humPercent + 5, 150, 20, 25, TFT_BLACK);
  tft.drawRect(5, 150, 230, 25, TFT_BLUE);
  tft.fillRect(5, 150, humPercent, 25, TFT_BLUE);

  tft.setCursor(55, 210, 2);
  tft.setTextSize(2);
  if (indication == true){
    if (presdeque[239] > presdeque[238]){
      tft.setTextColor(TFT_GREEN,TFT_BLACK);
    }
    else if(presdeque[239] == presdeque[238]){
      tft.setTextColor(TFT_WHITE,TFT_BLACK);
    }
    else{
      tft.setTextColor(TFT_RED,TFT_BLACK);
    }
  }
  else{
    tft.setCursor(200, 210, 2);
    if (presdeque[239] > presdeque[238]){
      tft.print("+");
    }
    else if(presdeque[239] == presdeque[238]){
      tft.print("|");
    }
    else{
      tft.print("-");
    }
  }
  tft.setCursor(60, 210, 2);
  tft.print(Pres);
  tft.print("Pa");
  presPercent = Pres*2.29;
  presPercent = presPercent - 2150;
  tft.fillRect(presPercent + 5, 250, 20, 25, TFT_BLACK);
  tft.drawRect(5, 250, 230, 25, TFT_GREEN);
  tft.fillRect(5, 250, presPercent, 25, TFT_GREEN);
}

void Menu(){
  tft.setCursor(0, 0, 2);     //display menu
  tft.setTextSize(2);
  tft.println("Overview");
  tft.println("Forecast");
  tft.println("Temperature");
  tft.println("Humidity");
  tft.println("Pressure");
  tft.println("History");
  tft.println("Settings");
  if (debug == true){
    tft.println("Debug"); 
  }
}

void Temperature(){
  tft.setCursor(60, 20, 2);
  tft.setTextSize(2);
  if (indication == true){
    if (tempdeque[239] > tempdeque[238]){
      tft.setTextColor(TFT_GREEN,TFT_BLACK);
    }
    else if(tempdeque[239] == tempdeque[238]){
      tft.setTextColor(TFT_WHITE,TFT_BLACK);
    }
    else{
      tft.setTextColor(TFT_RED,TFT_BLACK);
    }
  }
  else{
    tft.setCursor(200, 10, 2);
    if (tempdeque[239] > tempdeque[238]){
      tft.print("+");
    }
    else if(tempdeque[239] == tempdeque[238]){
      tft.print("|");
    }
    else{
      tft.print("-");
    }
  }
  tft.setCursor(60, 10, 2);
  tft.print(Temp);
  tft.print("`C");
  tempPercent = tempPercent + 20;
  tempPercent = Temp*2.5;
  tft.fillRect(tempPercent + 5, 50, 20, 25, TFT_BLACK);
  tft.drawRect(5, 50, 230, 25, TFT_RED);
  tft.fillRect(5, 50, tempPercent, 25, TFT_RED);
}

void Humidity(){
  tft.setCursor(70, 20, 2);
  tft.setTextSize(2);
  if (indication == true){
    if (humdeque[239] > humdeque[238]){
      tft.setTextColor(TFT_GREEN,TFT_BLACK);
    }
    else if(humdeque[239] == humdeque[238]){
      tft.setTextColor(TFT_WHITE,TFT_BLACK);
    }
    else{
      tft.setTextColor(TFT_RED,TFT_BLACK);
    }
  }
  else{
    tft.setCursor(200, 10, 2);
    if (humdeque[239] > humdeque[238]){
      tft.print("+");
    }
    else if(humdeque[239] == humdeque[238]){
      tft.print("|");
    }
    else{
      tft.print("-");
    }
  }
  tft.setCursor(72, 10, 2);
  tft.print(Hum);
  tft.print("%");
  humPercent = Hum*2.5;
  tft.fillRect(humPercent + 5, 50, 20, 25, TFT_BLACK);
  tft.drawRect(5, 50, 230, 25, TFT_BLUE);
  tft.fillRect(5, 50, humPercent, 25, TFT_BLUE);
}

void Pressure(){
  tft.setCursor(55, 20, 2);
  tft.setTextSize(2);
  if (indication == true){
    if (presdeque[239] > presdeque[238]){
      tft.setTextColor(TFT_GREEN,TFT_BLACK);
    }
    else if(presdeque[239] == presdeque[238]){
      tft.setTextColor(TFT_WHITE,TFT_BLACK);
    }
    else{
      tft.setTextColor(TFT_RED,TFT_BLACK);
    }
  }
  else{
    tft.setCursor(200, 10, 2);
    if (presdeque[239] > presdeque[238]){
      tft.print("+");
    }
    else if(presdeque[239] == presdeque[238]){
      tft.print("|");
    }
    else{
      tft.print("-");
    }
  }
  tft.setCursor(60, 10, 2);
  tft.print(Pres);
  tft.print("Pa");
  presPercent = Pres*2.5;
  presPercent = presPercent - 2150;
  tft.fillRect(presPercent + 5, 50, 20, 25, TFT_BLACK);
  tft.drawRect(5, 50, 230, 25, TFT_GREEN);
  tft.fillRect(5, 50, presPercent, 25, TFT_GREEN);
}

void History(){
  tft.setCursor(0, 0, 2);     //make graph for value history
  tft.setRotation(0); 
  if (m > 0){
    drawTemp = tempdeque[m]; 
    drawTemp = drawTemp + 20;
    drawTemp = drawTemp * 5; 
    drawHum = humdeque[m]; 
    drawHum = drawHum * 3; 
    drawPres = presdeque[m]; 
    drawPres = drawPres - 850;
    tft.fillRect(m, drawTemp, 2, 2, TFT_RED);     //draws point on current value 
    tft.fillRect(m, drawHum, 2, 2, TFT_BLUE);
    tft.fillRect(m, drawPres, 2, 2, TFT_GREEN);
    m--;
  }
  else{
    m = 240;
    tft.fillScreen(ILI9341_BLACK);
  }
}

void DebugMenu(){             //output variables for debug purpose
  tft.setCursor(0, 0, 2);
  if (i == 1){
    tft.fillScreen(TFT_BLACK);
    }
  tft.print(x);
  tft.println("   ");
  tft.print(y);
  tft.println("   ");
  tft.print(touchPower);
  tft.println("   ");
  tft.print("touch: ");
  tft.println(touch);
  tft.print("Menustate: ");
  tft.println(menuState);
  tft.print("Counter: ");
  tft.println(i);
  tft.println("\t\t");
}

void Forecast(){
  tft.setTextSize(3);                             //uses the last two pressure values to predicht weather
  forecast = presdeque[239] - presdeque[238];
  forecast = presdeque[239] + forecast;
  if (forecast >= 988){
    tft.setCursor(62, 60, 2);
    tft.setTextColor(TFT_BLUE,TFT_BLACK);
    tft.print(" Rain ");
  }
  else if((forecast > 983) and ( forecast < 988)){
    tft.setCursor(55, 60, 2);
    tft.setTextColor(TFT_WHITE,TFT_BLACK);
    tft.print("Cloudy");
  }
  else{
    tft.setCursor(58, 60, 2);
    tft.setTextColor(TFT_YELLOW,TFT_BLACK);
    tft.print("Sunny"); 
  }
}

void Settings(){
  tft.setCursor(0, 0, 2);
  tft.setTextSize(1);
  tft.println("Colour indication");
  tft.drawRect(200, 5, 21, 21, TFT_WHITE);
  if ((indication == true) and (indicationChange == true)){   //when both true, print X
    tft.setCursor(207, 7, 2);                                 //indication change is necessary, otherwise it would flicker
    tft.print("X"); 
    tft.drawRect(200, 5, 21, 21, TFT_WHITE);
    indicationChange = false;
  }
  else if ((indication == false) and (indicationChange == true)){
    tft.fillRect(200, 5, 21, 21, TFT_BLACK);
    tft.drawRect(200, 5, 21, 21, TFT_WHITE);
    indicationChange = false;
  }
  
  tft.setCursor(0, 50, 2);
  tft.println("Debug");
  if ((debug == true) and (debugChange == true)){
    tft.setCursor(207, 52, 2); 
    tft.print("X"); 
    tft.drawRect(200, 50, 21, 21, TFT_WHITE);
    debugChange = false;
  }
  else if ((debug == false) and (debugChange == true)){
    tft.fillRect(200, 50, 21, 21, TFT_BLACK);
    tft.drawRect(200, 50, 21, 21, TFT_WHITE);
    debugChange = false;
  }
}
