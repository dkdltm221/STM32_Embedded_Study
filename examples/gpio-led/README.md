# STM32 GPIO LED 점멸 실습

STM32F411RE의 GPIO를 제어하여 LD2 LED를 반복해서 켜고 끄는 STM32CubeIDE 프로젝트입니다.

## 개발 환경

- MCU: STM32F411RET6
- IDE: STM32CubeIDE
- 주변장치 설정: STM32CubeMX (`test_led.ioc`)
- 라이브러리: STM32F4 HAL / CMSIS
- LED 출력: PA5 (`LD2_Pin`, `LD2_GPIO_Port`)

## 동작

GPIO를 초기화한 뒤 다음 동작을 무한 반복합니다.

1. LD2 켜기
2. 500ms 대기
3. LD2 끄기
4. 500ms 대기

한 주기는 약 1초이며, 전원을 다시 켜거나 리셋하면 프로그램이 처음부터 실행됩니다.

## 프로젝트 구조

| 경로 | 내용 |
| --- | --- |
| `Core/Src/main.c` | 초기화 및 LED 점멸 코드 |
| `Core/Inc/main.h` | GPIO 핀 정의 |
| `Drivers/` | HAL 및 CMSIS 드라이버 |
| `test_led.ioc` | 핀과 클럭 등 CubeMX 설정 |
| `STM32F411RETX_*.ld` | 메모리 배치를 정의하는 링커 스크립트 |

## 빌드 및 실행

1. STM32CubeIDE에서 **File → Import → General → Existing Projects into Workspace**를 선택합니다.
2. 이 저장소 폴더를 선택하여 `test_led` 프로젝트를 가져옵니다.
3. **Project → Build Project**로 빌드합니다.
4. ST-LINK를 통해 보드를 PC에 연결합니다.
5. **Run** 또는 **Debug**로 프로그램을 보드에 다운로드합니다. 디버깅 중 정지했다면 **Resume**으로 실행합니다.
6. LD2 LED가 0.5초 간격으로 켜지고 꺼지는지 확인합니다.

## 수정 시 참고

- `HAL_Delay(500)`의 값을 바꾸면 점멸 간격을 변경할 수 있습니다. 단위는 밀리초입니다.
- CubeMX에서 코드를 재생성할 때 사용자 코드를 보존하려면 기존 `USER CODE BEGIN`과 `USER CODE END` 사이에 작성합니다. 반복 코드는 `USER CODE BEGIN 3` 영역에 둡니다.
- `Debug/`, `Release/`와 로컬 임시 파일은 `.gitignore`로 제외합니다. 소스 코드, `.ioc`, IDE 프로젝트 설정 및 드라이버는 함께 관리합니다.

## 라이선스 참고

포함된 ST 및 Arm 드라이버의 이용 조건은 해당 파일의 저작권 고지와 `Drivers/` 아래의 라이선스 파일을 확인하세요.
