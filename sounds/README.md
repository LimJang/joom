# 🎵 Custom Sound Guide

이 폴더에 커스텀 사운드 파일을 추가할 수 있습니다.

## 📁 지원되는 파일 형식
- `.wav` (추천 - 무손실, 빠른 로딩)
- `.mp3` (압축됨, 작은 파일 크기)
- `.ogg` (고품질 압축)
- `.flac` (무손실 압축)
- `.aiff` (무손실)

## 🎮 사운드 파일 이름 규칙

### 발자국 사운드
- `footstep1.wav`, `footstep2.wav` - 커스텀 발자국 소리
- `step_concrete.wav`, `step_metal.wav` - 다양한 표면 소리

### 환경 사운드
- `ambient1.wav` - 배경 분위기음
- `cave_echo.wav` - 동굴 에코
- `wind.wav` - 바람 소리

### 무기/액션 사운드
- `weapon1.wav` - 무기 소리
- `reload.wav` - 재장전 소리
- `pickup.wav` - 아이템 획득 소리

### UI 사운드
- `menu_click.wav` - 메뉴 클릭음
- `error.wav` - 오류음
- `success.wav` - 성공음

### 기타 사운드
- `door1.wav` - 문 열림/닫힘
- `switch1.wav` - 스위치 소리 (손전등 전용)
- `explosion.wav` - 폭발음

## 🎯 게임에서 사용하기

### 자동 로딩
게임 시작시 `sounds/` 폴더의 모든 사운드 파일이 자동으로 로딩됩니다.

### 수동 테스트
게임 실행 중 숫자 키로 사운드를 테스트할 수 있습니다:
- `1` : footstep1.wav 재생
- `2` : footstep2.wav 재생  
- `3` : ambient1.wav 재생
- `4` : weapon1.wav 재생
- `5` : door1.wav 재생
- `6` : switch1.wav 재생 (손전등 사운드)

### 커스텀 발자국 토글
- `C` 키 : 커스텀 발자국 소리 사용 ON/OFF

## 🔧 프로그래밍으로 추가하기

### C++ 코드에서 사운드 로딩
```cpp
// 특정 파일 로딩
audioManager->loadSoundFile("sounds/my_sound.wav", "my_sound_name");

// 사운드 재생
audioManager->playSound("my_sound_name");

// 특정 슬롯에 로딩
audioManager->loadSoundFile("sounds/footstep.wav", SoundType::CUSTOM_FOOTSTEP_1);
audioManager->addCustomFootstepSound(SoundType::CUSTOM_FOOTSTEP_1);
```

### 디렉토리 전체 로딩
```cpp
// sounds 폴더의 모든 파일 로딩
audioManager->loadSoundsFromDirectory("sounds");

// 로딩된 사운드 목록 확인
auto soundNames = audioManager->getLoadedSoundNames();
```

## 🎨 사운드 제작 팁

### 권장 설정
- **샘플레이트**: 44.1kHz (CD 품질)
- **비트 뎁스**: 16bit (충분한 품질)
- **채널**: 모노 또는 스테레오
- **길이**: 
  - 발자국: 100-300ms
  - 환경음: 1-5초
  - UI음: 50-200ms

### 무료 사운드 소스
- [Freesound.org](https://freesound.org) - 무료 사운드 라이브러리
- [Zapsplat](https://zapsplat.com) - 고품질 효과음
- [BBC Sound Effects](https://sound-effects.bbcrewind.co.uk) - BBC 사운드
- [Adobe Audition](https://www.adobe.com/products/audition.html) - 사운드 편집

## 📝 예제 파일들

현재 폴더가 비어있습니다. 다음과 같은 파일들을 추가해보세요:

```
sounds/
├── footstep1.wav      # 커스텀 발자국 1
├── footstep2.wav      # 커스텀 발자국 2
├── ambient1.wav       # 배경음 1
├── weapon1.wav        # 무기 소리
├── door1.wav          # 문 소리
├── switch1.wav        # 스위치 소리 (손전등)
├── pickup.wav         # 아이템 획득
└── explosion.wav      # 폭발음
```

사운드 파일을 추가한 후 게임을 재시작하면 자동으로 로딩됩니다! 🎵
