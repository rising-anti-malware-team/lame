<?php
require("lame.scanner.php");

$s_ef_cb = function ($filename, $depth, &$user_data) {
	return LSCT_CONTINUE;
};
$s_lf_cb = function ($filename, $depth, &$user_data, $l) {
	if (null != $user_data)
	{
		echo "$filename	Infected: " . $user_data . "\n";
	}
	else
	{
		echo "$filename\n";
	}
};
$s_a_cb = function ($filename, $info, &$user_data) {
	$user_data = $info->vname;
	return LSCT_CONTINUE;
};
$e_ef_cb = function ($filename, $fmt, $depth, $flag, $handle, &$user_data) {
	if (null != $handle && $depth >= 1)
	{
		$real_path = "dump/" . $filename;
		$new_filename = __DIR__ . "/" . $real_path;
		analyze_path($real_path);
		dump_file($handle, $new_filename, $user_data);
	}
	return LCT_DO_EXTRACT;
};
$e_lf_cb = function ($filename, $fmt, $depth, $flag, $handle, &$user_data, $lv) {
};

function read_by_pos($handle, $offset, $length, $scanner)
{
	if (!$scanner->ExtractSeek($handle, $offset, LAME_SEEK_SET))
		return null;
	return $scanner->ExtractRead($handle, $length);
}
function check_dir($target_path)
{
	if (!file_exists($target_path))
	{
		mkdir($target_path, 0777, true);
	}
}
function analyze_path($target_path)
{
	$list = explode("/", $target_path);
	$length = sizeof($list);
	if ($length <= 1) return;
	$path = $list[0];
	check_dir($path);
	for ($i = 1; $i < $length - 1; $i++)
	{
		$path = $path . "/" . $list[$i];
		check_dir($path);
	}
}
function dump_file($handle, $target_path, $scanner)
{
	$size = $scanner->ExtractGetSize($handle);
	$offset = 0;
	$f = fopen($target_path, "w") or die("unable to open file!");
	while ($size > 0)
	{
		$bytes_read = 0x1000 < $size ? 0x1000 : $size;
		$contents = read_by_pos($handle, $offset, $bytes_read, $scanner);
		if (null == $contents) break;
		fwrite($f, $contents);
		$offset += $bytes_read;
		$size -= $bytes_read;
	}
	fclose($f);
}

function main()
{
	global $s_ef_cb, $s_lf_cb, $s_a_cb, $e_ef_cb, $e_lf_cb;
	$filename = "/rsdata/virus/";
	$scan = new Scanner();
	$scan->Load();
	$scan->ScanFile($filename);
	$virus_name = null;
	$scan->SetCallback($s_ef_cb, $s_lf_cb, $s_a_cb);
	$scan->ScanFileWithCallback($filename, $virus_name);

	$filename = "/rsdata/virus/1";
	$contents = file_get_contents($filename);
	$scan->ScanMemory($contents);
	$virus_name = null;
	$scan->SetCallback($s_ef_cb, $s_lf_cb, $s_a_cb);
	$scan->ScanMemoryWithCallback($contents, $virus_name);

	$filename = "/rsdata/t.tgz";
	$scan->SetCallback($e_ef_cb, $e_lf_cb);
	$scan->Extract($filename, $scan);
}

echo "\n***************************** test-start **************************************\n\n";
main();
echo "\n***************************** test-finished **************************************\n";

?>