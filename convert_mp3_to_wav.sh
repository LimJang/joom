#!/bin/bash

# MP3 to WAV 변환 스크립트

echo "🎵 MP3 to WAV Converter for Joom"
echo ""

# sounds 디렉토리 확인
if [ ! -d "sounds" ]; then
    echo "📁 Creating sounds directory..."
    mkdir sounds
fi

# FFmpeg 설치 확인
if ! command -v ffmpeg &> /dev/null; then
    echo "⚠️  FFmpeg not found. Installing via Homebrew..."
    if command -v brew &> /dev/null; then
        brew install ffmpeg
    else
        echo "❌ Homebrew not found. Please install FFmpeg manually:"
        echo "   Visit: https://ffmpeg.org/download.html"
        exit 1
    fi
fi

# MP3 파일 찾기
mp3_files=(sounds/*.mp3)

if [ ! -e "${mp3_files[0]}" ]; then
    echo "ℹ️  No MP3 files found in sounds/ directory"
    echo ""
    echo "💡 Usage:"
    echo "   1. Copy your MP3 files to sounds/ directory"
    echo "   2. Run this script: ./convert_mp3_to_wav.sh"
    echo ""
    exit 0
fi

echo "🔍 Found MP3 files:"
converted_count=0

for mp3_file in "${mp3_files[@]}"; do
    if [ -f "$mp3_file" ]; then
        # 파일명에서 확장자 제거
        base_name=$(basename "$mp3_file" .mp3)
        wav_file="sounds/${base_name}.wav"
        
        echo "  📄 $mp3_file"
        
        # WAV 파일이 이미 존재하는지 확인
        if [ -f "$wav_file" ]; then
            read -p "   ⚠️  $wav_file already exists. Overwrite? (y/N): " -n 1 -r
            echo
            if [[ ! $REPLY =~ ^[Yy]$ ]]; then
                echo "   ⏭️  Skipped"
                continue
            fi
        fi
        
        # FFmpeg로 변환
        echo "   🔄 Converting to WAV..."
        if ffmpeg -i "$mp3_file" -ar 44100 -ac 2 -sample_fmt s16 "$wav_file" -y &>/dev/null; then
            echo "   ✅ Converted: $wav_file"
            converted_count=$((converted_count + 1))
            
            # 원본 파일 삭제 여부 확인
            read -p "   🗑️  Delete original MP3 file? (y/N): " -n 1 -r
            echo
            if [[ $REPLY =~ ^[Yy]$ ]]; then
                rm "$mp3_file"
                echo "   🗑️  Deleted: $mp3_file"
            fi
        else
            echo "   ❌ Failed to convert: $mp3_file"
        fi
        echo ""
    fi
done

echo "📊 Conversion complete!"
echo "   Converted: $converted_count files"
echo ""
echo "🎮 Ready to use in Joom!"
echo "   Run: ./build_with_audio.sh && cd build && ./Joom"
