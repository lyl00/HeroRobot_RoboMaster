/******************
������Ϣ�����͸������֡��װ����
update: 2017.5.7
    ��������������������
    ȫ�ֱ���˵����uart2referee.h
    ֧���ϴ�3��float����
******************/
#include "sys.h"
#include "stdio.h"  // define NULL
#include "stdbool.h" 

// flaot���ֽڻ�ת
typedef union {
    float f;
    unsigned char b[4];
} Bytes2Float;

// flaot��u32��ת
typedef union {
    u32 u32_value;
    unsigned char b[4];
} Bytes2U32;



// λ����Ϣ(�������ṹ�����), ��λ���ݵ�λΪ�ף�С�������λΪ��Ч���ݡ�
typedef __packed struct {
    uint8_t flag;
    float x;
    float y;
    float z;
    float compass;
}tLocData;

// ����������Ϣ��0x0001��, ����Ƶ��Ϊ50Hz�� ��36�ֽ�
typedef __packed struct {
	uint16_t stageRemianTime;
	uint8_t gameProgress;
	uint8_t robotLevel;
	uint16_t remainHP;
	uint16_t maxHP;
}extGameRobotState_t;

/*
ʵʱѪ���仯��Ϣ(0��0002)
    0-3bits: ���仯����Ϊװ���˺�ʱ����ʶװ��ID
    0��00: 0 ��װ���� ��ǰ��
    0��01: 1 ��װ���� ����
    0��02: 2 ��װ���� ����
    0��03: 3 ��װ���� ���ң�
    0��04: 4 ��װ���� ����1��
    0��05: 5 ��װ���棨��2��
������������  
	4-7bits: Ѫ���仯����  
*/
typedef __packed struct {
    uint8_t armorType:4;  // ��ʶװ��ID 
    uint8_t hurtType:4;  // Ѫ���仯����
}extRobotHurt_t;

// ʵʱ�����Ϣ(0x0003) �ܹ�6�ֽ�
typedef __packed struct {
    uint8_t bulletType;  // ��������
    uint8_t bulletFreq;  // ������Ƶ	
    float bulletSpeed;  // ��������
}extShootData_t;

// ʵʱ�����������ݣ�0x0004��
typedef __packed struct {
	float chassisVolt;
	float chassisCurrent;
	float chassisPower;
	float chassisPowerBuffer;
	uint16_t shooterHeat0;
	uint16_t shooterHeat1;
}extPowerHeatData_t;

//������λ�ú�ǹ�ڳ�������(0x0008)
typedef __packed struct {
	float x;
	float y;
	float z;
	float yaw;
}extGameRobotPos_t;



