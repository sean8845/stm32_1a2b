#include "stm32f1xx_hal.h"
#include "main.h"
#include "lcd.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"

#ifdef __GNUC__

#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

UART_HandleTypeDef huart1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);

static GPIO_InitTypeDef  GPIO_InitStruct;

unsigned char String3[]={DS,Du,Dc,Dc,De,Ds,Ds};
unsigned char String1[2]={0};																	//count on LCD
unsigned char String2[11]={DT,Do,Dt,Da,Dl,DC,Do,Du,Dn,Dt,21};
unsigned char	a[4]={0,0,0,0},																	//location
							I[4],O[4]={0,0,0,0},														//I/P(Q) & O/P(computer guess)
							A=0,B=0,																				//_A_B
							count,
							c[4]={0,0,0,0};																	//data location
int i=0,j=0;																									//LCD location
unsigned char b[10]={5,6,7,8,9,0,1,2,3,4};										//initial array
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART1_UART_Init();	
	LCD_Init();
	LCD_Clear();
	
	Input_Error:	
		printf("Please enter four numbers\r\n");									//input
		HAL_UART_Receive(&huart1, (uint8_t *)&I[0], 1, 0xffff);
		printf("%c\n\r",I[0]);
		HAL_UART_Receive(&huart1, (uint8_t *)&I[1], 1, 0xffff);
		printf("%c\n\r",I[1]);
		HAL_UART_Receive(&huart1, (uint8_t *)&I[2], 1, 0xffff);
		printf("%c\n\r",I[2]);
		HAL_UART_Receive(&huart1, (uint8_t *)&I[3], 1, 0xffff);
		printf("%c\n\r",I[3]);
	if(I[0]==I[1]||I[0]==I[2]||I[0]==I[3]||I[1]==I[2]||I[1]==I[3]||I[2]==I[3]){ //input error
		printf("ERROR!!!\n\r");
		goto Input_Error;
	}
	
	I[0]=I[0]-48;
	I[1]=I[1]-48;
	I[2]=I[2]-48;
	I[3]=I[3]-48;
	
  while (1)
  {
		while((a[0]==0||a[1]==0||a[2]==0||a[3]==0)&&(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==1)){ 
			while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==1){}
			
			Output_Error:
       if (a[0] == 0) {																							//computer guess
           c[0]+=3;
           O[0] = b[c[0] % 10];
       }
       if (a[1] == 0) {
           c[1]+=3;
           O[1] = b[c[1] % 10];
           while (O[1] == O[0] ) {
               c[1]+=3;
               O[1] = b[c[1] % 10];
           }
       }
       if (a[2] !=1) {
           c[2] += 3;
           O[2] = b[c[2] % 10];
           while (O[2] == O[1] || O[2] == O[0] ) {
               c[2]+=3;
               O[2] = b[c[2] % 10];
           }
       }
       if (a[3] != 1) {
           c[3]+=3;
           O[3] = b[c[3] % 10];
           while (O[3] == O[1] || O[3] == O[0] || O[3] == O[2]) {
               c[3]+=3;
               O[3] = b[c[3] % 10];
           }
       }
      if (O[0] == O[1] || O[0] == O[2] || O[0] == O[3] || O[1] == O[2] || O[1] == O[3] || O[2] == O[3]) goto Output_Error; //output error
							
			printf("Guest:%d%d%d%d\n\r",O[0],O[1],O[2],O[3]);	//output on UART
					
			if(I[0]==O[0]){																		//_A_B
							a[0]=1;
							A=A+1;
					}
			if(O[0]==I[1]||O[0]==I[2]||O[0]==I[3]){
					B+=1;
			}    
			if(I[1]==O[1]){
					a[1]=1;
					A=A+1;
			}
			if(O[1]==I[0]||O[1]==I[2]||O[1]==I[3]){
					B+=1;
			}
			if(I[2]==O[2]){
					a[2]=1;
					A=A+1;
			}
			if(O[2]==I[0]||O[2]==I[1]||O[2]==I[3]){
					B+=1;
			}
			if(I[3]==O[3]){
					a[3]=1;
					A=A+1;
			}
			if(O[3]==I[0]||O[3]==I[2]||O[3]==I[1]){
					B+=1;
			}
			count++;
			printf("%dA%dB\r\n",A,B);
			
			String1[0]=count/10;
			String1[1]=count%10;
			
			if(A==4){					
				i=80;	
				printf("Total count:%d\n\rSuccess\n\r",count);								//UART 
				while(1){			
					LCD_Clear();
					LCD_DrawString(6 , i-15 , String3,sizeof(String3));			//sucess light
					LCD_DrawString(4 , i-25 , String3,sizeof(String3));
					LCD_DrawString(2 , i-35 , String3,sizeof(String3));
					LCD_DrawString(0, 0, String2, sizeof(String2));
					LCD_DrawString(0, 88, String1, sizeof(String1));
					HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);						//LED light
					HAL_Delay(60);
					HAL_GPIO_TogglePin(LED2_GPIO_PORT, LED2_PIN);  
					HAL_Delay(60);
					HAL_GPIO_TogglePin(LED3_GPIO_PORT, LED3_PIN);
					HAL_Delay(60);
					HAL_GPIO_TogglePin(LED4_GPIO_PORT, LED4_PIN);
					HAL_Delay(60);
					i-=5;	
					if(i<0) i=80;
				}				
			}
			A=0;
			B=0;
		}
	}
}
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
	
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);

  return ch;
}


/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

}

/* USART1 init function */
void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart1);
}

/** Pinout Configuration
*/
void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __GPIOA_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();
	LED1_GPIO_CLK_ENABLE();
	LED2_GPIO_CLK_ENABLE();
  LED3_GPIO_CLK_ENABLE();
  LED4_GPIO_CLK_ENABLE();
	
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

  GPIO_InitStruct.Pin = LED1_PIN;
  HAL_GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LED2_PIN;
  HAL_GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LED3_PIN;
  HAL_GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LED4_PIN;
  HAL_GPIO_Init(LED4_GPIO_PORT, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
