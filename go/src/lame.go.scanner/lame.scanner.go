package scanner

import (
	"os"
	"fmt"
	"io/ioutil"
	"lame.go"
	"errors"
)

type Scanner struct{
	_lame *lame.Lame
	paramlist []string
}

func (this *Scanner) Init() bool {
	this._lame = new(lame.Lame)
	return this._lame.GetProcess()
}

func (this *Scanner) ShowVersion() {
	if nil == this._lame {
		return
	}
	ver := this._lame.GetVersion()
	if nil != ver {
		fmt.Printf("engine version: %v\n", ver.Engv)
		fmt.Printf("virusdb version: %v\n", ver.Vdbv)
	} else {
		fmt.Println("GetLameInfo failed.")
	}
}

func (this *Scanner) ShowLicence() {
	if nil == this._lame {
		return
	}
	info := this._lame.GetLicenceInfo()
	if nil != info {
		fmt.Printf("Version: %v\n", info.Version)
		fmt.Printf("Owner: %v\n", info.Owner)
		fmt.Printf("Date: %v\n", info.Date)
		fmt.Printf("Authm: %v\n", info.Authm)
		fmt.Printf("Data: %v\n", info.Data)
	} else {
		fmt.Println("GetLicenceInfo failed.")
	}
}

func (this *Scanner) Load() (bool, error) {
	vdb := this._lame.OpenVdb("")
	if 0 == vdb {
		return false, errors.New("Open viruslib failed.")
	}
	engine := this._lame.Create(vdb)
	if 0 == engine {
		this.Unload()
		return false, errors.New("create engine failed.")
	}
	for i:=0; i<len(this.paramlist);i++  {
		this._lame.SetParam(this.paramlist[i])
	}
	if !this._lame.Init() {
		this.Unload()
		return false, errors.New("Init engine failed.")

	}
	return true, errors.New("")
}

func (this *Scanner) SetParameters(param string) {
	this.paramlist = append(this.paramlist, param)
}

func (this *Scanner) Unload() {
	this._lame.CloseVdb()
	this._lame.Destory()
}

func (this *Scanner) ScanFile(path string) *lame.DetectResult {
	if nil == this._lame {
		return nil
	}
	return this._lame.ScanFile(path)
}

func (this *Scanner) ScanFileWithCallback(path string, cb lame.ScanCallBack, userdata interface{}) bool {
	if nil == this._lame {
		return false
	}
	return this._lame.ScanFileWithCallback(path, cb, userdata)
}

func (this *Scanner) ScanMem(data [] byte, size int) *lame.DetectResult  {
	if nil != this._lame && nil != data && size > 0 {
		return this._lame.ScanMem(data,size)
	}
	return nil
}

func (this *Scanner) ScanMemWithCallback(data [] byte, size int, cb lame.ScanCallBack, userdata interface{}) bool  {
	if nil != this._lame && nil != data && size > 0 {
		return this._lame.ScanMemWithCallback(data,size,cb,userdata)
	}
	return false
}

func (this *Scanner) ReadPos(_lame *lame.Lame, handle uintptr, data [] byte, pos int, size uint32) uint32 {
	if !_lame.Extract_Seek(handle, pos, lame.LAME_SEEK_SET) {
		return 0
	}
	return _lame.Extract_Read(handle, data, size)
}

func (this *Scanner) DumpFile(handle uintptr, filename string) bool {
	var size uint32 = uint32(this._lame.Extract_GetSize(handle))
	const block = 0x1000
	data := make([]byte, block)
	var offset uint32 = 0
	var bytes uint32

	fd,err := os.OpenFile(filename,os.O_RDWR|os.O_CREATE|os.O_TRUNC,0644)
	if err != nil {
		return false
	}
	defer fd.Close()

	for ; size>0; {
		if block < size {
			bytes = block
		} else {
			bytes = uint32(size)
		}
		if bytes != this.ReadPos(this._lame, handle, data[:], int(offset), bytes) {
			return false
		}
		fd.Write(data)
		offset += bytes
		size -= bytes
	}
	return true
}

func (this *Scanner) ReadFile(path string)( []byte, error) {
	fh, ferr := os.Open(path)
	if ferr != nil {
		fmt.Printf("An error occurred on opening the inputfile\n" +
			"Does the file exist?\n" +
			"Have you got acces to it?\n")
		return nil, ferr
	}
	defer fh.Close()
	return ioutil.ReadAll(fh)
}

func (this *Scanner) ExtractFile(filename string, password string, cb lame.ExtractCallBack, userdata interface{}) bool {
	if nil == this._lame {
		return false
	}
	return this._lame.ExtractFile(filename, password, cb, userdata)
}

func (this *Scanner) ExtGetSize(handle uintptr) int {
	return this._lame.Extract_GetSize(handle)
}

func (this *Scanner) ExtSeek(handle uintptr, offset int, method int32) bool {
	return this._lame.Extract_Seek(handle,offset, method)
}

func (this *Scanner) ExtTell(handle uintptr) int32 {
	return this._lame.Extract_Tell(handle)
}

func (this *Scanner) ExtRead(handle uintptr, buf [] byte, size uint32) uint32 {
	return this._lame.Extract_Read(handle,buf,size)
}
