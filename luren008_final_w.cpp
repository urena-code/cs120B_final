//Name : Luis Urena
//email: luren008@ucr.edu
//demo : https://youtu.be/O9pm1KDjcTM

//sources used: https://www.instructables.com/Bitbanging-step-by-step-Arduino-control-of-WS2811-/



#define NUM_BYTES     (NUM_RGB*3) // Number of LEDs (3 per each WS281X)
#define PORT          (PORTB)     // Digital pin's port
#define PORT_PIN      (PORTB0)    // Digital pin's bit position

unsigned char NUM_RGB = 50;

unsigned char NUM_BITS = 8;
unsigned char R = 155;
unsigned char G = 0;
unsigned char B = 155;

unsigned char* arrayOfRGBValueSet = NULL;
uint32_t t_f;
unsigned char CURR_LED_POS = 0; 
unsigned char PLAY_SIG = 0;
unsigned char BTN_PRESSED = 0;
unsigned char ROUND = 0;
unsigned char TURN_SIG=0;
unsigned char TURN_BACK_SIG=0;
unsigned char LEAST_DIST=0;

unsigned char LOC_OF_LEDLIGHTSTRIP = 8;

unsigned char SERVO_LOC=2;

//LOCATION OF THE PLAYER's BUTTONS 
unsigned char LOC_OF_P1_BUTTON = 13;
unsigned char LOC_OF_P2_BUTTON = 6;

//LOCATION OF THE COIN DEPOSIT BUTTON
unsigned char LOC_OF_COIN = 3;

//WHICH IS THE BONUS POINT 
unsigned char LOC_OF_P1_BONUS = 18;


//LOCATION OF THE ULTRASONIC SENSORS
unsigned char PIN_ECHO = 5;
unsigned char PIN_TRIG = A1;

unsigned char PIN_ECHO2 = A0;
unsigned char PIN_TRIG2 = 4;

unsigned char PLAYER1_SCORE=0;
unsigned char PLAYER2_SCORE=0;

typedef struct task {
  int state;
  unsigned long period;
  unsigned long elapsedTime;
  int (*TickFct)(int);

} task;

//declare functions

//void addToArrayofSRGB(unsigned char idx, unsigned char r, unsigned char g, unsigned char b);
void showLEDS(void);

int delay_gcd;
const unsigned short tasksNum = 5;
task tasks[tasksNum];


void flashLightLocation(){
  static int counter=0;

  if(counter%2){
    addToArrayofSRGB(CURR_LED_POS, 0 , 0, 0);
  }else{
    addToArrayofSRGB(CURR_LED_POS, R,G,B);
  }
  counter++;
  showLEDS();
}

void rotateCounterClockwise(){
  
  CURR_LED_POS--;
  if(CURR_LED_POS==0){
    CURR_LED_POS=NUM_RGB-1;
    addToArrayofSRGB(0, 0 , 0, 0);
    addToArrayofSRGB(CURR_LED_POS-1, R,G,B);
  }else{
    addToArrayofSRGB(CURR_LED_POS, 0 , 0, 0);
    addToArrayofSRGB(CURR_LED_POS-1, R,G,B);
  }
  
  showLEDS();
}



void rotateClockwise(){
  //Serial.print("rotateC");
  addToArrayofSRGB(CURR_LED_POS, 0 , 0, 0);
  if(LEAST_DIST <= 10){
  addToArrayofSRGB(CURR_LED_POS+1, R,0,0);
  }else{
    if(ROUND%3==2){
      addToArrayofSRGB(CURR_LED_POS+1, 0,0,150);
    }else if (ROUND%3==1){
      addToArrayofSRGB(CURR_LED_POS+1, 0,150,0);
    }else if (ROUND%3==0){
      addToArrayofSRGB(CURR_LED_POS+1, 0,150,150);
    }
    
    
  }
  CURR_LED_POS++;
  if(CURR_LED_POS==NUM_RGB){
    CURR_LED_POS=0;
  }
  showLEDS();
}

