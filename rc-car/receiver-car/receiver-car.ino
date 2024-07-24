/*
    DIY Arduino based RC Transmitter Project
              == Receiver Code ==
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8);   // nRF24L01 (CE, CSN)
const byte address[6] = "00001";

unsigned long lastReceiveTime = 0;
unsigned long currentTime = 0;

// Max size of this struct is 32 bytes - NRF24L01 buffer limit
struct Data_Package {
  byte j1PotX;
  byte j1PotY;
  byte j1Button;
};
//LIGHTS
#define light 2
Data_Package data; //Create a variable with the above structure

// MOTOT 1 -- vel
#define IN1 10
#define IN2 9

// MOTOR 2 -- Direccion
#define IN3 6
#define IN4 5

void setup() {
  Serial.begin(115200);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.startListening(); //  Set the module as receiver
  resetData();
  pinMode(light,OUTPUT);//lights
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}
void loop() {
  // Check whether there is data to be received
  if (radio.available()) {
    radio.read(&data, sizeof(Data_Package)); // Read the whole data and store it into the 'data' structure
    lastReceiveTime = millis(); // At this moment we have received the data
  }
  // Check whether we keep receving data, or we have a connection between the two modules
  currentTime = millis();
  if ( currentTime - lastReceiveTime > 1000 ) { // If current time is more then 1 second since we have recived the last data, that means we have lost connection
    resetData(); // If connection is lost, reset the data. It prevents unwanted behavior, for example if a drone has a throttle up and we lose connection, it can keep flying unless we reset the values
  }
  
  // Print the data in the Serial Monitor
  Serial.print("j1PotX: ");
  Serial.print(data.j1PotX);
  Serial.print("; j1PotY: ");
  Serial.print(data.j1PotY);
  Serial.print("; j1Button: ");
  Serial.print(data.j1Button);
  Serial.println("\n");
  
  //lights
  if(data.j1Button == 0){
    digitalWrite(light,HIGH);
    Serial.print("luz prendida");
    }else{
      digitalWrite(light,LOW);
      }

//FWD
if(data.j1PotX > 132){
    Serial.print("ADELANTE: ");
    Serial.print((data.j1PotX-128)*2);
      analogWrite(IN1,(data.j1PotX-128)*2);
      digitalWrite(IN2, LOW);
    
  }

//RVS
if(data.j1PotX < 123){
    Serial.print("ATRAS: ");
    Serial.print((127-data.j1PotX)*2);
      analogWrite(IN2,(127-data.j1PotX)*2);
      digitalWrite(IN1, LOW);
  }
//STAY STILL
if(data.j1PotX>122 && data.j1PotX<133){
    Serial.print("DETENIDO: ");
    digitalWrite(IN1,LOW);
    digitalWrite(IN2, LOW);
  }
 
//RIGHT
if(data.j1PotY > 132){
    Serial.print("DERECHA: ");
    Serial.print((data.j1PotY-128)*2);
      analogWrite(IN3,(data.j1PotY-128)*2);
      digitalWrite(IN4, LOW);
    
  }

//LEFT
if(data.j1PotY < 123){
    Serial.print("IZQUIERDA: ");
    Serial.print((127-data.j1PotY)*2);
      analogWrite(IN4,(127-data.j1PotY)*2);
      digitalWrite(IN3, LOW);
  }
//STRAIGHT
if(data.j1PotY>122 && data.j1PotY<133){
    Serial.print("RECTO: ");
    digitalWrite(IN3,LOW);
    digitalWrite(IN4, LOW);
  }

}

void resetData() {
  // Reset the values when there is no radio connection - Set initial default values
  data.j1PotX = 127;
  data.j1PotY = 127;
  data.j1Button = 1;

}
