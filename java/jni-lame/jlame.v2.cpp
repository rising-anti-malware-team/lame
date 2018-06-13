#include "stdafx.h"
#include "jlame.v2.h"


JNIEXPORT jlong JNICALL Java_engine_RavEngine_LameOpenVdb
	(JNIEnv * env, jobject jobj, jstring vlibf)
{
	Jni_auto_release_string jar_vlibf(env,vlibf);
	return (jlong)lame_open_vdb(jar_vlibf);
}

JNIEXPORT void JNICALL Java_engine_RavEngine_LameCloseVdb
	(JNIEnv * env, jobject jobj, jlong vdb)
{
	lame_close_vdb((void*)vdb);
}

JNIEXPORT jlong JNICALL Java_engine_RavEngine_LameCreate
	(JNIEnv * env, jobject jobj, jlong vdb)
{
	if(0 == vdb) return 0;
	return (jlong)lame_create((void*)vdb);
}

JNIEXPORT void JNICALL Java_engine_RavEngine_LameDestroy
	(JNIEnv * env, jobject jobj, jlong lame)
{
	if(0 != lame) lame_destroy((void*)lame);
}

JNIEXPORT jlong JNICALL Java_engine_RavEngine_LameFork
	(JNIEnv * env, jobject jobj, jlong lame)
{
	if(0 == lame) return 0;
	return (jlong)lame_fork((void*)lame);
}

JNIEXPORT jlong JNICALL Java_engine_RavEngine_LameParamSet
	(JNIEnv * env, jobject jobj, jlong lame, jstring param)
{
	if(0 == lame || NULL == param) return E_INVALIDARG;
	Jni_auto_release_string jar_param(env,param);
	return lame_param_set((void*)lame,jar_param);
}

JNIEXPORT jlong JNICALL Java_engine_RavEngine_LameInit
	(JNIEnv * env, jobject jobj, jlong lame)
{
	if(0 == lame) return E_INVALIDARG;
	return lame_init((void*)lame);
}

static jobject local_lame_set_scanresult(JNIEnv * env, jclass objectcls, jobject obj, lame_scan_result * result)
{
	if(NULL != objectcls && NULL != obj && NULL != result)
	{
		jfieldID engid			= env->GetFieldID(objectcls,"engid","Ljava/lang/String;");
		jfieldID vname			= env->GetFieldID(objectcls,"vname","Ljava/lang/String;");
		jfieldID hitag			= env->GetFieldID(objectcls,"hitag","Ljava/lang/String;");
		jfieldID kclass			= env->GetFieldID(objectcls,"kclass","Ljava/lang/String;");
		jfieldID kclass_desc_a	= env->GetFieldID(objectcls,"kclass_desc_a","Ljava/lang/String;");
		jfieldID kclass_desc_w	= env->GetFieldID(objectcls,"kclass_desc_w","Ljava/lang/String;");
		jfieldID mklass			= env->GetFieldID(objectcls,"mklass","I");
		jfieldID vid32			= env->GetFieldID(objectcls,"vid32","I");
		jfieldID treat			= env->GetFieldID(objectcls,"treat","I");

		env->SetObjectField(obj,engid,env->NewStringUTF(result->engid));
		env->SetObjectField(obj,vname,env->NewStringUTF(result->vname));
		env->SetObjectField(obj,hitag,env->NewStringUTF(result->hitag));
		env->SetObjectField(obj,kclass,env->NewStringUTF(result->kclass));
		env->SetObjectField(obj,kclass_desc_a,env->NewStringUTF(result->kclass_desc_a));
		env->SetObjectField(obj,kclass_desc_w,env->NewString((jchar*)result->kclass_desc_w,wcslen(result->kclass_desc_w)));
		env->SetIntField(obj,mklass,result->mklass);
		env->SetIntField(obj,vid32,result->vid32);
		env->SetIntField(obj,treat,result->treat);
	}
	return obj;
}

JNIEXPORT jobject JNICALL Java_engine_RavEngine_LameScanFile
	(JNIEnv * env, jobject jobj, jlong lame, jstring fname)
{
	if(0 == lame || NULL == fname) return NULL;
	Jni_auto_release_string jar_fname(env,fname);
	lame_scan_result result;
	if(S_OK == lame_scan_file((void*)lame,jar_fname,&result))
	{
		jclass objectcls = env->FindClass("engine/ScanResult");
		jobject obj = env->AllocObject(objectcls);
		return local_lame_set_scanresult(env, objectcls, obj, &result);
	}
	return NULL;
}

