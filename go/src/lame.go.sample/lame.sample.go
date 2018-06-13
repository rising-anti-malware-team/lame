package main

import (
	"lame.go"
	"lame.go.scanner"
	"path/filepath"
	"os"
	"log"
	"strings"
	"fmt"
)

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