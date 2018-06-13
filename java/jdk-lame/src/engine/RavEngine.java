package engine;

public class RavEngine {

	//scan file
	public native long LameOpenVdb(String vlibf);
	public native void LameCloseVdb(long vdb);

	public native long LameCreate(long vdb);
	public native void LameDestroy(long lame);
	public native long LameFork(long lame);

	public native long LameParamSet(long lame, String param);
	public native long LameInit(long lame);

	public native ScanResult LameScanFile(long lame, String fname);
	public native long LameScanFileWithCallback(long lame, String fname, ScanFeedback cb, Object usrdata);
	public native ScanResult LameScanMem(long lame, byte [] data);
	public native long LameScanMemWithCallback(long lame, byte [] data, ScanFeedback cb, Object usrdata);
	public native void LameScanResultRelease(ScanResult res);

	public native LameInfo LameGetVersion();
	public native void LameInfoRelease(LameInfo res);

	public native LicenceInfo LameGetLicenceInfo();
	public native void LameLicenceRelease(LicenceInfo res);

	// extract file
	public native long LameExtractFile(long lame, String fname, String password, ExtractFeedback cb, Object usrdata);
	public native long LameFileGetSize(long handle);
	public native long LameFileSeek(long handle, long offset, long method);
	public native long LameFileTell(long handle);
	public native long LameFileRead(long handle, byte [] buf, long size);

	static {
		System.loadLibrary("jlame");
	}

	/*
	public static void main(String[] args) {
		long vdb = LameOpenVdb(null);
		if(0 == vdb) return;
		long lame = LameCreate(vdb);
		if(0 == lame){
			LameCloseVdb(vdb);
			return;
		}
		LameParamSet(lame,"precise-format=0");
		if(0 > LameInit(lame)) {
			LameCloseVdb(vdb);
			LameDestroy(lame);
			return;
		}

		String filename = args[0];

		UserData ud = new UserData();
		LameScanFileWithCallback(lame,filename,ud);

		ScanResult sr = LameScanFile(lame,filename);
		if(null != sr){
			System.out.println(filename + "\t" + sr.vname + "!" + sr.vid32);
			LameScanResultRelease(sr);
		} else {
			System.out.println(filename);
		}

		if(0 != vdb) LameCloseVdb(vdb);
		if(0 != lame) LameDestroy(lame);

		LameInfo lv = LameGetVersion();
		if(null != lv){
			System.out.println("sdk version info");
			System.out.println("engine: " + lv.engine_version);
			System.out.println("malware: " + lv.virus_db_version);
			LameInfoRelease(lv);
		}

		LicenceInfo li = LameGetLicenceInfo();
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
	*/
}
