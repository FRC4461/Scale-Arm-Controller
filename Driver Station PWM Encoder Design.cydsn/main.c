
/******************************************************************************
* Copyright 2010, Cypress Semiconductor Corporation.
* Copyright 2015, Jim DeSilva
*
*
*******************************************************************************

*******************************************************************************
*  Project Name: Robot Controla Adapter
*  Project Revision: 3.0
*  Software Version: PSoC Creator 3.2
*  Device Tested: CY8C5888LTI-LP097
*  Compilers Tested:  ARM GCC
*  This version for PSoC Kit 059  
*****************************************************************************/

/************************************************************************************************************
* Project Description:
* This project will create an advanced USB HID joystick that can be used in PC gaming or other applications
* that support HID controls. 
* This version supports 24 digital inputs (buttons) and 2 analog inputs for analog sensors or controls
* and includes support for an optional LCD display.
* This version is targeted to the CY8C5888LTI-LP097 based Kit059  and is NOT boot loadable.
**************************************************************************************************************/

#include <project.h>
#include <device.h>
#include <stdio.h>
#include <globals.h>
#define X 0
#define Y 1
#define Z 2
#define Btns1 3
#define Btns2 4


void StartUp (void); 	    	/* Function prototype for component startup API */
void ReadJoystick (void);   /* Function prototype for reading and altering data for joystick information */
void ReadButtons (void);    /* Function prototype for reading and altering data for button information */
void LCD_Update (void);     /* Function prototype for updating data on LCD Display  .  */
static int8 X_Axis=0, Y_Axis=0, Z_Axis=0;  // Encoder counter values
static int8 X_Data, Y_Data, Z_Data;       // x y and z data to pass back to PC via USB connection  
static int8  Joystick_Data[5] = {0,0,0,0,0}; //  [0] = X-Axis, [1] = Y-Axis, [2] = Z-Axis , [3] = Buttons1 , [4] = Buttons2  
static uint8 Buttons1, Buttons2 ;    // Button data now split into 2 - 8 bit blocks      
uint16 DisplayCnt=0;
uint16 LoopCnt = 0;
//uint32 LCD_Checksum=0;        // These are used to control display update timing.  Display should only be used during debugging
//uint32 LCD_ChecksumLast=0;    // use of display may cause some minor data glitching due to altered timing.
//uint32 BtnTmp =0 ;

// start new PWM defines
char sprintfBuffer[8];
uint8 CapStatus=0;
char BlankLine[]=     "                    ";     
char NoSignal[]=      " *** No Signal ***  ";
char displayString[] ="                    ";
extern uint16 TimerStartCount;
extern uint16 TimerEndCount;
extern uint16 ElapsedCount;      
extern uint8 CaptureFlag;
extern uint8 SignalFlag;

int8 Axis[3];
// end pwm defines



void main()
{
    StartUp(); 								/* Calls the proper start API for all the components */
    for(;;)
    {   // PWM Code
        CapStatus = isr_Capture_GetState(); 
        if (CapStatus==0)      // enable interupt routine if not already enabled
        {
          isr_Capture_Enable();
        }
        // end PWM code  

        // start HID USB code
        while(!USBFS_1_bGetEPAckState(1)); 	// Wait for ACK before loading data 
        	
        ReadJoystick();						// Calls function to read joystick movement 
        ReadButtons();						// Calls function to monitor button presses 
        Joystick_Data[X] = X_Data;	
        Joystick_Data[Y] = Y_Data;
        Joystick_Data[Z] = Z_Data;
        Joystick_Data[Btns1] = Buttons1 ;
        Joystick_Data[Btns2] = Buttons2 ;
        USBFS_1_LoadInEP(1, (uint8 *)Joystick_Data, 5); // Load latest data into EP1 and send to PC  Load 5 bytes. X,y & Z axis = 8 bits each
                                                        // buttons1, buttons2   = 8 bits each

#ifdef LCD
        if (DisplayCnt>=50)
        {
            LCD_Update();
        }
#endif
        
        DisplayCnt++;
        LoopCnt ++;
    }                                                   
}

void StartUp (void)
{
   
	CyGlobalIntEnable;           					/* Enable Global interrupts */
    CLK_1MHZ_Start();
    Timer_Start();

#ifdef LCD
    LCD_Start();
#endif

    CyDelay(10);
	USBFS_1_Start(0, USBFS_1_DWR_VDDD_OPERATION);	/* Start USBFS operation/device 0 and with 5V operation */
    
    while(!USBFS_1_bGetConfiguration())	;		/* Wait for Device to enumerate */
    
    USBFS_1_LoadInEP(1, (uint8 *)Joystick_Data, 5); /* Loads an inital value into EP1 and sends it out to the PC    */
    
#ifdef LCD
    LCD_Position(0,0);
    LCD_PrintString(" Hid Control Module ");
    LCD_Position(1,0);
    LCD_PrintString("Ver 3.0  for Kit059 ");
    LCD_Position(3,0);
    LCD_PrintString("By Jim DeSilva c2015");
#endif
    
    CyDelay(500);
    Timer_SetCaptureMode(RisingEdge);         //   Set Timerto look for first rising edge.  ISR then controls Capture mode
    isr_Capture_Start();
}

