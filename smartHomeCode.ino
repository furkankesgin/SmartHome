#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

//////Rfid_pins\\\\\\\\\\\\

#define RST_PIN 0           
#define SS_PIN  3   
//////Rfid_pins_finish\\\\\\\\\\\\

MFRC522 mfrc522(SS_PIN, RST_PIN);  
Servo servo;
bool door_status = false;
bool print_gas_value;
bool is_buzzer_active = false;
bool is_card_false = false;
bool is_gas_expose = false;
bool alarm = false;
bool open_status = false;
int readed_temp = 0;
int gelen_veri;
int ldr_data = 100;
float temp_voltage = 0;
float temp = 0;
String gaz_value_text = "Gas Sensor : ";
String temp_value_inside =  "Inside temp : ";
String temp_value_outside =  "Outside temp : ";
String ldr_sensor_text = "Outside ldr : ";
String gas_value_max = "Gas Threshold : ";


String gas_alert = "Evde gaz kacağı var !";

String dogru_kart_uid = "765cc3e4";
String dogru_kart_text = "doğru id";
int servo_motor_degree = 90;
int servo_start_loc = 180;
String password = "SMARTHOME";
//////////pins\\\\\\\\\\\\

int serial_port = 9600;
int buzzer_pin = 4, buzzer_frequency_rfid=1000;
int servo_pin = 7, servo_degree=180;
int esik_degeri = 120;
int buzzer_frequency_gas = 500;
int buzzer_frequency = 0;
int ldr_pin = A0;
int ledRed = 5, ledGreen = 6, ledBlue = A1;
int temp_in_pin = A2, temp_out_pin = A3;
int gas_pin = A4;
int lamb1_pin = 9;
int lamb2_pin = A5;

//////pins_finish\\\\\\\\\\\\


void setup() {
  noTone(buzzer_pin);
  serial_setup(serial_port);
  rfid_setup();
  servo_setup(servo_pin,servo_degree);
  gas_sensor_setup(buzzer_pin);
  buzzer_setup(buzzer_pin);
  ldr_setup(ldr_pin);
  rgb_setup(ledRed, ledGreen, ledBlue);
  lamb1_setup();
  lamb2_setup();
}

void loop() {
  door(dogru_kart_uid,dogru_kart_text, servo_motor_degree);
  buzzer_alarm_door(buzzer_pin);
  phone_gas();
//  delay(100);
  gas_sensor(gas_pin, esik_degeri,buzzer_pin, buzzer_frequency_gas, print_gas_value=true);
  buzzer_alarm_gas(buzzer_pin, gas_alert);
//  Serial.println(is_card_false);
//  Serial.println(is_gas_expose);
  temperature_sensor(temp_in_pin,temp_value_inside);
  temperature_sensor(temp_out_pin,temp_value_outside);
  ldr(ldr_pin, lamb1_pin);
  rgb();
  phone_door(password);
//  delay(100);
}

void serial_setup(int seri_port){
  Serial.begin(seri_port);
  }
void rfid_setup(){
  SPI.begin();
  mfrc522.PCD_Init(); 
  }
void buzzer_setup(int buzzer_pin){
  pinMode(buzzer_pin, OUTPUT);
  noTone(buzzer_pin);
  }
void servo_setup(int servo_pin, int servo_degree){
  servo.attach(servo_pin);
  servo.write(servo_degree);
  }
void gas_sensor_setup(int buzzer_pin){
  pinMode(buzzer_pin, OUTPUT);
  }
void ldr_setup(int ldr_pin){
    pinMode(ldr_pin, INPUT);
  }
