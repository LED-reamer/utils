emcc -o index.html main.c ../../utils/window.c --use-port=sdl3
python3 -m http.server --bind 127.0.0.1 8080
