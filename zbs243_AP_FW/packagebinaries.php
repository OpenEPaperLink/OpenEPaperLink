<?php

$types[0x00] = "AP_FW_1.54.bin";
$types[0x01] = "AP_FW_2.9.bin";
$types[0xF0] = "AP_FW_Segmented_UK.bin";
$types[0xFF] = "AP_FW_Nodisplay.bin";

$binpath = "../binaries/";
$tocmaxsize = 512;

$toc = array();
$output = '';
$output = str_pad($output,$tocmaxsize,"\0"); 

$version = exec("cat main.c | grep version | grep uint16_t");
list($blaat, $version) = explode("= 0x",$version);
if(strlen($version)<4)die("Couldn't read version from main.c, please check. I got $version");
$version = hexdec($version);

exec("ls -1 $binpath | grep 'AP_FW' | grep -v Pack", $binaries);
foreach($binaries as $file){
	$file = trim($file);
	$type = -1;
	foreach($types as $typeid => $typefile){
		if($typefile == $file){
			$type = $typeid;
		}
	}
	if($type==-1)die("We don't recognize filetype <$file>, sorry...\n");
	$binary = file_get_contents($binpath.$file);
	$length = strlen($binary);
	$offset = strlen($output);
	$subarr['type']=$type;
	$subarr['version']=$version;
	$subarr['name']=$file;
	$subarr['offset']=$offset;
	$subarr['length']=$length;
	$toc[] = $subarr;
	$output.=$binary;
}

$jtoc = json_encode($toc);
$tocsize = strlen($jtoc);
if($tocsize>$tocmaxsize)die("TOC is too big! adjust size and try again\n");
$output = substr_replace($output,$jtoc,0,strlen($jtoc));
file_put_contents($binpath."AP_FW_Pack.bin", $output);
print_r($toc);
echo "All done.\n";


