* [工程说明](#工程说明)
  * [lame.go](#lame.go)
  * [lame.go.scanner](#lame.go.scanner)
  * [lame.go.sample](#lame.go.sample)


# 工程说明
___
- **lame.go**
>本工程是C转go的接口。
- **lame.go.scanner**
>本工程是go封装的接口，利用lame.go封装的go接口封装成Lame类供用户使用。
- **lame.go.sample**
>本工程是测试用例代码，利用lame.go封装的go接口，使用了两种方式扫描文件，一种是直接扫描一种是使用回调函数扫描。



## lame.go
___

Lame：扫描文件接口类。
```go
func (this *Lame) OpenVdb(vlib string) uintptr
func (this *Lame) CloseVdb()
func (this *Lame) Create(vdb uintptr) uintptr
func (this *Lame) Destory()
func (this *Lame) Init() bool
func (this *Lame) SetParam(param string) bool
func (this *Lame) ScanFile(path string) *DetectResult
func (this *Lame) ScanMem(data [] byte, size int) *DetectResult
func (this *Lame) ScanFileWithCallback(path string, cb ScanCallBack, userdata interface{}) bool
func (this *Lame) ScanMemWithCallback(data [] byte, size int, cb ScanCallBack, userdata interface{}) bool
func (this *Lame) GetVersion() *LameVersion
func (this *Lame) GetLicenceInfo() *LicenceInfo
func (this *Lame) ExtractFile(filename string, password string, cb ExtractCallBack, userdata interface{}) bool
func (this *Lame) Extract_GetSize(handle uintptr) int
func (this *Lame) Extract_Seek(handle uintptr, offset int, method int32) bool
func (this *Lame) Extract_Tell(handle uintptr) int32
func (this *Lame) Extract_Read(handle uintptr, buf [] byte, size uint32) uint32
```

## lame.go.scanner
___

Scanner：扫描器类
```go
func (this *Scanner) Init() bool
func (this *Scanner) ShowVersion()
func (this *Scanner) ShowLicence()
func (this *Scanner) Load() (bool, error)
func (this *Scanner) SetParameters(param string)
func (this *Scanner) Unload()
func (this *Scanner) ScanFile(path string) *lame.DetectResult
func (this *Scanner) ScanFileWithCallback(path string, cb lame.ScanCallBack, userdata interface{}) bool
func (this *Scanner) ScanMem(data [] byte, size int) *lame.DetectResult
func (this *Scanner) ScanMemWithCallback(data [] byte, size int, cb lame.ScanCallBack, userdata interface{}) bool
func (this *Scanner) ReadPos(_lame *lame.Lame, handle uintptr, data [] byte, pos int, size uint32) uint32
func (this *Scanner) DumpFile(handle uintptr, filename string) bool
func (this *Scanner) ReadFile(path string)( []byte, error)
func (this *Scanner) ExtractFile(filename string, password string, cb lame.ExtractCallBack, userdata interface{}) bool
func (this *Scanner) ExtGetSize(handle uintptr) int
func (this *Scanner) ExtSeek(handle uintptr, offset int, method int32) bool
func (this *Scanner) ExtTell(handle uintptr) int32
func (this *Scanner) ExtRead(handle uintptr, buf [] byte, size uint32) uint32
```

## lame.go.sample
___


扫描器示例
```go
func scan_enter_file(file_name string, depth uint32, usr_data interface{}) uint32 {
	println(file_name, usr_data.(string))
	return lame.LSCT_CONTINUE
}

func scan_leave_file(file_name string, depth uint32, usr_data interface{}, l uint32) {
	println(file_name, usr_data.(string))
}

func scan_alarm(file_name string, info *lame.DetectResult, usr_data interface{}) uint32 {
	println(file_name, info.Vname, usr_data.(string))
	return lame.LSCT_CONTINUE
}

func get_current_directory() string {
	dir, err := filepath.Abs(filepath.Dir(os.Args[0]))
	if err != nil {
		log.Fatal(err)
	}
	return strings.Replace(dir, "\\", "/", -1)
}

func extract_enter_file(file_name string, format string, depth uint32, FCLS uint64, handle uintptr, usr_data interface{}) uint32 {
	if 0 != depth {
		sample := usr_data.(*Sample)
		path := get_current_directory() + "/" + file_name
		sample.scan.DumpFile(handle, path)
	}
	println(file_name, format)
	return lame.LCT_DO_EXTRACT
}

func extract_leave_file(file_name string, format string, depth uint32, FCLS uint64, handle uintptr, usr_data interface{}, l uint32) {
	println(file_name, format)
}

type Sample struct {
	scan *scanner.Scanner
	usecallbak bool
}

func (this *Sample) Init(usecallbak bool) {
	this.scan = new(scanner.Scanner)
	this.usecallbak = usecallbak

	this.scan.Init()
	this.scan.ShowVersion()
	this.scan.ShowLicence()
	this.scan.SetParameters("precise-format=0")
	this.scan.Load()
}

func (this *Sample) Uninit() {
	this.scan.Unload()
}

func (this *Sample) ScanFile(path string) {
	if this.usecallbak {
		scb := lame.ScanCallBack{scan_enter_file,scan_leave_file,scan_alarm}
		this.scan.ScanFileWithCallback(path, scb,"scanfile_callback")
	} else {
		dr := this.scan.ScanFile(path)
		if dr != nil {
			println(path,dr.Vname,dr.Engid)
		} else {
			println(path)
		}
	}
}

func (this *Sample) ScanMem(path string) {
	data, _ := this.scan.ReadFile(path)
	if this.usecallbak {
		mcb := lame.ScanCallBack{scan_enter_file, scan_leave_file, scan_alarm}
		this.scan.ScanMemWithCallback(data, len(data), mcb, "scanmem_callback")
	} else {
		dr := this.scan.ScanMem(data, len(data))
		if dr != nil {
			println(dr.Vname,dr.Engid)
		}
	}
}

func (this *Sample) ExtractFile(path string) {
	ecb := lame.ExtractCallBack{extract_enter_file, extract_leave_file}
	this.scan.ExtractFile(path,"", ecb, this)
}

func (this *Sample) ScanDir(path string) {
	err := filepath.Walk(path, func(path string, f os.FileInfo, err error) error {
		if f == nil {
			return err
		}
		if f.IsDir() {
			return nil
		}
		this.ScanFile(path)
		this.ScanMem(path)
		this.ExtractFile(path)
		return nil
	})
	if err != nil {
		fmt.Printf("filepath.Walk() returned %v\n", err)
	}
}

func main() {
	target := "D:\\rsdata\\5"
	sample := new(Sample)
	sample.Init(true)
	sample.ScanDir(target)
	sample.Uninit()
}
```
