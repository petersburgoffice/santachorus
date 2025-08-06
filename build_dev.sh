#!/bin/bash

# SaturVST Development Build Script
# Удобный скрипт для разработки с разными опциями

set -e  # Exit on any error

# Цвета для вывода
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
NC='\033[0m' # No Color

# Функция помощи
show_help() {
    echo -e "${BLUE}🎵 SaturVST Development Build Script${NC}"
    echo ""
    echo "Использование:"
    echo "  ./build_dev.sh [команда]"
    echo ""
    echo "Команды:"
    echo -e "  ${GREEN}build${NC}     - Полная сборка проекта (по умолчанию)"
    echo -e "  ${GREEN}quick${NC}     - Быстрая сборка (только компиляция)"
    echo -e "  ${GREEN}run${NC}       - Сборка + запуск Standalone версии"
    echo -e "  ${GREEN}clean${NC}     - Очистка build директории"
    echo -e "  ${GREEN}rebuild${NC}   - Полная пересборка (clean + build)"
    echo -e "  ${GREEN}help${NC}      - Показать эту справку"
    echo ""
    echo "Примеры:"
    echo "  ./build_dev.sh run    # Собрать и запустить"
    echo "  ./build_dev.sh quick  # Быстрая сборка"
    echo ""
}

# Настройка SSH для git (если нужно)
setup_ssh() {
    if [ ! -z "$SSH_AUTH_SOCK" ]; then
        echo -e "${YELLOW}📡 SSH уже настроен${NC}"
    else
        echo -e "${YELLOW}📡 Настраиваем SSH для git...${NC}"
        export SSH_AUTH_SOCK=~/Library/Group\ Containers/2BUA8C4S2C.com.1password/t/agent.sock
    fi
}

# Проверка JUCE
check_juce() {
    if [ ! -d "JUCE" ]; then
        echo -e "${YELLOW}📦 JUCE не найден. Клонируем JUCE framework...${NC}"
        setup_ssh
        git clone https://github.com/juce-framework/JUCE.git
        echo -e "${GREEN}✅ JUCE успешно клонирован${NC}"
    fi
}

# Создание build директории
create_build_dir() {
    if [ ! -d "build" ]; then
        mkdir build
        echo -e "${BLUE}📁 Создана build директория${NC}"
    fi
}

# Полная сборка
full_build() {
    echo -e "${PURPLE}🎵 Полная сборка SaturVST Plugin...${NC}"
    
    # Увеличиваем номер версии
    echo -e "${BLUE}📊 Увеличиваем номер версии...${NC}"
    ./increment_version.sh
    
    check_juce
    create_build_dir
    
    cd build
    
    echo -e "${BLUE}⚙️  Конфигурируем с CMake...${NC}"
    cmake ..
    
    echo -e "${BLUE}🔨 Собираем плагин...${NC}"
    cmake --build . --config Release
    
    cd ..
    
    echo -e "${GREEN}🎉 Сборка успешно завершена!${NC}"
    echo ""
    echo -e "${YELLOW}Plugin установлен в:${NC}"
    echo "  VST3: ~/Library/Audio/Plug-Ins/VST3/"
    echo "  AU:   ~/Library/Audio/Plug-Ins/Components/"
    echo ""
}

# Быстрая сборка (только компиляция)
quick_build() {
    echo -e "${PURPLE}⚡ Быстрая сборка SaturVST...${NC}"
    
    if [ ! -d "build" ]; then
        echo -e "${RED}❌ Build директория не найдена. Запустите полную сборку сначала.${NC}"
        exit 1
    fi
    
    cd build
    echo -e "${BLUE}🔨 Компилируем изменения...${NC}"
    cmake --build . --config Release
    cd ..
    
    echo -e "${GREEN}⚡ Быстрая сборка завершена!${NC}"
}

# Сборка и запуск
build_and_run() {
    echo -e "${PURPLE}🎵 Сборка и запуск SaturVST...${NC}"
    
    quick_build
    
    echo -e "${BLUE}🚀 Запускаем Standalone версию...${NC}"
    if [ -f "build/SaturVST_artefacts/Standalone/SaturVST.app/Contents/MacOS/SaturVST" ]; then
        open build/SaturVST_artefacts/Standalone/SaturVST.app
        echo -e "${GREEN}✅ Приложение запущено!${NC}"
    else
        echo -e "${RED}❌ Standalone версия не найдена. Попробуйте полную сборку.${NC}"
        exit 1
    fi
}

# Очистка
clean_build() {
    echo -e "${YELLOW}🧹 Очищаем build директорию...${NC}"
    if [ -d "build" ]; then
        rm -rf build
        echo -e "${GREEN}✅ Build директория очищена${NC}"
    else
        echo -e "${YELLOW}📁 Build директория уже отсутствует${NC}"
    fi
}

# Полная пересборка
rebuild() {
    echo -e "${PURPLE}🔄 Полная пересборка SaturVST...${NC}"
    clean_build
    full_build
}

# Основная логика
case "${1:-build}" in
    "build")
        full_build
        ;;
    "quick")
        quick_build
        ;;
    "run")
        build_and_run
        ;;
    "clean")
        clean_build
        ;;
    "rebuild")
        rebuild
        ;;
    "help"|"-h"|"--help")
        show_help
        ;;
    *)
        echo -e "${RED}❌ Неизвестная команда: $1${NC}"
        echo ""
        show_help
        exit 1
        ;;
esac 