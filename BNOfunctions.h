#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#define BNO055_SAMPLERATE_DELAY_MS (10)

// Check I2C device address and correct line below (by default address is 0x29 or 0x28)
//                                   id, address
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

void InitialiceMPU(Adafruit_BNO055 &bno){
      if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }

  delay(1000);

  /* Use external crystal for better accuracy */
  bno.setExtCrystalUse(true);
}

Matrix<4, 1> getQuaternio(Adafruit_BNO055 &bno){
    sensors_event_t event;
    bno.getEvent(&event);
    Matrix<4, 1> qCurrent ; 
    imu::Quaternion quat = bno.getQuat();
    qCurrent(0) = quat.w() ;
    qCurrent(1) = quat.x() ;
    qCurrent(2) = quat.y() ;
    qCurrent(3) = quat.z() ; 
    return qCurrent; 
}