void rgb_setup(int ledRed, int ledGreen, int ledBlue){
  pinMode(ledRed, OUTPUT);
  pinMode(ledBlue, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  }
void lamb1_setup(){
  pinMode(lamb1_pin, OUTPUT);
}
void lamb2_setup(){
    pinMode(lamb2_pin, OUTPUT);

}

void door(String uid,String text, int new_servo_degree){
  if (mfrc522.PICC_IsNewCardPresent()){  
    if (mfrc522.PICC_ReadCardSerial())
      if(get_uid() == uid){
        Serial.println(text);
        servo.write(new_servo_degree);
        is_card_false = false;
      }
      else{
        Serial.println(get_uid());
        is_card_false = true;
        servo.write(servo_start_loc);
      }
  }
}
void buzzer_alarm_door(int buzzer_pin){
  if(is_card_false){
      tone(buzzer_pin,buzzer_frequency_rfid);
      Serial.println("Hırsız var!");
  }
  else if(!is_card_false && !is_gas_expose){
      noTone(buzzer_pin);
  }
}
void gas_sensor(int gas_pin, 
                int esik_degeri, 
                int buzzer_pin, 
                int buzzer_frequncy_gas, 
                bool print_gas_value){
    gelen_veri = analogRead(gas_pin);
    Serial.println(gas_value_max + String(esik_degeri));

    if(gelen_veri > esik_degeri){
      if(print_gas_value){
        Serial.println(gaz_value_text + String(gelen_veri));
        }
    is_gas_expose = true;
  }
  else{                    
    is_gas_expose = false;
            Serial.println(gaz_value_text + String(gelen_veri));

    }
  }
  
void buzzer_alarm_gas(int buzzer_pin, String gas_alert){
  if(is_gas_expose){
      tone(buzzer_pin,buzzer_frequency_gas);
      Serial.println(gas_alert);
  }
   else if(!is_card_false && !is_gas_expose){
      noTone(buzzer_pin);
   }
  }
void ldr(int ldr_pin, int lamb_pin){
int data = analogRead(ldr_pin);
  data = map(data, 0, 917, 0, 255);
  if(data < ldr_data){
    digitalWrite(lamb1_pin,HIGH);
    digitalWrite(lamb2_pin,HIGH);
  }
  else{
        digitalWrite(lamb1_pin,LOW);
        digitalWrite(lamb2_pin,LOW);


  }
  Serial.println(ldr_sensor_text+String(data));
}
void temperature_sensor(int pin, String value){
  readed_temp = analogRead(pin);
  temp_voltage = (5000.0/1023.0) * readed_temp;
  temp = temp_voltage / 10.0;
  Serial.println(value + String (temp));
}

void phone_door(String password){
  String alinanDeger = Serial.readString();
  if(alinanDeger.substring(0,4) == "PWD="){
    alinanDeger = alinanDeger.substring(4);
    Serial.println(alinanDeger);
     if(alinanDeger.substring(0,password.length()) == password){
    int alinanDegerIndex = alinanDeger.indexOf(',');
    alinanDeger = alinanDeger.substring(alinanDegerIndex+1);
        Serial.println(alinanDeger);
    if(alinanDeger == "1"){
        servo.write(90); // on
    }
    else{
        servo.write(180); // off
    }
  }
 
  }
}

void phone_gas(){
  int value = 0;
  String alinanDeger = Serial.readString();
  if(alinanDeger.substring(0,4) == "GAS="){
    alinanDeger = alinanDeger.substring(4);
    Serial.println(alinanDeger);
     value = alinanDeger.toInt();
     esik_degeri = value;
  }
}
String get_uid(){
    String uid = "";
    uid += String(dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size));
    Serial.println(uid);
    return uid;
  }
String dump_byte_array(byte *buffer, byte bufferSize) {
    String CardID = "";
    for (byte i = 0; i < bufferSize; i++) {
        CardID += String(buffer[i], HEX);
    }
    return CardID;
}

void rgb(){
  String alinanDeger =Serial.readString();
  if(alinanDeger.substring(0,4) == "LED="){
  alinanDeger = alinanDeger.substring(4);
  int commaIndex = alinanDeger.indexOf(',');
  int secondCommaIndex = alinanDeger.indexOf(',', commaIndex + 1);
  String firstValue = alinanDeger.substring(0, commaIndex);
  String secondValue = alinanDeger.substring(commaIndex + 1, secondCommaIndex);
  String thirdValue = alinanDeger.substring(secondCommaIndex + 1); // To the end of the string
  int r = firstValue.toInt();
  int g = secondValue.toInt();
  int b = thirdValue.toInt();
  analogWrite(ledRed, r);
  analogWrite(ledGreen, g);
  analogWrite(ledBlue, b);
  }
}
