* [工程说明](#工程说明)
  * [lame.dotnet](#lame.dotnet)
  * [lame.dotnet.scanner](#lame.dotnet.scanner)
  * [lame.dotnet.sample](#lame.dotnet.sample)


# 工程说明
___
- **lame.dotnet**
>本工程是C转C#的接口。
- **lame.dotnet.scanner**
>本工程是C#封装的接口，利用lame.dotnet封装的C#接口封装成Lame类供用户使用。
- **lame.dotnet.sample**
>本工程是测试用例代码，利用lame.dotnet封装的C#接口，使用了两种方式扫描文件，一种是直接扫描一种是使用回调函数扫描。



## lame.dotnet
___

Lame：扫描文件接口类。
```c
public class Lame : LameBase
{
	public LameScanResult ScanFile(string sFile);
	public LameScanResult ScanMem(byte[] bytes);
	public object Clone();
}
```

LameWithEvent：带回调函数的扫描文件接口类。
```c
public delegate void ScanInternalFileEvent(string fname, LameScanResult result);
public class LameWithEvent : LameBase {
	public void ScanFile(string sFile);
	public void ScanMem(byte[] bytes);
	private void ScanInternalFile(string fname, IntPtr resp, IntPtr zero);
	public object Clone();
	private LameScanResult FetchResult(IntPtr resp);
}
```

VirusLib：病毒库加载和释放类
```c
public class VirusLib
{
	public bool lame_open_vdb(string vdbf);
	public void lame_close_vdb();
	public IntPtr vdb_handle;
}
```


## lame.dotnet.scanner
___

IScan：扫描文件接口基类
```c
interface IScan
{
	bool SetParam(string param);
	bool Load();
	void Scan(string path);
	IScan Clone();
	void ShowVersion();
	void ShowLicense();
	void UnLoad();
}
```

Scanner：扫描器类
```c
class Scanner : IScan
{
	public Scanner(VirusLib vdb);
	public Scanner(Lame lame ,VirusLib vdb );
	public bool SetParam(string param);
	public bool Load();
	public void Scan(string path);
	public IScan Clone();
	public void ShowVersion();
	public void ShowLicense();
	public void UnLoad();
}
```

ScannerEx：带回调函数的扫描器类
```c
class ScannerEx : IScan
{
	public ScannerEx(VirusLib vdb);
	public ScannerEx(LameWithEvent lame, VirusLib vdb);
	public bool SetParam(string param);
	public bool Load();
	public IScan Clone();
	public void Scan(string path);
	public void ShowVersion();
	public void ShowLicense();
	public void UnLoad();
}
```


## lame.dotnet.sample
___


扫描器示例
```c
class Program
{
	static void Main(string[] args)
	{
		if (args.Length == 0) return;
		VirusLib vdb = new VirusLib();
		if (!vdb.lame_open_vdb(null))
		{
			Console.WriteLine("Faild to load virus lib.");
			return;
		}
		LameTest(vdb, args[0]);
		LameWithEventTest(vdb, args[0]);
		vdb.lame_close_vdb();
	}
	static void PrintScanResult(string path , LameScanResult result) 
	{
		if (result == null) return;
		Console.Write(path);
		if (result != null)
		{
			Console.Write("     Infected:" + result.VirusName + " (" + result.EngineID + ")");
		}
		Console.WriteLine("");
	}
	static void LameTest(VirusLib vdb , string path) 
	{
		if (vdb == null) return;
		// 1. load
		Lame _lame = new Lame();
		if (!_lame.Load(vdb)) return;
		//2. scan
		if (File.Exists(path))
		{
			LameScanResult _result = _lame.ScanFile(path);
			PrintScanResult(path, _result);
		}
		else if (Directory.Exists(path))
		{
			string[] files = Directory.GetFiles(path);
			foreach (string f in files)
			{
				LameScanResult _result = _lame.ScanFile(f);
				PrintScanResult(f, _result);
			}
			//travel dir......
		}
		//3.
		_lame.Unload();
	}
	static void LameWithEventTest(VirusLib vdb, string path)
	{
		if (vdb == null) return;
		// 1. load
		LameWithEvent _lame = new LameWithEvent();
		_lame.SetEventHandle(PrintScanResult);
		if (!_lame.Load(vdb)) return;
		//2. scan
		if (File.Exists(path))
		{
			_lame.ScanFile(path);
		}
		else if (Directory.Exists(path))
		{
			string[] files = Directory.GetFiles(path);
			foreach (string f in files)
			{
				_lame.ScanFile(f);
			}
			//travel dir......
		}
		//3.
		_lame.Unload();
	}
}
```
