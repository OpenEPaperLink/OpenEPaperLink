<?php

$types[0x00] = "SOLUM_154_SSD1619-tag-00-0022.bin";
$types[0x01] = "SOLUM_29_SSD1619-tag-01-0022";
$types[0xF0] = "Tag_FW_Segmented_UK.bin";
$types[0x02] = "SOLUM_42_SSD1619-tag-02-0022.bin";
$types[0x11] = "SOLUM_29_UC8151-tag-11-0022.bin";

$binpath = "../binaries/Tag";
$tocmaxsize = 512;

$toc = array();
$output = '';
$output = str_pad($output,$tocmaxsize,"\0"); 

/*
$version = exec("cat main.c | grep version | grep uint16_t");
list($blaat, $version) = explode("= 0x",$version);
if(strlen($version)<4)die("Couldn't read version from main.c, please check. I got $version");
$version = hexdec($version);
*/
$version = 0;

// *** fixme: this should select only filenames containing the latest version. See python version of this script.
exec("ls -1 $binpath | grep 'Tag_FW' | grep -v battery | grep -v Pack | grep -v M3", $binaries);
foreach($binaries as $file){
	$file = trim($file);
	$type = -1;
	foreach($types as $typeid => $typefile){
		if($typefile == $file){
			$type = $typeid;
		}
	}
	if($type!=-1)echo("Adding filetype <$file>\n");
	$binary = file_get_contents($binpath.$file);
	$length = strlen($binary);
	$offset = strlen($output);
	$subarr['type']=$type;
	$subarr['version']=$version;
	$subarr['name']=$file;
	$subarr['offset']=$offset;
	$subarr['length']=$length;
	$toc[]=$subarr;
	$output.=$binary;
}

$jtoc = json_encode($toc);
$tocsize = strlen($jtoc);
if($tocsize>$tocmaxsize)die("TOC is too big! adjust size and try again\n");
$output = substr_replace($output,$jtoc,0,strlen($jtoc));
file_put_contents($binpath."Tag_FW_Pack.bin", $output);
print_r($toc);
echo "All done.\n";

