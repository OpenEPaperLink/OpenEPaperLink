#!/bin/bash 
echo "*****************************************************"
echo "this is post build!! current configure is :$1"
rm -rf $2
cp -f $1 $2
echo "**************** end of post build ******************"
