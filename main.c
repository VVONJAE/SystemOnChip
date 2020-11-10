#include "xil_types.h"
#include "xparameters.h"
#include "xiicps.h"
#include "seven_seg.h"
#include "textlcd.h"
#include "LED.h"
#include "xtime_l.h"

//from pushbutton 0607
#include "xscugic.h"
#include "pushbutton.h"
#include <stdio.h>
#include <stdlib.h>
#include "xil_exception.h"

#include "xuartps_hw.h"
#include "sleep.h"

#define   IIC_SLAVE_ADDR   0x51
#define   IIC_SCLK_RATE   100000

//from pushbutton 0607
#define INTC_DEVICE_ID      XPAR_SCUGIC_0_DEVICE_ID
#define INTC_DEVICE_INT_ID   31
//

// from UART
#define      CR            0x0D                  // carriage return
#define    NUM_MAX         1
//

//from pushbutton 0607
int GicConfigure(u16 DeviceId);
void ServiceRoutine(void* CallbackRef); //
//

void   WriteTLCDReg(char *pRegVal, int val);
int      ReadRTC(XIicPs Iic, u8 *SendBuffer, u8 *RecvBuffer);

//for UART
void   PrintChar(u8 *str);
void   PrintMsg(u8 *str);
void   GetNumber(u8 *number);

unsigned int power(int, int); // 2의 배수 만들기
u32 NumberTransformFor7seg(u32, u8);

//from pushbutton 0607
XScuGic InterruptController;         // Instance of the Interrupt Controller
static XScuGic_Config* GicConfig;    // The configuration parameters of the controller
//

u32 score;
//u8 a = 0b00010010;
//u4 b = 0b0011;
//u8 c = 0b01000101;
//u4 d = 0b0110;
//u8 e = 0b01111000;
// global 변수 0607

XTime seed;
int LED;

u8 score_ten;
u8 score_one;
u8 StartSecReg;
u8 EndSecReg;
u8 GameStartSecReg;
u8 CheckSecReg;
int level;
int TimeLedOn;// time of LED  : TimeLedOn
int TimeLedOff;
int Status;
int SegReg;
char *mode;
char TlcdReg_upline[16];
char TlcdReg_downline[16];
int ExitOuterLoop;
int DeadMole;
u8 LiveMole;