enum LEDS_STRIP_States { LEDS_STRIP_INIT, LEDS_STRIP_GAME_OFF, 
                         LEDS_STRIP_PLAY, LEDS_STRIP_PLAYCC, LEDS_STRIP_FLASH_ON,
                         LEDS_STRIP_FLASH_OFF,LEDS_STRIP_GAME_OVER_EVEN, 
                         LEDS_STRIP_GAME_OVER_ODD};
int LEDSTRIP_Tick(int state1) {
  switch (state1) { // State transitions
    static int i=0;
    case LEDS_STRIP_INIT:
      state1=LEDS_STRIP_GAME_OFF;
      break;
    //case on:
      
      break;
  }
  switch (state1) { // State Action
    static int i=0;
    static int j=0;
    static  int tickNum=0;

    static unsigned int multiplier=1;
    
    case LEDS_STRIP_INIT:
      break;
    case LEDS_STRIP_GAME_OFF:
      if(tickNum%30==0){
      rotateCounterClockwise();
      }
      if(PLAY_SIG){
        state1=LEDS_STRIP_PLAY;
      }
      tickNum++;
      
      break;

    case LEDS_STRIP_PLAY:
    if(ROUND >= 10){
      state1=LEDS_STRIP_GAME_OVER_EVEN;
      i=0;
      j=0;
      ROUND = 0;
      break;
    }
    if(LEAST_DIST <= 10){
      multiplier = 5;
      //Serial.println(LEAST_DIST);
      
    }else{
      multiplier = 1;
    }
    
    if(tickNum%((24-(ROUND))/multiplier)==0){ 
      rotateClockwise();
      //Serial.println(tickNum);
    }
    tickNum++;
    if(i == 65000){
      i=0;
    }
      if(BTN_PRESSED){
        state1=LEDS_STRIP_FLASH_ON;
        
        i=0;
        j=0;
      }
    break;
    
    case LEDS_STRIP_FLASH_ON:
    //Serial.println("FLASH_ON");
    i=i+1;
    tickNum++;
    if(tickNum%40==0){
      state1=LEDS_STRIP_FLASH_OFF;
      j++;
      //Serial.println(CURR_LED_POS);
      addToArrayofSRGB(CURR_LED_POS, 50,50,50);
      showLEDS();
      i=0;
    }
    if(j==10){ //so this fixes the issue when blinking it was increasing the curr_led_pos 3x when this was j==4
      state1=LEDS_STRIP_PLAY;
      j=0;
    }
    break;
    
    case LEDS_STRIP_FLASH_OFF:
    //Serial.println("FLASH_OFF");
    i++;
    tickNum++;
    if(tickNum%40==0){
      state1=LEDS_STRIP_FLASH_ON;
      addToArrayofSRGB(CURR_LED_POS, 0,0,0);
      showLEDS();
      
    }
    
    break;

    case LEDS_STRIP_GAME_OVER_EVEN:
    i++;
    PLAYER1_SCORE=0;
    PLAYER2_SCORE=0;
    PLAY_SIG=0;
    
    for(unsigned char k = 1; k <= 50 ; k++){
        //Serial.println(k);
            if(k%3==2){
              addToArrayofSRGB(k-1, 155,0,0);
            }
            if(k%3==1){
              addToArrayofSRGB(k-1, 0,0,155);
            
            }else if(k%3==0){
              addToArrayofSRGB(k-1, 0,155,0);
            }
              
      }
      showLEDS();
      
    if(i==10){
      state1=LEDS_STRIP_GAME_OVER_ODD;
      i=0;
    }
    
    if(j==4){
      j=0;
      i=0;
      state1=LEDS_STRIP_GAME_OFF;
      Serial.println("***ENTER COIN TO START GAME****");
    }
    
    break;

    case LEDS_STRIP_GAME_OVER_ODD:
    i++;
    
    for(unsigned char k = 1; k <= 50 ; k++){
        //Serial.println(k);
            if(k%3==2){
              addToArrayofSRGB(k-1, 0,0,155);
            }
            if(k%3==1){
              addToArrayofSRGB(k-1, 0,155,0);
            
            }else if(k%3==0){
              addToArrayofSRGB(k-1, 155,0,0);
            }
              
      }
      showLEDS();   
    if(i==10){
      state1=LEDS_STRIP_GAME_OVER_EVEN;
      j++;
      i=0;
    }
    
    break;
  }
  return state1;
}

