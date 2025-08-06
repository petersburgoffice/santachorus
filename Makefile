# Makefile для Santa Chorus с автоматическим версионированием

.PHONY: all clean build install-au install-vst3 install version quick help

# Основная цель - собрать и установить все форматы
all: version build install

# Увеличить версию
version:
	@echo "📈 Incrementing version..."
	@./increment_version.sh

# Собрать проект
build:
	@echo "🔨 Building Santa Chorus..."
	@mkdir -p build
	@cd build && cmake .. && cmake --build .
	@echo "📋 Current version: v$(shell cat version.txt)"

# Установить AU плагин
install-au:
	@echo "📦 Installing AU plugin..."
	@rm -rf ~/Library/Audio/Plug-Ins/Components/SantaChorus.component
	@cp -R build/SantaChorus_artefacts/AU/SantaChorus.component ~/Library/Audio/Plug-Ins/Components/
	@echo "✅ AU plugin installed!"

# Установить VST3 плагин
install-vst3:
	@echo "📦 Installing VST3 plugin..."
	@rm -rf ~/Library/Audio/Plug-Ins/VST3/SantaChorus.vst3
	@cp -R build/SantaChorus_artefacts/VST3/SantaChorus.vst3 ~/Library/Audio/Plug-Ins/VST3/
	@echo "✅ VST3 plugin installed!"

# Установить все плагины
install: install-au install-vst3
	@echo "🎉 All plugins installed!"
	@echo "📋 Current version: v$(shell cat version.txt)"

# Быстрая сборка (без инкремента версии)
quick:
	@echo "⚡ Quick build (no version increment)..."
	@mkdir -p build
	@cd build && cmake --build .

# Очистка
clean:
	@echo "🧹 Cleaning build directory..."
	@rm -rf build
	@echo "✅ Cleaned!"

# Показать текущую версию
show-version:
	@echo "📋 Current version: $(shell cat version.txt)"

# Помощь
help:
	@echo "Available targets:"
	@echo "  all          - Increment version, build and install all plugins"
	@echo "  version      - Increment version only"
	@echo "  build        - Build project only"
	@echo "  install      - Install all plugins (AU + VST3)"
	@echo "  install-au   - Install AU plugin only"
	@echo "  install-vst3 - Install VST3 plugin only"
	@echo "  quick        - Quick build without version increment"
	@echo "  clean        - Clean build directory"
	@echo "  show-version - Show current version"
	@echo "  help         - Show this help"