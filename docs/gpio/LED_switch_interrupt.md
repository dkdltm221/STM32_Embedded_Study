![스위치 인터럽트 실습 사진](../../assets/switch_01.jpg)


# STM32 레지스터 기반 EXTI 인터럽트 LED 제어

**학습일:** 2026.11.19  
**개발 보드:** NUCLEO-F411RE (STM32F411RET6)  
**개발 환경:** STM32CubeIDE / C / CMSIS  
**진행 상태:** 코드 작성 완료, 빌드 및 실제 보드 동작 검증 예정
switch_01
## 1. 프로젝트 목적

기존에는 HAL 함수를 통해 GPIO를 제어했지만, 이번에는 레퍼런스 매뉴얼 RM0383을 참고하여 GPIO 및 EXTI 레지스터를 직접 설정했다.

단순히 버튼을 누르면 LED가 켜지는 기능을 구현하는 것이 아니라, 외부 입력이 인터럽트로 변환되어 CPU에 전달되고 ISR이 실행되는 전체 과정을 이해하는 것이 목표였다.

## 2. 프로젝트 구성

| 구성 요소 | 핀 | 역할 |
|---|---|---|
| 스위치 1 | PB0 | EXTI0 → LED 전체 ON |
| 스위치 2 | PB1 | EXTI1 → LED 전체 OFF |
| LED 4개 | PC0~PC3 | GPIO 출력 |

- 스위치는 GND에 연결하고 내부 Pull-up 사용
- 평소 HIGH, 누르면 LOW
- HIGH → LOW 변화를 감지하는 Falling Edge 방식
- LED는 각 GPIO 출력에 1kΩ 저항을 직렬 연결하고 GND로 연결

## 3. GPIO 초기화

**RCC**

- `RCC_AHB1ENR`: GPIOB, GPIOC 클록 활성화
- `RCC_APB2ENR`: SYSCFG 클록 활성화
- 클록 활성화는 포트의 주변장치 클록 공급을 허용하는 것이며 PLL 주파수 설정과는 별개다.

**GPIOB — 입력**

- `MODER`: PB0, PB1을 입력 모드 `00`으로 설정
- `PUPDR`: 내부 Pull-up `01`로 설정
- 버튼을 누르지 않았을 때 입력이 Floating 상태가 되는 것을 방지

**GPIOC — 출력**

- `MODER`: PC0~PC3을 출력 모드 `01`로 설정
- `OTYPER`: Push-Pull
- `OSPEEDR`: Low speed
- `PUPDR`: No Pull
- `BSRR`: LED 출력 HIGH/LOW 제어

`MODER`에서 핀 하나당 2비트를 사용하므로 PC0~PC3의 출력 설정 값은 `0x55`가 된다.

## 4. SYSCFG와 EXTI 연결

GPIO 입력을 외부 인터럽트로 사용하려면 `SYSCFG_EXTICR`에서 해당 EXTI 라인에 연결할 GPIO 포트를 선택해야 한다.

```c
SYSCFG->EXTICR[0] =
    (SYSCFG->EXTICR[0] & ~(0xFUL << 0))
    | (0x1UL << 0);

SYSCFG->EXTICR[0] =
    (SYSCFG->EXTICR[0] & ~(0xFUL << 4))
    | (0x1UL << 4);
```

- `EXTICR[0]`: EXTI0~EXTI3 포트 선택
- EXTI0: bit [3:0]
- EXTI1: bit [7:4]
- GPIO Port B의 선택 값은 `0001`

**학습 중 헷갈렸던 부분**

`EXTICR[1]`은 EXTI1을 의미하는 것이 아니라 두 번째 EXTICR 레지스터를 의미한다. 따라서 EXTI1 설정도 `EXTICR[0]`에서 수행해야 한다.

## 5. EXTI 주요 레지스터

