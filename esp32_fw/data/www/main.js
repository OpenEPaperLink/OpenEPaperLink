const $ = document.querySelector.bind(document);

const contentModes = ["Static image", "Current date", "Counting days", "Counting hours", "Current weather", "Firmware update", "Memo text", "Image url", "Weather forecast"];
const models = ["1.54\" 152x152px", "2.9\" 296x128px", "4.2\" 400x300px"];
const contentModeOptions = [];
contentModeOptions[0] = ["filename","timetolive"];
contentModeOptions[1] = [];
contentModeOptions[2] = ["counter", "thresholdred"];
contentModeOptions[3] = ["counter", "thresholdred"];
contentModeOptions[4] = ["location"];
contentModeOptions[5] = ["filename"];
contentModeOptions[6] = ["text"];
contentModeOptions[7] = ["url","interval"];
contentModeOptions[8] = ["location"];

const imageQueue = [];
let isProcessing = false;
let servertimediff = 0;

let socket;
connect();
setInterval(updatecards, 1000);
window.addEventListener("load", function () { loadTags(0) });

function loadTags(pos) {
	fetch("/get_db?pos="+pos)
		.then(response => response.json())
		.then(data => {
			processTags(data.tags);
			if (data.continu && data.continu>pos) loadTags(data.continu);
		})
		//.catch(error => showMessage('loadTags error: ' + error));
}

function connect() {
	socket = new WebSocket("ws://" + location.host + "/ws");

	socket.addEventListener("open", (event) => {
		showMessage("websocket connected");
	});

	socket.addEventListener("message", (event) => {
		console.log(event.data);
		const msg = JSON.parse(event.data);
		if (msg.logMsg) {
			showMessage(msg.logMsg,false);
		}
		if (msg.errMsg) {
			showMessage(msg.errMsg,true);
		}
		if (msg.tags) {
			processTags(msg.tags);
		}
		if (msg.sys) {
			$('#sysinfo').innerHTML = 'free heap: ' + msg.sys.heap + ' bytes &#x2507; db size: ' + msg.sys.dbsize + ' bytes &#x2507; db record count: ' + msg.sys.recordcount + ' &#x2507; littlefs free: ' + msg.sys.littlefsfree + ' bytes';
			servertimediff = (Date.now() / 1000) - msg.sys.currtime;
		}
	});

	socket.addEventListener("close", (event) => {
		showMessage(`websocket closed ${event.code}`);
		setTimeout(connect, 5000);
	});
}

