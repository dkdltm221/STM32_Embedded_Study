# STM32 레지스터 기반 EXTI LED 제어 실습

2026-11-19 · NUCLEO-F411RE · STM32CubeIDE / C / CMSIS

## 1. 실습 목표

RM0383을 참고해 GPIO와 EXTI 레지스터를 직접 설정한다. 스위치 입력이 SYSCFG와 EXTI를 거쳐 NVIC에 전달된 뒤 ISR이 LED를 켜거나 끄는 과정을 확인한다. 각 블록의 역할은 [EXTI와 NVIC 개념 정리](Interrupt.md)에서 설명한다.

## 2. 프로젝트 구성

| 구성 요소 | 핀 | 역할 |
| --- | --- | --- |
| 스위치 1 | PB0 | EXTI0 → LED 전체 ON |
| 스위치 2 | PB1 | EXTI1 → LED 전체 OFF |
| LED 4개 | PC0~PC3 | GPIO 출력 |

스위치는 GND와 연결하고 내부 풀업을 사용한다. 평소 HIGH에서 스위치를 누르면 LOW로 바뀌며, EXTI는 이 하강 에지를 감지한다. LED는 각 GPIO 출력에 1 kΩ 저항을 직렬로 연결한 뒤 GND로 연결했다.

![외부 스위치와 LED를 연결한 실습 사진](../../assets/switch_01.jpg)

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

배열의 `EXTICR[1]`은 두 번째 EXTICR 레지스터로 EXTI4~EXTI7을 담당한다. EXTI1은 첫 번째 레지스터인 `EXTICR[0]`의 두 번째 필드에 설정한다.

## 5. EXTI 주요 레지스터

| 레지스터 | 역할 |
| --- | --- |
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
```

초기화 중에는 IMR로 요청을 차단한다. 하강 에지를 설정한 뒤 기존 Pending 플래그를 해제하고, NVIC의 우선순위를 설정한 다음 요청을 다시 허용한다.

**주의할 점**

- `FTSR`, `RTSR`, `IMR`은 EXTI 라인 하나당 1비트를 사용한다.
- `EXTICR`은 EXTI 라인 하나당 포트 선택에 4비트를 사용한다.
- `PR`은 W1C(Write 1 to Clear)이므로 1을 쓰면 해당 Pending 비트가 해제된다.
- `~(A | B)`와 `~A | B`는 서로 다르다. 여러 비트를 동시에 지우려면 괄호 범위에 주의해야 한다.

## 6. NVIC 우선순위 설정

```c
NVIC_SetPriority(EXTI0_IRQn, 5U);
NVIC_SetPriority(EXTI1_IRQn, 6U);

EXTI->IMR |= (1UL << 0) | (1UL << 1);
NVIC_EnableIRQ(EXTI0_IRQn);
NVIC_EnableIRQ(EXTI1_IRQn);
```

NVIC는 인터럽트 채널을 활성화하고 우선순위를 관리한다.

- EXTI0 우선순위: 5
- EXTI1 우선순위: 6
- 우선순위 숫자가 작을수록 높은 우선순위
- `NVIC_SetPriority()`와 `NVIC_EnableIRQ()`는 CMSIS가 제공하는 함수

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

HAL 콜백을 이용할 때도 같은 하드웨어 경로를 거친다. 이 실습에서는 어떤 레지스터가 핀 라우팅과 에지 감지, 요청 허용, 출력 변경을 담당하는지 코드에서 직접 확인했다.

## 9. 빌드와 보드 확인

프로젝트의 `.project`는 `examples/F411_EXTI_Practice/STM32CubeIDE/`에 있다. `F411_EXTI_Practice`를 가져온 뒤 빌드한다. GPIO와 EXTI 설정, 두 ISR은 `Core/Src/main.c`에 직접 구현되어 있으므로 동작을 확인할 때 해당 소스를 기준으로 본다.

빌드와 보드 동작은 확인 후 기록한다. 보드에서는 리셋 직후 LED가 꺼져 있는지, PB0을 누르면 모두 켜지는지, PB1을 누르면 모두 꺼지는지 확인한다.

- 레지스터 초기화와 두 IRQ 핸들러. [F411_EXTI_Practice의 main.c](../../examples/F411_EXTI_Practice/Core/Src/main.c)
- GPIO·SYSCFG·EXTI 레지스터. [RM0383 — STM32F411 레퍼런스 매뉴얼](https://www.st.com/resource/en/reference_manual/dm00119316.pdf)
