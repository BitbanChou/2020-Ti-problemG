//LingShun LAB
#include <Servo.h>
#include <dht11.h>
#include <LiquidCrystal.h>
#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards
//dht11 DHT;
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#include <SoftwareSerial.h>
SoftwareSerial softSerial(9, 10); // RX, TX
typedef struct
{
  char data[4][11];
  int len1 = 0;
  int len2 = 0;
  int len3 = 0;
  int len4 = 0;
}List;
List list;

#define Trig 6 //引脚Tring 连接 IO D2
#define Echo 7 //引脚Echo 连接 IO D3 
#define dataPin 8 //湿度模块
#define led 13
#define button 2

int a[2];
float cm; //距离变量
float temp; // 
boolean buttonState = 1;
String str;
String bal;
int sizea;

void setup() {
  softSerial.begin(115200);
  Serial.begin(115200);
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
  pinMode(led, OUTPUT);
  pinMode(button, INPUT);
  //myservo.attach(13);
  //lcd.begin(16, 2);
  u8g2.begin();
  u8g2.enableUTF8Print();
}

void loop() {
  
  if (digitalRead(button) == HIGH) {
    digitalWrite(led, buttonState);
    buttonState = !buttonState;
    while (digitalRead(button) == HIGH);
    ball();
  }
  /*
  else
  {
    buttonState = !buttonState;
    while (digitalRead(button) != HIGH);
  }
  */
    //给Trig发送一个低高低的短时间脉冲,触发测距
     digitalWrite(Trig, LOW); //给Trig发送一个低电平
    delayMicroseconds(2);    //等待 2微妙
    digitalWrite(Trig,HIGH); //给Trig发送一个高电平
    delayMicroseconds(10);    //等待 10微妙
    digitalWrite(Trig, LOW); //给Trig发送一个低电平
    
    temp = float(pulseIn(Echo, HIGH)); //存储回波等待时间,
    //pulseIn函数会等待引脚变为HIGH,开始计算时间,再等待变为LOW并停止计时
    //返回脉冲的长度
    
    //声速是:340m/1s 换算成 34000cm / 1000000μs => 34 / 1000
    //因为发送到接收,实际是相同距离走了2回,所以要除以2
    //距离(厘米)  =  (回波时间 * (34 / 1000)) / 2
    //简化后的计算公式为 (回波时间 * 17)/ 1000
    
    cm = (temp * 17 )/1000; //把回波时间换算成cm
    
    Serial.print("Echo =");
    Serial.print(temp);//串口输出等待时间的原始数据
    Serial.print(" | | Distance = ");
    Serial.print(cm);//串口输出距离换算成cm的结果
    Serial.println("cm\n");
  
    
    //delay(500);
    
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.setFontDirection(0);
    u8g2.firstPage();
    do {
      //if(num==2) u8g2.print("Hello World!");
      //if(num==3) u8g2.print("Hello World!");
      //u8g2.setCursor(0, 40); 
      //u8g2.print("Distance");
      //u8g2.setCursor(55, 15);
      ///u8g2.print("sss");
      if(cm<800)
      {
        u8g2.setCursor(0, 15);
        u8g2.print("Shape:");
        u8g2.print(list.data[1]);
        u8g2.setCursor(0, 30);
        u8g2.print("Distance:"); 
        u8g2.setCursor(80,30);
        u8g2.print(cm);
        u8g2.setCursor(0,45);
        u8g2.print("Size:");
        u8g2.print(list.data[0]);
        u8g2.setCursor(0,60);
        u8g2.print("Pos:(");
        u8g2.print(list.data[2]);
        u8g2.print(",");
        u8g2.print(list.data[3]);
        u8g2.print(")");
        //u8g2.print("cm");  
        //# Draw a rect around the blob.
        //b = blobs[0]
        //#img.draw_rectangle(b[0:4]) # rect
        //#img.draw_cross(b[5], b[6]) # cx, cy
      }
    if(softSerial.available())
    {
      getS();
      //u8g2.setCursor(55, 15); 
      //u8g2.print(list.data[1]);
      //delay(5000);
      Serial.print(list.data[1]);
      Serial.print("\n");
      //u8g2.setCursor(45,45); 
      //u8g2.print(list.data[0]);
      //delay(5000);
      Serial.print(list.data[0]);
      Serial.print("\n");
      Serial.print(list.data[2]);
      Serial.print("\n");
      Serial.print(list.data[3]);
      Serial.print("\n");
      clearList();
      /*
      if(str[0]=='s')
      {
        Serial.println(str);
        u8g2.setCursor(55, 15); 
        u8g2.print("square");
        //delay(10000);   
      }
      else if(str[0]=='c'){
          Serial.println(str);
          u8g2.setCursor(55, 15); 
          u8g2.print("circle");
          //delay(10000);
      }
      else if(str[0]=='t'){
        Serial.println(str);
        u8g2.setCursor(55, 15); 
        u8g2.print("triangle");
        //delay(10000); 
      }
      */
    }
  } while ( u8g2.nextPage() ); 
  //delay(500);
}


String detectString()
{
  while(softSerial.read() != '[');
  return(softSerial.readStringUntil(']'));
}
void clearList()
{
  memset(list.data, sizeof(list.data),0);
  list.len1 = 0;
  list.len2 = 0;
  list.len3 = 0;
  list.len4 = 0;
}
void getList(String s)
{
  //String s = detectString();
  String numStr = "";
  int k=0;
  for(int i = 0; i<s.length(); i++)
  {
    if(s[i]!='\"' && s[i]!=' ' && s[i]!='!' && s[i]!='#')
    {
      if(s[i]==',')
      {
         k++;
      }
      else{
        if(k==0)
        {
          list.data[0][list.len1]=s[i];
          list.len1++;
        }
        else if(k==1)
        {
          list.data[1][list.len2]=s[i];
          list.len2++;
        }
        else if(k==2)
        {
          list.data[2][list.len3]=s[i];
          list.len3++;
        }
        else if(k==3)
        {
          list.data[3][list.len4]=s[i];
          list.len4++;
        }
      }
    }
  }
}
void getS()
{
  str = detectString();
  Serial.println(str);
  getList(str);
}
String detectS()
{
  while(softSerial.read()!='"');
  return(softSerial.readStringUntil('"'));
}
void ball()
{
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.setFontDirection(0);
  u8g2.firstPage();
  while(1){
    do {
      if(softSerial.available())
      {
          getball();
      }
      u8g2.setCursor(0, 15);
      u8g2.print("measuring...");
      Serial.println("measuring...");
    }while ( u8g2.nextPage() );
  }
}
void getball()
{
    bal = detectString();
    Serial.println(bal);
    char kind = bal[2];
    Serial.println(kind);
}
