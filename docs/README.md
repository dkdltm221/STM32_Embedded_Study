# STM32 학습 문서

NUCLEO-F411RE 실습에서 정리한 개념, 레지스터, HAL 코드와 확인 결과를 모아 둡니다.

| 주제 | 문서 | 다루는 내용 |
| --- | --- | --- |
| 클록 | [RCC와 PLL](clock/clock.md) | HSI·HSE, SYSCLK과 버스 분주, F411 클록 계산 |
| GPIO 출력 | [LED 제어](gpio/gpio_led.md) | LD2 점멸, LED 8개, MODER·ODR·BSRR |
| GPIO 입력 | [스위치 입력](gpio/gpio_switch.md) | 디지털 입력, 풀업·풀다운, IDR과 폴링 |
| 인터럽트 | [EXTI와 NVIC](gpio/Interrupt.md) | GPIO 입력 라우팅, Pending, 벡터 테이블, ISR |
| EXTI 실습 | [스위치로 LED 4개 제어](gpio/LED_switch_interrupt.md) | PB0·PB1 하강 에지, 레지스터와 CMSIS 설정 |
| ADC | [ADC의 동작과 설정](gpio/ADC.md) | 샘플링, 분해능, SAR, 변환 모드 |
| UART | [USART2 통신과 에코](uart/uart.md) | UART 프레임, 오버샘플링·BRR, 인터럽트 수신 |

실습 프로젝트를 가져오는 방법은 [저장소 README](../README.md#stm32cubeide에서-열기), 오류 기록 양식은 [문제 해결 문서](../troubleshooting/README.md)에서 확인합니다.
