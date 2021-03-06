#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <geometry_msgs/TransformStamped.h>
#include <geometry_msgs/Twist.h>

#include <string>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include "quaternion.h"
#include "xbee/device.h"
#include "xbee/serial.h"
#include "serial_help.h"


float vx,vy,theta,omega;
Vec3 tarLoc;
Vec3 tarAtt;
Vec3 loc;
Vec3 att;
xbee_dev_t xbee;
xbee_serial_t serial_port;

void sendMessage(char* toSend){
  //uint8_t toSend[4*sizeof(float)];
  int loc=0;
  // memcpy(&toSend[sizeof(float)*loc++],&vx,sizeof(float));
  // memcpy(&toSend[sizeof(float)*loc++],&vy,sizeof(float));
  // memcpy(&toSend[sizeof(float)*loc++],&theta,sizeof(float));
  // memcpy(&toSend[sizeof(float)*loc++],&omega,sizeof(float));
  //xbee_frame_write(&xbee,NULL,0,toSend,16,0);
  std::cout<<toSend;
  uint16_t len=5+(strlen( toSend));//01,01,ff,fe,00,data
  uint8_t msb=len>>8;
  uint8_t lsb=len&255;
  uint8_t header[7];
  header[0]=0x7E;
  header[1]=msb;
  header[2]=lsb;
  header[3]=0x01;//frame type
  header[4]=0x01;//frame id
  header[5]=0xFF;//16 bit address msb
  header[6]=0xFE;//16 bit address lsb
  header[7]=0x00;
  uint8_t checksum=2;//frame type and id
  for(int i=0;i<strlen( toSend);i++){
    checksum+=toSend[i];
  }
  checksum=(uint8_t)255-(uint8_t)(checksum+0xFF+0xFE);
  uint8_t toWrite[9+(strlen( toSend))];
  for(int i=0;i<9+(strlen( toSend));i++){
    if(i<8)
      toWrite[i]=header[i];
    else if(i<8+(strlen( toSend)))
      toWrite[i]=toSend[i-8];
    else
      toWrite[(sizeof toWrite)-1]=checksum;
  }

  xbee_ser_write( &serial_port,toWrite,sizeof toWrite);
  // for(int i = 0;i<4;i++){
  //   std::cout<<*((float*)&toSend[i*4])<<"\n";
  // }
  time_t  startTime,currTime;
  time(&startTime);//seconds
  bool timeout = false;
  while ( false ){//!radio.available()
    time(&currTime);
    if ((currTime-startTime) > 10 ){//timeout after 10 seconds
        timeout = true;
        break;
    }
  }
  if ( timeout ){
    //TODO: fill this in lol
    // else{
  //   if (radio.available()){
  //
  //     for(int i = 0; i<4; i++){
  //       char log_msg [50];
  //       float got_time;
  //       radio.read( &got_time, sizeof(float) );
  //     }
  //   }
  // }

  }
}

void messageCallback( geometry_msgs::TransformStamped t){
  //const geometry_msgs::TransformStamped *t = &transform;
  std::string a =  t.header.frame_id;
  //Set target
  //Vec4 x;
  Vec4 quat;
  if(a=="target"){
    tarLoc.v[0] = t.transform.translation.x;
    tarLoc.v[1] = t.transform.translation.y;
    tarLoc.v[2] = t.transform.translation.z;
    quat.v[0] = t.transform.rotation.w;
    quat.v[1] = t.transform.rotation.x;
    quat.v[2] = t.transform.rotation.y;
    quat.v[3] = t.transform.rotation.z;
    tarAtt = Quat2RPY(quat);
  }
  else{
    loc.v[0] = t.transform.translation.x;
    loc.v[1] = t.transform.translation.y;
    loc.v[2] = t.transform.translation.z;
    quat.v[0] = t.transform.rotation.w;
    quat.v[1] = t.transform.rotation.x;
    quat.v[2] = t.transform.rotation.y;
    quat.v[3] = t.transform.rotation.z;
    att = Quat2RPY(quat);
  }
  //edit formulas!
  vx = (float)(tarLoc.v[0] - loc.v[0]);

  vy = (float)(tarLoc.v[1] - loc.v[1]);

  theta = (float)att.v[2];//yaw

  omega  = (float)(tarAtt.v[2]-att.v[2]);

  //sendMessage();

}


int main(int argc, char **argv){
  
  parse_serial_arguments(argc,argv,&serial_port);
 //  if (xbee_dev_init( &xbee, &serial_port, NULL, NULL))
  // {
  //  printf( "Failed to initialize device.\n");
  //  return 0;
  // }
  xbee_ser_open(&serial_port,9600);
  ros::init(argc,argv,"omnibot_throttle");
  ros::NodeHandle nh;
  //ros::Subscriber sub = nh.subscribe("/vicon/omnibot/omnibot",1000,messageCallback);


  double freq=1;
  ros::Rate rate(freq);
  ros::Duration dur(1/freq);
  
  while(ros::ok()){
    ros::Time start_ = ros::Time::now();
    //listen for updated position from vicon
    printf( "Send a command: ");
    char dummy[64];
		fgets( dummy, sizeof dummy, stdin);
    int index=0;
    while(dummy[index]>0){

      index++;}
    std::cout<<index<<'\n';
    char command[index-1];
    memcpy(command,&dummy[0],sizeof command);
    sendMessage(command);
    //ros::spinOnce();
    //listen for info from omnibot until it's time to check for updates from vicon
    while(ros::Time::now()-start_<dur){
      uint8_t b=xbee_ser_getchar(&serial_port);
      if(b==0x7E){

        uint8_t upper=xbee_ser_getchar(&serial_port);
        uint8_t lower=xbee_ser_getchar(&serial_port);
        uint16_t len = ((uint16_t)upper<<8) | lower;
        std::cout<<"Received "<<len<<" bytes\n";
        // if(len!=21)
        //   break;
        uint8_t data[len];
        uint8_t sum=0;
        for(int i=0;i<len;i++){
          data[i]=xbee_ser_getchar(&serial_port);
          sum+=data[i];
          std::cout<<data[i];
        }
        uint8_t checksum=xbee_ser_getchar(&serial_port);
        
        sum+=checksum;
        if(sum==0xFF){
          // float speeds[4];
          // int offset=len-16;
          // std::cout<<"Received speeds: ";
          // for(int i=0;i<4;i++){
          //   speeds[i]=*((float*)&data[i*4+offset]);
          //   std::cout<<speeds[i]<<"\t";
          // }
          std::cout<<"\nChecksum is happy!\n";
        }
      }
    }
    rate.sleep();
  }
}