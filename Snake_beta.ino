
#include<Wire.h>
#include<SPI.h>
#include<Adafruit_GFX.h>       
#include<Adafruit_SSD1306.h>  
// display set      (width,height)
Adafruit_SSD1306 display(128,64, &Wire, -1);  

#define INTPIN    3        
#define UPPIN     4       
#define DWNPIN    5
#define LFTPIN    7
#define RHTPIN    6
#define SND       9


// define directions
#define DIRUP     1       // these values is what the "snake" looks at to decide-
#define DIRDOWN   2       // the direction the snake will travel
#define DIRLEFT   3
#define DIRRIGHT  4

// set button variables

volatile uint8_t buttonpressed=0;        
bool butup=0;
bool butdown=0;   // we use this to set true to "detect" which direction pressed
bool butleft=0;
bool butright=0;

 // snake ints
byte snakePosX[30]; // array to make body of snake
byte snakePosY[30];

int snakeX=30;     // snake head position
int snakeY=30;
int snakeSize=1;   // snake size count limited to the size of the array

// world ints

uint8_t worldMinX=0;    // these set the limits of the play area
uint8_t worldMaxX=128;
uint8_t worldMinY=10;
uint8_t worldMaxY=63;

// collect food and position of food
bool scranAte =0;
uint8_t scranPosX=0;
uint8_t scranPosY=0;

// scores variables
long playscore=0;   
long highscore=0;  



void interruptpressed()
    {
      delay(150);  // slight delay for added "bounce" protection
      updatedirection();
    }

void updatedirection()
    {
      Serial.println("updatingdirection");
      butup=digitalRead(UPPIN);       // check which input went high and set relevant bool true
      butdown=digitalRead(DWNPIN);
      butleft=digitalRead(LFTPIN);
      butright=digitalRead(RHTPIN);
     
      
        if(butup == 0) //
           {
              buttonpressed=DIRUP;
              Serial.println("UP pressed");
              Serial.println(buttonpressed);
              butup=true;
              tone(SND,1500,10);
          }
        if(butdown==0)
          {
              buttonpressed=DIRDOWN;
         //     Serial.println("DOWN pressed");
         //     Serial.println(buttonpressed);
              butdown=true;
              tone(SND,1500,10);
          }

        if(butleft==0)
          {
              buttonpressed=DIRLEFT;
          //    Serial.println("LEFT pressed");
          //    Serial.println(buttonpressed);
              butleft=true;
              tone(SND,1500,10);
          }
        if(butright==0)
          {
               buttonpressed=DIRRIGHT;
          //     Serial.println("RIGHT pressed");
          //     Serial.println(buttonpressed);
                butright=true;
                tone(SND,1500,10);
          }
     }




void updateDisplay()  // draw scores and outlines  
    {
           // Serial.println("Update Display");

            display.fillRect(0,0, display.width()-1,8,BLACK);
            display.setTextSize(0);
            display.setTextColor(WHITE);
           
            // draw scores
            display.setCursor(2,1);
            display.print("Score:");
            display.print(String(playscore, DEC));
            display.setCursor(66,1);
            display.print("High:");
            display.print(String(highscore ,DEC));
            // draw play area
            //        pos  1x,1y, 2x,2y,colour
            display.drawLine(0,0,127,0,WHITE); // very top border
            display.drawLine(63,0,63,9,WHITE); // score seperator
            display.drawLine(0,9, 127,9,WHITE); // below text border
            display.drawLine(0,63,127,63,WHITE); // bottom border
            display.drawLine(0,0,0,63,WHITE); // left border
            display.drawLine(127,0,127,63,WHITE); //right border

        

    }


