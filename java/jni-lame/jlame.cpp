// jlame.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "jlame.h"


JNIEXPORT jlong JNICALL Java_engine_method_RxObjectCreate
  (JNIEnv * env, jclass jcls, jint objid)
{
	return (jlong)rx_obj_create((rx_obj_id_t)objid);
}

JNIEXPORT void JNICALL Java_engine_method_RxObjectReference
  (JNIEnv * env, jclass jcls, jlong obj)
{
	rx_obj_reference((rx_obj_t)obj);
}

JNIEXPORT void JNICALL Java_engine_method_RxObjectRelease
  (JNIEnv * env, jclass jcls, jlong obj)
{
	rx_obj_release((rx_obj_t)obj);
}

JNIEXPORT jlong JNICALL Java_engine_method_RxConfigureSetJson
  (JNIEnv * env, jclass jcls, jlong conf, jstring content)
{
	if(0 == conf || NULL == content) return E_INVALIDARG;
	Jni_auto_release_string jar_content(env,content);
	return rx_conf_set_json((rx_obj_t)conf,jar_content);
}

JNIEXPORT jlong JNICALL Java_engine_method_RxConfigureSetStr
  (JNIEnv * env, jclass jcls, jlong conf, jstring path, jstring val)
{
	if(0 == conf || NULL == path || NULL == val) return E_INVALIDARG;
	Jni_auto_release_string jar_path(env,path);
	Jni_auto_release_string jar_val(env,val);
	return rx_conf_set_str((rx_obj_t)conf,jar_path,jar_val);
}

JNIEXPORT jlong JNICALL Java_engine_method_RxConfigureSetInt
  (JNIEnv * env, jclass jcls, jlong conf, jstring path, jlong val)
{
	if(0 == conf || NULL == path) return E_INVALIDARG;
	Jni_auto_release_string jar_path(env,path);
	return rx_conf_set_int((rx_obj_t)conf,jar_path,val);
}

JNIEXPORT jlong JNICALL Java_engine_method_RxConfigureSetBool
  (JNIEnv * env, jclass jcls, jlong conf, jstring path, jboolean val)
{
	if(0 == conf || NULL == path) return E_INVALIDARG;
	Jni_auto_release_string jar_path(env,path);
	return rx_conf_set_bool((rx_obj_t)conf,jar_path,val?true:false);
}

JNIEXPORT jlong JNICALL Java_engine_method_RxConfigureErase
  (JNIEnv * env, jclass jcls, jlong conf, jstring path)
{
	if(0 == conf || NULL == path) return E_INVALIDARG;
	Jni_auto_release_string jar_path(env,path);
	return rx_conf_erase((rx_obj_t)conf,jar_path);
}

JNIEXPORT jstring JNICALL Java_engine_method_RxConfigureGetStr
  (JNIEnv * env, jclass jcls, jlong conf, jstring path)
{
	if(0 == conf || NULL == path) return NULL;
	Jni_auto_release_string jar_path(env,path);
	rx_cstr_t v = NULL;
	jstring str = NULL;
	if(S_OK==rx_conf_get_str((rx_obj_t)conf,jar_path,&v))
	{
		str = env->NewStringUTF(v);
	}
	return str;
}

JNIEXPORT jlong JNICALL Java_engine_method_RxConfigureGetInt
  (JNIEnv * env, jclass jcls, jlong conf, jstring path)
{
	if(0 == conf || NULL == path) return 0;
	Jni_auto_release_string jar_path(env,path);
	int64_t val = 0;
	rx_conf_get_int((rx_obj_t)conf,jar_path,&val);
	return val;
}


