#include  "menu.h"


extern struct button_obj_t ecbt;
static uint8_t selflag=0,menu_id=0;



void Menu_Init(void){
    // Initialize the menu system here
    // For example, you can initialize the LCD or other peripherals
    LCD_Fill(0, 0, LCD_W, LCD_H, MENU_BC); // Fill the LCD with black color

    // Display menu items on the LCD
    // LCD_ShowString(24, 0, (uint8_t*)"ADF4351_PLL_Menu", GREEN, MENU_BC, 24, 0);
    // // LCD_ShowString(0, 24, (uint8_t*)"1.", CYAN, BLACK, 24, 0);
    // // LCD_ShowString(0, 48, (uint8_t*)"2.", CYAN, BLACK, 24, 0);
    // // LCD_ShowString(0, 72, (uint8_t*)"3.", CYAN, BLACK, 24, 0);
    // // LCD_ShowString(0, 96, (uint8_t*)"4.", CYAN, BLACK, 24, 0);
    LCD_ShowString(0, 120, (uint8_t*)"Powered by Turtle@AutoC4M", WHITE, MENU_BC, 12, 0);
}
char text[20] = {0},mychar[10] = {0};
void Menu_Update(uint8_t op){
    // Update the menu based on the operation (op) received
    // For example, you can change the selected item or perform actions based on the operation
    switch(op){
        case OP_UP:
            // Handle up operation
            selflag = (selflag+1)%4; // Cycle through the menu items
            break;
        case OP_DOWN:
            // Handle down operation
            selflag = (selflag-1)%4; // Cycle through the menu items
            break;
        case OP_ENTER:
            // Handle enter operation
            if(menu_id==0){ // Main menu
                if(selflag==0) menu_id=1; // Go to PLL settings
                else if(selflag==1) menu_id=2; // Go to system settings
                else if(selflag==2) menu_id=3; // Go to device information
                else if(selflag==3) menu_id=4; // Reserved
            }else if(menu_id==1){ // PLL settings menu
                if(selflag==0) { /* Set Frequency */ 
                    //ADF4351_SetFrequency_RF(2400000000, 200000, &myADF4351,&myADF4351_settings); // Set frequency to 2.4GHz
                }
                else if(selflag==1) { /* Set Power Level */ 
                
                }
                else if(selflag==2) { /* Set Bandwidth */ }
                else if(selflag==3) { /* Set Phase Offset */ }
            }
            else if(menu_id==2){ // System settings menu
                if(selflag==0) { /* Set LCD Contrast */ }
                else if(selflag==1) { /* Set Backlight */ }
                else if(selflag==2) { /* Set Language */ }
                else if(selflag==3) { /* Reserved */ }
            }
            else if(menu_id==3){ // Device information menu
                if(selflag==0) { /* Device Name */ }
                else if(selflag==1) { /* Firmware Version */ }
                else if(selflag==2) { /* Hardware Version */ }
                else if(selflag==3) { /* Reserved */ }
            }
            else if(menu_id==4){ // Reserved menu
                // Handle reserved menu actions here
            }
            else if(menu_id==255){ // Device state
                menu_id = 0; // Go back to main menu
                selflag = 0; // Reset the selected item
            }
            break;
        case OP_EXIT:
            // Handle exit operation
            if(menu_id==0) { menu_id=255; /* Exit the menu */ }
            else if(menu_id==1) { menu_id=0; } // Go back to main menu
            else if(menu_id==2) { menu_id=0; } // Go back to main menu
            else if(menu_id==3) { menu_id=0; } // Go back to main menu
            else if(menu_id==4) { menu_id=0; } // Go back to main menu
            break;
        default:
            break;
    }
    if(selflag==255) selflag=3; // Ensure selflag is within bounds
    if(menu_id!=255){
        Show_table(menu_id); // Show the main menu on the LCD
    }
    else{
        /****Out of the menu, Show device state****/
        LCD_Fill(0, 24, LCD_W, 119, BLACK); // Fill the LCD with white color
        LCD_ShowString(0, 0 , (uint8_t*)"    ADF4351_PLL_   ", GREEN, MENU_BC, 24, 0);
        
        sprintf(text,"%dMHz",(int)(myADF4351.outfrq_div/1000000.0f));
        LCD_ShowString(0, 24, (uint8_t*)"Freq:", WHITE, MENU_BC, 32, 0);
        LCD_ShowString(80, 24, (uint8_t*)text, YELLOW, MENU_BC, 32, 0); //Show fundamental frequency

        sprintf(text,"%dMHz",(int)(myADF4351.outfrq_div/1000000.0f));
        LCD_ShowString(0, 56, (uint8_t*)"Div:", WHITE, MENU_BC, 32, 0);
        LCD_ShowString(80, 56, (uint8_t*)text, YELLOW, MENU_BC, 32, 0); //Show fundamental frequency
        
        if(myADF4351_settings.aux_output_enable){
            if(myADF4351_settings.aux_output_mode){ //Show Aux Out Mode
                sprintf(text,"Aux Out: Div");
                LCD_ShowString(0, 88, (uint8_t*)text, CYAN, MENU_BC, 24, 0); //Show Aux frequency
            }else {
                sprintf(text,"Aux Out: Fund");
                LCD_ShowString(0, 88, (uint8_t*)text, CYAN, MENU_BC, 24, 0); //Show divided frequency
            }
        }else {
            sprintf(text,"Aux OFF");
            LCD_ShowString(0, 88, (uint8_t*)text, CYAN, MENU_BC, 24, 0); //Show divided frequency
        }
    }

}

