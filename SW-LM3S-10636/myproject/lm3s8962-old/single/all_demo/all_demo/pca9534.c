#include <hw_types.h>
#include <hw_memmap.h>
#include <sysctl.h>
#include <gpio.h>
#include <i2c.h>
#define PCA9543_ADDR     0X20
#define CONTROL_BYTE  0x03

unsigned int iicintcount=0;
void Pca9534Write(unsigned char addr,unsigned char command,unsigned char data);
unsigned char Pca9534ReadReg(unsigned char addr,unsigned char command);
//*************************************************************
unsigned char Pca9534ReadPort(unsigned char addr);

//Be careful,this function read the port status of
//the pca9534 efficientlly .(of course the "Pca9534
//ReadReg" can also do the same function.)
//it assumes that the command byte has previously
//been programmed with 00h,so,befor you use this
//function you can call the "Pca9534ReadReg",
//which set the Pca9534 interal register to 0x00,
//that means the Input Port Register.
//*************************************************************
void Pca9534Write(unsigned char addr,unsigned char command,unsigned char data)
{

  //设置从机地址，写模式
  I2CMasterSlaveAddrSet(I2C0_MASTER_BASE,addr,false);
  
  //放置要发送的控制命令字到I2C master
  
  I2CMasterDataPut(I2C0_MASTER_BASE,command);

  //发送控制命令字
 
  
  I2CMasterControl(I2C0_MASTER_BASE,I2C_MASTER_CMD_BURST_SEND_START);
  
  while(I2CMasterBusy(I2C0_MASTER_BASE));

  
  I2CMasterDataPut(I2C0_MASTER_BASE,data);
 

  
   I2CMasterControl(I2C0_MASTER_BASE,I2C_MASTER_CMD_BURST_SEND_FINISH);
   
  while(I2CMasterBusy(I2C0_MASTER_BASE));  
 
  
  

}

unsigned char Pca9534ReadReg(unsigned char addr,unsigned char command)
{

  unsigned char getdata;
  //设置从机地址，写模式
  I2CMasterSlaveAddrSet(I2C0_MASTER_BASE,addr,false);
  
  //放置要发送的控制命令字到I2C master
  
  I2CMasterDataPut(I2C0_MASTER_BASE,command);

  //发送控制命令字
  
  I2CMasterControl(I2C0_MASTER_BASE,I2C_MASTER_CMD_BURST_SEND_START);
  
  while(I2CMasterBusy(I2C0_MASTER_BASE));
  
  //从设备地址+读写位写入发送数据寄存器
  
  I2CMasterSlaveAddrSet(I2C0_MASTER_BASE,addr,true);

  
  //再发从设备地址+读写位
  I2CMasterControl(I2C0_MASTER_BASE,I2C_MASTER_CMD_BURST_RECEIVE_START);
  
  while(I2CMasterBusy(I2C0_MASTER_BASE));
  
  
  I2CMasterControl(I2C0_MASTER_BASE,I2C_MASTER_CMD_BURST_RECEIVE_FINISH );
  
  
  while(I2CMasterBusy(I2C0_MASTER_BASE));
  getdata=I2CSlaveDataGet(I2C0_MASTER_BASE);
 
  
  return getdata;
  

}

unsigned char Pca9534ReadPort(unsigned char addr)
{


  unsigned char getdata;
  //设置从机地址，读模式
  I2CMasterSlaveAddrSet(I2C0_MASTER_BASE,addr,true);
  

  //发送从地址
  
  I2CMasterControl(I2C0_MASTER_BASE,I2C_MASTER_CMD_BURST_RECEIVE_START);
  
  while(I2CMasterBusy(I2C0_MASTER_BASE));
  
 // getdata=I2CSlaveDataGet(I2C0_MASTER_BASE,I2C_MASTER_CMD_BURST_RECEIVE_START);
  
  //发送停止位
  
  
  
  I2CMasterControl(I2C0_MASTER_BASE,I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
  
  while(I2CMasterBusy(I2C0_MASTER_BASE));
  
  getdata=I2CSlaveDataGet(I2C0_MASTER_BASE);
  
  return getdata;


}

void I2C_ISR(void)
{
I2CMasterIntClear(I2C0_MASTER_BASE);
iicintcount++;


}