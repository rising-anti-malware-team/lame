* [工程说明](#工程说明)
  * [Jni-lame](#jni-lame)
  * [Jdk-lame](#jdk-lame)
  * [Jdk-test](#jdk-test)


# 工程说明
___
- **jni-lame**
>本工程是C转JAVA的接口。
- **jdk-lame**
>本工程是JAVA封装的接口，利用jni-lame封装的C接口封装成Lame类供用户使用，最终生成jar包文件。
- **jdk-test**
>本工程是测试代码用例，导入jdk-lame生成的jar包，使用了两种方式扫描文件，一种是直接扫描一种是使用回调函数扫描。



## jni-lame
___

根据java工程封装的jni接口函数。
```c
EXTERN_C JNIEXPORT jlong JNICALL Java_engine_RavEngine_LameOpenVdb(JNIEnv*, jobject, jstring);
EXTERN_C JNIEXPORT void JNICALL Java_engine_RavEngine_LameCloseVdb(JNIEnv*, jobject, jlong);

EXTERN_C JNIEXPORT jlong JNICALL Java_engine_RavEngine_LameCreate(JNIEnv*, jobject, jlong);
EXTERN_C JNIEXPORT void JNICALL Java_engine_RavEngine_LameDestroy(JNIEnv*, jobject, jlong);
EXTERN_C JNIEXPORT jlong JNICALL Java_engine_RavEngine_LameFork(JNIEnv*, jobject, jlong);
EXTERN_C JNIEXPORT jlong JNICALL Java_engine_RavEngine_LameParamSet(JNIEnv*, jobject, jlong, jstring);
EXTERN_C JNIEXPORT jlong JNICALL Java_engine_RavEngine_LameInit(JNIEnv*, jobject, jlong);

EXTERN_C JNIEXPORT jobject JNICALL Java_engine_RavEngine_LameScanFile(JNIEnv*, jobject, jlong, jstring);
EXTERN_C JNIEXPORT jlong JNICALL Java_engine_RavEngine_LameScanFileWithCallback (JNIEnv *, jobject, jlong, jstring, jobject, jobject);
EXTERN_C JNIEXPORT jobject JNICALL Java_engine_RavEngine_LameScanMem(JNIEnv*, jobject, jlong, jbyteArray);
EXTERN_C JNIEXPORT jlong JNICALL Java_engine_RavEngine_LameScanMemWithCallback(JNIEnv *, jobject, jlong, jbyteArray, jobject, jobject);
EXTERN_C JNIEXPORT void JNICALL Java_engine_RavEngine_LameScanResultRelease(JNIEnv*, jobject, jobject);

EXTERN_C JNIEXPORT jobject JNICALL Java_engine_RavEngine_LameGetVersion(JNIEnv*, jobject);
EXTERN_C JNIEXPORT void JNICALL Java_engine_RavEngine_LameInfoRelease(JNIEnv*, jobject, jobject);

EXTERN_C JNIEXPORT jobject JNICALL Java_engine_RavEngine_LameGetLicenceInfo(JNIEnv*, jobject);
EXTERN_C JNIEXPORT void JNICALL Java_engine_RavEngine_LameLicenceRelease(JNIEnv*, jobject, jobject);

JNIEXPORT jlong JNICALL Java_engine_RavEngine_LameExtractFile(JNIEnv *, jobject, jlong, jstring, jstring, jobject, jobject);
JNIEXPORT jlong JNICALL Java_engine_RavEngine_LameFileGetSize(JNIEnv *, jobject, jlong);
JNIEXPORT jlong JNICALL Java_engine_RavEngine_LameFileSeek(JNIEnv *, jobject, jlong, jlong, jlong);
JNIEXPORT jlong JNICALL Java_engine_RavEngine_LameFileTell(JNIEnv *, jobject, jlong);
JNIEXPORT jlong JNICALL Java_engine_RavEngine_LameFileRead(JNIEnv *, jobject, jlong, jbyteArray, jlong);
```


## jdk-lame
___

扫描回调函数接口类
```java
public interface ScanFeedback {
	public abstract long DtEnterFile(String file_name, long depth, Object usr_data);
	public abstract void DtLeaveFile(String file_name, long depth, Object usr_data, long l);
	public abstract long DtAlarm(String file_name, ScanResult sr, Object usr_data);
}
```

根据jni函数封装好的java扫描接口类。
```java
public class Lame extends RavEngine {
	public long OpenVdb(String vdbf);
	public void CloseVdb();
	public boolean Load(long lib);
	public boolean Set(String param);
	public void Unload();
	public LicenceInfo GetLicence();
	public LameInfo GetVersion();
	public ScanResult ScanFile(String fname);
	public ScanResult ScanMem(byte [] data);
	public boolean ScanFileWithCallback(String fname,ScanFeedback cb, Object usrdata);
	public boolean ScanMemWithCallback(byte [] data,ScanFeedback cb, Object usrdata);
}
```

根据jni函数封装好的java文件解包接口类。
```java
public class Lame extends RavEngine {
	public boolean ExtractFile(String fname, String passwd, ExtractFeedback cb, Object usrdata);
	public long FileGetSize(long handle);
	public boolean FileSeek(long handle, long offset, long method);
	public long FileTell(long handle);
	public long FileRead(long handle, byte[] buf, long size);
}
```


## jdk-test
___

此示例中类scan使用回调或者不使用回调函数两种扫描方式都实现了，如果不使用回调函数则可以不实现JdkInterface接口函数。
```java
public class scan extends Lame implements ScanFeedback, ExtractFeedback {

	// test Jni API
	public void TestInterface(String filename) {
		long vdb = OpenVdb(null);
		if(0 == vdb) return;
		if(!Load(vdb)){
			Unload();
			return;
		}
		Set("precise-format=0");
		ScanResult sr = ScanFile(filename);
		if(null != sr){
			System.out.println(filename + "\t" + sr.vname + "!" + sr.vid32);
			LameScanResultRelease(sr);
		} else {
			System.out.println(filename);
		}
		Unload();

		LameInfo lv = GetVersion();
		if(null != lv){
			System.out.println("sdk version info");
			System.out.println("engine: " + lv.engine_version);
			System.out.println("malware: " + lv.virus_db_version);
			LameInfoRelease(lv);
		}
		
		LicenceInfo li = GetLicence();
		if(null != li){
			System.out.println("licence info");
			System.out.println("version: " + li.Version);
			System.out.println("Owner: " + li.Owner);
			System.out.println("Date: " + li.Date);
			System.out.println("Authm: " + li.Authm);
			System.out.println("Data: " + li.Data);
			LameLicenceRelease(li);
		}
	}
	//callbcak
	public long DtEnterFile(String file_name, long depth, Object usr_data) {
		System.out.println("[callback] ++ " + file_name + " && " + depth + " && " + usr_data);
		return 0;
	}
	public void DtLeaveFile(String file_name, long depth, Object usr_data, long l) {
		System.out.println("[callback] -- " + file_name + " && " + depth + " && " + usr_data + " && " + l);	
	}
	public long DtAlarm(String file_name, ScanResult sr, Object usr_data) {
		System.out.println("[callback] ** " + file_name + " && " + sr.vname + "!" + sr.vid32 + " && " + usr_data);
		return 1;
	}
	public long ExtEnterFile(String file_name, String format, long depth, long FCLS, long handle, Object usr_data) {
		//System.out.println("[callback] ++ " + file_name + " && " + format + " && " + depth + " && " + FCLS + " && " + handle + " && " + usr_data);
		if(depth >= 1) {
			long size = FileGetSize(handle);
			long offset = FileTell(handle);
			if(FileSeek(handle,0,0)) {
				byte [] buffer = new byte [(int)size]; 
				long lbytes = FileRead(handle, buffer, size);
				System.out.println(file_name + " " + size + " " + offset + " " + lbytes);	
			}	
		}
		return 1;
	}
	public void ExtLeaveFile(String file_name, String format, long depth, long FCLS, long handle, Object usr_data, long l) {
		//System.out.println("[callback] -- " + file_name + " && " + format + " && " + depth + " && " + FCLS + " && "  + handle + " && " + usr_data + " && " + l);
	}
	// test lame API
	public void PrintResultl(String path, boolean usecb) {
		ExtractFile(path, null, this, "__test_extract_string__");
		if(usecb) {
			ScanFileWithCallback(path, this, "__test_scan_string__");
		}
		else {
			ScanResult sr = ScanFile(path);
			if(null != sr) {
				System.out.println("[normal] " + path + "\tInfected: " + sr.vname);
			} else {
				System.out.println("[normal] " + path);
			}
		}
	}
	public void TravelPath(String path, boolean usecb) {
		File or = new File(path);
		if(!or.exists()) return;
		
		if(or.isDirectory()) {
			File [] files = or.listFiles();
			if (files != null) {
				for (File file : files) {
					if (file.isFile()) {
						PrintResultl(file.getAbsolutePath(), usecb);
					} else if (file.isDirectory()) {
						TravelPath(file.getAbsolutePath(), usecb);
					}
				}
			}
		} else if(or.isFile()) {
			PrintResultl(or.getAbsolutePath(), usecb);
		}
	}
	void LameTest(String path,boolean usecb) {
		File file = new File(path);
		if(!file.exists()) return;
		if(!Load(0)) return;
		TravelPath(path,usecb);
		Unload();
	}
	
	public static void main(String[] args) {
		System.out.println("------------ main start ----------------------");
		scan s = new scan();
		String path = args[0];
		boolean usecb = true;
		s.LameTest(path,usecb);
		s.Unload();
		System.out.println("------------ main end ----------------------");
	}
}
```
