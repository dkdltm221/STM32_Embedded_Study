
#include "stm32f4xx.h"


#define VREF 3.3f // A3.3 V 기준 전압
#define ADC_RES 4096.0f //12-bit ADC최대값
volatile uint16_t adc_raw = 0; //ADC 원자값
volatile float throttle_v = 0.0f; //전압 환산값

int main(void)
{
    // 1. RCC 클록 활성화
    RCC->AHB1ENR |= (1UL << 1); //gpio1번
    RCC->APB2ENR |= (1UL << 14);

    (void)RCC->AHB1ENR;
    (void)RCC->APB2ENR;

    // 2. PB0, PB1: 입력 + Pull-up
    GPIOB->MODER &= ~0xFUL;
    GPIOB->PUPDR = (GPIOB->PUPDR & ~0xFUL) | 0x5UL;

    // 3. PC0~PC3: Push-Pull, Low speed, No Pull
    GPIOC->OTYPER &= ~0xFUL;
    GPIOC->OSPEEDR &= ~0xFFUL;
    GPIOC->PUPDR &= ~0xFFUL;

    // LED 초기 상태 OFF
    GPIOC->BSRR = (0xFUL << 16);

    // PC0~PC3 출력 모드
    GPIOC->MODER =
        (GPIOC->MODER & ~0xFFUL) | 0x55UL;

    // 4. PB0 → EXTI0, PB1 → EXTI1
    SYSCFG->EXTICR[0] =
        (SYSCFG->EXTICR[0] & ~(0xFUL << 0))
        | (0x1UL << 0);

    SYSCFG->EXTICR[0] =
        (SYSCFG->EXTICR[0] & ~(0xFUL << 4))
        | (0x1UL << 4);


    // 7. 인터럽트 대기
    while (1)
    {

    }
}
