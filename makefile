.PHONY: all win web test deps clean

name = TuxFishing

all:
	cc main.c -I inc -Ofast -lglfw -lm -o release/$(name)_linux
	strip --strip-unneeded release/$(name)_linux
	upx --lzma --best release/$(name)_linux

win:
	i686-w64-mingw32-gcc main.c -Ofast -I inc -L. -lglfw3dll -lm -o release/$(name)_windows.exe
	strip --strip-unneeded release/$(name)_windows.exe
	upx --lzma --best release/$(name)_windows.exe

web:
	emcc main.c -DWEB -O3 --closure 1 -s FILESYSTEM=0 -s USE_GLFW=3 -s ENVIRONMENT=web -s TOTAL_MEMORY=256MB -I inc -o release/web/index.html --shell-file t.html

test:
	tcc main.c -I inc -Ofast -lglfw -lm -o /tmp/$(name)_test
	/tmp/$(name)_test
	rm /tmp/$(name)_test

deps:
	@echo https://emscripten.org/docs/getting_started/downloads.html
	@echo https://github.com/upx/upx/releases/tag/v4.2.4
	sudo apt install libglfw3-dev libglfw3
	sudo apt install tcc
	sudo apt install upx-ucl
	sudo apt install mingw-w64

clean:
	rm -f release/$(name)_linux
	rm -f release/$(name)_windows.exe
	rm -f release/web/index.html
	rm -f release/web/index.js
	rm -f release/web/index.wasm
