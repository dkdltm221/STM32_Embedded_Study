
#include "stm32f4xx.h"

// PB0: LED 4개 ON
void EXTI0_IRQHandler(void)
{
    if (EXTI->PR & (1UL << 0))
    {
        EXTI->PR = (1UL << 0);
        GPIOC->BSRR = 0xFUL;
    }
}

// PB1: LED 4개 OFF
void EXTI1_IRQHandler(void)
{
    if (EXTI->PR & (1UL << 1))
    {
        EXTI->PR = (1UL << 1);
        GPIOC->BSRR = (0xFUL << 16);
    }
}

int main(void)
{
    // 1. RCC 클록 활성화
    RCC->AHB1ENR |= (1UL << 1) | (1UL << 2);
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
        (SYSCFG->EXTICR[0] & ~(0xFUL << 0)) | (0x1UL << 0);

    SYSCFG->EXTICR[0] =
        (SYSCFG->EXTICR[0] & ~(0xFUL << 4)) | (0x1UL << 4);

    // 5. EXTI 초기화
    EXTI->IMR &= ~((1UL << 0) | (1UL << 1));
    EXTI->RTSR &= ~((1UL << 0) | (1UL << 1));
    EXTI->FTSR |= (1UL << 0) | (1UL << 1);

    // 기존 Pending 해제
    EXTI->PR = (1UL << 0) | (1UL << 1);

    // 6. NVIC 우선순위
    NVIC_SetPriority(EXTI0_IRQn, 5U);
    NVIC_SetPriority(EXTI1_IRQn, 6U);

    // EXTI 요청 허용
    EXTI->IMR |= (1UL << 0) | (1UL << 1);

    // NVIC 활성화
    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);

    // 7. 인터럽트 대기
    while (1)
    {
    }
}