typedef struct CallbackInfor
{
	jobject			objInterface;
	JNIEnv*			env;
	jobject			objBase;
	jobject			usr_data;
}CallbackInfor;

static JavaVM* loacl_attach_current_thread(JNIEnv* env)
{
	JavaVM* pVm = NULL;
	env->GetJavaVM(&pVm);
	if(NULL != pVm)	pVm->AttachCurrentThread((void**)&env,NULL);
	return pVm;
}

static void local_detach_current_thread(JavaVM* pVm)
{
	if(pVm) pVm->DetachCurrentThread();
}

static LSCT LAMESTDCALLTYPE local_lame_scan_enter_file(const char* file_name, uint32_t depth, void* usr_data)
{
	CallbackInfor* cbinfo = (CallbackInfor*)usr_data;
	if(NULL == cbinfo) return LSCT_CONTINUE;
	JNIEnv* env = cbinfo->env;
	if(NULL == env) return LSCT_CONTINUE;
	JavaVM* pVm = loacl_attach_current_thread(env);
	if(NULL == pVm) return LSCT_CONTINUE;

	jstring obj_filename = NULL;
	jlong jlret = 0;
	do
	{
		jobject obj_ScanFeedback = cbinfo->objInterface;
		if(NULL == obj_ScanFeedback) break;
		jclass cls_ScanFeedback = env->GetObjectClass(obj_ScanFeedback);
		if(NULL == cls_ScanFeedback) break;
		jmethodID mid_DtEnterFile = env->GetMethodID(cls_ScanFeedback,"DtEnterFile","(Ljava/lang/String;JLjava/lang/Object;)J");
		if(NULL == mid_DtEnterFile) break;
		obj_filename = env->NewStringUTF(file_name);
		jlret = env->CallLongMethod(obj_ScanFeedback,mid_DtEnterFile,obj_filename,(jlong)depth,cbinfo->usr_data);
	} while (0);
	if(obj_filename) env->DeleteLocalRef(obj_filename);

	local_detach_current_thread(pVm);
	return (LSCT)jlret;
}

static void LAMESTDCALLTYPE local_lame_scan_leave_file(const char* file_name, uint32_t depth, void* usr_data , LXLVT l)
{
	CallbackInfor* cbinfo = (CallbackInfor*)usr_data;
	if(NULL == cbinfo) return;
	JNIEnv* env = cbinfo->env;
	if(NULL == env) return;
	JavaVM* pVm = loacl_attach_current_thread(env);
	if(NULL == pVm) return;

	jstring obj_filename = NULL;
	do
	{
		jobject obj_ScanFeedback = cbinfo->objInterface;
		if(NULL == obj_ScanFeedback)
			break;
		jclass cls_ScanFeedback = env->GetObjectClass(obj_ScanFeedback);
		if(NULL == cls_ScanFeedback)
			break;
		jmethodID mid_DtLeaveFile = env->GetMethodID(cls_ScanFeedback,"DtLeaveFile","(Ljava/lang/String;JLjava/lang/Object;J)V");
		if(NULL == mid_DtLeaveFile)
			break;
		obj_filename = env->NewStringUTF(file_name);
		env->CallVoidMethod(obj_ScanFeedback,mid_DtLeaveFile,obj_filename,(jlong)depth,cbinfo->usr_data,(jlong)l);
	} while (0);
	if(obj_filename) env->DeleteLocalRef(obj_filename);

	local_detach_current_thread(pVm);
}

