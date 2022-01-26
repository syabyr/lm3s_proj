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

  //���ôӻ���ַ��дģʽ
  I2CMasterSlaveAddrSet(I2C0_MASTER_BASE,addr,false);
  
  //����Ҫ���͵Ŀ��������ֵ�I2C master
  
  I2CMasterDataPut(I2C0_MASTER_BASE,command);

  //���Ϳ���������
 
  
  I2CMasterControl(I2C0_MASTER_BASE,I2C_MASTER_CMD_BURST_SEND_START);
  
  while(I2CMasterBusy(I2C0_MASTER_BASE));

  
  I2CMasterDataPut(I2C0_MASTER_BASE,data);
 

  
   I2CMasterControl(I2C0_MASTER_BASE,I2C_MASTER_CMD_BURST_SEND_FINISH);
   
  while(I2CMasterBusy(I2C0_MASTER_BASE));  
 
  
  

}

unsigned char Pca9534ReadReg(unsigned char addr,unsigned char command)
{

  unsigned char getdata;
  //���ôӻ���ַ��дģʽ
  I2CMasterSlaveAddrSet(I2C0_MASTER_BASE,addr,false);
  
  //����Ҫ���͵Ŀ��������ֵ�I2C master
  
  I2CMasterDataPut(I2C0_MASTER_BASE,command);

  //���Ϳ���������
  
  I2CMasterControl(I2C0_MASTER_BASE,I2C_MASTER_CMD_BURST_SEND_START);
  
  while(I2CMasterBusy(I2C0_MASTER_BASE));
  
  //���豸��ַ+��дλд�뷢�����ݼĴ���
  
  I2CMasterSlaveAddrSet(I2C0_MASTER_BASE,addr,true);

  
  //�ٷ����豸��ַ+��дλ
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
  //���ôӻ���ַ����ģʽ
  I2CMasterSlaveAddrSet(I2C0_MASTER_BASE,addr,true);
  

  //���ʹӵ�ַ
  
  I2CMasterControl(I2C0_MASTER_BASE,I2C_MASTER_CMD_BURST_RECEIVE_START);
  
  while(I2CMasterBusy(I2C0_MASTER_BASE));
  
 // getdata=I2CSlaveDataGet(I2C0_MASTER_BASE,I2C_MASTER_CMD_BURST_RECEIVE_START);
  
  //����ֹͣλ
  
  
  
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