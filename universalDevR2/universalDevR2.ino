  #include <LiquidCrystal.h>
  #include <Servo.h>
  
 
   #define SERVO_PWM_PIN   2
   #define SERVO_ANGLE_STEP 20
  #define LCD_COLUMNS  16
  #define LCD_ROWS   2
  
  #define DEBUG_SERIAL
  
  #ifdef DEBUG_SERIAL
    #define WR_SERIAL(x,y)  {Serial.print(x);Serial.print(" : ");Serial.println(y);}
  #else
    #define WR_SERIAL(x,y)   
  #endif
  #define INVALID_IDX  0
  #define GLOBAL_NO_FUNC  0xFF

   #define ARROW_LFT_CHAR (char)(127)  // <-
   #define ARROW_RGT_CHAR (char)(126)  // ->
   #define DEGREE_CHAR    (char)(223)  // deg
   #define BLOCK_CHAR     (char)(255)  //[#]

   #define DWN_KEY     'd'
#define UP_KEY      'u'
#define SET_KEY     's'
#define UNSET_KEY   's'
#define RIGHT_KEY   'r'
#define LEFT_KEY    'l'

          
uint8_t menuSelnIdx=0;
uint8_t globalFuncIdx=GLOBAL_NO_FUNC;

#define SET_MENU_SELECTION_IDX(x)  menuSelnIdx=x;
#define CLEAR_MENU_SELECTION_IDX() menuSelnIdx=INVALID_IDX;
#define GET_MENU_SELECTION_IDX()   menuSelnIdx
#define SET_GLOBAL_FUNC__IDX(x) globalFuncIdx=x;
#define GET_GLOBAL_FUNC__IDX() globalFuncIdx
#define CLEAR_GLOBAL_FUNC__IDX() globalFuncIdx=GLOBAL_NO_FUNC;
Servo myservo;
LiquidCrystal lcd(8, 9, 6, 5, 4, 3);

typedef char  key_t;
typedef uint8_t err_t;

void servoInit();
err_t servoCtrl();



typedef struct
{
  uint8_t fIdx;
  char funcName[16];
  err_t (*MenuFuncPtr)(void);
  uint8_t sts;
  
  }funcSel_t;

 
 
char lcdMenuList[][LCD_COLUMNS-2]={"PLEASE SELECT..","TACHOMETER","ESC  RMP","SERVO TESTER"}; // menu list to display
const uint8_t MENU_ROWS=sizeof(lcdMenuList)/14;

funcSel_t funcList[MENU_ROWS]={ {INVALID_IDX,"INVALID",NULL,0},
  {1,"INVALID",NULL,0},
  {2,"INVALID",NULL,0},  
  {3,"--ESC RAMP--",&servoCtrl,0}
};

char navDir=' ';
key_t pressedKey_g=' ';

boolean updateLcd=false;
uint8_t listIdx=0;
uint8_t rowCurIdx=0;


boolean isKeyPressed=false;


void servoInit()
{
  myservo.attach(SERVO_PWM_PIN);
}


err_t servoCtrl()
{
  static uint8_t servoPos;
  
   lcd.setCursor(0,0);
   lcd.println(" Servo Position ");
   lcd.setCursor(0,1);
   lcd.write(" (-)        (+)");
   lcd.setCursor(2,1);
   lcd.write( ARROW_LFT_CHAR);
   lcd.setCursor(13,1);
   lcd.write(ARROW_RGT_CHAR);
    
    lcd.setCursor(10,1);
    lcd.write(DEGREE_CHAR);
    
  if(pressedKey_g==RIGHT_KEY)
  {
    servoPos<180?servoPos+=SERVO_ANGLE_STEP:180;
    lcd.setCursor(13,1);
    delay(100);
    lcd.write(BLOCK_CHAR);
    delay(100);
    lcd.write(ARROW_RGT_CHAR);
  }
  if(pressedKey_g==LEFT_KEY)
  {
    lcd.setCursor(2,1);
    servoPos>0?servoPos-=SERVO_ANGLE_STEP:0;
    delay(100);
    lcd.write(BLOCK_CHAR);
    delay(200);
    lcd.write(ARROW_RGT_CHAR);
   }
   lcd.setCursor(6,1);
   lcd.print(servoPos);
   myservo.write(servoPos);              // tell servo to go to position in variable 'servoPos'
   return   (err_t)0;        
}

  void resetMenu();
  void resetMenu()
  {
    lcdWritePos((char*)&lcdMenuList[0][0],(char*)&lcdMenuList[1][0]);
   
     Serial.println(MENU_ROWS);
     lcd.setCursor(0,rowCurIdx);
      lcd.print(ARROW_RGT_CHAR);
    }

 void setup()
 {
  Serial.begin(9600);
  lcd.begin(16, 2);
  resetMenu();
  
  servoInit();
 }
 