int main(void)
{
   XTime_GetTime(&seed);
   srand(seed); // rand 함수 시드 변경
   //for UART
   u32   CntrlRegister;
   u8   number[NUM_MAX]   =   {0, };

   // Read status register
   CntrlRegister = XUartPs_ReadReg(XPAR_PS7_UART_1_BASEADDR, XUARTPS_CR_OFFSET);

   // Enable TX and RX for the device
   XUartPs_WriteReg( XPAR_PS7_UART_1_BASEADDR, XUARTPS_CR_OFFSET,
               ((CntrlRegister & ~XUARTPS_CR_EN_DIS_MASK) | XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN) );

   //
   //from pushbutton 0607



   //
   /*
    *  Run the Gic configure, specify the Device ID generated in xparameters.h
    */
   Status = GicConfigure(INTC_DEVICE_ID);
   if (Status != XST_SUCCESS) {
      xil_printf("GIC Configure Failed\r\n");
      return XST_FAILURE;
   }
   //


   XIicPs   Iic;         /**< Instance of the IIC Device */
   int    IicStatus;
   u8      *SendBuffer;   /**< Buffer for Transmitting Data */
   u8      RecvBuffer[3];   /**< Buffer for Receiving Data */

   //char   TlcdReg_upline[16];
   //char   TlcdReg_downline[16];

   int      i;
   int      wait;



   while(TRUE)
   {
      xil_printf("--------------------------------\n");
      xil_printf("game start.\r\n");

      score = 0;


     // for UART
     xil_printf(" Select game mode 1 or 2 or 3 \n");
     xil_printf("1:Easy  2:Normal  3:Chaos\n ");
     xil_printf("--------------------------------\n");
     GetNumber(number);
     xil_printf("\n");




     switch(number[0])
     {
       case ('1') :
       {
         xil_printf(" You selected 'Normal Mode'.\r\n");
         TimeLedOn = 6;
         mode = "EASY MODE        ";
         break;
       }

       case ('2') :
      {
      xil_printf(" You selected 'Normal Mode'.\r\n");
      TimeLedOn = 3;
      mode = "NORMAL MODE        ";
      break;
      }

       case ('3')   :
       {
         xil_printf(" You selected 'Chaos Mode'\r\n");
         TimeLedOn = ( (int)(rand()%3) + 1 );
         xil_printf("TimeLedOn : %d'\r\n", TimeLedOn);
         mode = "CHAOS MODE        ";
         level = 2;
         break;
       }
       default :
       {
         xil_printf("Enter correct number please! (1 or 2 or 3) \n");
         break;
       }
     }
     xil_printf("--------------------------------\n");




     IicStatus = ReadRTC(Iic, SendBuffer, RecvBuffer);
      if (IicStatus != XST_SUCCESS)
      {
        return XST_FAILURE;
      }


     DeadMole = 0;                                          //잡은 두더지 수 초기화
     LiveMole = 10;
     while(DeadMole<10){                                          //GAME 1

        if(level == 2) TimeLedOn = ( (int)(rand()%3) + 1 );

      SEVEN_SEG_mWriteReg(XPAR_SEVEN_SEG_0_S00_AXI_BASEADDR, 0, NumberTransformFor7seg(score,LiveMole));
      sprintf(TlcdReg_upline, mode);
      sprintf(TlcdReg_downline, "Total Score :%d  ", (int)score);
      for(i = 0; i < 4; i++)
      {
         TEXTLCD_mWriteReg(XPAR_TEXTLCD_0_S00_AXI_BASEADDR, i * 4, TlcdReg_upline[i * 4 + 3] + (TlcdReg_upline[i * 4 + 2] << 8) + (TlcdReg_upline[i * 4 + 1] << 16) + (TlcdReg_upline[i * 4] << 24));
         TEXTLCD_mWriteReg(XPAR_TEXTLCD_0_S00_AXI_BASEADDR, i * 4 + 16, TlcdReg_downline[i * 4 + 3] + (TlcdReg_downline[i * 4 + 2] << 8) + (TlcdReg_downline[i * 4 + 1] << 16) + (TlcdReg_downline[i * 4] << 24));
      }


      // 난수 발생 코드
      LED = (int)(rand()%4) + 1;
      xil_printf("\n");
      xil_printf("%d번 구멍에서 두더지가 나타났습니다. \n", LED);
      xil_printf("             _,..~~~..,_\n");
     xil_printf("          /                ＼\n");
     xil_printf("        /    ∂         6    ＼\n");
     xil_printf("      │         ω           │\n");
     xil_printf("_____│                      │_____\n\n");





      LED_mWriteReg( XPAR_LED_0_S00_AXI_BASEADDR, LED_S00_AXI_SLV_REG0_OFFSET, power(2, LED-1)); //LED ON 신호를 PL에 전달





      //32 bit to extract sec from Time Reg When LED is on
      IicStatus = ReadRTC(Iic, SendBuffer, RecvBuffer);
      if (IicStatus != XST_SUCCESS)
      {
         return XST_FAILURE;
      }
      StartSecReg = (RecvBuffer[0] & 0b00001111) + 10*(  (RecvBuffer[0] & 0b01110000 ) >> 4) ;
      //xil_printf("시작시간  %d \n", StartSecReg);
      EndSecReg = (RecvBuffer[0] & 0b00001111) + 10*(  (RecvBuffer[0] & 0b01110000 ) >> 4) ;
      xil_printf(" 두더지가 등장한지  %d초 지났습니다. \n", EndSecReg - StartSecReg);


      sleep(TimeLedOn/3);


      // Time measure to judge point when pushbutton is pushed
      //sleep(TimeLedOn/3);



      IicStatus = ReadRTC(Iic, SendBuffer, RecvBuffer);
      if (IicStatus != XST_SUCCESS)
      {
         return XST_FAILURE;
      }
      EndSecReg =    (RecvBuffer[0] & 0b00001111) + 10*(  (RecvBuffer[0] & 0b01110000 ) >> 4) ;
      xil_printf("  두더지가 등장한지  %d초 지났습니다. \n", (StartSecReg <= EndSecReg) ? EndSecReg - StartSecReg : (60+EndSecReg) - StartSecReg);



      sleep(TimeLedOn/3);


      IicStatus = ReadRTC(Iic, SendBuffer, RecvBuffer);
      if (IicStatus != XST_SUCCESS)
      {
         return XST_FAILURE;
      }
      EndSecReg = (RecvBuffer[0] & 0b00001111) + 10*(  (RecvBuffer[0] & 0b01110000 ) >> 4) ;
      xil_printf("   두더지가 등장한지  %d초 지났습니다. \n", (StartSecReg <= EndSecReg) ? EndSecReg - StartSecReg : (60+EndSecReg) - StartSecReg);



      //sleep(TimeLedOn/3); // total turn on time of LED  : TimeLedOn

      sleep(TimeLedOn/3);


      IicStatus = ReadRTC(Iic, SendBuffer, RecvBuffer);
      if (IicStatus != XST_SUCCESS)
      {
         return XST_FAILURE;
      }
      EndSecReg = (RecvBuffer[0] & 0b00001111) + 10*(  (RecvBuffer[0] & 0b01110000 ) >> 4) ;
      xil_printf("    두더지가 등장한지  %d초 지났습니다. \n", (StartSecReg <= EndSecReg) ? EndSecReg - StartSecReg : (60+EndSecReg) - StartSecReg);




      // LED off
      LED = -1; // LED -1 IS OFF
      LED_mWriteReg(XPAR_LED_0_S00_AXI_BASEADDR, LED_S00_AXI_SLV_REG0_OFFSET, 0);  //LED OFF 신호를 PL에 전달
      xil_printf("     두더지가 1초동안 숨습니다.\n\n");
      sleep(1); // LED TimedLedOff초간 꺼짐





      for(wait = 0; wait < 1200; wait++);

     }
     xil_printf("\n");
     xil_printf("두더지 10마리를 모두 잡았습니다.\n");
     xil_printf("게임 종료\n");
     xil_printf("5초 뒤에 새 게임이 시작됩니다.\n");
     xil_printf("\n");
     mode = "GAME END           ";
     sprintf(TlcdReg_upline, mode);
     sprintf(TlcdReg_downline, "Total Score :%d  ", (int)score);
      for(i = 0; i < 4; i++)
      {
        TEXTLCD_mWriteReg(XPAR_TEXTLCD_0_S00_AXI_BASEADDR, i * 4, TlcdReg_upline[i * 4 + 3] + (TlcdReg_upline[i * 4 + 2] << 8) + (TlcdReg_upline[i * 4 + 1] << 16) + (TlcdReg_upline[i * 4] << 24));
        TEXTLCD_mWriteReg(XPAR_TEXTLCD_0_S00_AXI_BASEADDR, i * 4 + 16, TlcdReg_downline[i * 4 + 3] + (TlcdReg_downline[i * 4 + 2] << 8) + (TlcdReg_downline[i * 4 + 1] << 16) + (TlcdReg_downline[i * 4] << 24));
      }
     sleep(5);

   }
}