function processTags(tagArray) {
	for (const element of tagArray) {
		tagmac = element.mac;

		var div = $('#tag' + tagmac);
		if (div == null) {

			div = $('#tagtemplate').cloneNode(true);
			div.setAttribute('id', 'tag'+tagmac);
			div.dataset.mac = tagmac;
			$('#taglist').appendChild(div);

			$('#tag' + tagmac + ' .mac').innerHTML = tagmac;
			var img = $('#tag' + tagmac + ' .tagimg');
			img.addEventListener('error', function handleError() {
				img.style.display = 'none';
			});	
		} 

		div.style.display = 'block';

		let alias = element.alias;
		if (!alias) alias = tagmac;
		$('#tag' + tagmac + ' .alias').innerHTML = alias;

		if (div.dataset.hash != element.hash) loadImage(tagmac, '/current/' + tagmac + '.bmp?' + (new Date()).getTime());

		$('#tag' + tagmac + ' .contentmode').innerHTML = contentModes[element.contentMode];
		if (element.RSSI) {
			$('#tag' + tagmac + ' .model').innerHTML = models[element.hwType];
			$('#tag' + tagmac + ' .rssi').innerHTML = element.RSSI;
			$('#tag' + tagmac + ' .lqi').innerHTML = element.LQI;
			$('#tag' + tagmac + ' .temperature').innerHTML = element.temperature;
			$('#tag' + tagmac + ' .batt').innerHTML = element.batteryMv/1000;
			$('#tag' + tagmac + ' .received').style.opacity = "1";
		} else {
			$('#tag' + tagmac + ' .model').innerHTML = "waiting for hardware type";
			$('#tag' + tagmac + ' .received').style.opacity = "0";
		}

		if (element.nextupdate > 1672531200 && element.nextupdate!=3216153600) {
			var date = new Date(element.nextupdate * 1000);
			var options = { hour: '2-digit', minute: '2-digit', second: '2-digit', hour12: false };
			$('#tag' + tagmac + ' .nextupdate').innerHTML = "<span>next update</span>" + date.toLocaleString('nl-NL', options);
		} else {
			$('#tag' + tagmac + ' .nextupdate').innerHTML = "";
		}

		if (element.nextcheckin > 1672531200) {
			div.dataset.nextcheckin = element.nextcheckin;
		} else {
			div.dataset.nextcheckin = element.lastseen + 1800;
		}

		div.style.opacity = '1';
		$('#tag' + tagmac + ' .lastseen').style.color = "black";
		div.classList.remove("tagpending");
		div.dataset.lastseen = element.lastseen;
		div.dataset.hash = element.hash;
		$('#tag' + tagmac + ' .warningicon').style.display = 'none';
		$('#tag' + tagmac).style.background = "inherit";
		$('#tag' + tagmac + ' .pendingicon').style.display = (element.pending ? 'inline-block' : 'none');
		div.classList.add("tagflash");
		(function(tagmac) {
			setTimeout(function () { $('#tag' + tagmac).classList.remove("tagflash"); }, 1400);
		})(tagmac);
		if (element.pending) div.classList.add("tagpending");
	}
}

function updatecards() {
	document.querySelectorAll('[data-mac]').forEach(item => {
		let tagmac = item.dataset.mac;

		if (item.dataset.lastseen && item.dataset.lastseen > 1672531200) {
			let idletime = (Date.now() / 1000) + servertimediff - item.dataset.lastseen;
			$('#tag' + tagmac + ' .lastseen').innerHTML = "<span>last seen</span>"+displayTime(Math.floor(idletime))+" ago";
			if ((Date.now() / 1000) + servertimediff - 300 > item.dataset.nextcheckin) {
				$('#tag' + tagmac + ' .warningicon').style.display='inline-block';
				$('#tag' + tagmac).classList.remove("tagpending")
				$('#tag' + tagmac).style.background = '#ffffcc';
			}
			if (idletime > 24*3600) {
				$('#tag' + tagmac).style.opacity = '.5';
				$('#tag' + tagmac + ' .lastseen').style.color = "red";
			}
		} else {
			$('#tag' + tagmac + ' .lastseen').innerHTML = ""
		}

		if (item.dataset.nextcheckin > 1672531200) {
			let nextcheckin = item.dataset.nextcheckin - ((Date.now() / 1000) + servertimediff);
			$('#tag' + tagmac + ' .nextcheckin').innerHTML = "<span>expected checkin</span>" + displayTime(Math.floor(nextcheckin));
		}
	})
}

$('#clearlog').onclick = function () {
	$('#messages').innerHTML='';
}

$('.closebtn').onclick = function (event) {
	event.target.parentNode.style.display='none';
}

$('#taglist').addEventListener("click", (event) => {
	let currentElement = event.target;
	while (currentElement !== $('#taglist')) {
		if (currentElement.classList.contains("tagcard")) {
			break;
		}
		currentElement = currentElement.parentNode;
	}
	if (!currentElement.classList.contains("tagcard")) {
		return;
	}
	const mac = currentElement.dataset.mac;
	//if (event.target.classList.contains("configicon")) {
		$('#cfgmac').innerHTML = mac;
		$('#cfgmac').dataset.mac = mac;
		fetch("/get_db?mac=" + mac)
			.then(response => response.json())
			.then(data => {
				console.log(data);
				var tagdata = data.tags[0];
				$('#cfgalias').value = tagdata.alias;
				$('#cfgcontent').value = tagdata.contentMode;
				$('#cfgcontent').dataset.json = tagdata.modecfgjson;
				contentselected();
				$('#configbox').style.display = 'block';
			})
			//.catch(error => showMessage('Error: ' + error));
	//}
})