| 레지스터 | 역할 |
|---|---|
| `FTSR` | Falling Trigger Selection Register — HIGH → LOW 감지 |
| `RTSR` | Rising Trigger Selection Register — LOW → HIGH 감지 |
| `IMR` | Interrupt Mask Register — 인터럽트 요청 허용·차단 |
| `PR` | Pending Register — 발생한 인터럽트 요청의 상태 확인·해제 |

**설정 내용**

```c
EXTI->IMR &= ~((1UL << 0) | (1UL << 1));

EXTI->RTSR &= ~((1UL << 0) | (1UL << 1));
EXTI->FTSR |= (1UL << 0) | (1UL << 1);

EXTI->PR = (1UL << 0) | (1UL << 1);

EXTI->IMR |= (1UL << 0) | (1UL << 1);
```

초기화 중에는 IMR로 인터럽트 요청을 차단하고, 상승 에지를 비활성화한 다음 하강 에지를 활성화한다. 기존 Pending 플래그를 해제하고 요청을 허용한다.

**주의할 점**

- `FTSR`, `RTSR`, `IMR`은 EXTI 라인 하나당 1비트를 사용한다.
- `EXTICR`은 EXTI 라인 하나당 포트 선택에 4비트를 사용한다.
- `PR`은 W1C(Write 1 to Clear)이므로 1을 쓰면 해당 Pending 비트가 해제된다.
- `~(A | B)`와 `~A | B`는 서로 다르다. 여러 비트를 동시에 지우려면 괄호 범위에 주의해야 한다.

## 6. NVIC 우선순위 설정

```c
NVIC_SetPriority(EXTI0_IRQn, 5U);
NVIC_SetPriority(EXTI1_IRQn, 6U);

NVIC_EnableIRQ(EXTI0_IRQn);
NVIC_EnableIRQ(EXTI1_IRQn);
```

NVIC는 인터럽트 채널을 활성화하고 우선순위를 관리한다.

- EXTI0 우선순위: 5
- EXTI1 우선순위: 6
- 우선순위 숫자가 작을수록 높은 우선순위
- `NVIC_SetPriority()`와 `NVIC_EnableIRQ()`는 HAL이 아닌 CMSIS 함수

## 7. ISR과 BSRR 출력 제어

```c
// EXTI0 Pending 확인 및 해제
if (EXTI->PR & (1UL << 0))
{
    EXTI->PR = (1UL << 0);
    GPIOC->BSRR = 0xFUL;
}
```

인터럽트가 발생하면 CPU는 해당 인터럽트 서비스 루틴(ISR)에 진입한다.

ISR에서는 Pending 상태를 확인하고 해제한 후 LED를 제어한다.

**BSRR 구조**

- 하위 16비트: 해당 GPIO 출력 SET
- 상위 16비트: 해당 GPIO 출력 RESET
- `0xF`: PC0~PC3 HIGH
- `0xF << 16`: PC0~PC3 LOW

BSRR를 사용하면 ODR를 읽고 수정한 뒤 다시 쓰는 과정 없이 지정한 출력 비트를 변경할 수 있다.

## 8. 학습 회고

이번 실습에서는 이전에 배웠던 메모리 맵, 레지스터 오프셋, 비트 마스크, 시프트 연산을 실제 GPIO와 EXTI 초기화에 적용했다.

특히 다음 내용을 이해할 수 있었다.

1. 주변장치의 클록 활성화와 핀 모드 설정은 서로 다른 단계다.
2. 입력 핀을 EXTI에 연결하려면 SYSCFG 설정이 필요하다.
3. EXTI는 신호 변화를 감지하고, NVIC는 인터럽트 우선순위 및 CPU 전달을 관리한다.
4. ISR은 인터럽트 발생 이후 실제 작업을 수행하는 함수다.
5. HAL 없이도 레지스터와 CMSIS를 이용해 인터럽트 처리 경로를 구성할 수 있다.

기존에는 HAL 함수가 인터럽트를 간단하게 처리한다고 생각했지만, 실제로는 여러 레지스터의 설정과 하드웨어 동작이 연결된다는 것을 이해했다.

