package lame

/*
#cgo LDFLAGS: -L ../../bin -llame
#include "../../../include/lame.v2.h"
*/
import "C"
import (
	"unsafe"
	"syscall"
)

func check_convert(ok bool) {
	if false == ok {
		panic("type cast failed!")
	}
}
func CPtr2GoPtr(general interface {}) uintptr {
	switch general.(type) {
	case *C.char:
		p, ok := general.(*C.char)
		check_convert(ok)
		return uintptr(unsafe.Pointer(p))
	case *byte:
		p, ok := general.(*byte)
		check_convert(ok)
		return uintptr(unsafe.Pointer(p))
	}
	return 0
}

func StrPtr(s string) uintptr {
	if "" == s {
		return 0
	}
	return uintptr(unsafe.Pointer(syscall.StringBytePtr(s)))
}

func CPointerToGoArray(cArray uintptr) (goArray []byte) {
	for i:=0; ; i++ {
		j := *(*byte)(unsafe.Pointer(cArray))
		if 0 == j {
			break
		}
		goArray = append(goArray, j)
		cArray += unsafe.Sizeof(j)
	}
	return goArray
}

type DetectResult struct {
	Mklass, Vid32, Treat uint32
	Vid40 uint64
	Kclass, Kclass_desc_a, Engid, Vname, Hitag string
	Kclass_desc_w string
}

func (dr *DetectResult) convert(result *C.lame_scan_result) *DetectResult {
	if nil == result {
		return nil
	}
	dr.Mklass = uint32(result.mklass)
	dr.Kclass = string(CPointerToGoArray(CPtr2GoPtr(&result.kclass[0])))
	dr.Kclass_desc_a = string(CPointerToGoArray(CPtr2GoPtr(&result.kclass_desc_a[0])))
	dr.Kclass_desc_w = "" //string(CPointerToGoArray(CPtr2GoPtr(&result.kclass_desc_w[0])))
	dr.Engid = string(CPointerToGoArray(CPtr2GoPtr(&result.engid[0])))
	dr.Vname = string(CPointerToGoArray(CPtr2GoPtr(&result.vname[0])))
	dr.Vid32 = uint32(result.vid32)
	dr.Vid40 = uint64(result.vid40)
	dr.Hitag = string(CPointerToGoArray(CPtr2GoPtr(&result.hitag[0])))
	dr.Treat = uint32(result.treat)
	return dr
}

type LameVersion struct {
	Engv, Vdbv string
}

func (lv *LameVersion) convert(info *C.lame_info) *LameVersion {
	if nil == info {
		return nil
	}
	lv.Engv = string(CPointerToGoArray(CPtr2GoPtr(&info.engv[0])))
	lv.Vdbv = string(CPointerToGoArray(CPtr2GoPtr(&info.vdbv[0])))
	return lv
}

type LicenceInfo struct {
	Version, Owner, Date, Authm, Data string
}

func (li *LicenceInfo) convert(info *C.rx_licence_info) *LicenceInfo {
	if nil == info {
		return nil
	}
	li.Version = string(CPointerToGoArray(CPtr2GoPtr(&info.Version[0])))
	li.Owner = string(CPointerToGoArray(CPtr2GoPtr(&info.Owner[0])))
	li.Date = string(CPointerToGoArray(CPtr2GoPtr(&info.Date[0])))
	li.Authm = string(CPointerToGoArray(CPtr2GoPtr(&info.Authm[0])))
	li.Data = string(CPointerToGoArray(CPtr2GoPtr(&info.Data[0])))
	return li
}

type scan_enter_file func(file_name string, depth uint32, usr_data interface{}) uint32
type scan_leave_file func(file_name string, depth uint32, usr_data interface{}, l uint32)
type scan_alarm func(file_name string, info *DetectResult, usr_data interface{}) uint32

type ScanCallBack struct {
	EnterFile scan_enter_file
	LeaveFile scan_leave_file
	Alarm scan_alarm
}

