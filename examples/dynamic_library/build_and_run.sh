echo "build test dl"
gcc example_lib.c -shared -rdynamic -o dynamic_library.so
echo "build test"
gcc main.c ../../utils/dynamic_library.c -I../../ -ldl -o example && ./example
