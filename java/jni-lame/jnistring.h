#ifndef __JAVA_LAME_COMMON_H__
#define __JAVA_LAME_COMMON_H__

class Jni_auto_release_string
{
private:
	JNIEnv*		env;
	jstring		str;
	const char*	buffer;
public:
	Jni_auto_release_string(JNIEnv* e, jstring s):env(e),str(s),buffer(NULL){
		if(env && str) buffer = env->GetStringUTFChars(str,NULL);
	}
	~Jni_auto_release_string(){ if(buffer) env->ReleaseStringChars(str, (const jchar*)buffer); }
	operator const char* ()	{ return buffer; }
};

class Jni_auto_release_object
{
private:
	JNIEnv*		env;
	jobject		jobj;
public:
	Jni_auto_release_object(JNIEnv* e, jobject o):env(e),jobj(o){}
	~Jni_auto_release_object(){ if(jobj) env->DeleteLocalRef(jobj); }
	operator jobject() const { return jobj; }
};

template <class T>
class Jni_auto_release_array
{
private:
	JNIEnv*		env;
	jbyteArray	jay;
	T*			buffer;
public:
	Jni_auto_release_array(JNIEnv* e, jbyteArray a):env(e),jay(a),buffer(NULL){
		if(env && jay) env->GetByteArrayElements(jay, NULL);
	}
	~Jni_auto_release_array(){ if(buffer) env->ReleaseByteArrayElements(jay,(jbyte*)buffer,JNI_FALSE); }
	operator T* ()	{ return buffer; }
};

#endif //__JAVA_LAME_COMMON_H__