static LSCT LAMESTDCALLTYPE local_lame_scan_alarm(const char* file_name, lame_scan_result* result, void* usr_data)
{
	CallbackInfor* cbinfo = (CallbackInfor*)usr_data;
	if (!cbinfo) return LSCT_ABORT;
	JNIEnv* env = cbinfo->env;
	if(NULL == env) return LSCT_CONTINUE;
	JavaVM* pVm = loacl_attach_current_thread(env);
	if(NULL == pVm) return LSCT_CONTINUE;

	jstring obj_fileame = NULL;
	jobject obj_scanresult = NULL;
	jlong jlret = 0;
	do
	{
		jobject obj_ScanFeedback = cbinfo->objInterface;
		if(NULL == obj_ScanFeedback)
			break;
		jclass cls_ScanFeedback = env->GetObjectClass(obj_ScanFeedback);
		if(NULL == cls_ScanFeedback)
			break;
		jmethodID mid_DtAlarm = env->GetMethodID(cls_ScanFeedback,"DtAlarm","(Ljava/lang/String;Lengine/ScanResult;Ljava/lang/Object;)J");
		if(NULL == mid_DtAlarm)
			break;
		obj_fileame = env->NewStringUTF(file_name);
		jclass cls_ScanResult = env->FindClass("engine/ScanResult");
		if(NULL == cls_ScanResult)
			break;
		jmethodID mid_ScanResult = env->GetMethodID(cls_ScanResult, "<init>", "()V");
		obj_scanresult = env->NewObject(cls_ScanResult, mid_ScanResult);
		if(NULL == obj_scanresult)
			break;
		local_lame_set_scanresult(env,cls_ScanResult,obj_scanresult,result);
		jlret = env->CallLongMethod(obj_ScanFeedback,mid_DtAlarm,obj_fileame,obj_scanresult,cbinfo->usr_data);
	} while (0);
	if(obj_fileame) env->DeleteLocalRef(obj_fileame);
	if(obj_scanresult) env->DeleteLocalRef(obj_scanresult);

	local_detach_current_thread(pVm);
	return (LSCT)jlret;
}

JNIEXPORT jlong JNICALL Java_engine_RavEngine_LameScanFileWithCallback
	(JNIEnv * env, jobject jobj, jlong lame, jstring fname, jobject cb, jobject usr_data)
{
	if(0 == lame || NULL == fname || NULL == cb) return E_INVALIDARG;
	Jni_auto_release_string jar_fname(env,fname);

	CallbackInfor cbInfo = {};
	cbInfo.env = env;
	cbInfo.objBase = env->NewLocalRef(jobj);
	cbInfo.objInterface = env->NewLocalRef(cb);
	cbInfo.usr_data = env->NewLocalRef(usr_data);

	lame_scan_feedback feedback = {local_lame_scan_enter_file,local_lame_scan_leave_file,local_lame_scan_alarm};
	HRESULT hr = lame_scan_file_with_callback((void*)lame,jar_fname,&feedback,&cbInfo);

	env->DeleteLocalRef(cb);
	env->DeleteLocalRef(jobj);
	env->DeleteLocalRef(usr_data);
	return hr;
}

JNIEXPORT jobject JNICALL Java_engine_RavEngine_LameScanMem
	(JNIEnv * env, jobject jobj, jlong lame, jbyteArray data)
{
	if(0 == lame || NULL == data) return NULL;
	jsize length = env->GetArrayLength(data);
	Jni_auto_release_array<uint8_t> jay(env, data);
	if( 0 == jay || 0 == length ) return NULL;
	lame_scan_result result;
	if(S_OK == lame_scan_mem((void*)lame, jay, length, &result))
	{
		jclass objectcls = env->FindClass("engine/ScanResult");
		jobject obj = env->AllocObject(objectcls);
		return local_lame_set_scanresult(env,objectcls,obj,&result);
	}
	return NULL;
}

JNIEXPORT jlong JNICALL Java_engine_RavEngine_LameScanMemWithCallback
	(JNIEnv * env, jobject jobj, jlong lame, jbyteArray data, jobject cb, jobject usr_data)
{
	if(0 == lame || NULL == data || NULL == cb) return E_INVALIDARG;
	jsize length = env->GetArrayLength(data);
	Jni_auto_release_array<uint8_t> jay(env, data);
	if( 0 == jay || 0 == length ) return E_INVALIDARG;

	CallbackInfor cbInfo = {};
	cbInfo.env = env;
	cbInfo.objBase = env->NewLocalRef(jobj);
	cbInfo.objInterface = env->NewLocalRef(cb);
	cbInfo.usr_data = env->NewLocalRef(usr_data);

	lame_scan_feedback feedback = {local_lame_scan_enter_file,local_lame_scan_leave_file,local_lame_scan_alarm};
	HRESULT hr = lame_scan_mem_with_callback((void*)lame,jay,length,&feedback,&cbInfo);

	env->DeleteLocalRef(cb);
	env->DeleteLocalRef(jobj);
	env->DeleteLocalRef(usr_data);
	return hr;
}

