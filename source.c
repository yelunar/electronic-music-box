#include <mega128.h>
#include <delay.h>

#define VLOA    9008    // octave 2
#define VLOAX   8580
#define VLOB    8098

#define LOC     7644    // �� octave 3
#define LOCX    7214
#define LOD     6810    // �� 
#define LODX    6427
#define LOE     6066    // �� 
#define LOF     5726    // �� 
#define LOFX    5404
#define LOG     5101    // �� 
#define LOGX    4815
#define LOA     4544    // �� 
#define LOAX    4289
#define LOB     4049    // �� 

#define MIC     3821    // �� octave 4 (�⺻ ����)
#define MICX    3607      
#define MID     3404    // �� 
#define MIDX    3213
#define MIE     3033    // �� 
#define MIF     2862    // �� 
#define MIFX    2702
#define MIG     2550    // �� 
#define MIGX    2407
#define MIA     2272    // �� 
#define MIAX    2144
#define MIB     2024    // �� 

#define HIC     1910    // �� octave 5
#define HICX    3607      
#define HID     3404    // �� 
#define HIDX    3213
#define HIE     3033    // �� 
#define HIF     2862    // �� 
#define HIFX    2702
#define HIG     2550    // �� 
#define HIGX    2407
#define HIA     2272    // �� 
#define HIAX    2144
#define HIB     2024    // �� 

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

unsigned int tempo = 0;
unsigned int cnt, sec;

void play_note(unsigned int sound, unsigned int note);
void play_rest(unsigned int rest);

void main(void)
{
	DDRB = 0b00100000;    // PB5(OC1A) ��� ���� (����Ŀ)
	DDRE = 0b00111011;   // PE3 (OC3A) ��� ����  
	DDRF = 0b00000011;   // PF0,1 ��� ���� (����)

	TCCR1A = 0b01000000;      // COM1A1,A0(7,6)-01, mode 4 - CTC��� / OCR1A (WGM11,10 - 00)
	TCCR1B = 0b00001010;      // mode 4(WGM13,12 - 01), ���������� = 8 

	TCCR3A = 0b10000010;     // mode 14(ICRn top fast PWM) : 1110 (10Bit top), 1,0(WGM11,10)-10, 7,6(COM1A1,COM1A0)-10
	TCCR3B = 0b00011011;     // 4,3(WGM13,12)-11, 2-0(CS)-011 (64����) 
	ICR3H = 0x09;            // ICR1 = 2499 (0x09C3) -> �ֱ� : 10msec
	ICR3L = 0xC3;
	TCNT3H = 0x00;
	TCNT3L = 0x00;            // Ÿ�̸�3 �ʱ�ȭ     
	OCR3AH = 0b00000010;
	OCR3AL = 0b11000000;


	TIMSK = 0b00000001;
	TCCR0 = 0b00000111;    // �Ϲݸ��, ���������� = CK/1024
	TCNT0 = 0b00000000;    // Ÿ�̸�/ī����0 �������� �ʱⰪ
	SREG = 0b10000000;


	PORTF = 0b00000001;  // �ʱ� ���� ����
	delay_ms(1000);


	LCD_init();


	while (1) {       // while(1) : ���� ����
		tempo = 3;


		Command(HOME);
		LCD_String("We wish you a");         // ����1
		Command(LINE2);
		LCD_String("Merry christmas");       // ����2                                                       
		play_note(MID, NOTE4);    //��
		play_note(MIG, NOTE4);    //��      
		play_note(MIG, NOTE8);    //��
		play_note(MIA, NOTE8);    //��
		play_note(MIG, NOTE8);    //�� 
		play_note(MIFX, NOTE8);   //�ļ�
		play_note(MIE, NOTE4);    //��
		play_note(MIE, NOTE4);    //��    


		play_note(MIE, NOTE4);  //��
		play_note(MIA, NOTE4);  //��
		play_note(MIA, NOTE8);  //��
		play_note(MIB, NOTE8);  //��
		play_note(MIA, NOTE8);  //��
		play_note(MIG, NOTE8);  //��
		play_note(MIFX, NOTE4); //�ļ�
		play_note(MID, NOTE4);  //��


		play_note(MID, NOTE4);  //��
		play_note(MIB, NOTE4);  //��
		play_note(MIB, NOTE8);  //��
		play_note(HIC, NOTE8);  //��
		play_note(MIB, NOTE8);  //��
		play_note(MIA, NOTE8);  //��
		play_note(MIG, NOTE4);  //��
		play_note(MIE, NOTE4);  //��

		Command(CLEARDISPLAY);
		Command(HOME);
		LCD_String("And a happy");          // ����1
		Command(LINE2);
		LCD_String("New year");             //����2
		play_note(MID, NOTE8);  //��
		play_note(MID, NOTE8);  //��
		play_note(MIE, NOTE4);  //��
		play_note(MIA, NOTE4);  //��
		play_note(MIFX, NOTE4);  //�ļ�
		play_note(MIG, NOTE1);  //��        


		Command(CLEARDISPLAY);
		Command(HOME);
		LCD_String("Good tidings");          //����1
		Command(LINE2);
		LCD_String("       to you");         //����2
		play_note(MID, NOTE4);  //��
		play_note(MIG, NOTE4);  //��
		play_note(MIG, NOTE4);  //��
		play_note(MIG, NOTE4);  //��
		play_note(MIFX, NOTE2); //�ļ�

		Command(CLEARDISPLAY);
		Command(HOME);
		LCD_String("wherever");          //����1
		Command(LINE2);
		LCD_String("   you are");        //����2
		play_note(MIFX, NOTE4); //��
		play_note(MIG, NOTE4);  //��
		play_note(MIFX, NOTE4); //�ļ�
		play_note(MIE, NOTE4);  //��
		play_note(MID, NOTE2);  //�� 

		Command(CLEARDISPLAY);
		Command(HOME);
		LCD_String("Good tidings");          //����1
		Command(LINE2);
		LCD_String("for Christmas");         //����2
		play_note(MIA, NOTE4);  //��
		play_note(MIB, NOTE4);  //��
		play_note(MIA, NOTE4);  //��
		play_note(MIG, NOTE4);  //��
		play_note(HID, NOTE4);  //��
		play_note(HID, NOTE4);  //��

		Command(CLEARDISPLAY);
		Command(HOME);
		LCD_String("And a happy");          //����1
		Command(LINE2);
		LCD_String("New year");             //����2
		play_note(HID, NOTE8);  //��
		play_note(HID, NOTE8);  //��
		play_note(MIE, NOTE4);  //��
		play_note(MIA, NOTE4);  //��
		play_note(MIFX, NOTE4);  //�ļ�
		play_note(MIG, NOTE1);  //��




	}
}

