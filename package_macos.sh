#!/bin/bash

# 에러 발생 시 즉시 중단
set -e

echo "📦 Joom macOS 배포 패키지를 생성합니다..."

# 1. 프로젝트 빌드
echo "▶️ 1/4: 프로젝트를 빌드합니다..."
rm -rf build
mkdir build
cd build
cmake ..
make
cd ..

# 2. 변수 설정 및 디렉토리 생성
APP_BUNDLE="build/Joom.app"
FRAMEWORKS_DIR="$APP_BUNDLE/Contents/Frameworks"
EXECUTABLE_PATH="$APP_BUNDLE/Contents/MacOS/Joom"
RESOURCES_DIR="$APP_BUNDLE/Contents/Resources"

echo "▶️ 2/4: .app 번들 내부에 디렉토리를 생성합니다..."
mkdir -p "$FRAMEWORKS_DIR"
mkdir -p "$RESOURCES_DIR"

# 3. 리소스 복사
echo "▶️ 3/4: sounds, textures, maps 리소스를 복사합니다..."
cp -R sounds "$RESOURCES_DIR/"
cp -R textures "$RESOURCES_DIR/"
cp -R maps "$RESOURCES_DIR/"

# 4. 라이브러리 의존성 해결
echo "▶️ 4/4: 라이브러리를 복사하고 경로를 수정합니다..."

# 처리할 바이너리 목록 (실행 파일로 시작)
binary_list=("$EXECUTABLE_PATH")
# 이미 처리한 라이브러리를 추적하기 위한 배열
processed_libs=()

# $1: 처리할 바이너리 파일 경로
process_binary() {
    local bin_path="$1"
    local bin_name=$(basename "$bin_path")

    # otool로 Homebrew 라이브러리 의존성 목록 추출
    local dependencies=$(otool -L "$bin_path" | grep '/opt/homebrew/' | awk '{print $1}')

    for dep in $dependencies; do
        local dep_name=$(basename "$dep")

        # 라이브러리 경로 수정
        echo "  - $bin_name 의 의존성 수정: $dep_name"
        install_name_tool -change "$dep" "@executable_path/../Frameworks/$dep_name" "$bin_path"

        # 아직 처리되지 않은 라이브러리라면 목록에 추가
        if [[ ! " ${processed_libs[@]} " =~ " ${dep_name} " ]]; then
            echo "    - 새로운 라이브러리 발견: $dep_name. 복사 목록에 추가합니다."
            processed_libs+=("$dep_name")
            binary_list+=("$dep")
        fi
    done
}

# 실행 파일부터 시작하여 모든 의존성을 재귀적으로 처리
while [ ${#binary_list[@]} -gt 0 ]; do
    current_bin=${binary_list[0]}
    binary_list=("${binary_list[@]:1}") # 목록에서 첫 번째 항목 제거

    # 라이브러리 파일인 경우, 복사하고 자신의 ID 수정
    if [[ "$current_bin" != "$EXECUTABLE_PATH" ]]; then
        echo "  - 라이브러리 복사: $(basename "$current_bin")"
        cp "$current_bin" "$FRAMEWORKS_DIR/"
        
        # 복사된 라이브러리의 id를 수정하여 상대 경로를 사용하도록 함
        install_name_tool -id "@executable_path/../Frameworks/$(basename "$current_bin")" "$FRAMEWORKS_DIR/$(basename "$current_bin")"
        
        # 복사된 라이브러리의 의존성을 처리
        process_binary "$FRAMEWORKS_DIR/$(basename "$current_bin")"
    else
        # 실행 파일의 의존성을 처리
        process_binary "$EXECUTABLE_PATH"
    fi
done

echo "✅ 성공! 'build/Joom.app' 파일을 다른 Mac으로 복사하여 실행할 수 있습니다."
