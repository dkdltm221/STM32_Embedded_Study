#include "main.h"
#include <stdio.h>

static void MX_USART2_UART_Init(void);
UART_HandleTypeDef huart2;

int main(void)
{
  // 그래 시발 이걸 이제 레지스터로 해보자 개시발 가보자 rcc 주변 클록 부터 활성화 해줘야한다
  // 보드판에 led를 open? 해볼까나
  RCC->AHB1ENR |= (1UL << 2);
  //  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
  (void)RCC->AHB1ENR;

  // UART 하려고했는데 MODER내가 배우지않은 ALTER거시기 써야한데서 음 이건 추후에 공부해야할듯
  MX_USART2_UART_Init();

  GPIOC->OTYPER &= ~(1UL << 0);
  GPIOC->OSPEEDR &= ~(3UL << 0);
  GPIOC->PUPDR &= ~(3UL << 0);

  GPIOC->BSRR = (0x1UL << 16);

  GPIOC->MODER = (GPIOC->MODER & ~(3UL << 0)) | (1UL << 0);

  //  USART2->CR1 |= 0x3 << 2;
  //  USART2->BRR = 139;
  //
  //  USART2->CR1 |= 0x1 << 13;

  while (1)
  {
    if ((USART2->SR & USART_SR_RXNE) != 0U)
    {
      uint8_t rx = (uint8_t)USART2->DR;
      if (rx == 'S')
      {
        GPIOC->BSRR = 1UL << 0;
      }
      else if (rx == 'E')
      {
        GPIOC->BSRR = 1UL << 16;
      }
      while ((USART2->SR & USART_SR_TXE) == 0U)
        ;
      USART2->DR = rx;
    }
  }

  while (1)
  {
  }
}
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;

  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}
