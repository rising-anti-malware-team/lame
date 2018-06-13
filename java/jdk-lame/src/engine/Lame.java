package engine;

import java.util.ArrayList;
import java.io.File;
import java.util.List;

public class Lame extends RavEngine /*implements ScanFeedback, ExtractFeedback*/ {	
	protected List<String> parameters = new ArrayList<String>();
	protected long engine = 0;
	protected long vdb_handle = 0;

	public long OpenVdb(String vdbf) {
		return LameOpenVdb(vdbf);
	}
	public void CloseVdb() {
		if(0 != vdb_handle) {
			LameCloseVdb(vdb_handle);
			vdb_handle = 0;
		}
	}
	public boolean Load(long lib) {
		if(0 == lib) {
			lib = OpenVdb(null);
		}
		engine = LameCreate(lib);
		if(0 == engine) return false;
		for(String str:parameters) {
			LameParamSet(engine, str);
		}
		if(0 > LameInit(engine)) {
			LameDestroy(engine);
			return false;
		}
		vdb_handle = lib;
		return true;
	}
	public boolean Set(String param) {
		if(null == param) return false;
		parameters.add(param);
		return true;
	}
	public void Unload() {
		if(0 != vdb_handle) {
			CloseVdb();
			vdb_handle = 0;
		}
		if(0 != engine) {
			LameDestroy(engine);
			engine = 0;
		}
	}
	public LicenceInfo GetLicence() {
		LicenceInfo li = LameGetLicenceInfo();
		if(null == li) return null;
		LicenceInfo info = new LicenceInfo(li);
		LameLicenceRelease(li);
		return info;
	}
	public LameInfo GetVersion() {
		LameInfo li = LameGetVersion();
		if(null == li) return null;
		LameInfo info = new LameInfo(li);
		LameInfoRelease(li);
		return info;
	}
	public ScanResult ScanFile(String fname) {
		if(0 == engine || null == fname) return null;
		ScanResult result = LameScanFile(engine, fname);
		if(null == result) return null;
		ScanResult sr = new ScanResult(result);
		LameScanResultRelease(result);
		return sr;
	}
	public ScanResult ScanMem(byte [] data) {
		if(0 == engine || null == data) return null;
		ScanResult result = LameScanMem(engine, data);
		if(null == result) return null;
		ScanResult sr = new ScanResult(result);
		LameScanResultRelease(result);
		return sr;
	}
	public boolean ScanFileWithCallback(String fname, ScanFeedback cb, Object usrdata) {
		if(null == fname) return false;
		return 0 == LameScanFileWithCallback(engine, fname, cb, usrdata);
	}
	public boolean ScanMemWithCallback(byte [] data, ScanFeedback cb, Object usrdata) {
		if(null == data) return false;
		return 0 == LameScanMemWithCallback(engine, data, cb, usrdata);
	}
	public boolean ExtractFile(String fname, String passwd, ExtractFeedback cb, Object usrdata) {
		if(null == fname) return false;
		return 0 == LameExtractFile(engine, fname, passwd, cb, usrdata);
	}
	public long FileGetSize(long handle) {
		if(0 == handle) return 0;
		return LameFileGetSize(handle);
	}
	public boolean FileSeek(long handle, long offset, long method) {
		if(0 == handle) return false;
		return 0 == LameFileSeek(handle, offset, method);
	}
	public long FileTell(long handle) {
		if(0 == handle) return -1;
		return LameFileTell(handle);
	}
	public long FileRead(long handle, byte[] buf, long size) {
		if(0 == handle || null == buf || 0 >= size) return 0;
		return LameFileRead(handle, buf, size);
	}

	/*
	// test
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

	public void PrintResultl(String path, boolean usecb) {
		ExtractFile(path, null, this, "__test_extract_string__");
		
		if(usecb) {
			ScanFileWithCallback(path, this, "__test_scan_string__");
		}
		else {
			ScanResult sr = ScanFile(path);
			if(null != sr) System.out.println("[normal] " + path + "\tInfected: " + sr.vname);	
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
		Lame lame = new Lame();
		lame.LameTest("d:\\rsdata\\5",true);
		lame.Unload();
	}
	//*/
}