//1.2.4 ������Ϣ(0x0005)
//�������ʣ�2Hz���ֽ�ƫ�ƴ�С˵������12�ֽ�
typedef __packed struct  
{  
     uint8_t RobotColor :2;  //0x0 :�û�����Ϊ�췽  //0x1 :�û�����Ϊ����
     uint8_t RedBaseSta :2;  //0x0   : �췽������ͨ״̬  //0x1  : �����޵�
     uint8_t BlueBaseSta:2;  //0x0   : ����������ͨ״̬  //0x1 : �����޵�
     uint8_t IslandLanding :2;  //0x0 : �޻����˵ǵ�  
								//0x1 : �췽Ӣ�۵ǵ�  
								//0x2 : ����Ӣ�۵ǵ�  
								//0x3 : ˫��Ӣ�۾��ǵ�
  
     uint8_t RedAirPortSta :4;  //�ָ�����״̬ ���� 
     uint8_t BlueAirPortSta:4;  //[0:3] �췽ͣ��ƺ״̬  
								//[4:7] ����ͣ��ƺ״̬   
								//0x0 : �ָ�������Ч  
								//0x1 : �ָ������ɱ�����  
								//0x2 : �ָ��������ڱ�����  
								//0x3 : �ָ������Ѽ���  
								//0x4 : �ָ�������ȴ��
 
  
     uint8_t No1PillarSta:4;  //[0:3]  : 1������״̬  
     uint8_t No2PillarSta:4;  //[4:7]  : 2������״̬  
     uint8_t No3PillarSta:4;  //[ 8:11] :3������״̬  
     uint8_t No4PillarSta:4;  //[12:15]: 4������״̬  
     uint8_t No5PillarSta:4;  //[16:19]: 5������״̬  
     uint8_t No6PillarSta:4;  //[20:23]: 6������״̬  
								//0x0: ������Ч  
								//0x1: �����ɱ�ռ��  
								//0x2: �췽����ռ�������  
								//0x3: ��������ռ�������  
								//0x4: �췽��ռ��  
								//0x5: ������ռ��  
  
     uint8_t RedBulletBoxSta :4;  //���ӵ�����״̬  
									//[0:3] �� �췽�ӵ�����״̬  
									//0x00 �� ֹͣ�ӵ�  
									//0x01 �� �ӵ���
     uint8_t BlueBulletBoxSta:4;  //[4:7] �� �����ӵ�����״̬  
									//0x00 �� ֹͣ�ӵ�  
									//0x01 �� �ӵ���
     uint16_t RedBulletAmount;  //���״̬  
     uint16_t BlueBulletAmount;  

     uint8_t No0BigRuneSta :4;  //[0:3] 0�Ŵ��״̬  
     uint8_t No1BigRuneSta :4;  //[3:7] 1�Ŵ��״̬  
							 //  0x0 : �����Ч  
							//  0x1 : ����ɱ�����  
							//  0x2 : ������ڱ��췽����  
							//  0x3 : ������ڱ���������  
							//  0x4 : ����ѱ��췽����  
							//  0x5 : ����ѱ���������
     uint8_t AddDefendPrecent;  //  �����ӳɰٷ���

}tStudentPropInfo;


// ѧ���ϴ��Զ�������(0x0006)
// ֧��ѧ���ϴ�3��float���͵�����. ��Ҫ����Э��������.
// ����Ƶ�����200Hz.
typedef __packed struct {
    float data1;
    float data2;
    float data3;
		uint8_t mask;
}extShowData_t;

// ȫ�ֲ�����Ϣ�ֶζ���

extern extGameRobotState_t extGameRobotState;  // ����������Ϣ��0x0001��
extern extRobotHurt_t extRobotHurt;  // �˺�����(0x0002)
extern extShootData_t extShootData;  // ʵʱ�����Ϣ(0x0003)
extern extPowerHeatData_t extPowerHeatData;//ʵʱ������������(0x0004)
extern extGameRobotPos_t extGameRobotPos; //������λ�ú�ǹ�ڳ�������(0x0008)
extern extShowData_t extShowData;  // ѧ���ϴ��Զ�������(0x0005)
extern tStudentPropInfo StudentPropInfo;


// ʹ��ǰ�ĳ�ʼ�����в�����Ϣ��ؽṹ��, ��������Ӷ����ֵ
// �ɲ��ã�ϵͳ����Ĭ�ϳ�ֵ��0����   �еĺ�����ʾ��������


// ʹ����������֡���̸���ȫ��������Ϣ��ؽṹ�塣(��У��)
u8 frame_interpret(uint8_t * frame);

// ���뵥�ֽ�������ȫ��������Ϣ��ؽṹ��, 
// �����ۻ��ֽ�Ϊһ�������ݰ�ʱ �ŵ���frame_interpret���� ��������ؽṹ�塣

// �Զ�������֡, ��װ������ͷָ��custom_frame������ = 5+2+12+2 = 21
// ����ǰ��ȷ��ȫ�ֱ���MyData�ṹ���Ѹ���ֵ, 
// ����ʾ��:
// for(i=0;i<21;i++) {
//     USART_SendData(USART2, custom_frame_test[i]);
//     while(USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET);
// }


// debug�õ�ȫ�ֱ���
extern u8 referee_message[64];  // ��������֡���, ����44�͹���
extern u8 cmdID;;
extern u8 blood_counter;  // (debug)�������

// ������ʱ����
//// У������֡, CRC8��CRC16
//u8 Verify_frame(uint8_t * frame);

extern void update_from_dma(void);
extern u8 seq_real;
extern u8 usart6_dma_flag;
extern int shoot_counter_referee;