JNIEXPORT jobject JNICALL Java_engine_RavEngine_LameGetVersion
	(JNIEnv * env, jobject jobj)
{
	lame_info info = {};
	jobject obj = NULL;
	if(S_OK == lame_get_version(&info))
	{
		jclass objectcls = env->FindClass("engine/LameInfo");
		if(objectcls)
		{
			obj = env->AllocObject(objectcls);
			jfieldID engine_version		= env->GetFieldID(objectcls,"engine_version","Ljava/lang/String;");
			jfieldID virus_db_version	= env->GetFieldID(objectcls,"virus_db_version","Ljava/lang/String;");

			env->SetObjectField(obj,engine_version,env->NewStringUTF(info.engv));
			env->SetObjectField(obj,virus_db_version,env->NewStringUTF(info.vdbv));
		}
	}
	return obj;
}

JNIEXPORT jobject JNICALL Java_engine_RavEngine_LameGetLicenceInfo
	(JNIEnv * env, jobject jobj)
{
	rx_licence_info info = {};
	jobject obj = NULL;
	if(S_OK == lame_get_licence_info(&info))
	{
		jclass objectcls = env->FindClass("engine/LicenceInfo");
		if(objectcls)
		{
			obj = env->AllocObject(objectcls);
			jfieldID Version	= env->GetFieldID(objectcls,"Version","Ljava/lang/String;");
			jfieldID Owner		= env->GetFieldID(objectcls,"Owner","Ljava/lang/String;");
			jfieldID Date		= env->GetFieldID(objectcls,"Date","Ljava/lang/String;");
			jfieldID Authm		= env->GetFieldID(objectcls,"Authm","Ljava/lang/String;");
			jfieldID Data		= env->GetFieldID(objectcls,"Data","Ljava/lang/String;");

			env->SetObjectField(obj,Version,env->NewStringUTF(info.Version));
			env->SetObjectField(obj,Owner,env->NewStringUTF(info.Owner));
			env->SetObjectField(obj,Date,env->NewStringUTF(info.Date));
			env->SetObjectField(obj,Authm,env->NewStringUTF(info.Authm));
			env->SetObjectField(obj,Data,env->NewStringUTF(info.Data));
		}
	}
	return obj;
}

JNIEXPORT void JNICALL Java_engine_RavEngine_LameScanResultRelease
	(JNIEnv * env, jobject jobj, jobject obj)
{
	if(NULL == obj) return;
	env->DeleteLocalRef(obj);
}

JNIEXPORT void JNICALL Java_engine_RavEngine_LameInfoRelease
	(JNIEnv * env, jobject jobj, jobject obj)
{
	if(NULL == obj) return;
	env->DeleteLocalRef(obj);
}

JNIEXPORT void JNICALL Java_engine_RavEngine_LameLicenceRelease
	(JNIEnv * env, jobject jobj, jobject obj)
{
	if(NULL == obj) return;
	env->DeleteLocalRef(obj);
}

//////////////////////////////////////////////////////////////////////////

static LXCT LAMESTDCALLTYPE local_lame_extract_enter_file(const char* file_name , const char* format ,
	uint32_t depth , uint64_t FCLS ,void* handle , void* usr_data)
{
	CallbackInfor* cbinfo = (CallbackInfor*)usr_data;
	if(NULL == cbinfo) return LCT_DO_EXTRACT;
	JNIEnv* env = cbinfo->env;
	if(NULL == env) return LCT_DO_EXTRACT;
	JavaVM* pVm = loacl_attach_current_thread(env);
	if(NULL == pVm) return LCT_DO_EXTRACT;

	jstring obj_filename = NULL, obj_format = NULL;
	jlong jlret = 0;
	do
	{
		jobject obj_ExtractFeedback = cbinfo->objInterface;
		if(NULL == obj_ExtractFeedback)
			break;
		jclass cls_ExtractFeedback = env->GetObjectClass(obj_ExtractFeedback);
		if(NULL == cls_ExtractFeedback)
			break;
		jmethodID mid_ExtEnterFile = env->GetMethodID(cls_ExtractFeedback,"ExtEnterFile","(Ljava/lang/String;Ljava/lang/String;JJJLjava/lang/Object;)J");
		if(NULL == mid_ExtEnterFile)
			break;
		obj_filename = env->NewStringUTF(file_name);
		obj_format = env->NewStringUTF(format);
		jlret = env->CallLongMethod(obj_ExtractFeedback,mid_ExtEnterFile,obj_filename,obj_format,(jlong)depth,(jlong)FCLS,(jlong)handle,cbinfo->usr_data);
	} while (0);
	if(obj_filename) env->DeleteLocalRef(obj_filename);
	if(obj_format) env->DeleteLocalRef(obj_format);

	return LCT_DO_EXTRACT;
}