type extract_enter_file func(file_name string, format string, depth uint32, FCLS uint64, handle uintptr, userdata interface{}) uint32
type extract_leave_file func(file_name string, format string, depth uint32, FCLS uint64, handle uintptr, userdata interface{}, l uint32)

type ExtractCallBack struct {
	EnterFile extract_enter_file
	LeaveFile extract_leave_file
}

const (
	LAME_SEEK_SET	= 0
	LMAE_SEEK_CUT	= 1
	LMAE_SEEK_TAIL	= 2

	LSCT_CONTINUE	= 1
	LSCT_ABORT		= 2

	LCT_CONTINUE	= 1
	LCT_DO_EXTRACT	= 2
	LCT_ABORT		= 3

	LXLVT_NORMAL	= 1
	LXLVT_ABORT 	= 2
	LXLVT_ERROR		= 3
)

type Lame struct{
	vdb uintptr
	engine uintptr

	lame_open_vdb *syscall.LazyProc
	lame_close_vdb *syscall.LazyProc

	lame_create *syscall.LazyProc
	lame_destroy *syscall.LazyProc
	lame_fork *syscall.LazyProc

	lame_param_set *syscall.LazyProc

	lame_init *syscall.LazyProc
	lame_scan_file *syscall.LazyProc
	lame_scan_file_w *syscall.LazyProc
	lame_scan_mem *syscall.LazyProc

	lame_scan_file_with_callback *syscall.LazyProc
	lame_scan_file_with_callback_w *syscall.LazyProc
	lame_scan_mem_with_callback *syscall.LazyProc

	lame_get_version *syscall.LazyProc
	lame_get_licence_info *syscall.LazyProc

	lame_extract_file *syscall.LazyProc
	lame_extract_file_w *syscall.LazyProc
	lame_file_get_size *syscall.LazyProc
	lame_file_seek *syscall.LazyProc
	lame_file_tell *syscall.LazyProc
	lame_file_read *syscall.LazyProc
}

func (this *Lame) GetProcess() bool {
	lamedll := syscall.NewLazyDLL("lame")
	this.lame_open_vdb = lamedll.NewProc("lame_open_vdb")
	this.lame_close_vdb = lamedll.NewProc("lame_close_vdb")
	this.lame_create = lamedll.NewProc("lame_create")
	this.lame_destroy = lamedll.NewProc("lame_destroy")
	this.lame_fork = lamedll.NewProc("lame_fork")
	this.lame_param_set = lamedll.NewProc("lame_param_set")
	this.lame_init = lamedll.NewProc("lame_init")
	this.lame_scan_file = lamedll.NewProc("lame_scan_file")
	this.lame_scan_file_w = lamedll.NewProc("lame_scan_file_w")
	this.lame_scan_mem = lamedll.NewProc("lame_scan_mem")
	this.lame_scan_file_with_callback = lamedll.NewProc("lame_scan_file_with_callback")
	this.lame_scan_file_with_callback_w = lamedll.NewProc("lame_scan_file_with_callback_w")
	this.lame_scan_mem_with_callback = lamedll.NewProc("lame_scan_mem_with_callback")
	this.lame_get_version = lamedll.NewProc("lame_get_version")
	this.lame_get_licence_info = lamedll.NewProc("lame_get_licence_info")
	this.lame_extract_file = lamedll.NewProc("lame_extract_file")
	this.lame_extract_file_w = lamedll.NewProc("lame_extract_file_w")
	this.lame_file_get_size = lamedll.NewProc("lame_file_get_size")
	this.lame_file_seek = lamedll.NewProc("lame_file_seek")
	this.lame_file_tell = lamedll.NewProc("lame_file_tell")
	this.lame_file_read = lamedll.NewProc("lame_file_read")
	return true
}

func (this *Lame) OpenVdb(vlib string) uintptr {
	vdb, _, _ := this.lame_open_vdb.Call(StrPtr(vlib))
	this.vdb = vdb
	return vdb
}

func (this *Lame) CloseVdb() {
	if 0 == this.vdb {
		return
	}
	this.lame_close_vdb.Call(this.vdb)
}

