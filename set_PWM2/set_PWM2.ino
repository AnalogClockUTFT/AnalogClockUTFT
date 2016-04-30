
#include <LiquidCrystal.h>;
#include <EEPROM.h>

LiquidCrystal lcd(8, 7, 5, 4, 3, 2); //rs, e, d4, d5, d6, d7

const int down              = 10;              // ����� ��������� 1/2 �
const int up                = 11;              // ����� ��������� 2/2 �
const int pwm1              = 9;               // ����� ���1
const int pwm2              = 6;               // ����� ���2
int mig                     = 0;               // ��� �������� (0 ����� 1 ���� 2 �����)

#define kn_menu       12                       // ���������� ������ "����"
#define kn_selection  A5                       // ���������� ������ "�����"
#define kn_pwm        A4                       // ���������� ������ "���"

boolean pwm1_2              = true;            // ������������� true ��� 1 / false ��� 2
int counter1                = 5;               // ���������� ������ �������� ���������� ���1
int counter2                = 5;               // ���������� ������ �������� ���������� ���2

int set                     = 0;               // ������ ����, 

void valcoder_set()
{
  boolean regup   = digitalRead(up);
  boolean regdown = digitalRead(down);

  if (regup < regdown) mig = 1;                            // �������� � ������� ����������
  if (regup > regdown) mig = 2;                            // �������� � ������� ���������
  if (!regup & !regdown)                                   // ������ ��� ������������
  {
    if (mig == 1) uup();                                   //+
    if (mig == 2) udn();                                   //-
    mig = 0;                                               // ���������� ��������� �����������
  }
}

//������� ��� �������� ��������
void uup()                                                 //������� +
{
    if (pwm1_2)                                            // ���� ������ ���1. ��� ������������� �������� � ��������� 0-255    
    {
		counter1++;                                        // ��� ������������� �������� � ��������� 0-255    
       if(counter1 > 255) counter1 = 255;                  // ���������� ������� 255, ������ ������.
       analogWrite(pwm1, counter1);                        // �������� ��� �� ����� ���1
	}
    else                                                   // ����� ������ ���2. ��� ������������� �������� � ��������� 0-255    
    {
       counter2++;                                         // ��� ������������� �������� � ��������� 0-255                                            
       if(counter2 > 255) counter2 = 255;                  // ���������� ������� 255, ������ ������. 
	   analogWrite(pwm2, counter1);                        // �������� ��� �� ����� ���1
    }
}
void udn()                                                 //�������� -
{
   if (pwm1_2)                                             // ���� ������ ���1. ��� ������������� �������� � ��������� 0-255  
    {
		counter1--;                                        // ��� ������������� �������� � ��������� 0-255    
       if(counter1 < 0) counter1 = 0;                      // ���������� ������� 0, ������ ������.
       analogWrite(pwm1, counter1);                        // �������� ��� �� ����� ���1
    }
    else                                                   // ����� ������ ���2. ��� ������������� �������� � ��������� 0-255  
    {
       counter2--;                                         // ��� ������������� �������� � ��������� 0-255                                            
       if(counter2 < 0) counter2 = 0;                      //  ���������� ������� 0, ������ ������. 
	   analogWrite(pwm2, counter1);                        // �������� ��� �� ����� ���1
    }
}
void control_buttons()
{
	 if (digitalRead(kn_menu) == LOW )                     // ������������ ������� ���� ������� "����"
	  {
		while (digitalRead(kn_menu) == LOW ) {}            // ������� ���������� ������ "����"
		set++;                                             // ��������� ����� ���� 
		if (set > 3)  set = 3;                             // ������������ ����� ����
	  }
	 if (digitalRead(kn_pwm) == LOW && set == 1)           // ���������� ����� ��� ���� ������ ������ ����� ����
	  {
        while (digitalRead(kn_pwm) == LOW ) {}             // ������� ���������� ������ ���
 		pwm1_2 = !pwm1_2;                                  // ����������� ��� 1/2 ( pwm1_2== true -  ���1,   pwm1_2== false -  ���2)
	  }
}

void display_print()
{
	if (set == 0)                                         //����������� �����,  ������� ������������� ���������� �� �������
	{
		lcd.setCursor (0, 1);
		lcd.print("U>");
		if (counter1 < 10) lcd.print(" ");                // ��������� ������, ���� �����, ����� �� ������� ��������
		lcd.print (counter1, 1);                          // ������� ������������� �������� ����������
		lcd.print ("V ");                                 // ����� ��� ��� ������
	}
	if (set == 1)                                         //
	{
		if (pwm1_2)                                                // ���� ������ ���1 - ���������� �� ������ �����
		{
			lcd.print ("pwm1:");
			lcd.print (counter1, 0);
			lcd.print ("  ");
		}
		else
		{
			lcd.print ("pwm2:");
			lcd.print (counter2, 0);
			lcd.print ("  ");
		}
	}
}

void setup()
{
  Serial.begin(9600);                      // ���������� �������� ��������� ����� 9600

  pinMode(pwm1, OUTPUT);                   // �� ����� 9
  pinMode(pwm2, OUTPUT);                   // �� ����� 6
  pinMode(down, INPUT);
  pinMode(up, INPUT);
 
  pinMode(kn_menu, INPUT);
  pinMode(kn_selection, INPUT);
  pinMode(kn_pwm, INPUT);

  digitalWrite(up, HIGH);                  // ������������ ������� ������� �� ������ �� ���������
  digitalWrite(down, HIGH);
 
  digitalWrite(kn_menu, HIGH);             //������������ ������� ������� �� ��������� ������
  digitalWrite(kn_selection, HIGH);
  digitalWrite(kn_pwm, HIGH);

  //������ �������
  lcd.begin(16, 2);                        // ������� 16 ��������, 2 ������
  lcd.print("    WELCOME!    ");
}

void loop()
{
 valcoder_set();
 control_buttons();
 display_print();
 delay(100);
}
