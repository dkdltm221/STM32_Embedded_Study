# STM32 Embedded Study — Codex 작업 지침

## 목적과 소통

- 이 저장소는 STM32 임베디드 학습과 실습 기록을 위한 프로젝트다.
- 기본 설명은 한국어로 한다. 코드 변경 시 무엇을 바꿨는지와 이유, 확인 결과를 간결하게 설명한다.
- 학습 설명이 필요한 경우 GPIO/주변장치 동작, 관련 레지스터, HAL 코드의 관계를 연결해서 설명한다.
- 요청 범위에 맞게 수정하고 기존 사용자 변경 사항을 보존한다.

## 프로젝트 구조

- 개발 환경: NUCLEO-F411RE, STM32F411RET6, Cortex-M4, C, STM32CubeIDE/CubeMX, STM32F4 HAL/CMSIS.
- LED 실습은 `examples/gpio-led/`에 있으며 IDE 프로젝트 이름은 `test_led`다. EXTI 실습은 `examples/F411_EXTI_Practice/`, UART 실습은 `examples/USART_TEST/`에 있다.
- 주요 파일: `Core/Src/main.c`, `Core/Inc/main.h`, `test_led.ioc` (실습 프로젝트 기준).
- `docs/`에 개념을 정리하고, `troubleshooting/`에 문제 해결을 기록한다. 회로와 실습 이미지는 `assets/`에 둔다.
- 루트에도 `test_led.ioc`가 있으므로, 설정 변경 시 대상 파일과 IDE 프로젝트 참조를 먼저 확인한다. 두 파일을 임의로 동기화하지 않는다.

## 펌웨어 수정

- CubeMX 생성 파일의 사용자 로직은 가능한 한 `USER CODE BEGIN` / `USER CODE END` 영역 안에서 수정하고 마커를 보존한다.
- 생성된 초기화 코드를 바꿀 때는 `.ioc` 설정과의 일관성 및 재생성 시 덮어쓰기 가능성을 확인한다.
- `Drivers/`의 HAL/CMSIS 코드는 일반적인 기능 수정 대상으로 삼지 않는다.
- 핀 이름은 `main.h`의 정의를 사용한다. 배선, LED 활성 레벨, 전기적 특성을 확인 없이 추정하지 않는다.
- 핀, 클록, 인터럽트, 지연 시간을 바꿀 때 영향과 필요한 보드 확인 방법을 설명한다.
- README의 실습 설명이 현재 코드보다 오래됐을 수 있다. 실제 동작은 소스와 설정을 대조한다.
- 기존 인코딩과 개행을 존중하고 한국어 파일은 UTF-8로 읽는다.

## 빌드와 검증

- 기본 빌드 경로: STM32CubeIDE에서 `examples/gpio-led`를 Existing Projects into Workspace로 가져온 뒤 `test_led`를 Build Project 한다.
- 명령줄 빌드가 필요하면 설치된 도구와 프로젝트 빌드 구성을 먼저 확인한다. 도구 경로나 빌드 명령을 추정하지 않는다.
- `Debug/`, `Release/` 등 생성 산출물을 직접 수정하거나 커밋 대상으로 추가하지 않는다.
- 펌웨어 변경은 가능한 환경에서 빌드로 확인한다. 보드 실행 확인과 빌드 성공을 구분해서 보고한다.
- 도구나 보드가 없어 확인하지 못한 항목은 명시하고 사용자가 수행할 검증 절차를 안내한다.
- ST-LINK 다운로드나 디버깅을 수행할 때는 사용자가 요청한 하드웨어 작업 범위 안에서 진행한다.
