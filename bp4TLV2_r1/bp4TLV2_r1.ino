
/* ������ ����
 *
 * set = 0;                           // ������� �����, ���������� ���������� ����������
 * set = 1;                           // Menu 1/4 , ����� ���������� ������� normal/shundown/drop
 * set = 2;                           // Menu 2/4 , ��������� ���������� ������������� ����
 * set = 3;                           // Menu 3/4 , ����� �������� �� ���� ?
 * set = 4;                           // Menu 4/4 , ��������� ���������
 * set = 5;                           // ����� ������ �� ����,
 * set = 6;                           // ������ � ����������
 *
 * mode = 0;                          // �������, ������������ ����, ������ �� ����
 * mode = 1;                          // ����� ������ �� ����
 * mode = 2;                          // ��� 2
 *
 * ������� �������� ����������
 * incomingByte = 97                  // �������� ����������
 * incomingByte = 98                  // counter1 < umax)       counter1 = counter1 + 0.1;   //���������
 * incomingByte = 99                  // �������� -
 * incomingByte = 100                 // �������� +
 * incomingByte = 101 mode = 0;       // ("standart");            // ��� 1
 * incomingByte = 102 mode = 1;       // ("shutdown");            // ������
 * incomingByte = 103 mode = 2;       // ("    drop");            // ��� 2
 * incomingByte = 104 save();         // ��������� ���������
 * incomingByte = 105                 // �������� ���� ���� ��� ���� ��������� � ����� ������� ���������
 * incomingByte = 106  off = true;    // "[   OVERLOAD   ]"
 * incomingByte = 107  ah = 0;        // C������ �����*����� ��������
 *
 *
 * ������� ��������� +
 * set = 0                            // ������� ����� - ��������� ����������
 * set = 1                            // ����������� ����� ������ ������
 * set = 2                            // ��������� ����, ��������� ���
 * set = 3                            // ����� �������� �*�
 * set = 4                            //���������� ������� �������� � ������
 *
 *
 * ������� ��������� -
 * set = 0                            // ������� ����� - ��������� ����������
 * set = 1                            // ����������� ����� ������ �����
 * set = 2                            // ��������� ����, ��������� ���
 *
 * ����������� �� �������
 * disp = 0                           // ������
 * disp = 1                           // ��������
 * disp = 2                           // ����� ��
 * disp = 3                           // ������������ ���
 * disp = 4                           // �������� ���
 * disp = 5                           // �������� ���� ????
 */

#include <LiquidCrystal.h>;
#include <EEPROM.h>

LiquidCrystal lcd(8, 7, 5, 4, 3, 2); //rs, e, d4, d5, d6, d7

// ������ ���������

float umax                  = 20.00;           // ������������ ����������
float umin                  = 0.00;            // ����������� ����������
float ah                    = 0.0000;          // C������ �����*�����
const int down              = 10;              // ����� ��������� 1/2 �
const int up                = 11;              // ����� ��������� 2/2 �
const int pwm1              = 9;               // ����� ���1
const int pwm2              = 6;               // ����� ���2
const int power             = A5;              // ���������� �������
long previousMillis         = 0;               // ������ ����� ���������� ���������� �������
unsigned long currentMillis = 0;               
long maxpwm                 = 0;               // ����� ��������� ������������� ���
long interval               = 500;             // �������� ���������� ���������� �� �������, ��
int mig                     = 0;               // ��� �������� (0 ����� 1 ���� 2 �����)
float level1                = 2000;            // "�������" ���1 �������
float level2                = 2000;            // "�������" ���2 �������
float com                   = 100;
long com2                   = 0;
int mode                    = 0;               // ����� (0 �������, ������������ ����, ������ �� ����)
float Ioutmax               = 1.0;             // �������� ���
int set                     = 0;               // ������ ����, ����������� ������...
int knopka_a                = 0;               // ��������� ������
int knopka_b                = 0;
int knopka_c                = 0;
int knopka_abc              = 0;
boolean off                 = false;
boolean red                 = false;
boolean blue                = false;
boolean pwm1_2              = false;           // ������������� ��� 1 / ��� 2
float counter1              = 5;               // ���������� ������ �������� ���������� ���1
float counter2              = 5;               // ���������� ������ �������� ���������� ���2
int disp                    = 0;               // ����� ����������� 0 ������, 1 ���������, 2 �����, 3 ������������� ���, 4 ��� �������
float Uout                  = 0;               // ���������� �� ������
float Ucorr                 = 0;               // ��������� ����������, ��� ������� ����� ����������
float Iout                  = 0;               // ������ ��� � ��������
#define kn_menu       12                       // ���������� ������ "����"
#define kn_selection  13                       // ���������� ������ "�����"
#define kn_pwm        A4                       // ���������� ������ "���"
#define led_red       A2                       // ���������� ���������� "RED"
#define led_blue      A3                       // ���������� ���������� "BLUE"

