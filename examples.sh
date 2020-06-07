#!/bin/bash 

echo "Compiling examples..."
sleep 1

cd examples

if uname -p | grep -q x86_64; then
    gcc -I/usr/include/SDL Test1.c -lm -lSDL -lGL /usr/lib/x86_64-linux-gnu/openobj3d.so -o Test1
    if [ "$?" -ne 0 ]
    then
        echo "Error compiling Test1.c"
    fi
else
    gcc -I/usr/include/SDL Test1.c -lm -lSDL -lGL /usr/lib/i386-linux-gnu/openobj3d.so -o Test1
    if [ "$?" -ne 0 ]
    then
        echo "Error compiling Test1.c"
    fi
fi

exit 0
