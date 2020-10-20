
#ifndef NATIVE_AUDIO_JNI_HELPER_H
#define NATIVE_AUDIO_JNI_HELPER_H

#include <jni.h>

#ifndef NELEM
# define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif

#define NATIVE_METHOD(className, functionName, signature) \
    { #functionName, signature, reinterpret_cast<void*>(className ## _ ## functionName) }

int jniRegisterNativeMethods(JNIEnv* env, const char *classPathName,
                             const JNINativeMethod *nativeMethods, jint nMethods);

#endif //NATIVE_AUDIO_JNI_HELPER_H