int incomingByte;

void EEPROM_float_write(int addr, float val)              // ��������� ������ � ��PROM
{
  byte *x = (byte *)&val;
  for (byte i = 0; i < 4; i++) EEPROM.write(i + addr, x[i]);
}
float EEPROM_float_read(int addr)                         // ��������� ������ �� ��PROM
{
  byte x[4];
  for (byte i = 0; i < 4; i++) x[i] = EEPROM.read(i + addr);
  float *y = (float *)&x;
  return y[0];
}

//������� ��� �������� ��������
void uup()                                                     //������� +
{
  if (set == 0)                                                //������� ����� - ��������� ����������
  {

    if (pwm1_2)                                                // ���� ������ ���1 - ���������� �� ������ �����
    {

      if (counter1 < umax)
      {
        counter1 = counter1 + 0.1;                             //���������
      }
    }
    else                                                       // ����� ������ ���2 - ���������� ��������� � ������� counter2
    {
      
      if (counter2 < umax)
      {
        counter2 = counter2 + 0.1;                            //���������
      }
    }

  }
  if (set == 1)                                                //����������� ����� ������ ������
  {
    mode++;
    if (mode > 2) mode = 2;
  }
  if (set == 2)                                                //��������� ����, ��������� ���
  {
    iplus();
  }

  if (set == 3)                                               //����� �������� �*�
  {
    ah = 0;
    set = 0;
    disp = 5;
  }

  if (set == 4)                                               //���������� ������� �������� � ������
  {
    save();
  }
}
void udn()                                                    //�������� -
{
  if (set == 0)
  {

   if (pwm1_2)                                                // ���� ������ ���1 - ���������� �� ������ �����
    {
        if (counter1 > umin + 0.1)counter1 = counter1 - 0.1;  // �������� ����������
    }
    else                                                      // ����� ������ ���2 - ���������� ��������� � ������� counter2
    {
        if (counter2 > umin + 0.1)counter2 = counter2 - 0.1;  // �������� ����������
    }

  }
  if (set == 1)
  {
    mode--;                                                   // ����������� ����� ������ �����
    if (mode < 0) mode = 0;
  }
  if (set == 2)                                               // �������� ���
  {
    iminus();
  }
}
void iplus()
{
  Ioutmax = Ioutmax + 0.01;
  if (Ioutmax > 0.2) Ioutmax = Ioutmax + 0.04;
  if (Ioutmax > 1) Ioutmax = Ioutmax + 0.05;
  if (Ioutmax > 10.00) Ioutmax = 10.00;
}
void iminus()
{
  Ioutmax = Ioutmax - 0.01;
  if (Ioutmax > 0.2) Ioutmax = Ioutmax - 0.04;
  if (Ioutmax > 1) Ioutmax = Ioutmax - 0.05;
  if (Ioutmax < 0.03) Ioutmax = 0.03;
}
void save()
{
  lcd.clear();                                             // �������� �������
  lcd.setCursor (0, 0);                                    // ���������� ������ � ������
  lcd.print(" S A V E  -  OK ");                           // ������� ���������

  EEPROM_float_write(0, counter1);
  EEPROM_float_write(4, Ioutmax);
  EEPROM_float_write(12, mode);
  EEPROM_float_write(10, disp);
  EEPROM_float_write(16, counter2);

  //������ ������������
  digitalWrite(led_red, HIGH);                             // �������� ������� ���������
  digitalWrite(led_blue, HIGH);                            // �������� ����� ���������
  delay(500);                                              // ���� 0,5 �������
  digitalWrite(led_red, LOW);                              // ��������� ������� ���������
  digitalWrite(led_blue, LOW);                             // ��������� ����� ���������
  set = 0;                                                 //������� �� ����
}