enum BUTTON_States { BUTTON_INIT,BUTTON_OFF, BUTTON_SAMPLING,BUTTON_WAIT};
int BUTTON_Tick(int state2) {
  switch (state2) { // State transitions
    static int i;
    case BUTTON_INIT:
      state2=BUTTON_OFF;
       i = 0;
    case BUTTON_OFF:
      if(PLAY_SIG){
        state2=BUTTON_SAMPLING;
      }
      break;
      
      case BUTTON_WAIT:
      //Serial.println("test");
      i++;
      //Serial.println(i);
      if(i==250){
        state2=BUTTON_OFF;
        BTN_PRESSED=0;
        i=0;
      }
      break;

      
      case BUTTON_SAMPLING:
      unsigned char tempinput= !digitalRead(LOC_OF_P1_BUTTON);
      unsigned char tempinput2= !digitalRead(LOC_OF_P2_BUTTON);
      //Serial.println(tempinput);
      if(!PLAY_SIG){
        state2=BUTTON_OFF;
      }
      
      if(tempinput||tempinput2){
 
        BTN_PRESSED=1;
        state2=BUTTON_WAIT;
        Serial.print("round #");
        Serial.println(ROUND+1);
        ROUND++;

       
        unsigned char addToScore =0;
      //calculate score: 
      if(CURR_LED_POS==18){
        addToScore+=10;
        Serial.println("10 pts BONUS!!!");
      }else if(CURR_LED_POS%2==1){
        Serial.println("5 pts");
        addToScore+=5;
      }else{
        addToScore+=1;
        Serial.println("1 pt");
      }

      if(tempinput){
        PLAYER1_SCORE+=addToScore;
        }else{
        PLAYER2_SCORE+=addToScore;
        }
        
      Serial.println("==========");
      Serial.print("PLAYER 1: ");
      Serial.println(PLAYER1_SCORE);

      Serial.print("PLAYER 2: ");
      Serial.println(PLAYER2_SCORE);

      Serial.println("=========");
      
       
      }
      break;


      
  }
  switch (state2) { // State Action
    case BUTTON_INIT:
      break;
    case BUTTON_OFF:
      break;
  }
  return state2;
}

void addToArrayofSRGB(unsigned char index, unsigned char r, unsigned char g, unsigned char b) 
{
  if(index < NUM_RGB) 
  {
    unsigned char *p = &arrayOfRGBValueSet[index*3]; 
    *p++ = g;  
    *p++ = r;
    *p = b;
  }
}

enum SERVO_PWM_States { SERVO_PWM_INIT, SERVO_PWM_OFF, SERVO_PWM_PWMH, SERVO_PWM_PMWL};
int SERVO_PWM_Tick(int state3) {
    static int H;
    static int L;
    static int i;
  
  switch (state3) { // State transitions
    
    case SERVO_PWM_INIT:
      i=0;
      state3=SERVO_PWM_OFF;
      break;
      
    case SERVO_PWM_OFF:
    if(TURN_SIG||TURN_BACK_SIG){//this is coin sig simulation
      if(TURN_SIG){
      H=1;
      L=18;
      state3 = SERVO_PWM_PWMH;
      }else if(TURN_BACK_SIG){
      H=2;
      L=17;
      state3 = SERVO_PWM_PWMH;
     }
      }
      break;
      
    case SERVO_PWM_PWMH:
    i++;
    digitalWrite(SERVO_LOC,HIGH);
    if(i>=H){
        state3 = SERVO_PWM_PMWL;
        i=0;
      }
      break;

    case SERVO_PWM_PMWL:
    i++;
    digitalWrite(SERVO_LOC,LOW);
     if(i>=L){
            state3=SERVO_PWM_OFF;
            i=0;
          }
      break;
  }
  return state3;
}