func (this *Lame) Create(vdb uintptr) uintptr {
	if 0 == vdb {
		return 0
	}
	engine, _, _ := this.lame_create.Call(vdb)
	if 0 == engine {
		return 0
	}
	this.engine = engine
	return engine
}

func (this *Lame) Destory() {
	if 0 == this.engine {
		return
	}
	this.lame_destroy.Call(this.engine)
}

func (this *Lame) Init() bool {
	if 0 == this.engine {
		return false
	}
	hr, _, _ := this.lame_init.Call(this.engine)
	if 0 != hr {
		return false
	}
	return true
}

func (this *Lame) SetParam(param string) bool {
	if 0 == this.engine {
		return false
	}
	hr, _, _ := this.lame_param_set.Call(this.engine, StrPtr(param))
	if 0 != hr {
		return false
	}
	return true
}

func (this *Lame) ScanFile(path string) *DetectResult {
	var result C.lame_scan_result
	if 0 == this.engine {
		return nil
	}
	hr, _, _ := this.lame_scan_file.Call(this.engine, StrPtr(path), uintptr(unsafe.Pointer(&result)))
	if 0 == hr {
		var dr DetectResult
		return dr.convert(&result)
	}
	return nil
}

func (this *Lame) ScanMem(data [] byte, size int) *DetectResult {
	var result C.lame_scan_result
	if 0 == this.engine {
		return nil
	}
	hr, _, _ := this.lame_scan_mem.Call(this.engine, CPtr2GoPtr(&data[0]), uintptr(size), uintptr(unsafe.Pointer(&result)))
	if 0 == hr {
		var dr DetectResult
		return dr.convert(&result)
	}
	return nil
}

func (this *Lame) ScanFileWithCallback(path string, cb ScanCallBack, userdata interface{}) bool {
	if 0 == this.engine {
		return false
	}
	var feedback C.lame_scan_feedback
	EnterFile := syscall.NewCallback(func(file_name uintptr, depth uint32, usr_data uintptr) uintptr {
		f := string(CPointerToGoArray(file_name))
		ret := cb.EnterFile(f, depth, userdata)
		return uintptr(ret)
	})
	LeaveFile := syscall.NewCallback(func(file_name uintptr, depth uint32, usr_data uintptr, l uint32) uintptr {
		f := string(CPointerToGoArray(file_name))
		cb.LeaveFile(f, depth, userdata, l)
		return 0
	})
	Alarm := syscall.NewCallback(func(file_name uintptr, info uintptr, usr_data uintptr) uintptr {
		f := string(CPointerToGoArray(file_name))
		p := unsafe.Pointer(info)
		var dr DetectResult
		dr.convert((*C.lame_scan_result)(p))
		ret := cb.Alarm(f, &dr, userdata)
		return uintptr(ret)
	})
	feedback.enter_file = C.lame_scan_enter_file(unsafe.Pointer(EnterFile))
	feedback.leave_file = C.lame_scan_leave_file(unsafe.Pointer(LeaveFile))
	feedback.alarm = C.lame_scan_alarm(unsafe.Pointer(Alarm))
	hr, _, _ := this.lame_scan_file_with_callback.Call(this.engine, StrPtr(path), uintptr(unsafe.Pointer(&feedback)), 0)
	if 0 == hr {
		return true
	} else {
		return false
	}
}