void serial_control()              // ������� ����������
{
  if (Serial.available() > 0)      //���� ���� ��������� ������  ��������� ����
  { 
    incomingByte = Serial.read();
  }
  else
  {
    incomingByte = 0;
  }

  if (incomingByte == 97)        // a
  { 
    if (counter1 > umin + 0.1)counter1 = counter1 - 0.1;    //�������� �����������
  }
  if (incomingByte == 98)        // b
  { 
    if (counter1 < umax)       counter1 = counter1 + 0.1;   //���������
  }

  if (incomingByte == 99)        // c
  { 
    iminus();
  }

  if (incomingByte == 100)       // d
  {
    iplus();
  }

  if (incomingByte == 101) mode = 0;
  if (incomingByte == 102) mode = 1;
  if (incomingByte == 103) mode = 2;
  if (incomingByte == 104) save();
  if (incomingByte == 105)
  {
    digitalWrite(power, HIGH);                              // ������� ���� ���� ��� ���� ���������
    delay(100);
    digitalWrite(led_red, LOW);                             // ����� ������� ���������
    Serial.print('t');
    Serial.print(0);
    Serial.print(';');
    off = false;
    set = 0;                                               // ������� �� ����
  }

  if (incomingByte == 106) off = true;
  if (incomingByte == 107) ah = 0;

  // ����� �������� ����������
}
void AV_protection()
{
  //�������� �������� ���������� � ���� � ��������
  Ucorr = -0.00;                                            // ��������� ����������, ��� ������� ����� ����������
  Uout = analogRead(A1) * ((5.0 + Ucorr) / 1023.0) * 5.0;   // ������ ���������� �� ������
  Iout = analogRead(A0) / 100.00;                           // ������ ��� � ��������

  if (Iout == 0.01) Iout =  0.03; else if (Iout == 0.02) Iout =  0.04; else if (Iout == 0.03) Iout =  0.05; else if (Iout == 0.04) Iout = 0.06; else if (Iout >= 0.05) Iout = Iout + 0.02;
  if (Iout >= 0.25)Iout = Iout + 0.01;
  //if(Iout>=1)Iout = Iout * 1.02;


  // ������ � ����������

  if (((Iout > (counter1 + 0.3) * 2.0) | Iout > 10.1  | off) & set<4 & millis()>100 )  // ������� ������

  {
    digitalWrite(power, LOW);                                      // �������� ����
    level1 = 8190;                                                 // ������� ��� ������
    digitalWrite(led_red, HIGH);                                   // �������� ������� ���������

    Serial.print('I0;U0;r1;W0;');
    Serial.println(' ');
    set = 6;
  }

  //������ �� ����������� ������������� ���
  if (level1 == 0 & off == false)
  {
    if (set < 4)                                                        // ���� ��� �� ��������� ������
    {
      maxpwm--;                                                         // ��������� +1 � ��������
      digitalWrite(led_red, HIGH);                                      // ������ ������� ��� �������������� � ������������ ���
    }
  }
  else                                                                  // ��� � ��� �� ������������, ������� �������� �������
  {
    maxpwm++;
    if (maxpwm > 8190)                                                  // ���� ������� ����� �� ����
    {
      maxpwm = 8190;                                                    // ����� ��� � ������
      if (set < 4) digitalWrite(led_red, LOW);                          // ����� ������� ���������. ���������� ���.
    }
  }
  // ������ �����
}
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
void control_system()                                           // ����������
{
  if (mode == 0 | mode == 1)                                    // ���� ��������� ������ ����������� (�� ����� ������������ ����)
  {
    if (Uout > counter1)                                        //���������� ���������� �� ������ � �������������, � ��������� ����..
    {
      float raz = Uout - counter1;                              //�� ������� ���������� �� ������ ������ ��������������...
      if (raz > 0.05)
      {
        level1 = level1 - raz * 20;                             //������� ������� ��������� ����� � ������!
      } else {
        if (raz > 0.015)  level1 = level1 -  raz * 3 ;          //������� ��������� ��������� �����
      }
    }
    if (Uout < counter1)
    {
      float raz = counter1 - Uout;                              //�� ������� ���������� ������ ��� �� �����
      if (raz > 0.05)
      {
        level1 = level1 + raz * 20; //�����
      } else {
        if (raz > 0.015)  level1 = level1 + raz * 3 ;           // �����
      }
    }
    // ??+++++++++++++++++++ ����� �������� � ���������� ��� 2 +++++++++++++++++++++++++++++++++++++++++++
    if (mode == 2)                                                 // ����� ����� ��� 2
    {
      Serial.print('PWM 2');                                      // ������� ��������� � Serial
    }
    // --------------------------------------------------------------------------------------------------

    if (mode == 1 && Iout > Ioutmax)                                //����� ������ �� ����, � �� ������ ��� �� ����������
    {
      digitalWrite(power, LOW);                                     //�������� ����
      Serial.print('t');
      Serial.print(2);
      Serial.print(';');
      digitalWrite(led_red, HIGH);                                  // �������� ������� ���������
      level1 = 8190;                                                // ������� ��� ������
      set = 5;                                                      // ����� ����� � ������...
    }

  }
  else                                                             // ����� ������������ ����
  { 

    if (Iout >= Ioutmax)
    {
      float raz = (Iout - Ioutmax);                                //������ ����� ������� ����� ����� � �������� � ������������� �����
      if (raz > 0.3)                                               // ����� ������ ��������� (��� ������ ��������� ����� ��� �� 0,3�)
      {
        level1 = level1 + raz * 20;                                // ����� �������� ���
      } else {
        if (raz > 0.05)                                            // ������ ��������� (��� ������ ��������� ����� ��� �� 0,1�)
        {
          level1 = level1 + raz * 5;                               // �������� ���
        } else {
          if (raz > 0.00) level1 = level1 + raz * 2;               // ������� �������� (0.1 - 0.01�) �������� ������
        }
      }

      digitalWrite(led_blue, HIGH);                                // �������� ����� ���������
    } else {                                                       // ����� ������������ ����, �� ��� � ��� � �������� �����, � ������ ���������� ������������ ����������
      digitalWrite(led_blue, LOW);                                 // ����� ��������� �� ��������

      //���������� ���������� �� ������ � �������������, � ��������� ����..
      if (Uout > counter1)
      {
        float raz = Uout - counter1;                               //�� ������� ���������� �� ������ ������ ��������������...
        if (raz > 0.1)
        {
          level1 = level1 + raz * 20;                              //������� ������� ��������� ����� � ������!
        } else {
          if (raz > 0.015)  level1 = level1 + raz * 5;             //������� ��������� ��������� �����
        }
      }
      if (Uout < counter1)
      {
        float raz = counter1 - Uout;                              //�� ������� ���������� ������ ��� �� �����
        float iraz = (Ioutmax - Iout); //
        if (raz > 0.1 & iraz > 0.1)
        {
          level1 = level1 - raz * 20;                              //�����
        } else {
          if (raz > 0.015)  level1 = level1 - raz ; //�����
        }
      }
    }
  }//����� ������ ������������ ����

  if (off) level1 = 8190;
  if (level1 < 0) level1 = 0;                                             //�� �������� ��� ���� ����
  if (level1 > 8190) level1 = 8190;                                       //�� ��������� ��� ���� 13 ���
                                                                      //��� ���������, ��������� � ���������� ������ ������� ��� �������� �����������.
  if (ceil(level1) != 255) analogWrite(pwm1, ceil(level1));               //������ ������ ������ �� ��� ����� (����� 255, ��� ��� ��� �����-�� ����)
}                               // ����������                            
void control_buttons()
{
  if (digitalRead(kn_menu) == LOW && digitalRead(kn_selection) == LOW && digitalRead(kn_pwm) == LOW && knopka_abc == 0 )
  { // ������ �� ������ a - � - c  ������
    knopka_abc = 1;
    //ah = 0.000;
    knopka_abc = 0;
  }

  if (digitalRead(kn_menu) == LOW && knopka_b == 0)                       // ������������ ������� ���� ������� "����"
	  {
		while (digitalRead(kn_menu) == LOW ) {}                           // ������� ���������� ������ "����"
		knopka_b = 1;
		set++;                                                            //
		if (set > 4 | off)                                                // ������������ ���� �� ������� ������, � ���� ������� �� ��� ��������. (��� �� ������ ����� �� ����� ����)
		{
		  off = false;
		  digitalWrite(power, HIGH);                                      // ������� ���� ���� ��� ���� ���������
		  delay(100);
		  digitalWrite(led_red, LOW);                                     //����� ������� ���������
		  Serial.print('t');
		  Serial.print(0);
		  Serial.print(';');
		  Serial.print('r');
		  Serial.print(0);
		  Serial.print(';');
		  Serial.println(' ');
		  set = 0;                                                         //������� �� ����
		}
		lcd.clear();                                                       //������ �������
	  }

  if (digitalRead(kn_selection) == LOW && knopka_a == 0)
	  {   
		while (digitalRead(kn_selection) == LOW ) {}                       // ������� ���������� ������ ��� 
		knopka_a = 1;
		mode++;                                                            //���������� ����������� ����� ����������� ����������
		if (mode > 2) mode = 0;                                            //����� �� �����, �������� �����
	  }

  if (digitalRead(kn_pwm) == LOW && set == 1 && knopka_c == 0)             // ������ �� ������ C (disp)
	  {
        while (digitalRead(kn_pwm) == LOW ) {}                             // ������� ���������� ������ ���
 		if (mode == 2) pwm1_2 = !pwm1_2;                                   // ����������� ��� 1/2 ( pwm1_2== true -  ���1,   pwm1_2== false -  ���2)
	  }

  //���������� �������� ������ ��� ����-�� ����� ����.
  if (digitalRead(kn_menu)      == HIGH && knopka_b == 1) knopka_b = 0;
  if (digitalRead(kn_selection) == HIGH && knopka_a == 1) knopka_a = 0;
  if (digitalRead(kn_pwm)       == HIGH && knopka_c == 1) knopka_c = 0;

}
void serial_print()
{
    //������� �����*����
    ah = ah + (Iout / 36000);

    Serial.print('U');
    Serial.print(Uout);
    Serial.print(';');

    Serial.print('I');
    Serial.print(Iout);
    Serial.print(';');

    Serial.print('i');
    Serial.print(Ioutmax);
    Serial.print(';');

    Serial.print('u');
    Serial.print(counter1);
    Serial.print(';');

    Serial.print('W');
    Serial.print(level1);
    Serial.print(';');

    Serial.print('c');
    Serial.print(ah);
    Serial.print(';');

    Serial.print('m');
    Serial.print(mode);
    Serial.print(';');

    Serial.print('r');
    Serial.print(digitalRead(led_red));
    Serial.print(';');

    Serial.print('b');
    Serial.print(digitalRead(led_blue));
    Serial.print(';');
    Serial.println(' ');
}
void display_print()
{
  if (set == 0)                                         //����������� �����,  ������� ������������� ���������� �� �������
  {
    lcd.setCursor (0, 1);
    lcd.print("U>");
    if (counter1 < 10) lcd.print(" ");                  // ��������� ������, ���� �����, ����� �� ������� ��������
    lcd.print (counter1, 1);                            // ������� ������������� �������� ����������
    lcd.print ("V ");                                   // ����� ��� ��� ������

														//���������� ����������
														//��������� �� ������ �� ������ ��������, ���� ������ ��
														// ������� �������� �������� �� �������

    if (currentMillis - previousMillis > interval)
    {
      previousMillis = currentMillis;                   // ��������� ����� ���������� ����������
      //������� ���������� �������� ���������� � ���� �� �������
      lcd.setCursor (0, 0);
      lcd.print("U=");
      if (Uout < 9.99) lcd.print(" ");
      lcd.print(Uout, 2);
      lcd.print("V I=");
      lcd.print(Iout, 2);
      lcd.print("A ");

      //�������������� ����������
      lcd.setCursor (8, 1);
      if (disp == 0)                                      // ������
      {
        lcd.print("         ");                           // ����������� ����������� ������ DHT11 ???
      }
      if (disp == 1)                                      // ��������
      {
        lcd.print(" ");
        lcd.print (Uout * Iout, 2);
        lcd.print("W   ");
      }
      if (disp == 2)                                      //����� ��
      {
        if (mode == 0)lcd.print  ("standart");            // ��� 1
        if (mode == 1)lcd.print  ("shutdown");            // ������
        if (mode == 2)lcd.print  ("    drop");            // ��� 2
      }
      if (disp == 3)                                      // ������������ ���
      {
        lcd.print (" I>");
        lcd.print (Ioutmax, 2);
        lcd.print ("A ");
      }
      if (disp == 4)                                     // �������� ���
      {
        lcd.print ("pwm1:");
        lcd.print (ceil(level1), 0);
        lcd.print ("  ");
      }
      if (disp == 5)                                     // �������� ��� ????
      {
        if (ah < 1)
        {
          //if(ah<0.001) lcd.print (" ");
          if (ah <= 0.01) lcd.print (" ");
          if (ah <= 0.1) lcd.print (" ");
          lcd.print (ah * 1000, 1);
          lcd.print ("mAh  ");
        }
        else
        {
          if (ah <= 10) lcd.print (" ");
          lcd.print (ah, 3);
          lcd.print ("Ah  ");
        }
      }
    }
  }

  // ��������� ����

  if (set == 1)                                      //����� ������
  {
    lcd.setCursor (0, 0);
    lcd.print("> MENU 1/4    ");
    lcd.setCursor (0, 1);
    lcd.print("mode: ");         //����� (0 �������, ������������ ����, ������ �� ����)

    if (mode == 0)  lcd.print("normal          ");
    if (mode == 1)  lcd.print("shutdown    ");
    if (mode == 2 && pwm1_2)  lcd.print("drop  pwm1");
	if (mode == 2 && !pwm1_2)  lcd.print("drop  pwm2");

  }
  if (set == 2)                                     //��������� ����
  {
    lcd.setCursor (0, 0);
    lcd.print("> MENU 2/4   ");
    lcd.setCursor (0, 1);
    lcd.print("I out max: ");
    lcd.print(Ioutmax);
    lcd.print("A");
  }
  if (set == 3)                                   //���������� ����� �� ���� �������� ���������
  {
    lcd.setCursor (0, 0);
    lcd.print("> MENU 3/4      ");
    lcd.setCursor (0, 1);
    lcd.print("Reset A*h? ->");
  }

  if (set == 4)                                  //���������� ����� �� ���� ��������� ���������
  {
    lcd.setCursor (0, 0);
    lcd.print("> MENU 4/4      ");
    lcd.setCursor (0, 1);
    lcd.print("Save options? ->");
  }

  // ��������� ������
  if (set == 5)                                 //������. ����� ����
  {
    lcd.setCursor (0, 0);
    lcd.print("ShutDown!        ");
    lcd.setCursor (0, 1);
    lcd.print("Iout");
    lcd.print(">Imax(");
    lcd.print(Ioutmax);
    lcd.print("A)");
    level1 = 0;
    Serial.print('I0;U0;r1;W0;');
    Serial.println(' ');
  }


  if (set == 6)                                 //������. ����� ���� ����������� ������� ����������
  {
    Serial.print('I0;U0;r1;W0;');
    digitalWrite(led_red, HIGH);
    Serial.println(' ');
    level1 = 0;
    lcd.setCursor (0, 0);

    if (off == false)
    {
      lcd.print("[   OVERLOAD   ]");
      lcd.setCursor (0, 1);
      //� ��������� ����� ��� ���������

      if ((Iout > (counter1 + 0.3) * 2.0) | Iout > 10.0)
      {
        Serial.print('t');
        Serial.print(1);
        Serial.print(';');
        lcd.print("  Iout >= Imax  ");
      }

    }
    else
    {

      lcd.print("[      OFF     ]");
      lcd.setCursor (0, 1);
      Serial.print('t');
      Serial.print(4);
      Serial.print(';');
    }
  }
}

