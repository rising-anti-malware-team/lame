#! /bin/bash

# build
cd src
javac -d ../bin -cp ../lib/lame.jar run/scan.java

# copy
cd ..
cp -rf lib/lame.jar ../../../../../rame.bin/
cp -rf bin/run      ../../../../../rame.bin/
cp -rf javatest.sh  ../../../../../rame.bin/

# run
cd bin
java -classpath ../lib/lame.jar: -Djava.library.path=./ run.scan $1
