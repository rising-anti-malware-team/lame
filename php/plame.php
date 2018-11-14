<?php
if (!extension_loaded('plame'))
{
	dl('plame.' . PHP_SHLIB_SUFFIX);
}

const LSCT_CONTINUE = 1;
const LSCT_ABORT = 2;

const LCT_CONTINUE = 1;
const LCT_DO_EXTRACT = 2;
const LCT_ABORT = 3;

const LAME_SEEK_SET = 0;
const LMAE_SEEK_CUT = 1;
const LMAE_SEEK_TAIL = 2;

class detect_result
{
	var $mklass;
	var $kclass;
	var $kclass_desc_a;
	var $engid;
	var $vname;
	var $vid32;
	var $vid40;
	var $hitag;
	var $treat;
	function copy($info)
	{
		$this->mklass = $info->mklass;
		$this->kclass = $info->kclass;
		$this->kclass_desc_a = $info->kclass_desc_a;
		$this->engid = $info->engid;
		$this->vname = $info->vname;
		$this->vid32 = $info->vid32;
		$this->vid40 = $info->vid40;
		$this->hitag = $info->hitag;
		$this->treat = $info->treat;
	}
	function get_virusname()
	{
		return $this->vname . "(" . $this->engid . ")";
	}
}
class lame_info
{
	var $engv;
	var $vdbv;
	function copy($info)
	{
		$this->engv = $info->engv;
		$this->vdbv = $info->vdbv;
	}
}
class licence_info
{
	var $Version;
	var $Owner;
	var $Date;
	var $Authm;
	var $Data;
	function copy($info)
	{
		$this->Version = $info->Version;
		$this->Owner = $info->Owner;
		$this->Date = $info->Date;
		$this->Authm = $info->Authm;
		$this->Data = $info->Data;
	}
}
class data_callback
{
	var $enter_file;
	var $leave_file;
	var $alarm;
	var $user_data;
	var $dr;
	var $ed;
	function __construct(&$user_data, $ef, $lf, $a = null)
	{
		$this->user_data = &$user_data;
		$this->enter_file = $ef;
		$this->leave_file = $lf;
		$this->alarm = $a;
	}
	function copy_dr($info)
	{
		$this->dr = new detect_result();
		$this->dr->copy($info);
	}
	function copy_ed($info)
	{
		$this->ed = new extract_data();
		$this->ed->copy($info);
	}
	function s_ef($filename, $depth)
	{
		if (!is_callable($this->enter_file)) return LSCT_CONTINUE;
		return call_user_func_array($this->enter_file, array($filename, $depth, &$this->user_data));
	}
	function s_lf($filename, $depth, $l)
	{
		if (!is_callable($this->leave_file)) return;
		return call_user_func_array($this->leave_file, array($filename, $depth, &$this->user_data, $l));
	}
	function s_a($filename, $info)
	{
		if (!is_callable($this->alarm)) return LSCT_CONTINUE;
		$this->copy_dr($info);
		return call_user_func_array($this->alarm, array($filename, $this->dr, &$this->user_data));
	}
	function e_ef($filename, $fmt, $depth, $flag, $handle)
	{
		if (!is_callable($this->enter_file)) return LSCT_CONTINUE;
		return call_user_func_array($this->enter_file, array($filename, $fmt, $depth, $flag, $handle, &$this->user_data));
	}
	function e_lf($filename, $fmt, $depth, $flag, $handle, $lv)
	{
		if (!is_callable($this->leave_file)) return;
		return call_user_func_array($this->leave_file, array($filename, $fmt, $depth, $flag, $handle, &$this->user_data, $lv));
	}
}
function scan_enter_file($filename, $depth, &$dc)
{
	return $dc->s_ef($filename, $depth);
}
function scan_leave_file($filename, $depth, &$dc, $l)
{
	return $dc->s_lf($filename, $depth, $l);
}
function scan_alarm($filename, $info, &$dc)
{
	return $dc->s_a($filename, $info);
}
class extract_data
{
	var $path;
	var $count;
	function copy($info)
	{
		$this->path = $info->path;
		$this->count = $info->count;
	}
}
function extract_enter_file($filename, $fmt, $depth, $flag, $handle, &$dc)
{
	return $dc->e_ef($filename, $fmt, $depth, $flag, $handle);
}
function extract_leave_file($filename, $fmt, $depth, $flag, $handle, &$dc, $lv)
{
	return $dc->e_lf($filename, $fmt, $depth, $flag, $handle, $lv);
}