void WriteTLCDReg(char *pRegVal, int val)
{
   int      i = 0;
   char   temp;

   for(i = 0; i < 8; i++)
   {
      temp = ((val >> ((7 - i) * 4)) & 0x0f) + 0x30;
      pRegVal[i] = temp;
      pRegVal[i+8] = 0x20;
   }
}

int ReadRTC(XIicPs Iic, u8 *SendBuffer, u8 *RecvBuffer)
{
   int            Status;
   XIicPs_Config   *Config;

   Config = XIicPs_LookupConfig(XPAR_XIICPS_0_DEVICE_ID);
   if (Config == NULL)
   {
      return XST_FAILURE;
   }

   Status = XIicPs_CfgInitialize(&Iic, Config, Config->BaseAddress);
   if (Status != XST_SUCCESS)
   {
      return XST_FAILURE;
   }

   XIicPs_SetSClk(&Iic, IIC_SCLK_RATE);

   *SendBuffer      = 0x02;
   RecvBuffer[0]   = 0x00;
   RecvBuffer[1]   = 0x00;
   RecvBuffer[2]   = 0x00;

   Status = XIicPs_MasterSendPolled(&Iic, SendBuffer, 1, IIC_SLAVE_ADDR);
   if(Status != XST_SUCCESS)
   {
      return XST_FAILURE;
   }

   while(XIicPs_BusIsBusy(&Iic))
   {
      /* NOP */
   }

   Status = XIicPs_MasterRecvPolled(&Iic, RecvBuffer, 3, IIC_SLAVE_ADDR);
   if (Status != XST_SUCCESS)
   {
      return XST_FAILURE;
   }

   return XST_SUCCESS;
}