void play_note(unsigned int sound, unsigned int note)
{
	OCR1A = sound;
	TCNT1 = 0x0000;

	TCCR1A = 0b01000000;    // COM1A1,A0(7,6)-01, mode 4 - CTC��� / OCR1A (WGM11,10 - 00)
	TCCR1B = 0b00001010;    // mode 4(WGM13,12 - 01), ���������� = 8

	delay_ms(note * tempo * 7);

	TCCR1B = 0b00001000;    // speaker off
}

void play_rest(unsigned int rest)
{
	delay_ms(rest * tempo * 7);
}


interrupt[TIM0_OVF] void timer_int0(void)
{
	cnt++; // 16.384ms x 61 = 0.999424sec
	if (cnt == 61) {
		sec++;

		if (sec == 1)
		{
			PORTF = 0b00000001;

		}

		else if (sec == 2)
		{
			PORTF = 00000000;
		}

	}
}



// LCD �ʱ�ȭ �Լ� 
void LCD_init(void)
{
	DDRC = 0xFF;	// ��Ʈ C ��� ����
	PORTC &= 0xFB;	//E = 0;

	// ����� �����ð��� ���� ����ȭ ���� 	              
	delay_ms(15);
	Command(0x20);	// D5=1
	delay_ms(5);
	Command(0x20);	// D5=1
	delay_us(100);
	Command(0x20);	// D5=1

	// �ʱ�ȭ ����
	Command(0x28);	// function set
	Command(0x06);	// entry mode set
	Command(0x01);	// all clear
	Command(0x0c);	// display on
}


// �ν�Ʈ���� ���� �Լ�
void Command(unsigned char byte)
{
	Busy();

	// �ν�Ʈ���� ���� ����Ʈ
	PORTC = (byte & 0xF0);	// ������	
	PORTC &= 0xFE;		// RS = 0;           
	PORTC &= 0xFD;		// RW = 0;  
	delay_us(1);
	PORTC |= 0x04;		// E = 1;
	delay_us(1);
	PORTC &= 0xFB;		// E = 0;

	// �ν�Ʈ���� ���� ����Ʈ
	PORTC = ((byte << 4) & 0xF0);	// ������
	PORTC &= 0xFE;		// RS = 0;
	PORTC &= 0xFD;		// RW = 0;
	delay_us(1);
	PORTC |= 0x04;		// E = 1;
	delay_us(1);
	PORTC &= 0xFB;		// E = 0;
}

// ���ڿ� ��� �Լ�
void LCD_String(char flash str[])
{
	char flash *pStr = 0;

	pStr = &str[0];
	while (*pStr) Data(*pStr++);
}

// char flash : pointer declaration for program memory
// char eeprom : pointer declaration for EEPROM

//������ ���� �Լ�
void Data(unsigned char byte)
{
	Busy();

	// ������ ���� ����Ʈ
	PORTC = (byte & 0xF0);	// ������
	PORTC |= 0x01;		//RS = 1;
	PORTC &= 0xFD;		//RW = 0;
	delay_us(1);
	PORTC |= 0x04;		//E = 1;
	delay_us(1);
	PORTC &= 0xFB;		//E = 0;

	// ������ ���� ����Ʈ
	PORTC = ((byte << 4) & 0xF0);  	// ������
	PORTC |= 0x01;		//RS = 1;
	PORTC &= 0xFD;		//RW = 0;     
	delay_us(1);
	PORTC |= 0x04;		//E = 1;
	delay_us(1);
	PORTC &= 0xFB;		//E = 0;
}

// Busy Flag Check -> �Ϲ����� BF�� üũ�ϴ� ���� �ƴ϶�
// ������ �ð� ������ �̿��Ѵ�.
void Busy(void)
{
	delay_ms(2);
}