class Virusdb
{
	var $vdb;
	function __construct($vlib_path = "")
	{
		$this->vdb = lame_open_vdb($vlib_path);
	}
	function __destruct()
	{
		if (null != $this->vdb)
			lame_close_vdb($this->vdb);
		$this->vdb = null;
	}
	function get_vlib()
	{
		return $this->vdb;
	}
}
class Lame
{
	var $_handle;
	var $parameters = array();
	var $vdb;
	function __construct($vdb = null)
	{
		$this->vdb = $vdb;
	}
	function __destruct()
	{
		$this->unload();
	}
	function open_vdb($vlib_path = "")
	{
		return lame_open_vdb($vlib_path);
	}
	function create()
	{
		if (!isset($this->vdb))
		{
			$vdb = $this->open_vdb("");
			if (null == $vdb) return null;
			$this->vdb = $vdb;
		}
		return lame_create($this->vdb);
	}
	function release(&$handle)
	{
		if (null != $handle)
			lame_destroy($handle);
		$handle = null;
	}
	function close_vdb(&$vdb)
	{
		if (null != $vdb)
			lame_close_vdb($vdb);
		$vdb = null;
	}
	function set_parameter($param)
	{
		if (!is_string($param)) return;
		array_push($this->parameters, $param);
		if (!isset($this->_handle)) return;
		return lame_param_set($lame, $param);
	}
	function load()
	{
		$this->_handle = $this->create();
		if (0 == $this->_handle) return false;
		foreach ($this->parameters as $param)
		{
			lame_param_set($this->_handle, $param);
		}
		if (0 != lame_init($this->_handle)) return false;
		return true;
	}
	function unload()
	{
		if (isset($this->vdb))
		{
			$this->close_vdb($this->vdb);
		}
		if (isset($this->_handle))
		{
			$this->release($this->_handle);
		}
	}
	function scan($filename)
	{
		$result = lame_scan_file($this->_handle, $filename);
		if (null != $result)
		{
			$dr = new detect_result();
			$dr->copy($result);
			return $dr;
		}
	}
	function scan_with_callback($filename, $enter_file, $leave_file, $alarm, &$user_data)
	{
		$dc = new data_callback($user_data, $enter_file, $leave_file, $alarm);
		lame_scan_file_with_callback($this->_handle, $filename, "scan_enter_file", "scan_leave_file", "scan_alarm", $dc);
	}
	function scan_memory($contents)
	{
		$result = lame_scan_mem($this->_handle, $contents);
		if (null != $result)
		{
			$dr = new detect_result();
			$dr->copy($result);
			return $dr;
		}
	}
	function scan_memory_with_callback($contents, $enter_file, $leave_file, $alarm, &$user_data)
	{
		$dc = new data_callback($user_data, $enter_file, $leave_file, $alarm);
		lame_scan_mem_with_callback($this->_handle, $contents, "scan_enter_file", "scan_leave_file", "scan_alarm", $dc);
	}
	function extract_file($filename, $enter_file, $leave_file, &$user_data)
	{
		$dc = new data_callback($user_data, $enter_file, $leave_file);
		$result = lame_extract_file($this->_handle, $filename, "", "extract_enter_file", "extract_leave_file", $dc);
	}
	function extract_get_size($handle)
	{
		return lame_file_get_size($handle);
	}
	function extract_seek($handle, $offset, $method)
	{
		return lame_file_seek($handle, $offset, $method);
	}
	function extract_tell($handle)
	{
		return lame_file_tell($handle);
	}
	function extract_read($handle, $size)
	{
		return lame_file_read($handle, $size);
	}
	function get_version()
	{
		$ver = lame_get_version();
		if (null != $ver)
		{
			$vi = new lame_info();
			$vi->copy($ver);
			return $vi;
		}
	}
	function get_licence_info()
	{
		$licence = lame_get_licence_info();
		if (null != $licence)
		{
			$li = new licence_info();
			$li->copy($licence);
			return $li;
		}
	}
}
?>