int GicConfigure(u16 DeviceId)
{
   int Status;

   /*
    * Initialize the interrupt controller driver so that it is ready to
    * use.
    */
   GicConfig = XScuGic_LookupConfig(DeviceId);
   if (NULL == GicConfig) {
      return XST_FAILURE;
   }

   Status = XScuGic_CfgInitialize(&InterruptController, GicConfig,
      GicConfig->CpuBaseAddress);
   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

   /*
    * Connect the interrupt controller interrupt handler to the hardware
    * interrupt handling logic in the ARM processor.
    */
   Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
      (Xil_ExceptionHandler)XScuGic_InterruptHandler,
      &InterruptController);

   /*
    * Enable interrupts in the ARM
    */
   Xil_ExceptionEnable();

   /*
    * Connect a device driver handler that will be called when an
    * interrupt for the device occurs, the device driver handler performs
    * the specific interrupt processing for the device
    */
   Status = XScuGic_Connect(&InterruptController, INTC_DEVICE_INT_ID,
      (Xil_ExceptionHandler)ServiceRoutine,
      (void*)&InterruptController);

   if (Status != XST_SUCCESS) {
      return XST_FAILURE;
   }

   /*
    * Enable the interrupt for the device and then cause (simulate) an
    * interrupt so the handlers will be called
    */
   XScuGic_Enable(&InterruptController, INTC_DEVICE_INT_ID);

   return XST_SUCCESS;
}

