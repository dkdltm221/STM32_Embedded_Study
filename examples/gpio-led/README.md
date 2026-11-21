# STM32 GPIO LED 실습

NUCLEO-F411RE의 내장 LD2와 외부 LED를 제어하는 STM32CubeIDE 프로젝트입니다. IDE 프로젝트 이름은 `test_led`이며, CubeMX 설정은 이 디렉터리의 `test_led.ioc`에 있습니다.

## 실습 동작

| 실습 | 핀 | 동작과 확인 상태 |
| --- | --- | --- |
| 내장 LD2 점멸 | PA5 (`LD2_Pin`) | 이전 실습에서는 0.5초 켜짐과 0.5초 꺼짐을 반복했습니다. 당시 보드에서 정상 동작을 확인했습니다. |
| 외부 LED 8개 | PC0~PC7 (`LED1_Pin`~`LED8_Pin`) | 현재 코드에서 목표로 하는 동작은 전체 LED를 1초 켜고 1초 끄는 것입니다. |
| 버튼 폴링 | PC13 (`B1_Pin`) | 입력에 따라 LD2를 제어하는 별도 학습 예제입니다. |

현재 반복문의 `HAL_GPIO_WritePin()` 두 곳에는 `LED5_Pin`~`LED8_Pin`과 핀 마스크 뒤 쉼표가 빠져 있습니다. ARM GCC 구문 검사에서 인자 개수 오류가 발생했습니다. LED 8개 실습을 빌드할 때 [학습 문서의 반복 코드](../../docs/gpio/gpio_led.md#반복-코드)와 대조해 핀 마스크 전체와 상태 인자를 확인합니다. 외부 LED 8개와 버튼의 보드 결과는 실습 후 기록합니다.

## 프로젝트 구조

| 경로 | 내용 |
| --- | --- |
| [Core/Src/main.c](Core/Src/main.c) | 클록·GPIO 초기화와 반복 동작 |
| [Core/Inc/main.h](Core/Inc/main.h) | GPIO 핀과 포트 이름 |
| [test_led.ioc](test_led.ioc) | STM32CubeMX 핀과 클록 설정 |
| [Drivers/](Drivers/) | STM32F4 HAL 및 CMSIS |
| `STM32F411RETX_*.ld` | Flash·RAM의 메모리 배치를 정의하는 링커 스크립트 |

## 빌드 및 보드 확인

1. STM32CubeIDE에서 **File → Import → General → Existing Projects into Workspace**를 선택합니다.
2. 루트 디렉터리로 `examples/gpio-led`를 지정하고 `test_led`를 가져옵니다. **Copy projects into workspace**는 해제합니다.
3. **Project → Build Project**로 빌드합니다.
4. ST-LINK USB로 보드를 연결하고 **Run** 또는 **Debug**로 다운로드합니다. 디버거가 진입점에서 멈췄다면 **Resume**을 누릅니다.
5. 실행한 예제에 맞는 핀과 점멸 간격을 확인합니다. 외부 LED는 [배선 및 핀 설정](../../docs/gpio/gpio_led.md#연결-및-설정)을 확인합니다.

## 수정 시 참고

- `HAL_Delay()`의 단위는 밀리초입니다. 켜짐과 꺼짐의 지연을 더하면 한 주기의 길이가 됩니다.
- CubeMX 재생성 시 보존할 반복 코드는 `USER CODE BEGIN 3`과 `USER CODE END 3` 사이에 작성합니다.
- `Debug/`와 `Release/` 등 빌드 산출물은 `.gitignore`로 제외합니다. 소스, `.ioc`, IDE 설정 및 드라이버는 함께 관리합니다.

ST 및 Arm 드라이버의 이용 조건은 파일의 저작권 고지 및 [Drivers/](Drivers/) 아래의 라이선스 파일을 확인합니다.
