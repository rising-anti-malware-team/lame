// lame.js
'use strict';
var node_lame = require('./build/release/node_lame');

function lame() {}

var vdb_handle = 0;
var engine_handle = 0;
var param_list = [];

lame.prototype.GetVersionInfo = function GetVersionInfo() {
	try {
		return node_lame.lame_get_version();
	} catch (e) {
		console.log(e);
	}
	return null;
}

lame.prototype.GetLicenceInfo = function GetLicenceInfo() {
	try {
		return node_lame.lame_get_licence_info();
	} catch (e) {
		console.log(e);
	}
	return null;
}

lame.prototype.OpenVdb = function OpenVdb(vlib) {
	var handle = null;
	try {
		handle = node_lame.lame_open_vdb(vlib);
	} catch (e) {
		console.log(e);
	}
	if (null != handle) {
		vdb_handle = handle;
		return true;
	} else {
		return false;
	}
}

lame.prototype.CloseVdb = function CloseVdb() {
	try {
		if (null != vdb_handle) {
			node_lame.lame_close_vdb(vdb_handle);
			vdb_handle = null;
		}
	} catch (e) {
		console.log(e);
	}
}

lame.prototype.Load = function Load() {
	try {
		if (null == vdb_handle) {
			throw ("invalid viruslib. Please open viruslib.");
		}
		engine_handle = node_lame.lame_create(vdb_handle);
		if (null == engine_handle) {
			throw ("Failed to create lame.");
		}
		for (var i = 0; i < param_list.length; i++) {
			if (!node_lame.lame_param_set(engine_handle, param_list[i])) {
				throw ("lame_param_set failed.");
			}
		}
		if (!node_lame.lame_init(engine_handle)) {
			throw ("lame_init failed.");
		}
	} catch (e) {
		console.log(e);
		return false;
	}
	return true;
}

lame.prototype.SetParameters = function SetParameters(param) {
	if (null == param) return false;
	param_list.push(param);
	return true;
}

lame.prototype.Unload = function Unload() {
	try {
		if (null != engine_handle) {
			node_lame.lame_destroy(engine_handle);
			engine_handle = null;
		}
	} catch (e) {
		console.log(e);
	}
}

lame.prototype.ScanFile = function ScanFile(path) {
	try {
		if (null != engine_handle) {
			return node_lame.lame_scan_file(engine_handle, path);
		}
	} catch (e) {
		console.log(e);
	}
	return null;
}

lame.prototype.ScanFileWithCallback = function ScanFileWithCallback(path, enter_file, leave_file, alarm, userdata) {
	try {
		if (null != engine_handle) {
			if (0 == node_lame.lame_scan_file_with_callback(engine_handle, path, enter_file, leave_file, alarm, userdata)) {
				return true;
			}
		}
	} catch (e) {
		console.log(e);
	}
	return false;
}

lame.prototype.ScanMem = function ScanMem(data, size) {
	try {
		if (null != engine_handle && null != data && size > 0) {
			return node_lame.lame_scan_mem(engine_handle, data, size);
		}
	} catch (e) {
		console.log(e);
	}
	return null;
}

lame.prototype.ScanMemWithCallback = function ScanMemWithCallback(data, size, enter_file, leave_file, alarm, userdata) {
	try {
		if (null != engine_handle && null != data && size > 0) {
			return 0 == node_lame.lame_scan_mem_with_callback(engine_handle, data, size, enter_file, leave_file, alarm, userdata);
		}
	} catch (e) {
		console.log(e);
	}
	return false;
}

lame.prototype.ExtractFile = function ExtractFile(filename, password, enter_file, leave_file, userdata) {
	try {
		if (null != engine_handle) {
			return 0 == node_lame.lame_extract_file(engine_handle, filename, password, enter_file, leave_file, userdata);
		}
	} catch (e) {
		console.log(e);
	}
	return false;
}

lame.prototype.Extract_GetSize = function Extract_GetSize(handle) {
	try {
		if (null != handle) {
			return node_lame.lame_file_get_size(handle);
		}
	} catch (e) {
		console.log(e);
	}
	return 0;
}

lame.prototype.Extract_Seek = function Extract_Seek(handle, offset, method) {
	try {
		if (null != handle) {
			return node_lame.lame_file_seek(handle, offset, method);
		}
	} catch (e) {
		console.log(e);
	}
	return false;
}

lame.prototype.Extract_Tell = function Extract_Tell(handle) {
	try {
		if (null != handle) {
			return node_lame.lame_file_tell(handle);
		}
	} catch (e) {
		console.log(e);
	}
	return -1;
}

lame.prototype.Extract_Read = function Extract_Read(handle, size) {
	try {
		if (null != handle && size > 0) {
			return node_lame.lame_file_read(handle, size);
		}
	} catch (e) {
		console.log(e);
	}
	return null;
}

module.exports = lame;
