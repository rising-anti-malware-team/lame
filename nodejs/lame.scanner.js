'use strict';

var os = require('os');
var fs = require('fs');
var path = require('path');
var Lame = require('./lame.js');

////////////////////////////////////////////////////////////////
var lame = null;

function Scanner() {
	try {
		lame = new Lame();
	} catch (e) {
		console.log(e);
	}
}

function isDirectory(filepath) {
	try {
		var info = fs.statSync(filepath);
		return info.isDirectory();
	} catch (e) {
		console.log(e);
	}
}

function DealFile(filename) {
	try {
		var dr = lame.ScanFile(filename);
		if (null != dr) {
			console.warn(filename + '\tInfected: ' + dr.vname + '(' + dr.engid + ')');
		} else {
			console.log(filename);
		}
	} catch (e) {
		console.log(e);
	}
}

function DealFileWithCallback(filename, enter_file, leave_file, alarm, userdata) {
	try {
		return lame.ScanFileWithCallback(filename, enter_file, leave_file, alarm, userdata);
	} catch (e) {
		console.log(e);
	}
	return false;
}

function readDirSync(filepath, list) {
	try {
		var pa = fs.readdirSync(filepath);
		pa.forEach(function (filename, index) {
			var fp = filepath + "/" + filename;
			if (isDirectory(fp)) {
				readDirSync(fp, list);
			} else {
				list.push(fp);
			}
		});
	} catch (e) {
		console.log(e);
	}
}

function GetFileList(filepath) {
	var list = [];
	try {
		if (isDirectory(filepath)) {
			readDirSync(filepath, list);
		} else {
			list.push(filepath);
		}
	} catch (e) {
		console.log(e);
	}
	return list;
}

Scanner.prototype.Load = function Load() {
	try {
		if (!lame.OpenVdb(null)) {
			throw ('lame.OpenVdb failed.');
		}
		if (!lame.Load()) {
			throw ('lame.Load failed.');
		}
		return true;
	} catch (e) {
		console.log(e);
		lame.Unload();
	}
	return false;
}

Scanner.prototype.Unload = function Unload() {
	try {
		lame.CloseVdb();
		lame.Unload();
	} catch (e) {
		console.log(e);
	}
}

Scanner.prototype.ScanFile = function ScanFile(filepath) {
	if (null == filepath) return;
	try {
		var list = GetFileList(filepath);
		while (list.length > 0) {
			var filename = list.shift();
			DealFile(filename);
		}
	} catch (e) {
		console.log(e);
	}
}

Scanner.prototype.ScanFileWithCallback = function ScanFileWithCallback(filepath, enter_file, leave_file, alarm, userdata) {
	if (null == filepath) return;
	try {
		var list = GetFileList(filepath);
		for (var key in list) {
			if (list.hasOwnProperty(key)) {
				var element = list[key];
				DealFileWithCallback(element, enter_file, leave_file, alarm, userdata);
			}
		}
	} catch (e) {
		console.log(e);
	}
}

Scanner.prototype.ScanMem = function ScanMem(data, size) {
	if (null == data || 0 == size) return;
	try {
		var dr = lame.ScanMem(data, size);
		if (null != dr) {
			console.warn(data + '\tInfected: ' + dr.vname + '(' + dr.engid + ')');
		} else {
			console.log(data);
		}
	} catch (e) {
		console.log(e);
	}
}

Scanner.prototype.ScanMemWithCallback = function ScanMemWithCallback(data, size, enter_file, leave_file, alarm, userdata) {
	try {
		return lame.ScanMemWithCallback(data, size, enter_file, leave_file, alarm, userdata);
	} catch (e) {
		console.log(e);
	}
	return false;
}

Scanner.prototype.ShowVersion = function ShowVersion() {
	try {
		var version = lame.GetVersionInfo();
		if (null != version) {
			console.log('engine version: ' + version.engv);
			console.log('viruslib version: ' + version.vdbv);
		} else {
			console.log('GetLameInfo failed.');
		}
	} catch (e) {
		console.log(e);
	}
}

Scanner.prototype.ShowLicence = function ShowLicence() {
	try {
		var info = lame.GetLicenceInfo();
		if (null != info) {
			console.log('Version: ' + info.Version);
			console.log('Owner: ' + info.Owner);
			console.log('Date: ' + info.Date);
			console.log('Authm: ' + info.Authm);
			console.log('Data: ' + info.Data);
		} else {
			console.log('GetLicenceInfo failed.');
		}
	} catch (e) {
		console.log(e);
	}
}

Scanner.prototype.ExtractFile = function ExtractFile(filename, password, enter_file, leave_file, userdata) {
	try {
		return lame.ExtractFile(filename, password, enter_file, leave_file, userdata);
	} catch (e) {
		console.log(e);
	}
	return false;
}

Scanner.prototype.ExtGetSize = function ExtGetSize(handle) {
	try {
		if (null != handle) {
			return lame.Extract_GetSize(handle);
		}
	} catch (e) {
		console.log(e);
	}
	return 0;
}

Scanner.prototype.ExtSeek = function ExtSeek(handle, offset, method) {
	try {
		return lame.Extract_Seek(handle, offset, method);
	} catch (e) {
		console.log(e);
	}
	return false;
}

Scanner.prototype.ExtTell = function ExtTell(handle) {
	try {
		return lame.Extract_Tell(handle);
	} catch (e) {
		console.log(e);
	}
	return -1;
}

Scanner.prototype.ExtRead = function ExtRead(handle, size) {
	try {
		if (null != handle && size > 0) {
			return lame.Extract_Read(handle, size);
		}
	} catch (e) {
		console.log(e);
	}
	return null;
}

var LAME_SEEK_SET	= 0;
var LMAE_SEEK_CUT	= 1;
var LMAE_SEEK_TAIL	= 2;

Scanner.prototype.ExtReadPos = function ExtReadPos(handle, offset, size) {
	try {
		if (null != handle && offset >= 0 && size > 0) {
			if (!lame.Extract_Seek(handle, offset, LAME_SEEK_SET))
				return null;
			return lame.Extract_Read(handle, size);
		}
	} catch (e) {
		console.log(e);
	}
	return null;
}

module.exports = Scanner;
