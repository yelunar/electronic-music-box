## 음악을 변경할 수 있고 가사가 나오는 전자 오르골
### (An electronic music box with lyrics that can be changed)
### 


> **<개발 이유>**
 
  시중에 판매하고 오르골은 정해진 특정 음악만 재생할 수 있다는 한계가 있습니다. 또한, 사용자가 테이프를 감아야만 모형이 돌아가며 음악이 재생되는 번거로움이 있습니다. 이러한 오르골의 한계를 해결하기 위해 Codevision을 사용하여 ‘음악을 변경할 수 있는 자동 오르골’을 고안하였습니다.
  
  > **<작동 원리>**

 ATMega128을 이용하여 모터, 스피커를 제어합니다. 회전 모터를 사용해 자동으로 모형이 돌아가게 하여 기존 오르골의 한계를 해결합니다. 멜로디 테이프가 내장된 오르골이 아닌, 회로 결선과 Codevision을 사용한 오르골로 코드 변경을 통해 음악이 변경이 가능하도록 합니다. LCD 액정에 가사가 나오게 하였고 액정에 나오는 문구는 음악과 마찬가지로 사용자가 원하는 대로 변경할 수 있습니다.
 
 
  실제 오르골 모형을 재현하기에 무리가 있으므로 간단한 종이 모형을 제작하여 모터에 부착하였습니다. 무한한 음악을 Codevision을 통해서 연주할 수 있지만 하나의 곡(‘we wish your merry christmas’)을 연주할 수 있는 코드를 예시로 입력하여 다양한 음악 연주 가능성을 살펴볼 수 있습니다.


 ###   
  
  ### 사용 LCD
  
  ###
  ![사용LCD](file:///C:/Users/lg/Downloads/%EC%82%AC%EC%9A%A9%20LCD.bmp)
  
  ### 구현 사진
  ![동작사진](https://raw.githubusercontent.com/yelunar/electronic-music-box/main/%EA%B5%AC%ED%98%84%20%EC%82%AC%EC%A7%84.bmp)

소스코드는 다음과 같이 작성할 수 있습니다.

``` C

#include <mega128.h>
#include <delay.h>

#define VLOA    9008    // octave 2
#define VLOAX   8580
#define VLOB    8098

#define LOC     7644    // 도 octave 3
#define LOCX    7214
#define LOD     6810    // 레 
#define LODX    6427
#define LOE     6066    // 미 
#define LOF     5726    // 파 
#define LOFX    5404
#define LOG     5101    // 솔 
#define LOGX    4815
#define LOA     4544    // 라 
#define LOAX    4289
#define LOB     4049    // 시 

#define MIC     3821    // 도 octave 4 (기본 음계)
#define MICX    3607      
#define MID     3404    // 레 
#define MIDX    3213
#define MIE     3033    // 미 
#define MIF     2862    // 파 
#define MIFX    2702
#define MIG     2550    // 솔 
#define MIGX    2407
#define MIA     2272    // 라 
#define MIAX    2144
#define MIB     2024    // 시 

#define HIC     1910    // 도 octave 5
#define HICX    3607      
#define HID     3404    // 레 
#define HIDX    3213
#define HIE     3033    // 미 
#define HIF     2862    // 파 
#define HIFX    2702
#define HIG     2550    // 솔 
#define HIGX    2407
#define HIA     2272    // 라 
#define HIAX    2144
#define HIB     2024    // 시 

// note length
#define NOTE32  1*3
#define NOTE16  2*3
#define NOTE16D 3*3
#define NOTE8   4*3
#define NOTE8D  6*3
#define NOTE4   8*3
#define NOTE4D  12*3
#define NOTE2   16*3
#define NOTE2D  24*3
#define NOTE1   32*3

// rest length
#define REST32  1*3
#define REST16  2*3
#define REST16D 3*3
#define REST8   4*3
#define REST8D  6*3
#define REST4   8*3
#define REST4D  12*3
#define REST2   16*3
#define REST2D  24*3
#define REST1   32*3

#define LINE2    0xC0    // 2nd Line Move
#define HOME    0x02    // Cursor Home
#define CLEARDISPLAY    0x01   // clear display

void LCD_init(void);
void LCD_String(char flash str[]);
void Busy(void);
void Command(unsigned char);
void Data(unsigned char);

unsigned int tempo=0;
unsigned int cnt,sec;

void play_note(unsigned int sound, unsigned int note);
void play_rest(unsigned int rest);

void main(void)
{               
    DDRB = 0b00100000;    // PB5(OC1A) 출력 설정 (스피커)
    DDRE = 0b00111011;   // PE3 (OC3A) 출력 설정  
    DDRF = 0b00000011;   // PF0,1 출력 설정 (모터)
        
    TCCR1A = 0b01000000;      // COM1A1,A0(7,6)-01, mode 4 - CTC모드 / OCR1A (WGM11,10 - 00)
    TCCR1B = 0b00001010;      // mode 4(WGM13,12 - 01), 프리스케일 = 8 
    
    TCCR3A = 0b10000010;     // mode 14(ICRn top fast PWM) : 1110 (10Bit top), 1,0(WGM11,10)-10, 7,6(COM1A1,COM1A0)-10
    TCCR3B = 0b00011011;     // 4,3(WGM13,12)-11, 2-0(CS)-011 (64분주) 
    ICR3H = 0x09;            // ICR1 = 2499 (0x09C3) -> 주기 : 10msec
    ICR3L = 0xC3;  
    TCNT3H = 0x00;
    TCNT3L = 0x00;            // 타이머3 초기화     
    OCR3AH = 0b00000010;
    OCR3AL = 0b11000000;


    TIMSK = 0b00000001;
    TCCR0 = 0b00000111;    // 일반모드, 프리스케일 = CK/1024
    TCNT0 = 0b00000000;    // 타이머/카운터0 레지스터 초기값
    SREG = 0b10000000;         
    
    
    PORTF=0b00000001;  // 초기 모터 설정
    delay_ms(1000);  
      
    
    LCD_init();

        
 while(1){       // while(1) : 무한 루프
            tempo = 3; 
            
        
            Command(HOME); 
            LCD_String("We wish you a");         // 라인1
            Command(LINE2);
            LCD_String("Merry christmas");       // 라인2                                                       
            play_note(MID, NOTE4);    //레
            play_note(MIG, NOTE4);    //솔      
            play_note(MIG, NOTE8);    //솔
            play_note(MIA, NOTE8);    //라
            play_note(MIG, NOTE8);    //솔 
            play_note(MIFX, NOTE8);   //파샵
            play_note(MIE, NOTE4);    //미
            play_note(MIE, NOTE4);    //미    
            

            play_note(MIE, NOTE4);  //미
            play_note(MIA, NOTE4);  //라
            play_note(MIA, NOTE8);  //라
            play_note(MIB, NOTE8);  //시
            play_note(MIA, NOTE8);  //라
            play_note(MIG, NOTE8);  //솔
            play_note(MIFX, NOTE4); //파샵
            play_note(MID, NOTE4);  //레
            
            
            play_note(MID, NOTE4);  //레
            play_note(MIB, NOTE4);  //시
            play_note(MIB, NOTE8);  //시
            play_note(HIC, NOTE8);  //도
            play_note(MIB, NOTE8);  //시
            play_note(MIA, NOTE8);  //라
            play_note(MIG, NOTE4);  //솔
            play_note(MIE, NOTE4);  //미
            
            Command(CLEARDISPLAY);
            Command(HOME); 
            LCD_String("And a happy");          // 라인1
            Command(LINE2);
            LCD_String("New year");             //라인2
            play_note(MID, NOTE8);  //레
            play_note(MID, NOTE8);  //레
            play_note(MIE, NOTE4);  //미
            play_note(MIA, NOTE4);  //라
            play_note(MIFX, NOTE4);  //파샵
            play_note(MIG, NOTE1);  //솔        
            
            
            Command(CLEARDISPLAY);
            Command(HOME); 
            LCD_String("Good tidings");          //라인1
            Command(LINE2);
            LCD_String("       to you");         //라인2
            play_note(MID, NOTE4);  //레
            play_note(MIG, NOTE4);  //솔
            play_note(MIG, NOTE4);  //솔
            play_note(MIG, NOTE4);  //솔
            play_note(MIFX, NOTE2); //파샵
            
            Command(CLEARDISPLAY);
            Command(HOME); 
            LCD_String("wherever");          //라인1
            Command(LINE2);
            LCD_String("   you are");        //라인2
            play_note(MIFX, NOTE4); //파
            play_note(MIG, NOTE4);  //솔
            play_note(MIFX, NOTE4); //파샵
            play_note(MIE, NOTE4);  //미
            play_note(MID, NOTE2);  //레 

            Command(CLEARDISPLAY);
            Command(HOME); 
            LCD_String("Good tidings");          //라인1
            Command(LINE2);
            LCD_String("for Christmas");         //라인2
            play_note(MIA, NOTE4);  //라
            play_note(MIB, NOTE4);  //시
            play_note(MIA, NOTE4);  //라
            play_note(MIG, NOTE4);  //솔
            play_note(HID, NOTE4);  //레
            play_note(HID, NOTE4);  //레
            
            Command(CLEARDISPLAY);
            Command(HOME); 
            LCD_String("And a happy");          //라인1
            Command(LINE2);
            LCD_String("New year");             //라인2
            play_note(HID, NOTE8);  //레
            play_note(HID, NOTE8);  //레
            play_note(MIE, NOTE4);  //미
            play_note(MIA, NOTE4);  //라
            play_note(MIFX, NOTE4);  //파샵
            play_note(MIG, NOTE1);  //솔
                                            
            
          
         
    }
}

void play_note(unsigned int sound, unsigned int note)
{
    OCR1A = sound;
    TCNT1 = 0x0000;   

    TCCR1A = 0b01000000;    // COM1A1,A0(7,6)-01, mode 4 - CTC모드 / OCR1A (WGM11,10 - 00)
    TCCR1B = 0b00001010;    // mode 4(WGM13,12 - 01), 프리스케일 = 8
    
    delay_ms(note * tempo * 7);
    
    TCCR1B = 0b00001000;    // speaker off
}

void play_rest(unsigned int rest)
{
        delay_ms(rest * tempo * 7);
}
 

interrupt [TIM0_OVF] void timer_int0(void) 
{                    
    cnt++; // 16.384ms x 61 = 0.999424sec
    if(cnt == 61) {
        sec++;
        
            if(sec==1)
                {   
                    PORTF=0b00000001;
                    
                }
                
            else if(sec==2)
                {
                    PORTF=00000000; 
                }
                    
    }
}



// LCD 초기화 함수 
void LCD_init(void)
{       
	DDRC = 0xFF;	// 포트 C 출력 설정
	PORTC &= 0xFB;	//E = 0;

	// 충분한 지연시간을 통한 안정화 과정 	              
	delay_ms(15);	
	Command(0x20);	// D5=1
	delay_ms(5);
	Command(0x20);	// D5=1
	delay_us(100);
	Command(0x20);	// D5=1

	// 초기화 과정
	Command(0x28);	// function set
	Command(0x06);	// entry mode set
	Command(0x01);	// all clear
	Command(0x0c);	// display on
}

                 
// 인스트럭션 쓰기 함수
void Command(unsigned char byte)
{
	Busy();

	// 인스트럭션 상위 바이트
	PORTC = (byte & 0xF0);	// 데이터	
	PORTC &= 0xFE;		// RS = 0;           
	PORTC &= 0xFD;		// RW = 0;  
	delay_us(1);     
	PORTC |= 0x04;		// E = 1;
	delay_us(1);
	PORTC &= 0xFB;		// E = 0;

	// 인스트럭션 하위 바이트
	PORTC = ((byte<<4) & 0xF0);	// 데이터
	PORTC &= 0xFE;		// RS = 0;
	PORTC &= 0xFD;		// RW = 0;
	delay_us(1);     	
	PORTC |= 0x04;		// E = 1;
	delay_us(1);
	PORTC &= 0xFB;		// E = 0;
}

// 문자열 출력 함수
void LCD_String(char flash str[])
{
	char flash *pStr=0;
	
	pStr = &str[0];	
	while(*pStr) Data(*pStr++);
}

// char flash : pointer declaration for program memory
// char eeprom : pointer declaration for EEPROM

//데이터 쓰기 함수
void Data(unsigned char byte)
{
	Busy();
        
	// 데이터 상위 바이트
	PORTC = (byte & 0xF0);	// 데이터
	PORTC |= 0x01;		//RS = 1;
	PORTC &= 0xFD;		//RW = 0;
	delay_us(1);     	
	PORTC |= 0x04;		//E = 1;
	delay_us(1);
	PORTC &= 0xFB;		//E = 0;

	// 데이터 하위 바이트
	PORTC = ((byte<<4) & 0xF0);  	// 데이터
	PORTC |= 0x01;		//RS = 1;
	PORTC &= 0xFD;		//RW = 0;     
	delay_us(1);     	
	PORTC |= 0x04;		//E = 1;
	delay_us(1);
	PORTC &= 0xFB;		//E = 0;
}

// Busy Flag Check -> 일반적인 BF를 체크하는 것이 아니라
// 일정한 시간 지연을 이용한다.
void Busy(void)
{
	delay_ms(2);
}

```
