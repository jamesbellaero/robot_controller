#include <SoftwareSerial.h>
#include <XBee.h>
#include <AltSoftSerial.h>
SoftwareSerial xbeeSerial = SoftwareSerial(12, 13);
AltSoftSerial roboSerial;
XBee xbee = XBee();
AltSoftSerial altser;
Rx16Response rx16 = Rx16Response();
long time;
bool read = false;//past tense;
void setup() {
  Serial.begin(115200);
  roboSerial.begin(9600);
  xbeeSerial.begin(9600);
  roboSerial.listen() ;
  xbee.setSerial(xbeeSerial);
  time=millis();
}

void loop() {
//  while(Serial.available()){
//    String msg=Serial.readString();
//    if(msg.length()>0){
//      Serial.println("Sent: "+msg);
//      roboSerial.println(msg);
//    }
//  }
//  while(roboSerial.available()){
////  char c=serial.read();
////  Serial.println(c);
//    
//    Serial.println("Received Message");
//    String msg=roboSerial.readString();
//    for(int i=0;i<msg.length();i++){
//        Serial.print(msg.charAt(i),HEX);
//        Serial.print(' ');
//    }
//    Serial.println();
//    Serial.println(msg);
//  }
  
  xbee.readPacket();
  if (xbee.getResponse().isAvailable()) {
    read=true;
    uint8_t* data = xbee.getResponse().getFrameData();
    int len=xbee.getResponse().getFrameDataLength();
    if(len>0){
      Serial.println(len);
      Serial.println("Received Frame");
      char command[len-4];
      for (int i=4;i<len;i++){//first 4 are frame id, 16 bit address, and option byte

        command[i-4]=data[i];
      }
      roboSerial.println(command);
      Serial.println(command);
    }
  }
 // roboSerial.listen();
  long time_listen=millis();
  while(read&&(millis()-time_listen<100)){//perhaps change to while(read&&time_waiting<10) or something
    roboSerial.listen();
    String dataString="";
    while(roboSerial.available()){
      dataString=roboSerial.readString();
    }
    if(dataString.length()>0){
      Serial.println(dataString);
      read=false;
      uint8_t data[dataString.length()];
      for(int i=0;i<dataString.length();i++){
        data[i]=dataString.charAt(i);
        Serial.print(data[i],HEX);
        Serial.print(' ');
      }
      Serial.println();
      Tx16Request tx = Tx16Request(0xFFFD,data,dataString.length());
      xbee.send(tx);
    }
  }
  read=false;
  xbeeSerial.listen();
  if(millis()-time>5000){
    time=millis();
    const char* dataStr="ping";
    char data[sizeof dataStr];
    strcpy(data,dataStr);
    Tx16Request tx = Tx16Request(0xFFFD,(uint8_t*)data,10);
    xbee.send(tx);
//    Serial.println("Pinged");
//    roboSerial.println("!g 400");
  }
}
