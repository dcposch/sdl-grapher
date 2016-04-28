#!/bin/sh
set -e
echo "You'll need GCC and SDL to build entropy-gulp"
echo "On Ubuntu, try 'apt-get install build-essential libsdl-dev' if you're missing dependencies"
echo "---"
gcc -Wall *.c -lSDL -lm -o entropy-gulp
echo "Done!"
