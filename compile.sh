#!/bin/bash 

echo "Compiling openobj3d library..."
sleep 1

cd src

gcc -c -Wall -Werror -fpic openobj3d.c

if [ "$?" -ne 0 ]
then
echo "Error compiling openobj3d.c"
exit 1
fi

gcc -shared -o openobj3d.so openobj3d.o

if [ "$?" -ne 0 ]
then
echo "Error creating openobj3d.so"
exit 1
fi

exit 0
