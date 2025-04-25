#ifndef MENU_H
#define MENU_H

#ifdef __cplusplus
extern "C" {
#endif

#define OP_UP 1
#define OP_DOWN 2
#define OP_EXIT 3
#define OP_ENTER 4
#define OP_NONE 0


#define CH_N 20
// Include necessary headers
#include "main.h"
#include  "lcd.h"
#include "ec_button.h"
#include "adf4351.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#define MENU_BC BLACK
// Function prototypes
void Menu_Init(void);
void Menu_Update(uint8_t op);
void Show_table(uint8_t num);
//void py_f2s4printf(char * stra, float x, uint8_t flen);

#ifdef __cplusplus
}
#endif

#endif // MENU_H