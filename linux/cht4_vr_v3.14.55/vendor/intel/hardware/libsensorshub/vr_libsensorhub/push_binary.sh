sudo adb remount

#$TARGET = system/vendor
TARGET=system

adb push $OUT/$TARGET/bin/sensorhubd $TARGET/bin

adb push $OUT/$TARGET/lib/libsensorhub.so $TARGET/lib

adb push $OUT/$TARGET/lib64/libsensorhub.so $TARGET/lib64

adb push $OUT/$TARGET/sensorhub_client /data

adb push $OUT/$TARGET/bin/event_notification $TARGET/bin

