<?php

exec("ls -1 | grep -v \".php\" | grep -v infopage | grep bin", $list);
foreach($list as $file){
	$data = file_get_contents($file);
	$data = substr($data,0,10240);
	$md5 = strtoupper(md5($data));
	echo "$file - $md5\n";
}
