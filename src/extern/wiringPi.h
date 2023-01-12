#define OUTPUT 1
#define HIGH 1
#define LOW 0

int  wiringPiSetup(void) ;
void  delay(unsigned int howLong) ;
void pinMode(int pin, int mode) ;
void digitalWrite(int pin, int value) ;