void updateGame()     // this updates the game area display
    {
      //Serial.print(analogRead(Y_PIN));
        display.clearDisplay();

          display.drawPixel(scranPosX,scranPosY, WHITE);
          scranAte = scranFood();   

            // check snake routines

              if (outOfArea()||selfCollision())
                   {
                      gameOver();
                   }

            // display snake
              for(int i=0;i<snakeSize;i++)
                  {
                    display.drawPixel(snakePosX[i],snakePosY[i],WHITE);
                  }
              
               // remove end pixel as movement occurs
              for(int i=snakeSize;i>0;i--)
                    {
                      snakePosX[i] = snakePosX[i-1];
                      snakePosY[i] = snakePosY[i-1];
                    }
              // add a extra pixel to the snake
              if(scranAte)
                  {
                    snakeSize+=1;
                    snakePosX[snakeSize-1]=snakeX;
                    snakePosY[snakeSize-1]=snakeY;
                  }


            switch(buttonpressed) // was snakeDirection
                  {
                    case DIRUP:
                        snakeY-=1;
                        break;
                    case DIRDOWN:
                        snakeY+=1;
                        break;
                    case DIRLEFT:
                        snakeX-=1;
                        break;
                    case DIRRIGHT:
                        snakeX+=1;
                        break;
                  }

        snakePosX[0] = snakeX;
        snakePosY[0] = snakeY;
        

          updateDisplay();
          display.display();
    }


        void placeScran()
            {
              scranPosX=random(worldMinX+1,worldMaxX-1);
              scranPosY=random(worldMinY+1,worldMaxY-1);

               }
               
        bool scranFood()
            {
                  if(snakeX==scranPosX&&snakeY==scranPosY)
                      {
                          playscore=playscore+10;

                           tone(SND,2000,10);
                              updateDisplay();
                              placeScran();
                            return 1;
                      }
                    else
                      {
                          return 0;
                      }
            }


    bool outOfArea()
        {
          return snakeX <= worldMinX||snakeX >=worldMaxX ||snakeY<=worldMinY|| snakeY>=worldMaxY;
        }


    void gameOver()
       {
          uint8_t rectX1,rectY1,rectX2,rectY2;
         
          rectX1=38;
          rectY1=28;
          rectX2=58;
          rectY2=12;
          display.clearDisplay();
          display.setCursor(40,30);
          display.setTextSize(1);
          tone(SND,2000,50);
          display.print("GAME ");
          tone(SND,1000,50);
          display.print("OVER");
    
          if(playscore>=highscore)   //check to see if score higher than high score
              {  
                highscore=playscore;  //single if statment to update high score
              }

             
          for(int i=0;i<=16;i++)   // this is to draw rectanlges around game over
                {
                  display.drawRect(rectX1,rectY1,rectX2,rectY2,WHITE);
                  Serial.println("if loop");
                  display.display();
                    rectX1-=2;      // shift over by 2 pixels
                    rectY1-=2;
                    rectX2+=4;      // shift over 2 pixels from last point
                    rectY2+=4;
                    tone(SND,i*200,3);
                }
           display.display();

          
                    //Screen Wipe after fame over
                    rectX1=0;   // set start position of line
                    rectY1=0;
                    rectX2=0;
                    rectY2=63;

                for (int i =0;i<=127;i++)
                      {
                           uint8_t cnt=0;
                           display.drawLine(rectX1,rectY1,rectX2,rectY2,BLACK); 
                            rectX1++;
                            rectX2++;
                            display.display();                  
                         
                      }
            display.clearDisplay();
          playscore=0;        // reset snake and player details
          snakeSize=1;
          snakeX=display.width()/2;
          snakeY=display.height()/2;

           waitForPress();        // wait for player to start game

          }

        void waitForPress()
              {
                bool waiting=0;  // loop ends whjen this is true
                display.clearDisplay();
                while(waiting==0)
                     
                    {


                              drawALineForMe(WHITE); // draw a random white line
                              drawALineForMe(BLACK); // draw a random black line so that the screen not completely fill white
                              display.fillRect(19,20,90,32,BLACK);        // blank background for text
                              display.setTextColor(WHITE);
                              display.setCursor(35,25);
                              display.setTextSize(2); // bigger font
                              display.println("SNAKE");
                                              //    x  y   w  h r  col
                              display.drawRoundRect(33,22,62,20,4,WHITE);   // border Snake
                              display.drawRect(19,20,90,32,WHITE);          // border box  - 3
                              display.setCursor(25,42);
                              display.setTextSize(0);                       // font back to normal
                              display.println("press START key");
                              display.fillRect(0,0,127,8,BLACK);
                              display.setCursor(10,0);
                              display.print("High Score :");                // display the high score
                              display.print(highscore);
                              display.display();
                              waiting = !digitalRead(INTPIN);  // check to see if key pressed waiting will change to 1 ending while
                              //waiting = !digitalRead(SW_PIN);
                              buttonpressed=0;        // reset button press to no direction
                        
                    }
              }

    void drawALineForMe(uint8_t clr)
            {
                     uint8_t line1X,line1Y,line2X,line2Y=0;
                          // set random co-ordinates for a line then draw it     
                          //  variable         no less      no more
                              line1X = random(worldMinX+1,worldMaxX-1);
                              line1Y = random(worldMinY+1,worldMaxY-1);
                              line2X = random(worldMinX+1,worldMaxX-1);
                              line2Y = random(worldMinY+1,worldMaxY-1);
                           
                              display.drawLine(line1X,line1Y,line2X,line2Y,clr);
            }


    bool selfCollision()
        {
          for(byte i=4;i<snakeSize;i++)
              {   // see if snake X and Y match == snakePos X and Y return true 1 if so
                  if (snakeX==snakePosX[i]&&snakeY==snakePosY[i])  
                        {
                          tone(SND,2000,20);
                          tone(SND,1000,20);
                          
                          return 1; 
                        }
              }
            return 0;
           }

void setup() 
  {
    delay(100); // just give stuff a chance to "boot"
        Serial.begin(9600);
        
        // uncheck this if you want to see the serial outputs
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
    display.clearDisplay();   // start with a clean display
    display.setTextColor(WHITE);// set up text color rotation size etc  
    display.setRotation(0); 
    display.setTextWrap(false);
    display.dim(0);         // set the display brighness
    pinMode(INTPIN,INPUT_PULLUP);  // set the correct ports to inputs
    pinMode(UPPIN,INPUT_PULLUP);
    pinMode(DWNPIN,INPUT_PULLUP);
    pinMode(LFTPIN,INPUT_PULLUP);
    pinMode(RHTPIN,INPUT_PULLUP);
    //pinMode(SW_PIN, INPUT_PULLUP);
    // this is the interupt command this "halts" the arduino to read the inputs
    //command-          function-         pin-function to execute-condition on pin
    //attachInterrupt(digitalPinToInterrupt(INTPIN),interruptpressed,RISING); 
    // Serial.println("Setup Passed");
    waitForPress();    // display the snake start up screen
    placeScran();  // place first bit of food
  }

void loop() 
  {
    
    updatedirection();
    updateGame();  
  }
