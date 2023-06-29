<?php

function checkmem(){
	$mem = exec("cat main.mem | grep available | grep Stack");
	list($blaat, $mem) = explode("with ", $mem);
	list($mem,) = explode(" ", $mem, 2);
	return $mem;
}

function bell(){
	echo exec("echo \"\007\"");
}

function bells(){
	for($c = 0; $c<5; $c++){
		bell();
//		usleep(220000);
	}
}


$stackdisturbed = false;
$mem = checkmem();
while(1){
	$errlist = array();
	exec("make BUILD=zbs154_ssd1619 CPU=8051 SOC=zbs243 2>&1 | grep error | grep -v make", $errlist);
        //exec("make BUILD=zbs_segmented_uk CPU=8051 SOC=zbs243 2>&1 | grep error | grep -v make", $errlist);
	if(checkmem()!=$mem){
		$stackdisturbed = true;
		echo "Stack size was $mem, is now ".checkmem()." !!!\n";
		bells();
	} else{
		if($stackdisturbed){
			bell();
			echo "stack back to $mem\n";
			$stackdisturbed = false;
		}
	}
	if(count($errlist)){
		echo "-------------------------------\n";
		foreach($errlist as $err){
			echo " - $err\n";
		}
		bells();
	}
	sleep(5);
}

