gcc \
main.c \
buffer.c \
editor.c \
terminal/term.c \
../utils/hstd/file.c \
-lncurses \
-fsanitize=address \
-o editor && ./editor
