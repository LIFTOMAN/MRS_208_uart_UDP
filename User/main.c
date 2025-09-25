/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/05/31
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
/*
 *@Note
UDP Client example, demonstrating that UDP Client receives data and sends back.
For details on the selection of engineering chips,
please refer to the "CH32V20x Evaluation Board Manual" under the CH32V20xEVT\EVT\PUB folder.
 */

#include "string.h"
#include "eth_driver.h"

#define UDP_RECE_BUF_LEN 1472
u8 MACAddr[6];                       // MAC address
u8 IPAddr[4] = {192, 168, 1, 10};    // IP address
u8 GWIPAddr[4] = {192, 168, 1, 1};   // Gateway IP address
u8 IPMask[4] = {255, 255, 255, 0};   // subnet mask
u8 DESIP[4] = {255, 255, 255, 255};  // destination IP address
u16 desport = 1000;                  // destination port
u16 srcport = 1000;                  // source port

u8 SocketId;
u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][UDP_RECE_BUF_LEN];  // socket receive buffer
u8 MyBuf[UDP_RECE_BUF_LEN];
u8 uart_rx_bf[1024];

volatile _Bool eth_rx_en;
volatile _Bool O_eth_rx_en;
volatile _Bool inverter;

u16 uart_pntr;
u32 iterator;
u32 ti_data;
uint8_t ip_flash;

//============== PRTOTYPES ================//
void UART_2_Init (uint32_t u_speed);
void UART_2_write_byte (uint8_t _byte);
void UART_2_write_byteS (uint8_t *mass, uint8_t lenght);
void u2_parsing(void);
void my_FLASH_WRITE(uint32_t fl_addres, uint32_t data);
void my_FLASH_ERASE();
uint32_t my_FLASH_READ(uint32_t address);
//============ END PRTOTYPES ================//

/*********************************************************************
 * @fn      mStopIfError
 *
 * @brief   check if error.
 *
 * @param   iError - error constants.
 *
 * @return  none
 */
void mStopIfError (u8 iError) {
    if (iError == WCHNET_ERR_SUCCESS)
        return;
    printf ("Error: %02X\r\n", (u16)iError);
}

/*********************************************************************
 * @fn      TIM2_Init
 *
 * @brief   Initializes TIM2.
 *
 * @return  none
 */
void TIM2_Init (void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};

    RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = SystemCoreClock / 1000000;
    TIM_TimeBaseStructure.TIM_Prescaler = WCHNETTIMERPERIOD * 1000 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit (TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig (TIM2, TIM_IT_Update, ENABLE);

    TIM_Cmd (TIM2, ENABLE);
    TIM_ClearITPendingBit (TIM2, TIM_IT_Update);
    NVIC_EnableIRQ (TIM2_IRQn);
}

/*********************************************************************
 * @fn      CreateUdpSocket
 *
 * @brief   Create UDP Socket
 *
 * @return  none
 */
void CreateUdpSocket (void) {
    u8 i;
    SOCK_INF TmpSocketInf;

    memset ((void *)&TmpSocketInf, 0, sizeof (SOCK_INF));
    memcpy ((void *)TmpSocketInf.IPAddr, DESIP, 4);
    TmpSocketInf.DesPort = desport;
    TmpSocketInf.SourPort = srcport++;
    TmpSocketInf.ProtoType = PROTO_TYPE_UDP;     // 2
    TmpSocketInf.RecvStartPoint = (u32)SocketRecvBuf[SocketId];
    TmpSocketInf.RecvBufLen = UDP_RECE_BUF_LEN;  // 1472
    i = WCHNET_SocketCreat (&SocketId, &TmpSocketInf);
    printf ("WCHNET_SocketCreat %d\r\n", SocketId);
    mStopIfError (i);
}