$('#cfgsave').onclick = function () {

	let contentMode = $('#cfgcontent').value;
	let extraoptions = contentModeOptions[contentMode];
	let obj={};
	extraoptions.forEach(element => {
		obj[element] = $('#opt' + element).value;
	});

	let formData = new FormData();
	formData.append("mac", $('#cfgmac').dataset.mac);
	formData.append("alias", $('#cfgalias').value);
	formData.append("contentmode", contentMode);
	formData.append("modecfgjson", JSON.stringify(obj));
	fetch("/save_cfg", {
		method: "POST",
		body: formData
	})
		.then(response => response.text())
		.then(data => showMessage(data))
		.catch(error => showMessage('Error: ' + error));
	$('#configbox').style.display = 'none';
}

$('#cfgdelete').onclick = function () {
	let mac = $('#cfgmac').dataset.mac;
}

function contentselected() {
	let contentMode = $('#cfgcontent').value;
	let extraoptions = contentModeOptions[contentMode];
	$('#customoptions').innerHTML="";
	var obj = {};
	if ($('#cfgcontent').dataset.json && ($('#cfgcontent').dataset.json!="null")) {
		obj = JSON.parse($('#cfgcontent').dataset.json);
	}
	console.log(obj);
	extraoptions.forEach(element => {
		var label = document.createElement("label");
		label.innerHTML = element;
		label.setAttribute("for", 'opt' + element);
		var input = document.createElement("input");
		input.type = "text";
		input.id = 'opt' + element;
		if (obj[element]) input.value = obj[element];
		var p = document.createElement("p");
		p.appendChild(label);
		p.appendChild(input);
		$('#customoptions').appendChild(p);
	});
}

function showMessage(message,iserr) {
	const messages = $('#messages');
	var date = new Date(),
        time = date.toLocaleTimeString('nl-NL', {hour12: false, hour: '2-digit', minute:'2-digit', second:'2-digit'});
	if (iserr) {
		messages.insertAdjacentHTML("afterbegin", '<li class="new error">' + htmlEncode(time + ' ' + message) + '</li>');
	} else {
		messages.insertAdjacentHTML("afterbegin", '<li class="new">'+htmlEncode(time+' '+message)+'</li>');
	}
}

function htmlEncode(input) {
	const textArea = document.createElement("textarea");
	textArea.innerText = input;
	return textArea.innerHTML.split("<br>").join("\n");
}

function loadImage(id, imageSrc) {
	imageQueue.push({ id, imageSrc });
	if (!isProcessing) {
		processQueue();
	}
}

function processQueue() {
	if (imageQueue.length === 0) {
		isProcessing = false;
		return;
	}
	isProcessing = true;
	const { id, imageSrc } = imageQueue.shift();
	const image = $('#tag' + id + ' .tagimg');
	image.onload = function () { 
		image.style.display = 'block';
		processQueue();
	}
	image.onerror = function () { 
		image.style.display = 'none';
		processQueue();
	};
	image.src = imageSrc;
}

function displayTime(seconds) {
	let hours = Math.floor(Math.abs(seconds) / 3600);
	let minutes = Math.floor((Math.abs(seconds) % 3600) / 60);
	let remainingSeconds = Math.abs(seconds) % 60;
	return (seconds < 0 ? '-' : '') + (hours > 0 ? `${hours}:${String(minutes).padStart(2, '0')}` : `${minutes}`) + `:${String(remainingSeconds).padStart(2, '0')}`;
}