enum SERVO_SIG_SM { SERVO_SIG_SM_INIT, SERVO_SIG_SM_OFF, SERVO_SIG_SM_TURN, SERVO_SIG_SM_WAIT,SERVO_SIG_SM_TURN_BACK};
int SERVO_SIG_SM_Tick(int state4){

  switch(state4){

  case SERVO_SIG_SM_INIT:
   state4=SERVO_SIG_SM_OFF;

   Serial.println("***ENTER COIN TO START GAME****");
  break;

  case SERVO_SIG_SM_OFF:
  //Serial.println("tick");
  TURN_BACK_SIG=0;
  TURN_SIG=0;

  //Serial.println(!digitalRead(LOC_OF_COIN));

  
  if(!digitalRead(LOC_OF_COIN)){
    //Serial.println("OMG");
    state4=SERVO_SIG_SM_TURN;
          Serial.println("==========");
      Serial.print("PLAYER 1: ");
      Serial.println(PLAYER1_SCORE);

      Serial.print("PLAYER 2: ");
      Serial.println(PLAYER2_SCORE);

      Serial.println("=========");
    PLAY_SIG=1;
  }
  break;

  case SERVO_SIG_SM_TURN:
    state4 = SERVO_SIG_SM_WAIT;
    TURN_SIG=1;
  break;

  case SERVO_SIG_SM_WAIT:
    TURN_SIG=0;
    state4=SERVO_SIG_SM_TURN_BACK;
    break;

  case SERVO_SIG_SM_TURN_BACK:
    TURN_BACK_SIG=1;
     state4=SERVO_SIG_SM_OFF;
    break;   
  }
  return state4;
}


enum ULTRASONIC { ULTRASONIC_INIT, ULTRASONIC_OFF, ULTRASONIC_READ};
int ULTRASONIC_Tick(int state5){

  switch(state5){

    case ULTRASONIC_INIT:
    state5=ULTRASONIC_OFF;

    break; 
   
    case ULTRASONIC_OFF:
    //Serial.println("ultrasonic off");
    if(PLAY_SIG){
      state5=ULTRASONIC_READ;
    }
    break;

    case ULTRASONIC_READ:
    //Serial.println("ultrasonicread");
    if(!PLAY_SIG){
      state5=ULTRASONIC_OFF;
    }
    
    int min_i = 5;
  int max_i = 6000;
  unsigned int distance = 0;
  unsigned char temp=0;

  //unsigned int LEAST_DIST = 0
  
  digitalWrite(PIN_TRIG,LOW);
  delayMicroseconds(60);
  digitalWrite(PIN_TRIG,HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG,LOW);
 
  delayMicroseconds(150);
  
  distance=(pulseIn(PIN_ECHO,HIGH,100000));

  distance = distance / 148;

  unsigned int distance2 = 0;

   digitalWrite(PIN_TRIG2,LOW);
   delayMicroseconds(60);
   digitalWrite(PIN_TRIG2,HIGH);
   delayMicroseconds(10);
   digitalWrite(PIN_TRIG2,LOW);
   delayMicroseconds(150);
     
  distance2=(pulseIn(PIN_ECHO2,HIGH,100000));
  distance2 = distance2/148;

  //Serial.println(distance);
  //Serial.println(distance2);


  if(distance<=distance2){
     LEAST_DIST=distance;
  }else{
     LEAST_DIST=distance2;
  }
  //Serial.println(LEAST_DIST);
    break;
    
  }
  return state5;

}

void setup() 
{
  pinMode(LOC_OF_LEDLIGHTSTRIP,OUTPUT);
  
  pinMode(LOC_OF_P1_BUTTON, INPUT_PULLUP);
  pinMode(LOC_OF_P2_BUTTON, INPUT_PULLUP);
  
  pinMode(LOC_OF_COIN, INPUT_PULLUP);
  
  pinMode(SERVO_LOC, OUTPUT);

  pinMode(PIN_ECHO,INPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO2,INPUT);
  pinMode(PIN_TRIG2, OUTPUT);

  
  
  digitalWrite(LOC_OF_LEDLIGHTSTRIP,0);
  if((arrayOfRGBValueSet = (unsigned char *)malloc(NUM_BYTES)))             
  {
    memset(arrayOfRGBValueSet, 0, NUM_BYTES);                         
  }        
  showLEDS();


  unsigned char i = 0;
  tasks[i].state = LEDS_STRIP_INIT;
  tasks[i].period = 2;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &LEDSTRIP_Tick;
  i++;
  tasks[i].state = BUTTON_INIT;
  tasks[i].period = 5;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &BUTTON_Tick;

  i++;
  tasks[i].state = SERVO_PWM_INIT;
  tasks[i].period = 1;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &SERVO_PWM_Tick;

   i++;
  tasks[i].state = SERVO_SIG_SM_INIT;
  tasks[i].period = 500;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &SERVO_SIG_SM_Tick;
  
     i++;
  tasks[i].state = ULTRASONIC_INIT;
  tasks[i].period = 500;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &ULTRASONIC_Tick;
  
  delay_gcd = 1; // GCD
  Serial.begin(9600);
}

