#!/bin/bash

echo "imamLL library installation"

./compile.sh

if [ "$?" -ne 0 ]
then
exit 1
fi

sudo ./install.sh

if [ "$?" -ne 0 ]
then
exit 1
fi

./examples.sh

echo "Done"

exit 0
