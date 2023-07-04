#!/bin/bash

(return 0 2>/dev/null) && sourced=1 || sourced=0

if [ $sourced -eq 0 ]; then
	if [ $# -eq 0 ]
	  then
	    echo "No IP address provided"
		exit 1
	fi

	IP=$1

	if [ -z "$IP" ]
	  then
	    echo "ERROR: Empty IP"
		exit 1
fi
fi
upload_file () {
	for file in "$@"
	do
		split=( $(echo $file  | tr ":" " ") )
		echo $split
		filename=${split[0]}
		if [ -z  ${split[1]} ]; then
			filepath=$(echo ${filename} | cut -d'/' -f2-)
		else
			filepath=${split[1]}
		fi
		echo $filename "-->" $filepath

		curl "http://${IP}/edit" -X POST \
		-H "Origin: http://${IP}" \
		-H 'Connection: keep-alive' \
		-H "Referer: http://${IP}/edit" \
		-F "data=@${filename};filename=\"${filepath}\""
		echo ""
	done
}

export -f upload_file

if [ $sourced -eq 0 ]; then
	export IP
	find data -type f -exec bash -c "upload_file {} $IP" \;
else
	echo "You can now call "
	echo "IP=1.2.3.4 upload_file data/file1.txt data/file2.txt:target.txt"
fi
