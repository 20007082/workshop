#include "SPI.h"
#include "TFT_eSPI.h"
#include "bitmaps.h"



// Use hardware SPI (Aansturing van het scherm)
TFT_eSPI tft = TFT_eSPI();



#define Joystick_X A6
#define Joystick_Y A7
#define Joystick_Button 5
#define maximum_snake_lenght 300 // maximum lengte dat de slang kan worden
#define speelveld_breedte 31 // Speelveld breedte
#define speelveld_hoogte 20 // Speelveld hoogte

#define p2s_x(p_x) (10*p_x+5) // Omrekening in spel Coordinaten
#define p2s_y(p_y) (10*p_y+35) // Omrekening in spel Coordinaten
// Een define mag ook een kleine functie zijn

//------------------------------------------------------------------------------------------------------------------------
//----------------------------------Variabelen initialiseren--------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
int JoystickX;
int JoystickY;

int score = 0;
int highscore=0;
int game_status = 0; // Waar de game start

int CursorX;
int CursorY;  

int dx=1,dy=0;
int slang_lengte; // Actuelle slang lengte
byte blocked[speelveld_breedte][speelveld_hoogte]; //Maximale lengte van de slang
byte position_x[maximum_snake_lenght];
byte position_y[maximum_snake_lenght];
int head = 0; 
int tail = 0; 
int counter = 5;

int random_food_x; // Variabele x waar het eten komt
int random_food_y; // Variabele y waar het eten komt
int random_generator_for_food_x; // Random genereert voor de appel
int random_generator_for_food_y; // Random genereert voor de appel

//------------------------------------------------------------------------------------------------------------------------
//-----------------------------------Variabelen initialiseren einde-------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------------------------------------------
//-----------------------------------Declaratie functies------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
void welcome_screen()
{
  tft.fillScreen(TFT_BLACK); // Om de achtergrond zwart te maken
  tft.setTextColor(TFT_WHITE, TFT_BLACK); // Om de kleur van de tekst juist te zetten
  tft.setTextSize(3); // Tekst grootte 
  tft.setCursor(40, 30); // Coordinaten waar je de tekst gaat schrijven
  tft.print("Welkom bij"); // Tekst
  tft.drawBitmap(110, 75, wellcome_snake, 92, 56, TFT_GREEN); //bitmap van een slang tekenen
  tft.setCursor(35, 160); // Coordinaten waar je de tekst gaat schrijven
  tft.setTextSize(2); //Tekst grootte 
  tft.print("Druk op de Joystick om"); // Tekst
  tft.setCursor(35, 180); 
  tft.print("te starten");
  
}

void game_screen()
{

  
  //Toon speelveld, achtergrond en Text:Score
  tft.fillScreen(TFT_BLACK); // Om de achtergrond zwart te maken
  tft.drawRect(0, 30, 320, 210, TFT_PURPLE); // Rechthoek maken
  tft.drawRect(1, 31, 318, 208, TFT_PURPLE);
  tft.drawRect(2, 32, 316, 206, TFT_PURPLE);
  tft.drawRect(3, 33, 314, 204, TFT_PURPLE);
  tft.drawRect(4, 34, 312, 202, TFT_PURPLE);
  
  tft.setTextColor(TFT_GREEN, TFT_BLACK); 
  tft.setTextSize(3);
  tft.setCursor(65, 4); 
  tft.print("Score:"); 
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.print(score); //om de variabele score te tonen
}

void loser_screen()
{
  
  tft.fillScreen(TFT_BLACK); 
  tft.setTextColor(TFT_WHITE, TFT_BLACK); 
  tft.setTextSize(3);
  tft.setCursor(50, 10);
  tft.print("! Verloren !"); 
  tft.drawBitmap(100, 55, dead_snake, 124, 38, TFT_RED);
  tft.setCursor(65, 120);
  tft.setTextColor(TFT_PURPLE, TFT_BLACK);
  tft.print("Score:");
  tft.print(score);
  tft.setCursor(40, 150);
  tft.print("Highscore:");
  tft.print(highscore);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK); 
  tft.setCursor(12, 200); 
  tft.print("Speel opnieuw?");
  tft.setCursor(12, 220);
  tft.print("Druk op de Joystick");
}

void food_generator()
{
      
    random_generator_for_food_x = random(0,31); // Random waarde tussen 0 en 31
    random_generator_for_food_y = random(0,20); // Random waarde tussen 0 en 20
    random_food_x = p2s_x(random_generator_for_food_x); // x coordinaat voor de appel
    random_food_y = p2s_y(random_generator_for_food_y); // Y coordinaat voor de appel
}

void move_tail()
{    
    if(counter > 0)
    {
      counter-=1;
    }
    else
    {
      tft.fillRect(p2s_x(position_x[tail]), p2s_y(position_y[tail]), 10, 10, TFT_BLACK); // Maakt het einde van de slang terug zwart
      blocked[position_x[tail]][position_y[tail]] = 0; //Het laatste blokje van de staart moet terug uit de array gehaald worden 
                                                       //mochten we hier anders opnieuw overheen komen, zal dit als een collisie tellen
      tail++; //Staart langer maken
      if(tail >= maximum_snake_lenght) //Maxium lengte van de staart controleren en dan reseten
      {
       tail = 0;
      }
    }
}