JNIEXPORT jboolean JNICALL Java_engine_method_RxConfigureGetBool
  (JNIEnv * env, jclass jcls, jlong conf, jstring path)
{
	if(0 == conf || NULL == path) return JNI_FALSE;
	Jni_auto_release_string jar_path(env,path);
	bool val = false;
	rx_conf_get_bool((rx_obj_t)conf,jar_path,&val);
	return val ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jlong JNICALL Java_engine_method_RxConfigureCloudInit
  (JNIEnv * env, jclass jcls, jlong conf)
{
	if(0 == conf) return E_INVALIDARG;
	return rx_conf_cloud_init((rx_obj_t)conf);
}

JNIEXPORT jlong JNICALL Java_engine_method_RxLameInit
  (JNIEnv * env, jclass jcls, jlong engine, jlong conf)
{
	if(0 == conf || 0 == engine) return E_INVALIDARG;
	return rx_lame_init((rx_obj_t)engine,(rx_obj_t)conf);
}

JNIEXPORT jlong JNICALL Java_engine_method_RxLameScan
  (JNIEnv * env, jclass jcls, jlong engine, jstring rx_url, jint opt)
{
	if(0 == engine || NULL == rx_url) return 0;
	Jni_auto_release_string jar_url(env,rx_url);
	return (jlong)rx_lame_scan((rx_obj_t)engine,jar_url,(rx_scan_opt_t)opt);
}

JNIEXPORT jlong JNICALL Java_engine_method_RxLameScanMemory2
  (JNIEnv * env, jclass jcls, jlong engine, jstring buffer, jint length)
{
	if(0 == engine || NULL == buffer || 0 == length) return 0;
	Jni_auto_release_string jar_buffer(env,buffer);
	return (jlong)rx_lame_scan_memory((rx_obj_t)engine,jar_buffer,length);
}

JNIEXPORT jlong JNICALL Java_engine_method_RxLameScanMemory
  (JNIEnv * env, jclass jcls, jlong engine, jbyteArray buffer)
{
	if( 0 == engine || NULL == buffer ) return 0;
	jsize  length = env->GetArrayLength(buffer);
	Jni_auto_release_array<char> jay(env,buffer);
	if( 0 == jay || 0 == length ) return 0;
	return (jlong)rx_lame_scan_memory((rx_obj_t)engine, jay, length);
}

JNIEXPORT jobject JNICALL Java_engine_method_RxSdrGetResult
  (JNIEnv * env, jclass jcls, jlong sdr)
{
	if(0 == sdr) return NULL;
	rx_result_t ret;
	jobject obj = NULL;
	if(rx_sdr_get_result((rx_sdr_t)sdr,&ret))
	{
		jclass objectcls = env->FindClass("engine/DetectResult");
		if(objectcls)
		{
			obj = env->AllocObject(objectcls);
			jfieldID engid = env->GetFieldID(objectcls,"engid","Ljava/lang/String;");
			jfieldID vname = env->GetFieldID(objectcls,"vname","Ljava/lang/String;");
			jfieldID hitag = env->GetFieldID(objectcls,"hitag","Ljava/lang/String;");
			jfieldID mklass = env->GetFieldID(objectcls,"mklass","I");
			jfieldID vid32 = env->GetFieldID(objectcls,"vid32","I");
			jfieldID treat = env->GetFieldID(objectcls,"treat","I");
			//jfieldID vid40 = env->GetFieldID(objectcls,"vid40","L");

			env->SetObjectField(obj,engid,env->NewStringUTF(ret.engid));
			env->SetObjectField(obj,vname,env->NewStringUTF(ret.vname));
			env->SetObjectField(obj,hitag,env->NewStringUTF(ret.hitag));
			env->SetIntField(obj,mklass,ret.mklass);
			env->SetIntField(obj,vid32,ret.vid32);
			env->SetIntField(obj,treat,ret.treat);
			//env->SetLongField(obj,vid40,ret.vid40);
		}
	}
	return obj;
}

JNIEXPORT void JNICALL Java_engine_method_RxSdrReleaseResult
	(JNIEnv * env, jclass jcls, jobject obj)
{
	if(NULL == obj) return;
	env->DeleteLocalRef(obj);
}

JNIEXPORT jstring JNICALL Java_engine_method_RxLameGetVersion
	(JNIEnv * env, jclass jcls)
{
	char ver[32] = {0};
	jstring str = NULL;
	if(rx_lame_get_version(ver, sizeof(ver)/sizeof(ver[0])))
		str = env->NewStringUTF(ver);
	return str;
}

JNIEXPORT jstring JNICALL Java_engine_method_RxLameGetVirusLibVersion
  (JNIEnv * env, jclass jcls, jstring path)
{
	if(NULL == path) return NULL;
	Jni_auto_release_string jar_buffer(env,path);
	char ver[32] = {0};
	jstring str = NULL;
	if(rx_lame_get_virus_library_version(ver, sizeof(ver)/sizeof(ver[0]), jar_buffer))
		str = env->NewStringUTF(ver);
	return str;
}


JNIEXPORT jobject JNICALL Java_engine_method_RxLameGetLicenceInfo
  (JNIEnv * env, jclass jcls, jstring path)
{
	if(NULL == path) return NULL;
	Jni_auto_release_string jar_buffer(env,path);
	rx_licence_info info = {0};
	jobject obj = NULL;
	if(rx_lame_get_licence_info(&info, jar_buffer))
	{
		jclass objectcls = env->FindClass("engine/LicenceInfo");
		if(objectcls)
		{
			obj = env->AllocObject(objectcls);
			jfieldID Version = env->GetFieldID(objectcls,"Version","Ljava/lang/String;");
			jfieldID Owner = env->GetFieldID(objectcls,"Owner","Ljava/lang/String;");
			jfieldID Date = env->GetFieldID(objectcls,"Date","Ljava/lang/String;");
			jfieldID Authm = env->GetFieldID(objectcls,"Authm","Ljava/lang/String;");
			jfieldID Data = env->GetFieldID(objectcls,"Data","Ljava/lang/String;");

			env->SetObjectField(obj,Version,env->NewStringUTF(info.Version));
			env->SetObjectField(obj,Owner,env->NewStringUTF(info.Owner));
			env->SetObjectField(obj,Date,env->NewStringUTF(info.Date));
			env->SetObjectField(obj,Authm,env->NewStringUTF(info.Authm));
			env->SetObjectField(obj,Data,env->NewStringUTF(info.Data));
		}
	}
	return obj;
}

JNIEXPORT void JNICALL Java_engine_method_RxLameReleaseLicenceInfo
	(JNIEnv * env, jclass jcls, jobject obj)
{
	if(NULL == obj) return;
	env->DeleteLocalRef(obj);
}