void loop() {
  unsigned char i;
  for (i = 0; i < tasksNum; ++i) {
    if ( (millis() - tasks[i].elapsedTime) >= tasks[i].period) {
      tasks[i].state = tasks[i].TickFct(tasks[i].state);
      tasks[i].elapsedTime = millis(); // Last time this task was ran
    }
  }//Serial.println("tasksfor");
}



void showLEDS(void) 
{
  if(!arrayOfRGBValueSet) return;

  while((micros() - t_f) < 50L); 

  cli(); 
  volatile unsigned char  
   *p    = arrayOfRGBValueSet, 
    val  = *p++, 
    high = PORT |  _BV(PORT_PIN), 
    low  = PORT & ~_BV(PORT_PIN), 
    tmp  = low,    
    nbits= NUM_BITS; 
  volatile unsigned char
    nbytes = NUM_BYTES; 
  asm volatile(

    // Instruction        CLK     Description                 Phase
   "nextbit:\n\t"         // -    label                       (T =  0) 
    "sbi  %0, %1\n\t"     // 2    signal HIGH                 (T =  2) 
    "sbrc %4, 7\n\t"      // 1-2  if MSB set                  (T =  ?)          
     "mov  %6, %3\n\t"    // 0-1   tmp'll set signal high     (T =  4) 
    "dec  %5\n\t"         // 1    decrease bitcount           (T =  5) 
    "nop\n\t"             // 1    nop (idle 1 clock cycle)    (T =  6)
    "st   %a2, %6\n\t"    // 2    set PORT to tmp             (T =  8)
    "mov  %6, %7\n\t"     // 1    reset tmp to low (default)  (T =  9)
    "breq nextbyte\n\t"   // 1-2  if bitcount ==0 -> nextbyte (T =  ?)                
    "rol  %4\n\t"         // 1    shift MSB leftwards         (T = 11)
    "rjmp .+0\n\t"        // 2    nop nop                     (T = 13)
    "cbi   %0, %1\n\t"    // 2    signal LOW                  (T = 15)
    "rjmp .+0\n\t"        // 2    nop nop                     (T = 17)
    "nop\n\t"             // 1    nop                         (T = 18)
    "rjmp nextbit\n\t"    // 2    bitcount !=0 -> nextbit     (T = 20)
   "nextbyte:\n\t"        // -    label                       -
    "ldi  %5, 8\n\t"      // 1    reset bitcount              (T = 11)
    "ld   %4, %a8+\n\t"   // 2    val = *p++                  (T = 13)
    "cbi   %0, %1\n\t"    // 2    signal LOW                  (T = 15)
    "rjmp .+0\n\t"        // 2    nop nop                     (T = 17)
    "nop\n\t"             // 1    nop                         (T = 18)
    "dec %9\n\t"          // 1    decrease bytecount          (T = 19)
    "brne nextbit\n\t"    // 2    if bytecount !=0 -> nextbit (T = 20)
    ::
    // Input operands         Operand Id (w/ constraint)
    "I" (_SFR_IO_ADDR(PORT)), // %0
    "I" (PORT_PIN),           // %1
    "e" (&PORT),              // %a2
    "r" (high),               // %3
    "r" (val),                // %4
    "r" (nbits),              // %5
    "r" (tmp),                // %6
    "r" (low),                // %7
    "e" (p),                  // %a8
    "w" (nbytes)              // %9
  );
  sei();                          // Enable interrupts
  t_f = micros();                 // t_f will be used to measure the 50us 
                                  // latching period in the next call of the 
                                  // function.
}
