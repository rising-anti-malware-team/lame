* [工程说明](#工程说明)
  * [lame.py](#lame.py)
  * [lame.scanner.py](#lame.scanner.py)
  * [lame.sample.py](#lame.sample.py)

# 工程说明
___
- **lame.py**
>使用python封装的扫描类接口，向用户直接提供。
- **lame.scanner.py**
>使用lame.py接口写的一个扫描器，可直接使用扫描文件。
- **lame.sample.py**
>使用lame.py的简单示例，向用户提供。



## lame.py
___

病毒库相关类(加载、释放)
```python
class VirusDb:
	def __init__(self , lpath);
	def OpenVdb(self,vdbf);
	def CloseVdb(self);
	def GetVbdHandle(self);
```

不带回调函数的扫描类
```python	
class Lame(LameBase):
	def __init__(self , lpath);
	def ScanFile(self,fname);
	def ScanMem(self, data);
	def Clone(self);
```

带回调函数的扫描类
```python
CMPFUNC = WINFUNCTYPE(None,c_char_p, POINTER(LameScanResult),c_void_p) 
def LameCallback(fname , result , userdata);

class LameWithFeedback(LameBase):
	def __init__(self , lpath);
	def SetCallack(self,cb);
	def Report(self , fname , result);
	def ScanFile(self,fname);
	def ScanMem(self, data );
	def Clone(self);
```


## lame.scanner.py
___

不带回调函数的扫描类
```python
class LameScanner:
    def __init__(self , lpath);
    def Load(self , vdb);
    def SetParam(self , param);
    def Scan(self , path_);
    def Unload(self);
    def ShowLicense(self);
    def ShowVersion(self);
```

带回调函数的扫描类
```python
class LameScanner2:
    def __init__(self , lpath);
    def Load(self , vdb);
    def SetParam(self , param);
    def Unload(self);
    def ShowLicense(self);
    def ShowVersion(self);
    def Scan(self , path_);
```


## lame.sample.py
___

简单示例代码
```python
def PrintLameScanResult(fname , result):
    sys.stdout.write(fname)
    if result is not None:
        sys.stdout.write(" " + "  Infected: " + result.vname + " (" + result.engid+")")
    sys.stdout.write("\n")

def lame_test(dbf , path):
    _lame = Lame(LAME_PATH)
    #_lame.SetParam("kill")
    if not _lame.Load(dbf):
        return
    _license = _lame.GetLicense()
    if _license is not None:
        print("License:")
        print("     Version: " + _license.version)
        print("     Owner  : " + _license.Owner)
        print("     Date   : " + _license.Data)
        print("     Authm  : " + _license.Authm)
    _version = _lame.GetVersion()
    if _version is not None: 
        print("Engine Version    :" + _version.engv)
        print("Viruse Lib Version: " + _version.vdbv)
    print(path)
    if os.path.isfile(path) :
        _result = _lame.ScanFile(path)
        PrintLameScanResult(path  , _result)
    else:
        for dirpath,dirnames,filenames in os.walk(path):
            for file in filenames:
                fullpath=os.path.join(dirpath,file)
                _result = _lame.ScanFile(fullpath)
                PrintLameScanResult(fullpath  , _result)
    _lame.Unload()

def LameCallBack(fname , result):
    sys.stdout.write(fname)
    if result is not None:
        sys.stdout.write(" " + "  Infected: " + result.vname + " (" + result.engid+")")
    sys.stdout.write("\n")

def lame_with_feedback_test(dbf , path):
    if dbf is None:
        return
    _lame = LameWithFeedback(LAME_PATH)
    _lame.SetCallack(LameCallBack)
    #_lame.SetParam("kill")
    if not _lame.Load(dbf):
        return
    _license = _lame.GetLicense()
    if _license is not None:
        print("License:")
        print("     Version: " + _license.version)
        print("     Owner  : " + _license.Owner)
        print("     Date   : " + _license.Data)
        print("     Authm  : " + _license.Authm)
    _version = _lame.GetVersion()
    if _version is not None: 
        print("Engine Version    :" + _version.engv)
        print("Viruse Lib Version: " + _version.vdbv)

    if os.path.isfile(path) :
        _lame.ScanFile(path)
    else:
        for dirpath,dirnames,filenames in os.walk(path):
            for file in filenames:
                fullpath=os.path.join(dirpath,file)
                _lame.ScanFile(fullpath)
    _lame.Unload()

```
