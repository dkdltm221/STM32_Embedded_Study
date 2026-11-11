# GPIO LED 실습 요약

2026.11.09 · NUCLEO-F411RE

## 핀 구성

| 용도 | 핀 | 코드 이름 |
| --- | --- | --- |
| LED (LD2) | PA5 | `LD2_Pin` |
| 버튼 (B1) | PC13 | `B1_Pin` |

## 초기화 순서

```c
HAL_Init();              // HAL 초기화, 기본 시간 기준 설정
SystemClock_Config();    // 시스템 클럭 설정
MX_GPIO_Init();          // GPIO 클럭과 핀 모드 설정
MX_USART2_UART_Init();   // UART 통신 설정
```

## 1. LED 점멸

`while (1)` 안에서 반복한다. 0.5초 켜짐 + 0.5초 꺼짐으로 한 주기는 약 1초다.

```c
HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
HAL_Delay(500);
HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
HAL_Delay(500);
```

## 2. 버튼 입력에 따른 LED 제어

점멸 코드 대신 `while (1)`의 `USER CODE BEGIN 3` 영역에 작성한다.

```c
if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_RESET)
{
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
}
else
{
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
}
```

PC13 입력이 LOW이면 LED를 켜고, HIGH이면 끈다. 반복해서 입력을 읽는 **폴링 방식**이다.

## 핵심 함수

| 함수 | 역할 |
| --- | --- |
| `HAL_GPIO_WritePin(포트, 핀, 상태)` | 출력 HIGH / LOW 설정 |
| `HAL_GPIO_ReadPin(포트, 핀)` | 입력 HIGH / LOW 읽기 |
| `HAL_Delay(ms)` | 지정한 시간만큼 대기 |

`GPIO_PIN_SET` = HIGH · `GPIO_PIN_RESET` = LOW

## 코드 생성과 빌드

**CubeMX Generate Code → CubeIDE Build Project → Run / Debug**

- 자동 생성: 핀·클럭 초기화 코드. `while`문 동작은 직접 작성한다.
- `B1_PIN` 오류: 정확한 이름은 **`B1_Pin`**이다. C는 대소문자를 구분한다.
- `.elf not found`: 빌드 성공 후 `Debug/test_led.elf`가 생성되는지 확인한다.

현재 핀 이름 오타는 수정했으며, 빌드 성공과 보드 동작은 추가 확인이 필요하다.

---

## 3. LED 8개 동시 제어 · 2026.11.10

**목표:** PC0~PC7에 연결한 LED 8개를 동시에 1초 켜고, 1초 끈다.

### 연결 및 설정

- 준비물: LED 8개, 1kΩ 저항 8개, 브레드보드, 점퍼선
- 각 LED 배선: `GPIO(PC0~PC7) → 저항 → LED(+) → LED(-) → GND`
- CubeMX: PC0~PC7을 `GPIO_Output`, Push-Pull, No Pull로 설정
- 핀 라벨: PC0~PC7에 각각 `LED1`~`LED8` 지정
- 보드 커넥터 이름과 GPIO 이름은 다를 수 있으므로 핀맵으로 위치 확인

### 반복 코드

`while (1)`의 `USER CODE BEGIN 3` 영역에 작성한다.

```c
// LED 8개 ON
HAL_GPIO_WritePin(GPIOC,
    LED1_Pin | LED2_Pin | LED3_Pin | LED4_Pin |
    LED5_Pin | LED6_Pin | LED7_Pin | LED8_Pin,
    GPIO_PIN_SET);
HAL_Delay(1000);

// LED 8개 OFF
HAL_GPIO_WritePin(GPIOC,
    LED1_Pin | LED2_Pin | LED3_Pin | LED4_Pin |
    LED5_Pin | LED6_Pin | LED7_Pin | LED8_Pin,
    GPIO_PIN_RESET);
HAL_Delay(1000);
```

### 문제와 핵심

- **문제:** OFF 이후 딜레이가 없어 즉시 다시 켜졌다.
- **해결:** `GPIO_PIN_RESET` 이후에도 `HAL_Delay(1000)`을 추가했다.
- `|`는 핀 마스크를 합쳐 **같은 포트의 여러 핀을 한 번에 제어**한다.
- 이 배선에서는 `SET`이면 켜지고 `RESET`이면 꺼진다. 딜레이 동안 해당 출력 상태가 유지되며, 전체 점멸 주기는 약 2초다.