void print_score()
{
    tft.fillRect(173, 4, 250, 24, TFT_BLACK); //Scherm deel van de score wissen
    tft.setTextColor(TFT_PURPLE);
    tft.setTextSize(3); 
    tft.setCursor(65, 4); 
    tft.print("Score:");
    tft.print(score);
}
//------------------------------------------------------------------------------------------------------------------------
//----------------------------------------Functie einde-------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------------
//----------------------------------------Void Setup----------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
void setup() //Io initialiseren
{
  Serial.begin(9600);
  pinMode(Joystick_Button, INPUT_PULLUP);
  tft.begin(); // Scherm initalisieren
  tft.setRotation(3); // Display orientatie juist zetten
  Serial.print(blocked[speelveld_breedte][speelveld_hoogte]);

}

//------------------------------------------------------------------------------------------------------------------------
//----------------------------------------Void Setup einde----------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------------------------------------------
//----------------------------------------Void Loop-----------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
void loop(void) //code die altijd doorlopen wordt
{
      
  //De x en y as van de joystick uitlezen
  int JoystickX = analogRead(Joystick_X);
  int JoystickY = analogRead(Joystick_Y);
  

  switch(game_status) 
  {
    case 0: //De game is nog niet gestart
          welcome_screen();
          game_status=1;
          break;

    case 1:
      if(digitalRead(Joystick_Button)==LOW) //Wanneer er op de joystick wordt gedrukt zal de game starten
      {
        game_screen(); // Game scherm starten
        game_status = 2;
        //Serial.println("De game zal starten ..."); //Op de serial monitor laten zien dat we de game gestart hebben

        //Variabelen juist resetten --------------
        CursorX = 0; dx=1;
        CursorY = 0; dy=0;
        head=0;
        tail=0;
        counter=4;
        score=0;
        print_score();
        // ------------------------------------

        //Serial.println("Speelveld leegmaken..."); //Speelveeld leeg maken. Dit doen we door de speelveld array te vullen met 0'n
        for (int i=0;i<31;i++) {
          for (int j=0;j<20;j++) {
            blocked[i][j]=0;
          }
        }
        //Serial.println(blocked[0][0]);
        //Serial.println("Speelveld leeggemaakt...");
        food_generator();
      }

      break;
      
    case 2: // De game is gestart
    
      if(blocked[CursorX][CursorY]==1){//Controleren of de slang niet door zichzelf heen gaat
        game_status=3; //Als je dood gaat.... 
        if(score > highscore)
        {
          highscore = score;
        }
      }
      // ------------------ Slang verplaatst zich ------------------------------------------------------------------------------------------------------------------------------------------------------

      tft.fillRect(random_food_x,random_food_y, 10, 10, TFT_RED); // Teken appel
      tft.fillRect(p2s_x(CursorX), p2s_y(CursorY), 10, 10, TFT_GREEN); // Teken de slang
      tft.fillRect(random_food_x,random_food_y, 10, 10, TFT_RED); // Teken appel
      blocked[CursorX][CursorY]=1;
        
      delay(125);

      position_x[head] = CursorX; //x positie van de slang opslaan
      position_y[head] = CursorY; //y positie van de slang opslaan
      head++; // Slang 1 blokje langer maken

      if(head >= maximum_snake_lenght) // Controleer of de slang zijn maxium lengte bereikt heeft
      {
        head=0;
      }

      move_tail();

      // ------------------ Slang verplaatst zich einde------------------------------------------------------------------------------------------------------------------------------------------------------
  
      //------Dedetecteer botsing en beeindig het spel ----------------


      CursorX+=dx;
      CursorY+=dy;
      
      if(CursorX == 31) // Tegen de rechterrand aangelopen
      {
        game_status = 3;
        if(score > highscore)
        {
          highscore = score;
        }
      }
      if(CursorX < 0) // Tegen de linkerrand aangelopen
      {
        game_status = 3;
        if(score > highscore)
        {
          highscore = score;
        }
      }
      if(CursorY == -1) // Tegen de bovenrand aangelopen
      {
        game_status = 3;
        if(score > highscore)
        {
          highscore = score;
        }
      }
      if(CursorY == 20) // Tegen de onderrand aangelopen
      {
        game_status = 3;
        if(score > highscore)
        {
          highscore = score;
        }
      }

      //------Dedetecteer botsing en beeindig het spel einde----------------
      
      //--------------------Appel gegeten -----------------------------------

      if(random_generator_for_food_x == CursorX && random_generator_for_food_y == CursorY) // Controleer of de slang de appel heeft gegeten, dan...
      {
        food_generator(); // Nieuwe appel maken
        counter=1; // Slang 1 blokje langer maken
        score+=10; // Score gaat met 10 punten omhoog
        print_score(); // Toon score
      }
      //--------------------Appel gegeten einde--------------------------------------

      //--------------Controleer joystick Positie en schrijf weg ---------------------
   
      if(JoystickX>2500)
      {
        //Serial.println("Gaan naar links ");  
        dx=1;dy=0;     
       }   
      if(JoystickX<500)
      {
        //Serial.println("Gaan naar rechts");
        dx=-1;dy=0;
      }
      if(JoystickY>2500)
      {
        //Serial.println("Gaan naar onder");
        dx=0;dy=-1;
      }
      if(JoystickY<500)
      {
        //Serial.println("Gaan naar boven");
        dx=0;dy=1;
      } 
      //--------------Controleer joystick Positie en schrijf weg einde----------------
      
      break;   

    case 3:
    {
      loser_screen(); //Toon eind scherm
      game_status=1; // Game resetten naar status 1, zodat we kunnen herstarten
      break;
    }
    
  }

}
 
//------------------------------------------------------------------------------------------------------------------------
//----------------------------------------Void Loop einde------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
