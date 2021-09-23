#include <SPI.h>
#include "printf.h"
#include "RF24.h"

// instantiate an object for the nRF24L01 transceiver
RF24 radio(14, 12); // using pin 7 for the CE pin, and pin 8 for the CSN pin

// Let these addresses be used for the pair
uint8_t address[][6] = {"1Node", "2Node"};
bool radioNumber = 1; // 0 uses address[0] to transmit, 1 uses address[1] to transmit


bool role = true;  // true = TX role, false = RX role
float payload = 0.0;

unsigned long timming ; 
unsigned long time2; 

bool sended, recieved ; 

void InitialiceRadio() {
    Matrix<4, 1> quaternio; 
    if (!radio.begin()) {
      Serial.println(F("radio hardware is not responding!!"));
      while (1) {} // hold in infinite loop
    }
    radioNumber = 1; 

    radio.setPALevel(RF24_PA_MAX);  // RF24_PA_MAX is default.

    radio.setRetries(1, 6); 

    // save on transmission time by setting the radio to only transmit the
    // number of bytes we need to transmit a float

    radio.setPayloadSize(sizeof(quaternio)); // float datatype occupies ?? bytes

    radio.setChannel(2475); 

    radio.setDataRate(RF24_2MBPS); 

    // set the TX address of the RX node into the TX pipe
    radio.openWritingPipe(address[radioNumber]);     // always uses pipe 0

    // set the RX address of the TX node into a RX pipe
    radio.openReadingPipe(1, address[!radioNumber]); // using pipe 1

    // additional setup specific to the node's role
    if (role) {
      radio.stopListening();  // put radio in TX mode
    } else {
      radio.startListening(); // put radio in RX mode
    }

    timming = micros(); 
    time2 = micros(); 
}

void deepSend(Matrix<4, 1> &qCurrent){
    bool report = radio.write(&qCurrent, sizeof(qCurrent));  

    if (report) {
      //Serial.print(F("Transmission successful! "));          // payload was delivered
      Serial.print(F("time taken to send: "));
      Serial.print(micros() - time2);                 // print the timer result
      time2 = micros(); 
      Serial.print(F(" us.")); 
      Serial << qCurrent << '\n' ; 
      radio.startListening();
      sended = true; 
    } else {
      // Sí no puede transmitir reiniciar el radio
      Serial.println(F("Transmission failed or timed out")); // payload was not delivered
      radio.startListening();
      //InitialiceRadio();
    }
}

bool send(Adafruit_BNO055 &bno, Matrix<4, 1> &qCurrent){
  bool sended = false; 


    radio.stopListening();
    Matrix<4, 1> raw_data = getQuaternio(bno) ; //Obtain the current quaternion from DMP

    raw_data = getQuaternio(bno);

    if(isQuaternion(raw_data)){ //Sí la información dada por la IMU es cuaternio almacena el valor
    qCurrent = raw_data ;  
    deepSend(raw_data); 
    } else{
      raw_data = getQuaternio(bno);
      if(isQuaternion(raw_data)){
          qCurrent = raw_data ; 
          deepSend(raw_data); 
      }
    }
    
  return sended; 
}

bool recieve(Matrix<4, 1> &qDeseado){
  bool recieved = false; 


  timming = micros(); 
  while(true){
    uint8_t pipe;
    radio.startListening(); 
    //Serial.println("listening"); 
    if (radio.available(&pipe)) {             // is there a payload? get the pipe number that recieved it
      uint8_t bytes = radio.getPayloadSize(); // get the size of the payload
      radio.read(&qDeseado, bytes) ; 
      Serial.print(F("Received "));
      Serial.print(bytes);                    // print the size of the payload
      Serial.print(F(" bytes on pipe "));
      Serial.print(pipe);                     // print the pipe number
      Serial.print("time taken to recieve: "); 
      Serial.println(micros() - time2); 
      time2 = micros(); 
      recieved = true; 
      radio.stopListening();
      break ; 
    }

    if(micros() - timming > 10000){
      Serial.println("time listening exceded!"); 
      role = !role; 
      break; 
    }
  }//while
  
  return recieved; 
}