void setup()
{
  cli();                                   // ��������� ������ ����������������
  DDRB |= 1 << 1 | 1 << 2;
  PORTB &= ~(1 << 1 | 1 << 2);
  TCCR1A = 0b00000010;
  //TCCR1A = 0b10100010;
  TCCR1B = 0b00011001;
  ICR1H = 255;
  ICR1L = 255;
  sei();

  int pwm_rez = 13;
  pwm_rez = pow(2, pwm_rez);               // ��������� �������� ����������� � �������� �������
  ICR1H   = highByte(pwm_rez);             // ������ �������� ����� � Input Capture Register 1 H
  ICR1L   = lowByte(pwm_rez);              // ������ �������� ����� � Input Capture Register 1 L

  Serial.begin(9600);                      // ���������� �������� ��������� ����� 9600

  pinMode(pwm1, OUTPUT);                   // �� ����� 9
  pinMode(pwm2, OUTPUT);                   // �� ����� 6
  pinMode(down, INPUT);
  pinMode(up, INPUT);
  pinMode(kn_menu, INPUT);
  pinMode(kn_selection, INPUT);
  pinMode(kn_pwm, INPUT);
  pinMode(power, OUTPUT);
  pinMode(led_red, OUTPUT);
  pinMode(led_blue, OUTPUT);
  digitalWrite(up, HIGH);                  // ������������ ������� ������� �� ������ �� ���������
  digitalWrite(down, HIGH);
  digitalWrite(kn_menu, HIGH);             //������������ ������� ������� �� ��������� ������
  digitalWrite(kn_selection, HIGH);
  digitalWrite(kn_pwm, HIGH);

  //������ �������
  lcd.begin(16, 2);                        // ������� 16 ��������, 2 ������
  lcd.print("    WELCOME!    ");
  counter1 = EEPROM_float_read(0);         //��������� ��������� �� ������ ��
  Ioutmax  = EEPROM_float_read(4);
  mode     = EEPROM_float_read(12);
  disp     = EEPROM_float_read(10);
  counter2 = EEPROM_float_read(16);
                                           // ���� � ������ ��� ��� �������� - ������ ��� ������ ����� �����
  if (counter1 == 0) counter1 = 5;         // 5 �����
  if (Ioutmax == 0) Ioutmax = 2;           // 2 ������

  digitalWrite(power, HIGH);               // �������� ����
}
void loop()                                //�������� ���� ������ ��
{
  currentMillis = millis();                // ������� �������� �������
  serial_control();                        // ������� ����������
  AV_protection();                         // ������ 
  valcoder_set();                          // ��������� �������� � ����� ���������
  control_system();                        // ����������
  if (currentMillis - com2 > com)
	  {
		 com2 = currentMillis;             // ��������� ����� ���������� ����������
		 serial_print();                   // ���������� ������ � ��� ����
	  }
  control_buttons();                       // �������� ������                                       
  display_print();                         // ��������� LCD
}