void ServiceRoutine(void* CallbackRef)
{
   char pb;
   u8 Timeinterval;
   int i;
   pb = PUSHBUTTON_mReadReg(XPAR_PUSHBUTTON_0_S00_AXI_BASEADDR, 0);
   PUSHBUTTON_mWriteReg(XPAR_PUSHBUTTON_0_S00_AXI_BASEADDR, 0, 0);


   //button Number Generator
   int buttonNumber = 0;
   if((pb & 1) == 1) buttonNumber = 1;
   else if((pb & 2) == 2) buttonNumber = 2;
   else if((pb & 4) == 4) buttonNumber = 3;
   else if((pb & 8) == 8) buttonNumber = 4;


   if (pb != 0)
   {
      xil_printf("!!!!!!!!!!\n");
      xil_printf("!!!쾅!!!      %d번 구멍을 내리쳤습니다.\r\n", buttonNumber);
      xil_printf("!!!!!!!!!!\n");

      if (LED == buttonNumber)
      {
          Timeinterval = (StartSecReg <= EndSecReg) ? EndSecReg - StartSecReg : (60+EndSecReg) - StartSecReg; // Time interval from LED ON to button push
         //Timeinterval = EndSecReg - StartSecReg;
           // xil_printf("%d번  Timeinterval = %d \n", buttonNumber, Timeinterval);
            if ((0 <= Timeinterval) && (Timeinterval < TimeLedOn/3))
            {
               xil_printf("완벽한 타이밍! , 3점 획득!. \n\n");
               score = score + 3;
               DeadMole++;
            }
            else if ((TimeLedOn/3 <= Timeinterval) && (Timeinterval < 2*TimeLedOn/3))
            {
               xil_printf("좋아요! , 2점 획득!\n\n");
               score = score + 2;
               DeadMole++;
            }
            else if ( (2*TimeLedOn/3) <= Timeinterval)
            {
               xil_printf("좀 더 빨리!, 1점 획득!\n\n");
               score = score + 1;
               DeadMole++;
            }
            else
            {
               xil_printf("알 수 없음\n\n");
            }
            //LED OFF
            LED = -1;
            LED_mWriteReg(XPAR_LED_0_S00_AXI_BASEADDR, LED_S00_AXI_SLV_REG0_OFFSET, 0);  //LED OFF 신호를 PL에 전달
      }
      else xil_printf("??띠용??      여기엔 두더지가 없었네요....\n\n");
   }
   LiveMole = 10-DeadMole;
   xil_printf("                                        잡은 두더지 수 : %d\n", DeadMole);
   xil_printf("                                        잡을 두더지 수 : %d\n", LiveMole);
   xil_printf("                                        현재 누적 점수 : %d\n", score);
   SEVEN_SEG_mWriteReg(XPAR_SEVEN_SEG_0_S00_AXI_BASEADDR, 0, NumberTransformFor7seg(score,LiveMole));
   sprintf(TlcdReg_upline, mode);
   sprintf(TlcdReg_downline, "Total Score :%d  ", (int)score);
   for(i = 0; i < 4; i++)
   {
      TEXTLCD_mWriteReg(XPAR_TEXTLCD_0_S00_AXI_BASEADDR, i * 4, TlcdReg_upline[i * 4 + 3] + (TlcdReg_upline[i * 4 + 2] << 8) + (TlcdReg_upline[i * 4 + 1] << 16) + (TlcdReg_upline[i * 4] << 24));
      TEXTLCD_mWriteReg(XPAR_TEXTLCD_0_S00_AXI_BASEADDR, i * 4 + 16, TlcdReg_downline[i * 4 + 3] + (TlcdReg_downline[i * 4 + 2] << 8) + (TlcdReg_downline[i * 4 + 1] << 16) + (TlcdReg_downline[i * 4] << 24));
   };
}

void PrintChar(u8 *str)
{
   XUartPs_SendByte(XPAR_PS7_UART_1_BASEADDR, CR);

   while(*str != 0)
   {
      XUartPs_SendByte(XPAR_PS7_UART_1_BASEADDR, *str++);
   }
}

void PrintMsg(u8 *str)
{
   while(*str != 0)
   {
      XUartPs_SendByte(XPAR_PS7_UART_1_BASEADDR, *str++);
   }
}

void GetNumber(u8 *number)
{
   int j = 0;
   char tmp;

   do
   {
      tmp = XUartPs_RecvByte(XPAR_PS7_UART_1_BASEADDR);

      if( (tmp >= '0') && (tmp <= '9') )
      {
         XUartPs_SendByte(XPAR_PS7_UART_1_BASEADDR, tmp);
         number[j] = tmp;
         j++;

      }
   }
   while( !((tmp == CR) || (j >= NUM_MAX-1)) );
   while(tmp != CR)
      tmp = XUartPs_RecvByte(XPAR_PS7_UART_1_BASEADDR);
}

unsigned int power(int base, int n)
{
   int i;
   int output = 1;
   for(i = 0; i < n; i++)
   {
      output = output * base;
   }
   return output;
}

u32 NumberTransformFor7seg(u32 input, u8 remainingMole)
{
   u32 output = 0;
   u32 temp = 0;
   if(input >= 10000) output = 0;
   else
   {
      for(int i = 0; i < 4; i++)
      {
         temp = ( input % power(10, i+1) ) / power(10, i); // i번째 자리수 추출
         output += (temp << i*4); // i+1번째 7seg에 세팅
      }

      for(int i = 0; i < 4; i++)
      {
         temp = ( remainingMole % power(10, i+1) ) / power(10, i);  // i번째 자리수 추출

         output += (temp << i*4 + 16); // i+1번째 7seg에 세팅
      }
   }
   return output;
}