func (this *Lame) ScanMemWithCallback(data [] byte, size int, cb ScanCallBack, userdata interface{}) bool {
	if 0 == this.engine {
		return false
	}
	var feedback C.lame_scan_feedback
	EnterFile := syscall.NewCallback(func(file_name uintptr, depth uint32, usr_data uintptr) uintptr {
		f := string(CPointerToGoArray(file_name))
		ret := cb.EnterFile(f, depth, userdata)
		return uintptr(ret)
	})
	LeaveFile := syscall.NewCallback(func(file_name uintptr, depth uint32, usr_data uintptr, l uint32) uintptr {
		f := string(CPointerToGoArray(file_name))
		cb.LeaveFile(f, depth, userdata, l)
		return 0
	})
	Alarm := syscall.NewCallback(func(file_name uintptr, info uintptr, usr_data uintptr) uintptr {
		f := string(CPointerToGoArray(file_name))
		p := unsafe.Pointer(info)
		var dr DetectResult
		dr.convert((*C.lame_scan_result)(p))
		ret := cb.Alarm(f, &dr, userdata)
		return uintptr(ret)
	})
	feedback.enter_file = C.lame_scan_enter_file(unsafe.Pointer(EnterFile))
	feedback.leave_file = C.lame_scan_leave_file(unsafe.Pointer(LeaveFile))
	feedback.alarm = C.lame_scan_alarm(unsafe.Pointer(Alarm))
	hr, _, _ := this.lame_scan_mem_with_callback.Call(this.engine, CPtr2GoPtr(&data[0]), uintptr(size), uintptr(unsafe.Pointer(&feedback)), 0)
	if 0 == hr {
		return true
	} else {
		return false
	}
}

func (this *Lame) GetVersion() *LameVersion {
	var info C.lame_info
	hr, _, _ := this.lame_get_version.Call(uintptr(unsafe.Pointer(&info)))
	if 0 == hr {
		var lv LameVersion
		return lv.convert(&info)
	}
	return nil
}

func (this *Lame) GetLicenceInfo() *LicenceInfo {
	var info C.rx_licence_info
	hr, _, _ := this.lame_get_licence_info.Call(uintptr(unsafe.Pointer(&info)))
	if 0 == hr {
		var li LicenceInfo
		return li.convert(&info)
	}
	return nil
}

func (this *Lame) ExtractFile(filename string, password string, cb ExtractCallBack, userdata interface{}) bool {
	if 0 == this.engine {
		return false
	}
	var feedback C.lame_extract_feedback
	EnterFile := syscall.NewCallback(func(file_name uintptr, format uintptr, depth uintptr, FCLS uintptr, handle uintptr, user_data uintptr) uintptr {
		f := string(CPointerToGoArray(file_name))
		pw := string(CPointerToGoArray(format))
		ret := cb.EnterFile(f, pw, uint32(depth), uint64(FCLS), handle, userdata)
		return uintptr(ret)
	})
	LeaveFile := syscall.NewCallback(func(file_name uintptr, format uintptr, depth uintptr, FCLS uintptr, handle uintptr, user_data uintptr, l uint32) uintptr {
		f := string(CPointerToGoArray(file_name))
		pw := string(CPointerToGoArray(format))
		cb.LeaveFile(f, pw, uint32(depth), uint64(FCLS), handle, userdata, l)
		return 0
	})
	feedback.enter_file = C.lame_extract_enter_file(unsafe.Pointer(EnterFile))
	feedback.leave_file = C.lame_extract_leave_file(unsafe.Pointer(LeaveFile))
	hr, _, _ := this.lame_extract_file.Call(this.engine, StrPtr(filename), StrPtr(password), uintptr(unsafe.Pointer(&feedback)), 0)
	if 0 == hr {
		return true
	} else {
		return false
	}
}

func (this *Lame) Extract_GetSize(handle uintptr) int {
	if 0 == this.engine || 0 == handle {
		return 0
	}
	size, _, _ := this.lame_file_get_size.Call(handle)
	return int(size)
}

func (this *Lame) Extract_Seek(handle uintptr, offset int, method int32) bool {
	if 0 == this.engine || 0 == handle {
		return false
	}
	hr, _, _ := this.lame_file_seek.Call(handle, uintptr(offset), uintptr(method))
	if 0 == hr {
		return true
	} else {
		return false
	}
}

func (this *Lame) Extract_Tell(handle uintptr) int32 {
	if 0 == this.engine || 0 == handle {
		return -1
	}
	pos, _, _ := this.lame_file_seek.Call(handle)
	return int32(pos)
}

func (this *Lame) Extract_Read(handle uintptr, buf [] byte, size uint32) uint32 {
	if 0 == this.engine || 0 == handle {
		return 0
	}
	length, _, _ := this.lame_file_read.Call(handle, CPtr2GoPtr(&buf[0]), uintptr(size))
	return uint32(length)
}
