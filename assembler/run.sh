set -ex

PROJECT_PATH=$(dirname "${BASH_SOURCE[0]}")

clang++ -O2 -Werror $PROJECT_PATH/main.cpp -o $PROJECT_PATH/main.bin
clang++ -g $PROJECT_PATH/main.cpp -o $PROJECT_PATH/debug.bin
$PROJECT_PATH/main.bin