static void LAMESTDCALLTYPE local_lame_extract_leave_file(const char* file_name , const char* format ,
	uint32_t depth , uint64_t FCLS ,void* handle , void* usr_data , LXLVT l)
{
	CallbackInfor* cbinfo = (CallbackInfor*)usr_data;
	if(NULL == cbinfo) return;
	JNIEnv* env = cbinfo->env;
	if(NULL == env) return;
	JavaVM* pVm = loacl_attach_current_thread(env);
	if(NULL == pVm) return;

	jstring obj_filename = NULL, obj_format = NULL;
	do
	{
		jobject obj_ExtractFeedback = cbinfo->objInterface;
		if(NULL == obj_ExtractFeedback)
			break;
		jclass cls_ExtractFeedback = env->GetObjectClass(obj_ExtractFeedback);
		if(NULL == cls_ExtractFeedback)
			break;
		jmethodID mid_ExtLeaveFile = env->GetMethodID(cls_ExtractFeedback,"ExtLeaveFile","(Ljava/lang/String;Ljava/lang/String;JJJLjava/lang/Object;J)V");
		if(NULL == mid_ExtLeaveFile)
			break;
		obj_filename = env->NewStringUTF(file_name);
		obj_format = env->NewStringUTF(format);
		env->CallVoidMethod(obj_ExtractFeedback,mid_ExtLeaveFile,obj_filename,obj_format,(jlong)depth,(jlong)FCLS,(jlong)handle,cbinfo->usr_data,(jlong)l);
	} while (0);
	if(obj_filename) env->DeleteLocalRef(obj_filename);
	if(obj_format) env->DeleteLocalRef(obj_format);

	local_detach_current_thread(pVm);
}


JNIEXPORT jlong JNICALL Java_engine_RavEngine_LameExtractFile
	(JNIEnv * env, jobject jobj, jlong lame, jstring fname, jstring password, jobject cb, jobject usr_data)
{
	if(0 == lame || NULL == fname || NULL == cb) return E_FAIL;
	Jni_auto_release_string jar_fname(env,fname);
	Jni_auto_release_string jar_password(env,password);

	CallbackInfor cbInfo = {};
	cbInfo.env = env;
	cbInfo.objBase = env->NewLocalRef(jobj);
	cbInfo.objInterface = env->NewLocalRef(cb);
	cbInfo.usr_data = env->NewLocalRef(usr_data);

	lame_extract_feedback feedback = {local_lame_extract_enter_file, local_lame_extract_leave_file};
	HRESULT hr = lame_extract_file((void*)lame,jar_fname,jar_password,&feedback,&cbInfo);

	env->DeleteLocalRef(cb);
	env->DeleteLocalRef(jobj);
	env->DeleteLocalRef(usr_data);
	return hr;
}

JNIEXPORT jlong JNICALL Java_engine_RavEngine_LameFileGetSize
	(JNIEnv * env, jobject jobj, jlong handle)
{
	if(0 == handle) return 0;
	return lame_file_get_size((void*)handle);
}

JNIEXPORT jlong JNICALL Java_engine_RavEngine_LameFileSeek
	(JNIEnv * env, jobject jobj, jlong handle, jlong offset, jlong method)
{
	if(0 == handle) return E_FAIL;
	return lame_file_seek((void*)handle,offset,(LSMETHOD)method);
}

JNIEXPORT jlong JNICALL Java_engine_RavEngine_LameFileTell
	(JNIEnv * env, jobject jobj, jlong handle)
{
	if(0 == handle) return -1;
	return lame_file_tell((void*)handle);
}

JNIEXPORT jlong JNICALL Java_engine_RavEngine_LameFileRead
	(JNIEnv * env, jobject jobj, jlong handle, jbyteArray buf, jlong size)
{
	if(0 == handle || NULL == buf || size <= 0) return 0;
	jsize length = env->GetArrayLength(buf);
	length = (size > length) ? length : size;
	uint8_t* lpBuffer = new uint8_t[length];
	if(NULL == lpBuffer) return 0;
	long lReadBytes = lame_file_read((void*)handle,lpBuffer,length);
	if(lReadBytes > 0) env->SetByteArrayRegion(buf,0,lReadBytes,(jbyte*)lpBuffer);
	delete [] lpBuffer;
	return lReadBytes;
}