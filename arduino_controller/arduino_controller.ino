#include <SoftwareSerial.h>
#include <XBee.h>
SoftwareSerial roboSerial = SoftwareSerial(10, 11);
SoftwareSerial xbeeSerial = SoftwareSerial(12, 13);
XBee xbee = XBee();
Rx16Response rx16 = Rx16Response();

bool read = false;//past tense;
void setup() {
  Serial.begin(9600);
  roboSerial.begin(9600);
  xbeeSerial.begin(9600);
  xbeeSerial.listen();
  roboSerial.listen();
  xbee.setSerial(xbeeSerial);
}

void loop() {
//  while(Serial.available()){
//    String msg=Serial.readString();
//    if(msg.length()>0){
//      Serial.println("Sent: "+msg);
//      serial.println(msg);
//    }
//  }
//  while(serial.available()){
////  char c=serial.read();
////  Serial.println(c);
//    Serial.println("Received Message");
//    Serial.println(serial.readString());
//  }
  xbee.readPacket();
  if (xbee.getResponse().isAvailable()) {
    read=true;
    uint8_t* data = xbee.getResponse().getFrameData();
    Serial.println(xbee.getResponse().getFrameDataLength());
    Serial.println("Received Frame");
    for (int i=0;i<18;i++){
      Serial.print(data[i],HEX);
      Serial.print(" ");
      roboSerial.write(data[i]);
    }
    roboSerial.print("\r");
//    Serial.println();
//    for (int i = 1; i < 5; i++) {
//      Serial.println(*((float*)&data[i*4-2]));
//    }
//    ex[0] = *((float*)&data[0*4+2]);//x error
//    ey[0] =*((float*)&data[1*4+2]);//y error
//    theta = *((float*)&data[2*4+2]);//rotation
//    eTheta[0] = *((float*)&data[3*4+2]);//rotation error
  }
  //updateWheels();
  if(read){
    String dataString="";
    while(roboSerial.available()){
      dataString=roboSerial.readString();
    }
    if(dataString.length()>0){
      read=false;
      uint8_t data[dataString.length()];
      for(int i=0;i<dataString.length();i++){
        data[i]=dataString.charAt(i);
      }
      Tx16Request tx = Tx16Request(0xFFFD,data,dataString.length());
      xbee.send(tx);
    }
    
  }
}
