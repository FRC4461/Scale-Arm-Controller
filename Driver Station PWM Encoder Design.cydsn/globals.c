/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include <project.h>
#include <globals.h>
#include <string.h>
#include <STDIO.h>


// declare variables here
uint16 TimerStartCount;
uint16 TimerEndCount;
uint16 ElapsedCount;
uint8 CaptureFlag;
uint8 SignalFlag;

/**************************************************************************************************/
void Beep(void)
{
    // need to assign port for Buzzer
    //Buzzer_Write(0);
    //CyDelay(75);
    //Buzzer_Write(1);
}
/***************************************************************************************************/
void SelfTest(void)
{
#ifdef LCD
    LCD_ClearDisplay();
    LCD_PrintString("Property of");
    LCD_Position(1,0);
    LCD_PrintString(" Jim DeSilva ");
    LCD_Position(2,0);
    LCD_PrintString("(206) 248-7268");
    CyDelay(2500);
    LCD_ClearDisplay();
    LCD_PrintString("  ***Self Test ***");
    LCD_Position(1,0);
    LCD_PrintString("     Please Wait ");
    CyDelay(750);
    LCD_PrintString("     Please Wait ");
    CyDelay(750);
#endif
    return;
}
/******End SelfTest()   ***********************************************************************/

void Ver(void)
{
#ifdef LCD
    LCD_ClearDisplay();
    LCD_Position(0,5); 
    LCD_PrintString("Skunkworks");
    LCD_Position(1,3);
    LCD_PrintString("Lidar Lite Range Finder");
    LCD_Position(2,1);
    LCD_PrintString("PWM Mode");      
#endif
}
/********  End   Ver()  ***********************************************************************/

 

void PrintFloatNumber(float NumberToPrint, char Line, char space)
{
#ifdef LCD
    char DecimalStr[16];
    sprintf(DecimalStr, "%.3f", NumberToPrint);      
    LCD_Position(Line,space);
    LCD_PrintString(DecimalStr);
#endif
}
/******** End PrintDecNumber()  ***************************************************************/
 






/* [] END OF FILE */