void ReadJoystick (void)
{
    if (ElapsedCount  > 2000)
    {
        ElapsedCount = 2000;
    }
    if (ElapsedCount < 1000)
    {
        ElapsedCount = 1000;
    } 
   
    X_Axis = (ElapsedCount-1500) / 4;           /* Get Encoder value and store in variable Z_Axis */
    Y_Axis = 127;
    Z_Axis = 127;                               /* Get Encoder value and store in variable Z_Axis */
	
  
  
    X_Data = X_Axis;            /* Copy working data to USB Buffers */					
    Y_Data = Y_Axis;            /* Adjust axis to center joystick */         
    Z_Data = Z_Axis;            /* Inverts Y-Axis for PC direction formatting */

}

void ReadButtons (void)               // Detect if button was pressed	
{
  /**********************************   Buttons 0 thru 7   ****************************/
	if(Thumb_Button_Read() != 0)				
	Buttons1 |= 0x01;											// If pressed, mask bit to indicate button press 
	else
	Buttons1 &= ~0x01;										// If released, clear bit 
	 
	if(Button_1_Read() != 0)
	Buttons1 |= 0x02;
	else
	Buttons1 &= ~0x02;
	
	if(Button_2_Read() != 0)
	Buttons1 |= 0x04;
	else
	Buttons1 &= ~0x04;
	
	if(Button_3_Read() != 0)
	Buttons1 |= 0x08;
	else
	Buttons1 &= ~0x08;
	
	if(Button_4_Read() != 0)
	Buttons1 |= 0x10;
	else
	Buttons1 &= ~0x10;
    
  if(Button_5_Read() != 0)
	Buttons1 |= 0x20;
	else
	Buttons1 &= ~0x20;
	
	if(Button_6_Read() != 0)
	Buttons1 |= 0x40;
	else
	Buttons1 &= ~0x40;
	
	if(Button_7_Read() != 0)
	Buttons1 |= 0x80;
	else
	Buttons1 &= ~0x80;
	
  /**********************************   Buttons 8 thru 15   ****************************/
  
  if(Button_8_Read() != 0)
	Buttons2 |= 0x01;
	else
	Buttons2 &= ~0x01;
    
  if(Button_9_Read() != 0)
	Buttons2 |= 0x02;
	else
	Buttons2 &= ~0x02;
	
	if(Button_10_Read() != 0)
	Buttons2 |= 0x04;
	else
	Buttons2 &= ~0x04;
	
	if(Button_11_Read() != 0)
	Buttons2 |= 0x08;
	else
	Buttons2 &= ~0x08;
	
	if(Button_12_Read() != 0)
	Buttons2 |= 0x10;
	else
	Buttons2 &= ~0x10;
    
  if(Button_13_Read() != 0)
	Buttons2 |= 0x20;
	else
	Buttons2 &= ~0x20;
	
	if(Button_14_Read() != 0)
	Buttons2 |= 0x40;
	else
	Buttons2 &= ~0x40;
	
	if(Button_15_Read() != 0)
	Buttons2 |= 0x80;
	else
	Buttons2 &= ~0x80;
  
  
  //Buttons1 = 0x01;          // Button test data
  //Buttons2=  0x01;
  //Buttons3=  0x01;
    
  }   
 
void LCD_Update(void)
{     // BtnTmp is a copy of button data used to decide if LCD needs updating
      //BtnTmp = Buttons1 + (Buttons2 * 256) + (Buttons3 *65536) ; 
  if (SignalFlag==0)     // check to see if input is present. if not present display no signal msg.
      {
      ElapsedCount=0;
      strcpy(displayString, NoSignal );
      }
    else
      {
      strcpy(displayString, BlankLine );  //clear no signal msg.
      }
    LCD_WriteControl(LCD_CLEAR_DISPLAY);   //  Format and display 
    LCD_Position(0,0);
    LCD_PrintString("PWM Pulse Width");
    LCD_PrintDecUint16(CaptureFlag);
    LCD_Position(1,0); 
    LCD_PrintDecUint16(ElapsedCount);
    LCD_Position(1,6);
    LCD_PrintString("Micro Sec");
    LCD_Position(2,0); 
    //LCD_PrintString(displayString);
    LCD_PrintDecUint16(Axis[1]);
    LCD_PrintString("  ");
    LCD_PrintDecUint16(Axis[2]); 
    LCD_Position(3,0); 
    LCD_PrintString("PWM Input on P3.0 ");
    SignalFlag=0;
     
  /*
  BtnTmp = Buttons1 + (Buttons2 >>8) + (Buttons3 >>16) ; 
  if (LCD_Checksum != LCD_ChecksumLast) 
  {
  //LCD_WriteControl(LCD_CLEAR_DISPLAY) ;
  LCD_Position(0,0);
  LCD_PrintInt32(LCD_Checksum);
  LCD_Position(1,0);
  LCD_PrintNumber(Buttons1);
  LCD_Position(1,10);
  LCD_PrintNumber(X_Axis);
  LCD_Position(2,0);
  LCD_PrintNumber(Buttons2);
  LCD_Position(2,10);
  LCD_PrintNumber(Y_Axis);
  LCD_Position(3,0);
  LCD_PrintNumber(Buttons3);
  LCD_ChecksumLast=LCD_Checksum;
  LCD_Position(3,10);
  //LCD_PrintNumber(Loop_Cnt);
  */
  DisplayCnt=0;
 } 
// }  



/* End of File */
