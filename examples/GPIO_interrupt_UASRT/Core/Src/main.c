#include "stm32f4xx.h"
#include <stddef.h>

static const char messageHIG[] = "HIGH\n\r";
static const char messageLow[] = "LOW\n\r";
static const char *volatile rx;
#define DEBOUNCE_MS 500U

volatile uint32_t exti0_count = 0;
volatile uint32_t exti0_valid_count = 0;
volatile uint32_t exti1_count = 0;
volatile uint32_t exti1_valid_count = 0;
static uint32_t last_exti0 = 0;
static uint32_t last_exti1 = 0;
static uint8_t exti0_initialized = 0;
static uint8_t exti1_initialized = 0;
volatile uint32_t tick_ms = 0;

void SysTick_Init(void)
{
  SysTick->CTRL = 0;

  SysTick->LOAD = 16000 - 1;
  SysTick->VAL = 0;

  SysTick->CTRL = 0x7UL;
}

// 본 프로젝트는 내가 아는 범위에서 레지스터 제어를 해보고자한다.
void EXTI0_IRQHandler(void)
{
  if (EXTI->PR & (1UL << 0))
  {
    EXTI->PR = (1UL << 0);
    exti0_count++;
    uint32_t now = tick_ms;

    if (exti0_initialized &&
        (now - last_exti0) < DEBOUNCE_MS)
    {
      return;
    }

    exti0_initialized = 1;
    last_exti0 = now;
    exti0_valid_count++;
    GPIOC->BSRR = 0x1UL;
    rx = messageHIG;
  }
}
void EXTI1_IRQHandler(void)
{
  if (EXTI->PR & (1UL << 1))
  {
    EXTI->PR = (1UL << 1);

    exti1_count++;

    uint32_t now = tick_ms;
    if (exti1_initialized && (now - last_exti1) < DEBOUNCE_MS)
    {
      return;
    }
    exti1_initialized = 1;
    last_exti1 = now;
    exti1_valid_count++;
    GPIOC->BSRR = (0x1UL << 16);
    rx = messageLow;
  }
}
void USART2_Init(void)
{
  // 1. RCC 클록 활성화
  RCC->AHB1ENR |= (1UL << 0);  // GPIOA
  RCC->APB1ENR |= (1UL << 17); // USART2

  GPIOA->MODER &= ~(3UL << 4);
  GPIOA->MODER |= (2UL << 4);

  GPIOA->MODER &= ~(3UL << 6);
  GPIOA->MODER |= (2UL << 6);

  // 해당 부분은 추후에 공부가 필요함
  GPIOA->AFR[0] &= ~(0xFUL << 8);
  GPIOA->AFR[0] |= (7UL << 8);

  GPIOA->AFR[0] &= ~(0xFUL << 12);
  GPIOA->AFR[0] |= (7UL << 12);

  USART2->CR1 &= ~(1UL << 13); // USART 비활성화
  USART2->CR1 &= ~(1UL << 15); // Oversampling 16배

  USART2->BRR = 139; // PCLK1 16MHz, Baud Rate 115200

  USART2->CR1 |= (3UL << 2);  // TE, RE 활성화
  USART2->CR1 |= (1UL << 13); // USART 활성화
}
void GPIO_EXTI_Init(void)
{
  // 1. PC0의 클록 활성화
  RCC->AHB1ENR |= (1UL << 1) | (1UL << 2);

  (void)RCC->AHB1ENR;

  // 4. PB0: 입력 + Pull-up
  GPIOB->MODER &= ~0xFUL;
  GPIOB->PUPDR = (GPIOB->PUPDR & ~0xFUL) | 0x5UL;

  // 5. PC0: Push-Pull, Low speed, No Pull
  GPIOC->MODER = (GPIOC->MODER & ~(3UL << 0)) | (1UL << 0);
  GPIOC->OTYPER &= ~(1UL << 0);
  GPIOC->OSPEEDR &= ~(3UL << 0);
  GPIOC->PUPDR &= ~(3UL << 0);

  // LED 초기 상태 OFF
  GPIOC->BSRR = (1UL << 16);

  // 6. PB0 → EXTI0 설정
  SYSCFG->EXTICR[0] =
      (SYSCFG->EXTICR[0] & ~(0xFUL << 0)) | (0x1UL << 0);
  SYSCFG->EXTICR[0] =
      (SYSCFG->EXTICR[0] & ~(0xFUL << 4)) | (0x1UL << 4);
  // 스위치 설정
  EXTI->IMR &= ~((1UL << 0) | (1UL << 1));
  EXTI->RTSR &= ~((1UL << 0) | (1UL << 1));
  EXTI->FTSR |= (1UL << 0) | (1UL << 1);
}

int main(void)
{

  //  시스템 클록  활성화
  RCC->APB2ENR |= (1UL << 14);
  (void)RCC->APB2ENR;

  //  PC0의 클록 활성화
  GPIO_EXTI_Init();
  USART2_Init();
  SysTick_Init();
  // 펜딩 해제
  EXTI->PR = (1UL << 0) | (1UL << 1);
  // 우선순위 설정
  NVIC_SetPriority(EXTI0_IRQn, 5U);
  NVIC_SetPriority(EXTI1_IRQn, 6U);

  // EXTI 요청 허용
  EXTI->IMR |= (1UL << 0) | (1UL << 1);

  // NVIC 활성화
  NVIC_EnableIRQ(EXTI0_IRQn);
  NVIC_EnableIRQ(EXTI1_IRQn);

  while (1)
  {
    if (rx == NULL)
    {
      continue;
    }

    const char *message_ptr = rx;
    rx = NULL;

    while (*message_ptr != '\0')
    {
      if (USART2->SR & USART_SR_TXE)
      {
        USART2->DR = (uint8_t)*message_ptr;
        message_ptr++;
      }
    }
  }
}
