/******************
裁判信息解读与透传数据帧封装程序
update: 2017.5.7
    视情况调用最后三个函数
    全局变量说明见uart2referee.h
    支持上传3个float数据
******************/
#include "sys.h"
#include "stdio.h"  // define NULL
#include "stdbool.h" 

// flaot和字节互转
typedef union {
    float f;
    unsigned char b[4];
} Bytes2Float;

// flaot和u32互转
typedef union {
    u32 u32_value;
    unsigned char b[4];
} Bytes2U32;



// 位置信息(被其他结构体调用), 定位数据单位为米，小数点后两位为有效数据。
typedef __packed struct {
    uint8_t flag;
    float x;
    float y;
    float z;
    float compass;
}tLocData;

// 比赛进程信息（0x0001）, 发送频率为50Hz。 总36字节
typedef __packed struct {
	uint16_t stageRemianTime;
	uint8_t gameProgress;
	uint8_t robotLevel;
	uint16_t remainHP;
	uint16_t maxHP;
}extGameRobotState_t;

/*
实时血量变化信息(0×0002)
    0-3bits: 若变化类型为装甲伤害时：标识装甲ID
    0×00: 0 号装甲面 （前）
    0×01: 1 号装甲面 （左）
    0×02: 2 号装甲面 （后）
    0×03: 3 号装甲面 （右）
    0×04: 4 号装甲面 （上1）
    0×05: 5 号装甲面（上2）
其它暂作保留  
	4-7bits: 血量变化类型  
*/
typedef __packed struct {
    uint8_t armorType:4;  // 标识装甲ID 
    uint8_t hurtType:4;  // 血量变化类型
}extRobotHurt_t;

// 实时射击信息(0x0003) 总共6字节
typedef __packed struct {
    uint8_t bulletType;  // 弹丸类型
    uint8_t bulletFreq;  // 弹丸射频	
    float bulletSpeed;  // 弹丸射速
}extShootData_t;

// 实时功率热量数据（0x0004）
typedef __packed struct {
	float chassisVolt;
	float chassisCurrent;
	float chassisPower;
	float chassisPowerBuffer;
	uint16_t shooterHeat0;
	uint16_t shooterHeat1;
}extPowerHeatData_t;

//机器人位置和枪口朝向数据(0x0008)
typedef __packed struct {
	float x;
	float y;
	float z;
	float yaw;
}extGameRobotPos_t;



//1.2.4 赛场信息(0x0005)
//发送速率：2Hz数字节偏移大小说明，总12字节
typedef __packed struct  
{  
     uint8_t RobotColor :2;  //0x0 :该机器人为红方  //0x1 :该机器人为蓝方
     uint8_t RedBaseSta :2;  //0x0   : 红方基地普通状态  //0x1  : 基地无敌
     uint8_t BlueBaseSta:2;  //0x0   : 蓝方基地普通状态  //0x1 : 基地无敌
     uint8_t IslandLanding :2;  //0x0 : 无机器人登岛  
								//0x1 : 红方英雄登岛  
								//0x2 : 蓝方英雄登岛  
								//0x3 : 双方英雄均登岛
  
     uint8_t RedAirPortSta :4;  //恢复立柱状态 红蓝 
     uint8_t BlueAirPortSta:4;  //[0:3] 红方停机坪状态  
								//[4:7] 蓝方停机坪状态   
								//0x0 : 恢复立柱无效  
								//0x1 : 恢复立柱可被激活  
								//0x2 : 恢复立柱正在被激活  
								//0x3 : 恢复立柱已激活  
								//0x4 : 恢复立柱冷却中
 
  
     uint8_t No1PillarSta:4;  //[0:3]  : 1号立柱状态  
     uint8_t No2PillarSta:4;  //[4:7]  : 2号立柱状态  
     uint8_t No3PillarSta:4;  //[ 8:11] :3号立柱状态  
     uint8_t No4PillarSta:4;  //[12:15]: 4号立柱状态  
     uint8_t No5PillarSta:4;  //[16:19]: 5号立柱状态  
     uint8_t No6PillarSta:4;  //[20:23]: 6号立柱状态  
								//0x0: 立柱无效  
								//0x1: 立柱可被占领  
								//0x2: 红方正在占领该立柱  
								//0x3: 蓝方正在占领该立柱  
								//0x4: 红方已占领  
								//0x5: 蓝方已占领  
  
     uint8_t RedBulletBoxSta :4;  //加子弹机构状态  
									//[0:3] ： 红方加弹机构状态  
									//0x00 ： 停止加弹  
									//0x01 ： 加弹中
     uint8_t BlueBulletBoxSta:4;  //[4:7] ： 蓝方加弹机构状态  
									//0x00 ： 停止加弹  
									//0x01 ： 加弹中
     uint16_t RedBulletAmount;  //大符状态  
     uint16_t BlueBulletAmount;  

     uint8_t No0BigRuneSta :4;  //[0:3] 0号大符状态  
     uint8_t No1BigRuneSta :4;  //[3:7] 1号大符状态  
							 //  0x0 : 大符无效  
							//  0x1 : 大符可被激活  
							//  0x2 : 大符正在被红方激活  
							//  0x3 : 大符正在被蓝方激活  
							//  0x4 : 大符已被红方激活  
							//  0x5 : 大符已被蓝方激活
     uint8_t AddDefendPrecent;  //  防御加成百分数

}tStudentPropInfo;


// 学生上传自定义数据(0x0006)
// 支持学生上传3个float类型的数据. 需要按照协议来发送.
// 发送频率最大200Hz.
typedef __packed struct {
    float data1;
    float data2;
    float data3;
		uint8_t mask;
}extShowData_t;

// 全局裁判信息字段定义

extern extGameRobotState_t extGameRobotState;  // 比赛进程信息（0x0001）
extern extRobotHurt_t extRobotHurt;  // 伤害数据(0x0002)
extern extShootData_t extShootData;  // 实时射击信息(0x0003)
extern extPowerHeatData_t extPowerHeatData;//实时功率热量数据(0x0004)
extern extGameRobotPos_t extGameRobotPos; //机器人位置和枪口朝向数据(0x0008)
extern extShowData_t extShowData;  // 学生上传自定义数据(0x0005)
extern tStudentPropInfo StudentPropInfo;


// 使用前的初始化所有裁判信息相关结构体, 可自行添加定义初值
// 可不用，系统好像默认初值是0？？   有的好像显示不出来啊


// 使用完整数据帧立刻更新全部裁判信息相关结构体。(带校验)
u8 frame_interpret(uint8_t * frame);

// 读入单字节来更新全部裁判信息相关结构体, 
// 即仅累积字节为一完整数据包时 才调用frame_interpret函数 来更新相关结构体。

// 自定义数据帧, 封装入数组头指针custom_frame，长度 = 5+2+12+2 = 21
// 调用前请确保全局变量MyData结构体已更新值, 
// 发送示例:
// for(i=0;i<21;i++) {
//     USART_SendData(USART2, custom_frame_test[i]);
//     while(USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET);
// }


// debug用的全局变量
extern u8 referee_message[64];  // 完整数据帧存放, 理论44就够。
extern u8 cmdID;;
extern u8 blood_counter;  // (debug)被打计数

// 以下暂时不用
//// 校验数据帧, CRC8和CRC16
//u8 Verify_frame(uint8_t * frame);

extern void update_from_dma(void);
extern u8 seq_real;
extern u8 usart6_dma_flag;
extern int shoot_counter_referee;