void NET_DataInUart (uint8_t id) {
    u32 len;

    // Receive buffer end address
    u32 endAddr = SocketInf[id].RecvStartPoint + SocketInf[id].RecvBufLen;

    // Calculate the length of the received data
    if ((SocketInf[id].RecvReadPoint + SocketInf[id].RecvRemLen) > endAddr)
        len = endAddr - SocketInf[id].RecvReadPoint;
    else
        len = SocketInf[id].RecvRemLen;


    // send data
    //  i = WCHNET_SocketSend(id, (u8 *) SocketInf[id].RecvReadPoint, &len);
    //  if (i == WCHNET_ERR_SUCCESS)
    printf ("inp_data:");

    for (int f = 0; f < len; f++) 
    {
        UART_2_write_byte(SocketRecvBuf[id][f]);
    }

    WCHNET_SocketRecv (id, NULL, &len);  // Clear NET_RX data
}

/*********************************************************************
 * @fn      WCHNET_HandleSockInt
 *
 * @brief   Socket Interrupt Handle
 *
 * @param   socketid - socket id.
 *          intstat - interrupt status
 *
 * @return  none
 */
void WCHNET_HandleSockInt (u8 socketid, u8 intstat) {
    if (intstat & SINT_STAT_RECV)  // receive data
    {
        // WCHNET_DataLoopback(socketid);                        //Data loopback
        NET_DataInUart(socketid);
        //eth_rx_en = 1;

        inverter = !inverter;
        inverter ? GPIO_SetBits (GPIOC, GPIO_Pin_0) : GPIO_ResetBits (GPIOC, GPIO_Pin_0);
    }

    if (intstat & SINT_STAT_CONNECT)  // connect successfully
    {
        printf ("TCP Connect Success\r\n");
    }

    if (intstat & SINT_STAT_DISCONNECT)  // disconnect
    {
        printf ("TCP Disconnect\r\n");
    }

    if (intstat & SINT_STAT_TIM_OUT)  // timeout disconnect
    {
        printf ("TCP Timeout\r\n");
    }
}

/*********************************************************************
 * @fn      WCHNET_HandleGlobalInt
 *
 * @brief   Global Interrupt Handle
 *
 * @return  none
 */
void WCHNET_HandleGlobalInt (void) {
    u8 intstat;
    u16 i;
    u8 socketint;

    intstat = WCHNET_GetGlobalInt();  // get global interrupt flag
    if (intstat & GINT_STAT_UNREACH)  // Unreachable interrupt
    {
        printf ("GINT_STAT_UNREACH\r\n");
    }
    if (intstat & GINT_STAT_IP_CONFLI)  // IP conflict
    {
        printf ("GINT_STAT_IP_CONFLI\r\n");
        ip_flash++;
        my_FLASH_WRITE(0x800FFF0, ip_flash);
        NVIC_SystemReset();
    }
    if (intstat & GINT_STAT_PHY_CHANGE)  // PHY status change
    {
        i = WCHNET_GetPHYStatus();
        if (i & PHY_Linked_Status)
            printf ("PHY Link Success\r\n");
    }
    if (intstat & GINT_STAT_SOCKET) {
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {  // socket related interrupt
            socketint = WCHNET_GetSocketInt (i);
            if (socketint)
                WCHNET_HandleSockInt (i, socketint);
        }
    }
}

void my_NET_init(void)
{
    u8 i;

    //my_FLASH_WRITE(0x800FFC0);
    ip_flash = my_FLASH_READ(0x800FFF0) & 0xFF;

    //if FLASH is empty => factory settings 
    if(ip_flash == 57)
    {
        my_FLASH_WRITE(0x800FFF0, 0x01);
        printf ("load_def_flash");
    }

    ip_flash = my_FLASH_READ(0x800FFF0) & 0xFF;

    IPAddr[3] = ip_flash;
        
    i = ETH_LibInit (IPAddr, GWIPAddr, IPMask, MACAddr);  // Ethernet library initialize
    mStopIfError (i);

    if (i == WCHNET_ERR_SUCCESS)
        printf ("WCHNET_LibInit Success\r\n");
    for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++)
        CreateUdpSocket();  // Create UDP Socket
}

