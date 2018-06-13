'use strict';
var fs = require('fs');

var Scanner = require('./lame.scanner.js');

var LSCT_CONTINUE = 1;
var LSCT_ABORT = 2;

var LCT_CONTINUE = 1;
var LCT_DO_EXTRACT = 2;
var LCT_ABORT = 3;

function scan_enter_file(filename, depth, userdata) {
	return LSCT_CONTINUE;
}

function scan_leave_file(filename, depth, userdata, l) {

}

function scan_alarm(filename, info, userdata) {
	if (null != info) {
		console.warn(filename + '\tInfected: ' + info.vname);
	} else {
		console.log(filename);
	}
	return LSCT_CONTINUE;
}

function dump_file(scanner, handle, filename) {
	var size = scanner.ExtGetSize(handle);
	var offset = 0;
	while (size > 0) {
		var bytesRead = 0x1000 < size ? 0x1000 : size;
		var data = scanner.ExtReadPos(handle, offset, bytesRead);
		if (null == data) break;
		fs.appendFileSync(filename, data);
		offset += data.length;
		size -= data.length;
	}
}

function extract_enter_file(filename, format, depth, FCLS, handle, userdata) {
	var scanner = userdata;
	if (null != handle && depth >= 1) {
		var path = __dirname + "/" + filename;
		dump_file(scanner, handle, path);
	}
	return LCT_DO_EXTRACT;
}

function extract_leave_file(filename, format, depth, FCLS, handle, userdata, l) {
	console.log(filename + ' format: ' + format);
}

function GetVersion() {
	var scanner = new Scanner();
	scanner.ShowVersion();
	scanner.ShowLicence();
}

function DetectFile(path, flag) {
	var scanner = new Scanner();
	if (scanner.Load()) {
		if (flag) {
			scanner.ScanFile(path);
		} else {
			scanner.ScanFileWithCallback(path, scan_enter_file, scan_leave_file, scan_alarm, "filescan_callbck");
		}
		scanner.Unload();
	}
}

function DetectMemory(data, size, flag) {
	var scanner = new Scanner();
	if (scanner.Load()) {
		if (flag) {
			scanner.ScanMem(data, size);
		} else {
			scanner.ScanMemWithCallback(data, data.length, scan_enter_file, scan_leave_file, scan_alarm, "memscan_callbck");
		}
		scanner.Unload();
	}
}

function Extract(path) {
	var scanner = new Scanner();
	if (scanner.Load()) {
		scanner.ExtractFile(path, "", extract_enter_file, extract_leave_file, scanner);
		scanner.Unload();
	}
}

function main() {
	GetVersion();

	// extract
	Extract('d:/rsdata/5/5.zip');

	// file
	DetectFile('d:/rsdata/5/3949568_22900584_MYGL.exe.###', false);

	// memory
	var data = fs.readFileSync('d:/rsdata/5/3949568_22900584_MYGL.exe.###');
	DetectMemory(data, data.length, false);
}

// main
console.log("--------------- test start ---------------");
main();
console.log("--------------- test end -----------------");