void lcdWritePos(char *row1,char *row2)
{

  lcd.setCursor(2,0);
  lcd.write(row1);
  lcd.setCursor(2,1);
  lcd.write(row2);
}

uint8_t top=0,bottom=1;




int navThruMenu(key_t keyIn);


void updateKeypressInfo(key_t pressedKey)
{
  isKeyPressed=true;
  pressedKey_g=pressedKey;
  if(pressedKey == UNSET_KEY)
  {
      if(GET_GLOBAL_FUNC__IDX() != GLOBAL_NO_FUNC)
      {
          CLEAR_GLOBAL_FUNC__IDX();
          CLEAR_MENU_SELECTION_IDX();
          /*main menu reset*/
          top=0;
          bottom=1;
          updateLcd=false;
          listIdx=0;
          rowCurIdx=0;
          lcd.clear();
          resetMenu();
      }
  }
}

int navThruMenu(key_t keyInput)
{
 
   if(keyInput == SET_KEY && GET_MENU_SELECTION_IDX() != INVALID_IDX)
   {
     SET_GLOBAL_FUNC__IDX(GET_MENU_SELECTION_IDX());
     updateLcd=false;
     
   }
   if(keyInput == DWN_KEY && updateLcd)
   {
        WR_SERIAL("down,curpos",rowCurIdx);
        if(rowCurIdx==0)
        {
          lcd.setCursor(0,0);
          lcd.print(' ');
          rowCurIdx=1;
          lcd.setCursor(0,1);
          lcd.print(ARROW_RGT_CHAR);
          SET_MENU_SELECTION_IDX(top+1);
          // logic for list map and cursor pos match
        }
        else
        {
          
           if(top< MENU_ROWS -2)
           {
              top++;
              bottom=top+1;
              lcd.clear();
              rowCurIdx=0;
              lcd.setCursor(0,0);
              lcd.print(ARROW_RGT_CHAR);
              SET_MENU_SELECTION_IDX(top);
              lcdWritePos((char*)&lcdMenuList[top][0],(char*)&lcdMenuList[bottom][0]);
              lcd.setCursor(0,1);
              lcd.print(' ');
           }
           else
           {
              lcd.setCursor(0,0);
              lcd.print(' ');
              lcd.setCursor(0,1);
              lcd.print(ARROW_RGT_CHAR);
              rowCurIdx=1;
           }

            
        
             // Serial.println((char*)&lcdMenuList[top][0]);
        }
   }
   if(keyInput == UP_KEY && top>=0  && updateLcd)
   {
     //WR_SERIAL("top",top);
    if(rowCurIdx == LCD_ROWS-1)
    {
      
      lcd.setCursor(0,1);
      lcd.print(' ');
      rowCurIdx=0;
      lcd.setCursor(0,rowCurIdx);
      lcd.print(ARROW_RGT_CHAR);
      SET_MENU_SELECTION_IDX(top);
    }
    else
    {
      
      if(top >0 && !rowCurIdx)
      {
        bottom=top;
        top--;
        lcd.clear();
        lcdWritePos((char*)&lcdMenuList[top][0],(char*)&lcdMenuList[bottom][0]);
        rowCurIdx=0;
        lcd.setCursor(0,rowCurIdx);
        lcd.print(' ');
        rowCurIdx++;
        lcd.setCursor(0,rowCurIdx);
        lcd.print(ARROW_RGT_CHAR);
        SET_MENU_SELECTION_IDX(bottom);
     // Serial.println((char*)&lcdMenuList[top][0]);
      }
     
     
    }
   }
   updateLcd=false;
}

 void loop()
 {
  
  if(Serial.available())
  {
    navDir=Serial.read();
    updateKeypressInfo(navDir);
    updateLcd=true;
    
     
    
  }
  
  if(updateLcd)
  {
    navThruMenu(navDir);  
    WR_SERIAL("selnIdx: ",GET_MENU_SELECTION_IDX());
  }

  if(GET_GLOBAL_FUNC__IDX() != GLOBAL_NO_FUNC && isKeyPressed)
  {
    //funcList[GET_GLOBAL_FUNC__IDX()].sts=1;
    funcList[GET_GLOBAL_FUNC__IDX()].MenuFuncPtr();
    isKeyPressed=false;
    
  }
      
}
   
    
  
  
 
