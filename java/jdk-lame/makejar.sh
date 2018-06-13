#! /bin/bash

javac -d bin src/engine/*.java
cd bin
jar -cf lame.jar engine/*.class
jar umf ../src/MANIFEST.MF lame.jar
cd ..
cp -f bin/lame.jar ../jdk-test/lib/
