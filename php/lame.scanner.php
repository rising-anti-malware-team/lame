<?php
require("plame.php");
class cb_data
{
	var $ef;
	var $lf;
	var $a;
	function set($ef, $lf, $a = null)
	{
		$this->ef = $ef;
		$this->lf = $lf;
		$this->a = $a;
	}
}
class Scanner
{
	var $lame;
	var $cb;
	var $use_callback;
	var $user_data;
	function __construct()
	{
		$this->lame = new Lame();
		$this->cb = new cb_data();
	}
	function __destruct()
	{
		$this->Unload();
	}
	function ScanFile($target_path)
	{
		$this->use_callback = false;
		$this->travl_path($target_path);
	}
	function SetCallback($enter_file, $leave_file, $alarm = null)
	{
		$this->cb->set($enter_file, $leave_file, $alarm);
	}
	function ScanFileWithCallback($target_path, &$user_data)
	{
		$this->use_callback = true;
		$this->user_data = $user_data;
		$this->travl_path($target_path);
	}
	function ScanMemory($contents)
	{
		$dr = $this->lame->scan_memory($contents);
		if (null != $dr)
		{
			echo "memory block \tInfected: " . $dr->get_virusname(), PHP_EOL;
		}
	}
	function ScanMemoryWithCallback($contents, &$user_data)
	{
		$this->lame->scan_memory_with_callback($contents, $this->cb->ef, $this->cb->lf, $this->cb->a, $user_data);
	}
	function ShowVersion()
	{
		$ver = $this->lame->get_version();
		if (null != $ver)
		{
			echo "engv:", $ver->engv, PHP_EOL;
			echo "vdbv:", $ver->vdbv, PHP_EOL;
		}
	}
	function ShowLicence()
	{
		$licence = $this->lame->get_licence_info();
		if (null != $licence)
		{
			echo "Version:", $licence->Version, PHP_EOL;
			echo "Owner:", $licence->Owner, PHP_EOL;
			echo "Date:", $licence->Date, PHP_EOL;
			echo "Authm:", $licence->Authm, PHP_EOL;
			echo "Data:", $licence->Data, PHP_EOL;
		}
	}
	function Load()
	{
		if (!$this->lame->load())
		{
			echo "load failed.", PHP_EOL;
		}
	}
	function Unload()
	{
		$this->lame->unload();
	}
	function Extract($filename, &$user_data)
	{
		return $this->lame->extract_file($filename, $this->cb->ef, $this->cb->lf, $user_data);
	}
	function ExtractGetSize($handle)
	{
		return $this->lame->extract_get_size($handle);
	}
	function ExtractSeek($handle, $offset, $method)
	{
		return $this->lame->extract_seek($handle, $offset, $method);
	}
	function ExtractTell($handle)
	{
		return $this->lame->extract_tell($handle);
	}
	function ExtractRead($handle, $size)
	{
		return $this->lame->extract_read($handle, $size);
	}
	function travl_path($target_path)
	{
		if (!is_dir($target_path))
		{
			scan($target_path);
		}
		else
		{
			$handle = opendir($target_path);
			if ($handle)
			{
				while (($fl = readdir($handle)))
				{
					$temp = $target_path . DIRECTORY_SEPARATOR . $fl;
					if (is_dir($temp) && $fl != '.' && $fl != '..')
					{
						$this->travl_path($temp);
					}
					else
					{
						if ($fl != '.' && $fl != '..')
						{
							$this->scan($temp);;
						}
					}
				}
			}
		}
	}
	function scan($filename)
	{
		if ($this->use_callback)
		{
			$this->lame->scan_with_callback($filename, $this->cb->ef, $this->cb->lf, $this->cb->a, $this->cb->user_data);
		}
		else
		{
			$dr = $this->lame->scan($filename);
			if (null != $dr)
			{
				echo $filename . "\tInfected: " . $dr->get_virusname(), PHP_EOL;
			}
			else
			{
				echo "$filename", PHP_EOL;
			}
		}
	}
}
?>