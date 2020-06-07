#!/bin/bash

echo "Installing openobj3d library..."
sleep 2

cd src

if uname -p | grep -q x86_64; then
        rm /usr/lib/x86_64-linux-gnu/openobj3d.*
        cp openobj3d.so /usr/lib/x86_64-linux-gnu/
        if [ "$?" -ne 0 ]
        then
            echo "Error installing openobj3d library"
            exit 1
        fi
        cp openobj3d.h /usr/include/
        if [ "$?" -ne 0 ]
        then
            echo "Error installing openobj3d header"
            exit 1
        fi
else
        rm /usr/lib/i386-linux-gnu/openobj3d.*
        cp openobj3d.so /usr/lib/i386-linux-gnu/
        if [ "$?" -ne 0 ]
        then
            echo "Error installing openobj3d library"
            exit 1
        fi
        cp openobj3d.h /usr/include/
        if [ "$?" -ne 0 ]
        then
            echo "Error installing openobj3d header"
            exit 1
        fi
fi

exit 0
