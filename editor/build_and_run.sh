gcc \
main.c \
buffer.c \
editor.c \
terminal/term.c \
../hstd/file.c \
-lncurses \
-fsanitize=address \
-o editor && ./editor
