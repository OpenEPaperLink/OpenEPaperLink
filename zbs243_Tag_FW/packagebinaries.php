<?php

$version = "0027";


$types[0x00] = "SOL_M2_154_SSD_".$version.".bin";
$types[0x01] = "SOL_M2_29_SSD_".$version.".bin";
$types[0x02] = "SOL_M2_42_SSD_".$version.".bin";  //"SOLUM_42_SSD1619-tag-02-0022.bin";
$types[0x11] = "SOL_M2_29_UC_".$version.".bin"; //"SOLUM_29_UC8151-tag-11-0022.bin";
$types[0x21] = "SOL_M2_29_LT_".$version.".bin"; 

$binpath = "../binaries/Tag/";
$tocmaxsize = 512;

$toc = array();
$output = '';
$output = str_pad($output,$tocmaxsize,"\0");

foreach($types as $type => $file){
	$file = trim($file);
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

