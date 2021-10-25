APPNAME=kyvadlo
LABEL?=$(APPNAME)
APKFILE ?= $(APPNAME).apk
RAWDRAWANDROID=rawdrawandroid
RAWDRAWANDROIDSRCS=$(RAWDRAWANDROID)/android_native_app_glue.c
CFLAGS:=-I. -ffunction-sections -fdata-sections -Os -flto -fshort-enums -fvisibility=hidden -Wno-switch
LDFLAGS:=-s -flto -Wl,--gc-sections
PACKAGENAME?=com.github.eetagent.$(APPNAME)
SRC:=main.c

ANDROIDVERSION=31
ANDROIDTARGET=22

ANDROID_FULLSCREEN=y

TARGETS:=makecapk/lib/arm64-v8a/lib$(APPNAME).so makecapk/lib/armeabi-v7a/lib$(APPNAME).so makecapk/lib/x86_64/lib$(APPNAME).so

CFLAGS_ARM64=-m64
CFLAGS_ARM32=-mfloat-abi=softfp -m32
CFLAGS_x86=-march=i686 -mtune=native -mssse3 -mfpmath=sse -m32
CFLAGS_x86_64=-march=x86-64 -msse4.2 -mpopcnt -m64 -mtune=native

STOREPASS?=kyberfestival

include rawdrawandroid/Makefile