void process (void) 
{
    u2_parsing();

    // GPIO_SetBits(GPIOC, GPIO_Pin_0);
    // Delay_Ms(50);
    // GPIO_ResetBits(GPIOC, GPIO_Pin_0);
    // Delay_Ms(50);
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program
 *
 * @return  none
 */
int main (void) {
    u8 i;

    Delay_Init();
    //USART_Printf_Init(115200);        //USART initialize
    UART_2_Init(115200);
    printf ("UDPClient Test\r\n");
    if ((SystemCoreClock == 60000000) || (SystemCoreClock == 120000000))
        printf ("SystemClk:%d\r\n", SystemCoreClock);
    else
        printf ("Error: Please choose 60MHz and 120MHz clock when using Ethernet!\r\n");
    printf ("net version:%x\n", WCHNET_GetVer());
    if (WCHNET_LIB_VER != WCHNET_GetVer()) {
        printf ("version error.\n");
    }
    WCHNET_GetMacAddr (MACAddr);  // get the chip MAC address
    printf ("mac addr:");
    for (i = 0; i < 6; i++)
        printf ("%x ", MACAddr[i]);
    printf ("\n");
    TIM2_Init();

    my_NET_init();

    while (1) {
        process();
        /*Ethernet library main task function,
         * which needs to be called cyclically*/
        WCHNET_MainTask();
        /*Query the Ethernet global interrupt,
         * if there is an interrupt, call the global interrupt handler*/
        if (WCHNET_QueryGlobalInt()) {
            WCHNET_HandleGlobalInt();
        }
        // printf("cy\r\n");
    }
}

void u2_parsing(void)
{
    if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)
    {
        uart_rx_bf[uart_pntr] = USART_ReceiveData(USART2);

        ti_data = 0;

        if (uart_pntr < 2048)
            uart_pntr++;
    }

    ti_data++;

    if ((ti_data > 65500) && uart_pntr)
    {
        uint8_t br_ip[] = {255 , 255 , 255 , 255};

        WCHNET_SocketUdpSendTo(SocketId, uart_rx_bf, &uart_pntr, br_ip, desport);
        Delay_Ms(10);
        uart_pntr = 0;
    }
}

void UART_2_Init (uint32_t u_speed)  
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART2, ENABLE);

    /* USART2  TX-->A.2  RX-->A.3  */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init (GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = u_speed;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init (USART2, &USART_InitStructure);

    USART_Cmd (USART2, ENABLE);
}

void UART_2_write_byte (uint8_t _byte) 
{
    USART_SendData (USART2, _byte);

    while (USART_GetFlagStatus (USART2, USART_FLAG_TXE) == RESET);
}

void UART_2_write_byteS (uint8_t *mass, uint8_t lenght) 
{
    for (int i = 0; i < lenght; i++) 
    {
        USART_SendData (USART2, mass[i]);
        while (USART_GetFlagStatus (USART2, USART_FLAG_TXE) == RESET);
    }
}

void my_FLASH_WRITE(uint32_t fl_addres, uint32_t data)
{
    __disable_irq();
    FLASH_Unlock_Fast();
    Delay_Ms(1);

    FLASH_ErasePage_Fast(0x800FF00);
    Delay_Ms(1);

    /* fl_addres = 0x800FFC0 */

    // for (uint32_t i = 0; i < 61; i += 4)
    // {
    //     FLASH_ProgramWord(fl_addres + i, data);
    //     Delay_Ms(1);
    // }
    
    FLASH_ProgramWord(fl_addres, data);
    Delay_Ms(1);

    FLASH_Lock_Fast();
    __enable_irq();
}

void my_FLASH_ERASE()
{
    __disable_irq();
    FLASH_Unlock_Fast();
    FLASH_ErasePage_Fast(0x800FF00);
    Delay_Ms(100);
    FLASH_Lock_Fast();
    __enable_irq();
}

uint32_t my_FLASH_READ(uint32_t address)
{
    return(*(__IO uint32_t*) address);
}