uint8_t Menu1(void){
    //Menu_Update();
    while(1){

    }
}



char Main_table[][CH_N] = {
    "  ADF4351_PLL_Menu ",
    "1. Output settings ",  
    "2. System settings ",
    "3. Dev Information ",
    "4. [Reserved]      ",
};

char PLL_table[][CH_N] = {
    "  Output_Settings  ",
    "1.VCO_Freq      MHz",
    "2.RFOutDiv         ",
    "3.ChA_Pow       dBm",
    "4.ChB_Pow       dBm",
    // "5. Set R-Divider   ",
    // "6. Set F-Divider   ",
    // "7. Set ChrgPmpCurr ",
    // //"8. Set Loop Filter Resistor",
    // //"9. Set Loop Filter Capacitor",
    // "8. Set VCO Divider ",
};

char PLL_table2[][CH_N] = {
    "  Output_Settings  ",
    "5. RFPowerDown     ",
    "6. AuxPowerDown    ",
    "7. ChB_OutMode     ",
    "8. NoiseMode",
    // "5. Set R-Divider   ",
    // "6. Set F-Divider   ",
    // "7. Set ChrgPmpCurr ",
    // //"8. Set Loop Filter Resistor",
    // //"9. Set Loop Filter Capacitor",
    // "8. Set VCO Divider ",
};

char Sys_table[][CH_N] = {
    "  System Settings  ",
    "1. Set LCD Contrast",
    "2. Set Backlight   ",
    "3. [Reserved]      ",
    "4. [Reserved]      ",
};
char Dev_table[][CH_N] = {
    " Device Information",
    "1.ADF4351-PLLModule",
    "2. STM32G030K8T6   ",
    "3. Firmware V0.1   ",
    "4. Hardware V1.0   ",
};

void Show_table(uint8_t num){
    char table[5][CH_N];
    switch(num){
        case 0:memcpy(table, Main_table, sizeof(table));break;
        case 1:memcpy(table, PLL_table, sizeof(table));break;
        case 2:memcpy(table, Sys_table, sizeof(table));break;
        case 3:memcpy(table, Dev_table, sizeof(table));break;
    }
    LCD_ShowString(0, 0, table[0], GREEN, MENU_BC, 24, 0);
    for(uint8_t  i = 1; i < 5; i++){
        if(i==selflag+1){
            LCD_ShowString(0, (i)*24, table[i], YELLOW, MENU_BC, 24, 0);
        }else{
            LCD_ShowString(0, (i)*24, table[i], CYAN, MENU_BC, 24, 0);
        }
    }   
}

void edit_table_PLL();

// void py_f2s4printf(char * stra, float x, uint8_t flen)
// {
// 	uint32_t base;
// 	int64_t dn;
// 	char mc[32];

// 	base = pow(10,flen);
// 	dn = x*base;
// 	sprintf(stra, "%d.", (int)(dn/base));
// 	dn = abs(dn);
// 	if(dn%base==0)
// 	{
// 		for(uint8_t j=1;j<=flen;j++)
// 		{
// 			stra = strcat(stra, "0");
// 		}
// 		return;
// 	}
// 	else
// 	{
// 		if(flen==1){
// 			sprintf(mc, "%d", (int)(dn%base));
// 			stra = strcat(stra, mc);
// 			return;
// 		}

// 		for(uint8_t j=1;j<flen;j++)
// 		{
// 			if((dn%base)<pow(10,j))
// 			{
// 				for(uint8_t k=1;k<=(flen-j);k++)
// 				{
// 					stra = strcat(stra, "0");
// 				}
// 				sprintf(mc, "%d", (int)(dn%base));
// 				stra = strcat(stra, mc);
// 				return;
// 			}
// 		}
// 		sprintf(mc, "%d", (int)(dn%base));
// 		stra = strcat(stra, mc);
// 		return;
// 	}
// }
// // ————————————————

// //                             版权声明：本文为博主原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接和本声明。
                        
// // 原文链接：https://blog.csdn.net/hwytree/article/details/126119245
