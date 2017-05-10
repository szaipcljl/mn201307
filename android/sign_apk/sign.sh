#!/bin/bash
java -Xmx2048m -jar -Djava.library.path=. signapk.jar -w platform.x509.pem platform.pk8 $1.apk $1_sign.apk
#java -Xmx2048m -jar -Djava.library.path=. signapk.jar -w releasekey.x509.pem releasekey.pk8 $1.apk $1_sign.apk
