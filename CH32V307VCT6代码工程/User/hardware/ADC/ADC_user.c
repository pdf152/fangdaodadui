#include "ADC_user.h"

/* Global Variable */
s16 Calibrattion_Val = 0; //校准值
/********************************************************************
 * 函 数 名       : ADC1_Init
 * 函数功能       : 初始化ADC
 * 输    入       : 无
 * 输    出       : 无
 ********************************************************************/
void ADC1_Init(void) {

    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;


    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);       //初始化ADC时钟，设置时钟为PCLK2的8分频，最大时钟为14MHz

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);                      //配置PA1口为AD输入口

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);                      //配置PA1口为AD输入口

    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;     //设置AD模式为单独模式，只使用ADC1
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;              //禁用多通道模式，启用单通道模式
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;          //启动连续转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //不启用外部触发源，启动软件触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;      //数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 2;                     //要转换通道数量
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_Cmd(ADC1, ENABLE);                                      //使能ADC

    ADC_BufferCmd(ADC1, DISABLE);                               //disable buffer
    //测量ADC校准数据，也可以不使用。
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
    Calibrattion_Val = Get_CalibrationValue(ADC1);

    ADC_BufferCmd(ADC1, ENABLE);                                //enable buffer
}
/********************************************************************
 * 函 数 名       : Get_ADC_Val()
 * 函数功能    : 返回ADCx转换出的结果数据。
 * 输    入          : 通道数
 * 输    出          : ADCx转换出的结果数据
 ********************************************************************/
void Get_ADC_Val(uint16_t ADC_Value[], uint8_t ADC_channel[], uint8_t n) {
    uint8_t i;
    for (i = 0; i < n; i++) {
        ADC_RegularChannelConfig(ADC1, ADC_channel[i], i + 1,
                ADC_SampleTime_239Cycles5);
    }
    for (i = 0; i < n; i++) {
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
        ADC_Value[i] = ADC_GetConversionValue(ADC1);
    }

}

//void ADC1_Init(void)
//{
//    ADC1_GPIO_Config();     //下面有详细的代码配置
//    ADC1_Mode_Config();
//}
//
//static void ADC1_GPIO_Config(void)
//{
// GPIO_InitTypeDef GPIO_InitStructure;                   //定义写入寄存器的结构体
// RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);     //使能时钟，注意使能的是DMA1
// RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);    //本实验使用的是GPIOC1，使能ADC1,GPIO时钟
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;              //配置为PC.1
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;          //设置为模拟输入
// GPIO_Init(GPIOC, &GPIO_InitStructure);                 // 初始化写入寄存器中
//}
//
//static void ADC1_Mode_Config(void)
//{
// DMA_InitTypeDef DMA_InitStructure;                                 //定义写入DMA寄存器的结构体
// ADC_InitTypeDef ADC_InitStructure;                                 //定义写入ADC寄存器的结构体
// DMA_DeInit(DMA1_Channel1);                                         //复位初始化，配置为复位模式，下面我们再重新配置（不知道有用没）
// DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;        //设置源地址
// DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;   //设置内存地址
// DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                 // 设置传输方向
// DMA_InitStructure.DMA_BufferSize = 1;                              //因为我们最后一直改变一个变量，所以只有一位
// DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   //外设地址固定
// DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;           //内存地址固定
// DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;    //外设传输数据大小为半字
// DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;            //内存传输数据大小为半字
// DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                    //循环模式
// DMA_InitStructure.DMA_Priority = DMA_Priority_High;                //高优先级
// DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                       //禁止内存间传输数据
// DMA_Init(DMA1_Channel1, &DMA_InitStructure);                       //将结构体写入寄存器中
//
// DMA_Cmd(DMA1_Channel1, ENABLE);                                    //使能DMA寄存器的1通道
//
// /* ADC1 configuration */
// ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                 //因为只有一个通道，所以为独立ADC模式
// ADC_InitStructure.ADC_ScanConvMode = DISABLE ;                     //一个通道，所以不使能扫描模式
// ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                 //开启连续转换，就是不停转换
// ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;            //不使用外部触发，外部触发只有几个固定引脚，详情看手册
// ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;             //我们定义16位数，采集12位，所以要定义对齐方向，在这我们采取右对齐
// ADC_InitStructure.ADC_NbrOfChannel = 1;                            //要转换的通道数目
// ADC_Init(ADC1, &ADC_InitStructure);                                //将结构体写入寄存器中
//
// RCC_ADCCLKConfig(RCC_PCLK2_Div8);                                  //72M时钟8分频为9M PS：ADC最大时钟速率为14M，后面解释
//
// ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_55Cycles5);  //初始化，采样时间为55.5us，采样序号是1，PS：采样时间最                                                                         // 为1.5us，转换时间为固定12.5us，所以相当于采集完一次数为14us
// ADC_DMACmd(ADC1, ENABLE);                                          //  使能DMA1
// ADC_Cmd(ADC1, ENABLE);                                             //使能ADC1
// ADC_ResetCalibration(ADC1);                                        //复位校准寄存器
// while(ADC_GetResetCalibrationStatus(ADC1));                        //等待复位校准寄存器完成
// ADC_StartCalibration(ADC1);                                        //ADC校准
// while(ADC_GetCalibrationStatus(ADC1));                             //等待校准完成
// ADC_SoftwareStartConvCmd(ADC1, ENABLE);                            //使能ADC1的软件触发
//}
//



