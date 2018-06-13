package run;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;

import engine.ExtractFeedback;
import engine.ScanFeedback;
import engine.LicenceInfo;
import engine.LameInfo;
import engine.ScanResult;
import engine.Lame;

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
	public boolean JudgeFileExist(String filename) {
		File file = new File(filename);
		if (file.exists()) {
			file.delete();
		}
		try {
			file.createNewFile();
			return true;
		} catch (IOException e) {
			e.printStackTrace();
			return false;
		}
	}
	public void DumpFile(long handle, String filename) {
		OutputStream os = null;
		try {
			int size = (int)FileGetSize(handle);
			int offset = 0;
			if (!JudgeFileExist(filename)) return;
			os = new FileOutputStream(filename, true);
			while (size > 0) {
				if(FileSeek(handle,offset,0)) {
					int bytesRead = 0x1000 < size ? 0x1000 : size;
					byte [] buffer = new byte [(int)bytesRead];
					if (bytesRead != FileRead(handle, buffer, size)) {
						return;
					}
					os.write(buffer);
					offset += bytesRead;
					size -= bytesRead;	
				}
			}	
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				os.close();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}
	public long ExtEnterFile(String file_name, String format, long depth, long FCLS, long handle, Object usr_data) {
		System.out.println("[callback] ++ " + file_name + " && " + format + " && " + depth + " && " + FCLS + " && " + handle + " && " + usr_data);
		if(depth >= 1) {
			String filename = System.getProperty("user.dir") + "/" + file_name;
			DumpFile(handle, filename);	
		}
		return 1;
	}
	public void ExtLeaveFile(String file_name, String format, long depth, long FCLS, long handle, Object usr_data, long l) {
		System.out.println("[callback] -- " + file_name + " && " + format + " && " + depth + " && " + FCLS + " && "  + handle + " && " + usr_data + " && " + l);
	}
	// test lame API
	public void ScanFile(String path, boolean usecb) {
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
						ScanFile(file.getAbsolutePath(), usecb);
					} else if (file.isDirectory()) {
						TravelPath(file.getAbsolutePath(), usecb);
					}
				}
			}
		} else if(or.isFile()) {
			ScanFile(or.getAbsolutePath(), usecb);
		}
	}
	void Detect(String path,boolean usecb) {
		File file = new File(path);
		if(!file.exists()) return;
		if(!Load(0)) return;
		TravelPath(path,usecb);
		Unload();
	}
	void Extract(String path) {
		if(!Load(0)) return;
		ExtractFile(path, null, this, "__test_extract_string__");
		Unload();
	}

	public static void main(String[] args) {
		System.out.println("------------ main start ----------------------");
		String path = args[0];
		scan s = new scan();
		s.Detect(path, true);
		s.Extract(path);
		s.Unload();
		System.out.println("------------ main end ----------------------");
	}
}