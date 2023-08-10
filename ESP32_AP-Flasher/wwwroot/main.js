const $ = document.querySelector.bind(document);

const WAKEUP_REASON_TIMED = 0;
const WAKEUP_REASON_BOOT = 1;
const WAKEUP_REASON_GPIO = 2;
const WAKEUP_REASON_NFC = 3;
const WAKEUP_REASON_FIRSTBOOT = 0xFC;
const WAKEUP_REASON_NETWORK_SCAN = 0xFD;
const WAKEUP_REASON_WDT_RESET = 0xFE;

var tagTypes = {};

const apstate = [
	{ state: "offline", color: "red" },
	{ state: "online", color: "green" },
	{ state: "flashing", color: "orange" },
	{ state: "wait for reset", color: "blue" },
	{ state: "requires power cycle", color: "purple" },
	{ state: "failed", color: "red" },
	{ state: "coming online", color: "yellow" }
];
const runstate = [
	{ state: "⏹︎ stopped" },
	{ state: "⏸pause" },
	{ state: "" }, // hide running
	{ state: "⏳︎ init" }
];

const imageQueue = [];
let isProcessing = false;
let servertimediff = 0;
let paintLoaded = false, paintShow = false;
var cardconfig;
let otamodule;

window.addEventListener("load", function () {
	fetch('/content_cards.json')
		.then(response => response.json())
		.then(data => {
			cardconfig = data;
			loadTags(0);
			connect();
			setInterval(updatecards, 1000);
		})
		.catch(error => {
			console.error('Error:', error);
			alert("I can\'t load /www/content_cards.json.\r\nHave you upload it to the data partition?");
		});
	fetch("/get_ap_config")
		.then(response => response.json())
		.then(data => {
			if (data.alias) {
				$(".logo").innerHTML = data.alias;
				this.document.title = data.alias;
			}
		});
});

let socket;

function loadTags(pos) {
	fetch("/get_db?pos=" + pos)
		.then(response => response.json())
		.then(data => {
			processTags(data.tags);
			if (data.continu && data.continu > pos) loadTags(data.continu);
		})
	//.catch(error => showMessage('loadTags error: ' + error));
}

function connect() {
	socket = new WebSocket("ws://" + location.host + "/ws");

	socket.addEventListener("open", (event) => {
		showMessage("websocket connected");
	});

	socket.addEventListener("message", (event) => {
		// console.log(event.data)
		const msg = JSON.parse(event.data);
		if (msg.logMsg) {
			showMessage(msg.logMsg, false);
		}
		if (msg.errMsg) {
			showMessage(msg.errMsg, true);
		}
		if (msg.tags) {
			processTags(msg.tags);
		}
		if (msg.sys) {
			let filesystem = 'filesystem free: ' + convertSize(msg.sys.littlefsfree);
			if (msg.sys.littlefsfree < 31000) {
				filesystem = 'filesystem <span class="blink-red" title="Generating content is paused">FULL! ' + convertSize(msg.sys.littlefsfree) + '</span>';
			}
			$('#sysinfo').innerHTML = 'free heap: ' + convertSize(msg.sys.heap) + ' &#x2507; db size: ' + convertSize(msg.sys.dbsize) + ' &#x2507; db record count: ' + msg.sys.recordcount + ' &#x2507; ' + filesystem;
			if (msg.sys.apstate) {
				$("#apstatecolor").style.color = apstate[msg.sys.apstate].color;
				$("#apstate").innerHTML = apstate[msg.sys.apstate].state;
				$("#runstate").innerHTML = runstate[msg.sys.runstate].state;
				if (msg.sys.temp) $("#temp").innerHTML = msg.sys.temp.toFixed(1) + '°C';
			}
			servertimediff = (Date.now() / 1000) - msg.sys.currtime;
		}
		if (msg.apitem) {
			var row = $("#aptable").insertRow();
			row.insertCell(0).innerHTML = "<a href=\"http://" + msg.apitem.ip + "\" target=\"_new\">" + msg.apitem.ip + "</a>";
			row.insertCell(1).innerHTML = msg.apitem.alias;
			row.insertCell(2).innerHTML = msg.apitem.count;
			row.insertCell(3).innerHTML = msg.apitem.channel;
			row.insertCell(4).innerHTML = msg.apitem.version;
		}
		if (msg.console) {
			if (otamodule && typeof (otamodule.print) === "function") {
				let color = "#c0c0c0";
				if (msg.console.startsWith("Fail") || msg.console.startsWith("Err")) {
					color = "red";
				}
				otamodule.print(msg.console, color);
			}
		}
	});

	socket.addEventListener("close", (event) => {
		showMessage(`websocket closed ${event.code}`);
		setTimeout(connect, 5000);
	});
}

function convertSize(bytes) {
	if (bytes >= 1073741824) { bytes = (bytes / 1073741824).toFixed(2) + " GB"; }
	else if (bytes >= 1048576) { bytes = (bytes / 1048576).toFixed(2) + " MB"; }
	else if (bytes >= 1024) { bytes = (bytes / 1024).toFixed(2) + " kB"; }
	else if (bytes > 1) { bytes = bytes + " bytes"; }
	else if (bytes == 1) { bytes = bytes + " byte"; }
	else { bytes = "0 bytes"; }
	return bytes;
}

function processTags(tagArray) {
	for (const element of tagArray) {
		tagmac = element.mac;

		var div = $('#tag' + tagmac);
		if (div == null) {
			div = $('#tagtemplate').cloneNode(true);
			div.setAttribute('id', 'tag' + tagmac);
			div.dataset.mac = tagmac;
			div.dataset.hwtype = -1;
			$('#taglist').appendChild(div);
		}

		div.style.display = 'block';

		if (element.contentMode == 255) {
			div.remove();
			showMessage(tagmac + " removed by remote AP");
			continue;
		}

		if (element.isexternal) {
			$('#tag' + tagmac + ' .mac').innerHTML = tagmac + " via ext AP";
		} else {
			$('#tag' + tagmac + ' .mac').innerHTML = tagmac;
		}
		let alias = element.alias;
		if (!alias) alias = tagmac.replace(/^0{1,4}/, '');
		if ($('#tag' + tagmac + ' .alias').innerHTML != alias) {
			$('#tag' + tagmac + ' .alias').innerHTML = alias;
			//GroupSortFilter();
		}

		let contentDefObj = getContentDefById(element.contentMode);
		if (contentDefObj) $('#tag' + tagmac + ' .contentmode').innerHTML = contentDefObj.name;
		if (element.RSSI) {
			div.dataset.hwtype = element.hwType;
			(async () => {
				const localTagmac = tagmac;
				const data = await getTagtype(element.hwType);
				$('#tag' + localTagmac + ' .model').innerHTML = data.name;
			})();
			let statusline = "";
			if (element.RSSI != 100) {
				if (element.ch > 0) statusline += `CH ${element.ch}, `;
				statusline += `RSSI ${element.RSSI}, LQI ${element.LQI}`;
			} else {
				statusline = "AP";
			}
			if (element.batteryMv != 0 && element.batteryMv != 1337) {
				statusline += ", " + (element.batteryMv >= 2600 ? "&#x2265;" : "") + (element.batteryMv / 1000) + "V";
			}
			if (element.ver != 0 && element.ver != 1) {
				$('#tag' + tagmac + ' .received').title = `fw: ${element.ver}`;
			} else {
				$('#tag' + tagmac + ' .received').title = "";
			}
			$('#tag' + tagmac + ' .received').innerHTML = statusline;
			$('#tag' + tagmac + ' .received').style.opacity = "1";
		} else {
			$('#tag' + tagmac + ' .model').innerHTML = "waiting for hardware type";
			$('#tag' + tagmac + ' .received').style.opacity = "0";
		}

		if (element.contentMode == 20) {
			$('#tag' + tagmac + ' .tagimg').style.display = 'none';
		} else if (div.dataset.hash != element.hash && div.dataset.hwtype > -1 && (element.isexternal == false || element.contentMode != 12)) {
			loadImage(tagmac, '/current/' + tagmac + '.raw?' + element.hash);
			div.dataset.hash = element.hash;
		}
		if (element.isexternal == true && element.contentMode == 12) $('#tag' + tagmac + ' .tagimg').style.display = 'none';

		if (element.nextupdate > 1672531200 && element.nextupdate != 3216153600) {
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
		div.dataset.wakeupreason = element.wakeupReason;
		div.dataset.nextupdate = element.nextupdate;
		div.dataset.channel = element.ch;
		div.dataset.isexternal = element.isexternal;
		$('#tag' + tagmac + ' .warningicon').style.display = 'none';
		$('#tag' + tagmac).style.background = "#ffffff";
		if (element.contentMode == 12) $('#tag' + tagmac).style.background = "#e4e4e0";
		switch (parseInt(element.wakeupReason)) {
			case WAKEUP_REASON_TIMED:
				break;
			case WAKEUP_REASON_BOOT:
				$('#tag' + tagmac + ' .nextcheckin').innerHTML = "<font color=yellow>First boot</font>"
				$('#tag' + tagmac).style.background = "#b0d0b0";
				break;
			case WAKEUP_REASON_GPIO:
				$('#tag' + tagmac + ' .nextcheckin').innerHTML = "GPIO wakeup"
				$('#tag' + tagmac).style.background = "#c8f1bb";
				break;
			case WAKEUP_REASON_NFC:
				$('#tag' + tagmac + ' .nextcheckin').innerHTML = "NFC wakeup"
				break;
			case WAKEUP_REASON_FIRSTBOOT:
				$('#tag' + tagmac + ' .nextcheckin').innerHTML = "<font color=yellow>First boot</font>"
				$('#tag' + tagmac).style.background = "#b0d0b0";
				break;
			case WAKEUP_REASON_NETWORK_SCAN:
				$('#tag' + tagmac + ' .nextcheckin').innerHTML = "<font color=yellow>Network scan</font>"
				$('#tag' + tagmac).style.background = "#c0c0d0";
				break;
			case WAKEUP_REASON_WDT_RESET:
				$('#tag' + tagmac + ' .nextcheckin').innerHTML = "Watchdog reset!"
				$('#tag' + tagmac).style.background = "#d0a0a0";
				break;
		}
		$('#tag' + tagmac + ' .pendingicon').style.display = (element.pending ? 'inline-block' : 'none');
		div.classList.add("tagflash");
		(function (tagmac) {
			setTimeout(function () { $('#tag' + tagmac).classList.remove("tagflash"); }, 1400);
		})(tagmac);
		if (element.pending) div.classList.add("tagpending");
	}
	GroupSortFilter();
}

function updatecards() {
	$('#taglist').querySelectorAll('[data-mac]').forEach(item => {
		let tagmac = item.dataset.mac;

		if (item.dataset.lastseen && item.dataset.lastseen > 1672531200) {
			let idletime = (Date.now() / 1000) - servertimediff - item.dataset.lastseen;
			$('#tag' + tagmac + ' .lastseen').innerHTML = "<span>last seen</span>" + displayTime(Math.floor(idletime)) + " ago";
			if ((Date.now() / 1000) - servertimediff - 600 > item.dataset.nextcheckin) {
				$('#tag' + tagmac + ' .warningicon').style.display = 'inline-block';
				$('#tag' + tagmac).classList.remove("tagpending")
				$('#tag' + tagmac).style.background = '#e0e0a0';
			}
			if (idletime > 24 * 3600) {
				$('#tag' + tagmac).style.opacity = '.5';
				$('#tag' + tagmac + ' .lastseen').style.color = "red";
			}
		} else {
			if ($('#tag' + tagmac + ' .lastseen')) {
				$('#tag' + tagmac + ' .lastseen').innerHTML = ""
			} else {
				console.log(tagmac + " not found")
			}
		}

		if (item.dataset.nextcheckin > 1672531200 && parseInt(item.dataset.wakeupreason) == 0) {
			let nextcheckin = item.dataset.nextcheckin - ((Date.now() / 1000) - servertimediff);
			$('#tag' + tagmac + ' .nextcheckin').innerHTML = "<span>expected checkin</span>" + displayTime(Math.floor(nextcheckin));
		}
	})
}

$('#clearlog').onclick = function () {
	$('#messages').innerHTML = '';
}

document.querySelectorAll('.closebtn').forEach(button => {
	button.addEventListener('click', (event) => {
		event.target.parentNode.style.display = 'none';
		$('#advancedoptions').style.height = '0px';
	});
});

//clicking on a tag: load config dialog for tag
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
	loadContentCard(mac);
})

function loadContentCard(mac) {
	$('#cfgmac').innerHTML = mac;
	$('#cfgmac').dataset.mac = mac;
	fetch("/get_db?mac=" + mac)
		.then(response => response.json())
		.then(data => {
			var tagdata = data.tags[0];
			$('#cfgalias').value = tagdata.alias;
			$('#cfgmore').style.display = "none";
			if (populateSelectTag(tagdata.hwType, tagdata.capabilities)) {
				$('#cfgcontent').parentNode.style.display = "flex";
				$('#cfgcontent').value = tagdata.contentMode;
				$('#cfgcontent').dataset.json = tagdata.modecfgjson;
				contentselected();
				if (tagdata.contentMode != 12) $('#cfgmore').style.display = 'block';
			} else {
				$('#customoptions').innerHTML = "";
				$('#cfgcontent').parentNode.style.display = "none";
			}
			$('#cfgrotate').value = tagdata.rotate;
			$('#cfglut').value = tagdata.lut;
			$('#cfgmore').innerHTML = '&#x25BC;';
			$('#configbox').style.display = 'block';
		})
}

let typedString = '';
document.addEventListener('keypress', (event) => {
	const keyPressed = event.key;
	if (keyPressed.length === 1) {
		typedString += keyPressed;
	} else if (keyPressed === 'Enter') {
		typedString = ('0000' + typedString).slice(-16);
		const hexRegExp = /^[0-9A-Fa-f]{16}$/;
		const isMac = typedString.match(hexRegExp);
		if (isMac) {
			console.log('scanned ' + typedString);
			loadContentCard(typedString);
		}
		typedString = '';
	}
});

$('#cfgmore').onclick = function () {
	$('#cfgmore').innerHTML = $('#advancedoptions').style.height == '0px' ? '&#x25B2;' : '&#x25BC;';
	$('#advancedoptions').style.height = $('#advancedoptions').style.height == '0px' ? $('#advancedoptions').scrollHeight + 'px' : '0px';
};

$('#cfgsave').onclick = function () {
	let contentMode = $('#cfgcontent').value;
	let contentDef = getContentDefById(contentMode);
	let extraoptions = contentDef?.param ?? null;
	let obj = {};

	let formData = new FormData();
	formData.append("mac", $('#cfgmac').dataset.mac);
	formData.append("alias", $('#cfgalias').value);

	if (contentMode) {
		extraoptions.forEach(element => {
			if ($('#opt' + element.key)) {
				obj[element.key] = $('#opt' + element.key).value;
			}
		});

		formData.append("contentmode", contentMode);
		formData.append("modecfgjson", JSON.stringify(obj));
	} else {
		formData.append("contentmode", "0");
		formData.append("modecfgjson", String());
	}

	formData.append("rotate", $('#cfgrotate').value);
	formData.append("lut", $('#cfglut').value);

	fetch("/save_cfg", {
		method: "POST",
		body: formData
	})
		.then(response => response.text())
		.then(data => showMessage(data))
		.catch(error => showMessage('Error: ' + error));

	$('#advancedoptions').style.height = '0px';
	$('#configbox').style.display = 'none';
}

function sendCmd(mac, cmd) {
	let formData = new FormData();
	formData.append("mac", mac);
	formData.append("cmd", cmd);
	fetch("/tag_cmd", {
		method: "POST",
		body: formData
	})
		.then(response => response.text())
		.then(data => {
			var div = $('#tag' + $('#cfgmac').dataset.mac);
			if (cmd == "del") div.remove();
			showMessage(data);
		})
		.catch(error => showMessage('Error: ' + error));
	$('#advancedoptions').style.height = '0px';
	$('#configbox').style.display = 'none';
}

$('#cfgdelete').onclick = function () {
	sendCmd($('#cfgmac').dataset.mac, "del");
}

$('#cfgclrpending').onclick = function () {
	sendCmd($('#cfgmac').dataset.mac, "clear");
}

$('#cfgrefresh').onclick = function () {
	sendCmd($('#cfgmac').dataset.mac, "refresh");
}

$('#cfgtagreboot').onclick = function () {
	sendCmd($('#cfgmac').dataset.mac, "reboot");
}

$('#cfgscan').onclick = function () {
	sendCmd($('#cfgmac').dataset.mac, "scan");
}

$('#cfgdeepsleep').onclick = function () {
	sendCmd($('#cfgmac').dataset.mac, "deepsleep");
}

$('#cfgreset').onclick = function () {
	sendCmd($('#cfgmac').dataset.mac, "reset");
}

$('#rebootbutton').onclick = function () {
	showMessage("rebooting AP....", true);
	fetch("/reboot", {
		method: "POST"
	});
	socket.close();
}

$('#apconfigbutton').onclick = function () {
	var table = document.getElementById("aptable");
	var rowCount = table.rows.length;
	for (var i = rowCount - 1; i > 0; i--) {
		table.deleteRow(i);
	}
	fetch("/get_ap_config")
		.then(response => response.json())
		.then(data => {
			$('#apcfgalias').value = data.alias;
			$('#apcfgchid').value = data.channel;
			$("#apcfgledbrightness").value = data.led;
			$("#apcfglanguage").value = data.language;
			$("#apclatency").value = data.maxsleep;
			$("#apcpreventsleep").value = data.stopsleep;
			$("#apcpreview").value = data.preview;
			$("#apcwifipower").value = data.wifipower;
			$("#apctimezone").value = data.timezone;
		})
	$('#apconfigbox').style.display = 'block'
}

$('#apcfgsave').onclick = function () {
	let formData = new FormData();
	formData.append("alias", $('#apcfgalias').value);
	formData.append("channel", $('#apcfgchid').value);
	formData.append('led', $('#apcfgledbrightness').value);
	formData.append('language', $('#apcfglanguage').value);
	formData.append('maxsleep', $('#apclatency').value);
	formData.append('stopsleep', $('#apcpreventsleep').value);
	formData.append('preview', $('#apcpreview').value);
	formData.append('wifipower', $('#apcwifipower').value);
	formData.append('timezone', $('#apctimezone').value);
	fetch("/save_apcfg", {
		method: "POST",
		body: formData
	})
		.then(response => response.text())
		.then(data => showMessage(data))
		.catch(error => showMessage('Error: ' + error));
	$(".logo").innerHTML = $('#apcfgalias').value;
	$('#apconfigbox').style.display = 'none';
}

$('#updatebutton').onclick = function () {
	$('#apconfigbox').style.display = 'none';
	$('#apupdatebox').style.display = 'block';
	loadOTA();
}

async function loadOTA() {
	otamodule = await import('./ota.js?v=' + Date.now());
	otamodule.initUpdate();
}

$('#paintbutton').onclick = function () {
	if (paintShow) {
		paintShow = false;
		$('#cfgsave').parentNode.style.display = 'block';
		contentselected();
	} else {
		paintShow = true;
		$('#cfgsave').parentNode.style.display = 'none';
		$('#customoptions').innerHTML = "<div id=\"buttonbar\"></div><div id=\"canvasdiv\"></div><div id=\"layersdiv\"></div><p id=\"savebar\"></p>";
		const mac = $('#cfgmac').dataset.mac
		const hwtype = $('#tag' + mac).dataset.hwtype;
		var [width, height] = [tagTypes[hwtype].width, tagTypes[hwtype].height] || [0, 0];
		if (paintLoaded) {
			startPainter(mac, width, height);
		} else {
			loadScript('painter.js', function () {
				startPainter(mac, width, height);
			});
		}
	}
}

function loadScript(url, callback) {
	var script = document.createElement('script');
	script.src = url;
	script.onload = function () {
		if (callback) {
			callback();
		}
	};
	document.head.appendChild(script);
}

function contentselected() {
	let contentMode = $('#cfgcontent').value;
	$('#customoptions').innerHTML = "";
	var obj = {};
	if ($('#cfgcontent').dataset.json && ($('#cfgcontent').dataset.json != "null")) {
		obj = JSON.parse($('#cfgcontent').dataset.json);
	}
	$('#paintbutton').style.display = 'none';
	if (contentMode) {
		let contentDef = getContentDefById(contentMode);
		if (contentDef) {
			$('#customoptions').innerHTML = "<p>" + contentDef?.desc + "</p>"
		}
		$('#paintbutton').style.display = (contentMode == 0 ? 'inline-block' : 'none');
		let extraoptions = contentDef?.param ?? null;
		extraoptions.forEach(element => {
			var label = document.createElement("label");
			label.innerHTML = element.name;
			label.setAttribute("for", 'opt' + element.key);
			if (element.desc) {
				label.style.cursor = 'help';
				label.title = element.desc;
			}
			var input = document.createElement("input");
			switch (element.type) {
				case 'text':
					input.type = "text";
					break;
				case 'int':
					input.type = "number";
					break;
				case 'ro':
					input.type = "text";
					input.disabled = true;
					break;
				case 'select':
					input = document.createElement("select");
					for (const key in element.options) {
						const optionElement = document.createElement("option");
						optionElement.value = key;
						optionElement.text = element.options[key];
						if (element.options[key].substring(0, 1) == "-") {
							optionElement.text = element.options[key].substring(1);
							optionElement.selected = true;
						} else {
							optionElement.selected = false;
						}
						input.appendChild(optionElement);
					}
					break;
			}
			input.id = 'opt' + element.key;
			input.title = element.desc;
			if (obj[element.key]) input.value = obj[element.key];
			var p = document.createElement("p");
			p.appendChild(label);
			p.appendChild(input);
			$('#customoptions').appendChild(p);
		});
	}
	paintShow = false;
	$('#cfgsave').parentNode.style.display = 'block';
}

function populateSelectTag(hwtype, capabilities) {
	var selectTag = $("#cfgcontent");
	selectTag.innerHTML = "";
	var optionsAdded = false;
	var option;
	cardconfig.forEach(item => {
		var capcheck = item.capabilities ?? 0;
		var hwtypeArray = item.hwtype;
		if ((hwtypeArray.includes(hwtype) || tagTypes[hwtype].contentids.includes(item.id)) && (capabilities & capcheck || capcheck == 0)) {
			option = document.createElement("option");
			option.value = item.id;
			option.text = item.name;
			selectTag.appendChild(option);
			optionsAdded = true;
		}
	});

	var rotateTag = $("#cfgrotate");
	rotateTag.innerHTML = "";

	for (let i = 0; i < 4; i++) {
		if (i == 0 || tagTypes[hwtype].width == tagTypes[hwtype].height || (i == 2)) {
			option = document.createElement("option");
			option.value = i;
			option.text = (i * 90) + " degrees";
			rotateTag.appendChild(option);
		}
	}

	var lutTag = $("#cfglut");
	lutTag.innerHTML = "";

	option = document.createElement("option");
	option.value = "0";
	option.text = "auto";
	lutTag.appendChild(option);

	if (hwtype != 240) {
		option = document.createElement("option");
		option.value = "1";
		option.text = "Always full refresh";
		lutTag.appendChild(option);
	}

	return optionsAdded;
}

function getContentDefById(id) {
	if (id == null) return null;
	var obj = cardconfig.find(item => item.id == id);
	return obj ? obj : null;
}

function showMessage(message, iserr) {
	const messages = $('#messages');
	var date = new Date(),
		time = date.toLocaleTimeString('nl-NL', { hour12: false, hour: '2-digit', minute: '2-digit', second: '2-digit' });
	if (iserr) {
		messages.insertAdjacentHTML("afterbegin", '<li class="new error">' + htmlEncode(time + ' ' + message) + '</li>');
	} else {
		messages.insertAdjacentHTML("afterbegin", '<li class="new">' + htmlEncode(time + ' ' + message) + '</li>');
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
	const canvas = $('#tag' + id + ' .tagimg');
	canvas.style.display = 'block';
	const hwtype = $('#tag' + id).dataset.hwtype;
	if (tagTypes[hwtype] && tagTypes[hwtype].busy) setTimeout(processQueue, 50);

	fetch(imageSrc, { cache: "force-cache" })
		.then(response => response.arrayBuffer())
		.then(buffer => {
			[canvas.width, canvas.height] = [tagTypes[hwtype].width, tagTypes[hwtype].height] || [0, 0];
			if (tagTypes[hwtype].rotatebuffer) [canvas.width, canvas.height] = [canvas.height, canvas.width];
			const ctx = canvas.getContext('2d');
			const imageData = ctx.createImageData(canvas.width, canvas.height);
			const data = new Uint8ClampedArray(buffer);

			if (tagTypes[hwtype].bpp == 16) {

				const is16Bit = data.length == tagTypes[hwtype].width * tagTypes[hwtype].height * 2;
				for (let i = 0; i < tagTypes[hwtype].width * tagTypes[hwtype].height; i++) {
					const dataIndex = is16Bit ? i * 2 : i;
					const rgb = is16Bit ? (data[dataIndex] << 8) | data[dataIndex + 1] : data[dataIndex];

					imageData.data[i * 4] = is16Bit ? ((rgb >> 11) & 0x1F) << 3 : (((rgb >> 5) & 0x07) << 5) * 1.13;
					imageData.data[i * 4 + 1] = is16Bit ? ((rgb >> 5) & 0x3F) << 2 : (((rgb >> 2) & 0x07) << 5) * 1.13;
					imageData.data[i * 4 + 2] = is16Bit ? (rgb & 0x1F) << 3 : ((rgb & 0x03) << 6) * 1.3;
					imageData.data[i * 4 + 3] = 255;
				}

			} else {

				const offsetRed = (data.length >= (canvas.width * canvas.height / 8) * 2) ? canvas.width * canvas.height / 8 : 0;
				var pixelValue = 0;
				var colorTable = tagTypes[hwtype].colortable;
				for (let i = 0; i < data.length; i++) {
					for (let j = 0; j < 8; j++) {
						const pixelIndex = i * 8 + j;
						if (offsetRed) {
							pixelValue = ((data[i] & (1 << (7 - j))) ? 1 : 0) | (((data[i + offsetRed] & (1 << (7 - j))) ? 1 : 0) << 1);
						} else {
							pixelValue = ((data[i] & (1 << (7 - j))) ? 1 : 0);
						}
						imageData.data[pixelIndex * 4] = colorTable[pixelValue][0];
						imageData.data[pixelIndex * 4 + 1] = colorTable[pixelValue][1];
						imageData.data[pixelIndex * 4 + 2] = colorTable[pixelValue][2];
						imageData.data[pixelIndex * 4 + 3] = 255;
					}
				}
			}

			ctx.putImageData(imageData, 0, 0);
			processQueue();
		})
		.catch(error => {
			processQueue();
		});
}

function displayTime(seconds) {
	let hours = Math.floor(Math.abs(seconds) / 3600);
	let minutes = Math.floor((Math.abs(seconds) % 3600) / 60);
	let remainingSeconds = Math.abs(seconds) % 60;
	return (seconds < 0 ? '-' : '') + (hours > 0 ? `${hours}:${String(minutes).padStart(2, '0')}` : `${minutes}`) + `:${String(remainingSeconds).padStart(2, '0')}`;
}

$("#filterOptions").addEventListener("click", function (event) {
	if (event.target.tagName === "INPUT") {
		GroupSortFilter();
	}
});

function GroupSortFilter() {
	const sortableGrid = $('#taglist');
	const gridItems = Array.from(sortableGrid.querySelectorAll('.tagcard:not(#tagtemplate)'));

	let grouping = document.querySelector('input[name="group"]:checked')?.value;
	let sorting = document.querySelector('input[name="sort"]:checked')?.value ?? 'alias';

	gridItems.sort((a, b) => {
		let itemA = String(sorting).startsWith('data-') ? a.dataset[sorting.slice(5)] : a.querySelector('.' + sorting).textContent;
		let itemB = String(sorting).startsWith('data-') ? b.dataset[sorting.slice(5)] : b.querySelector('.' + sorting).textContent;
		if (sorting == 'data-lastseen') [itemA, itemB] = [itemB, itemA];
		if (grouping) {
			let groupA = String(grouping).startsWith('data-') ? a.dataset[grouping.slice(5)] : a.querySelector('.' + grouping).textContent;
			let groupB = String(grouping).startsWith('data-') ? b.dataset[grouping.slice(5)] : b.querySelector('.' + grouping).textContent;
			if (groupA !== groupB) {
				return groupA.localeCompare(groupB);
			} else {
				return itemA.localeCompare(itemB);
			}
		} else {
			return itemA.localeCompare(itemB);
		}
	});

	let currentGroup = null;
	let order = 1;

	let headItems = Array.from($('#taglist').getElementsByClassName('taggroup'));
	headItems.forEach(item => {
		item.dataset.clean = 1;
	})

	gridItems.forEach(item => {

		if (grouping) {
			const group = String(grouping).startsWith('data-') ? item.dataset[grouping.slice(5)] || '' : item.querySelector('.' + grouping).textContent || '';
			if (group !== currentGroup && group != '') {
				let header = document.getElementById('header' + group);
				if (!header) {
					header = document.createElement('div');
					switch (grouping) {
						case 'model':
							header.textContent = 'Tag model: ' + group;
							break;
						case 'contentmode':
							header.textContent = 'Content: ' + group;
							break;
						case 'data-channel':
							header.textContent = 'Channel: ' + group;
							break;
					}
					header.classList.add('taggroup');
					header.id = 'header' + group;
					sortableGrid.appendChild(header);
				}
				header.style.order = order++;
				header.dataset.clean = 0;
				currentGroup = group;
			}
		}

		let show = true;
		if ($('input[name="filter"][value="remote"]').checked && item.dataset.isexternal == "false") show = false;
		if ($('input[name="filter"][value="local"]').checked && item.dataset.isexternal == "true") show = false;
		if ($('input[name="filter"][value="inactive"]').checked && item.querySelector('.warningicon').style.display != 'inline-block') show = false;
		if ($('input[name="filter"][value="pending"]').checked && !item.classList.contains("tagpending")) show = false;
		if (show == false) item.style.display = 'none'; else item.style.display = 'block';
		item.style.order = order++;
	});

	headItems = Array.from($('#taglist').getElementsByClassName('taggroup'));
	headItems.forEach(item => {
		if (item.dataset.clean == 1) item.parentNode.removeChild(item);
	})
}

$('#toggleFilters').addEventListener('click', () => {
	event.preventDefault();
	const filterOptions = $('#filterOptions');
	filterOptions.classList.toggle('active');
	if (filterOptions.classList.contains('active')) {
		filterOptions.style.maxHeight = filterOptions.scrollHeight + 20 + 'px';
	} else {
		filterOptions.style.maxHeight = 0;
	}
});

async function getTagtype(hwtype) {
	if (tagTypes[hwtype] && tagTypes[hwtype].busy) {
		await new Promise(resolve => {
			const checkBusy = setInterval(() => {
				if (!tagTypes[hwtype].busy) {
					clearInterval(checkBusy);
					resolve();
				}
			}, 10);
		});
	}

	if (tagTypes[hwtype]) {
		return tagTypes[hwtype];
	}

	try {
		tagTypes[hwtype] = { busy: true };
		const response = await fetch('/tagtypes/' + hwtype.toString(16).padStart(2, '0').toUpperCase() + '.json');
		if (!response.ok) {
			let data = { name: 'unknown id ' + hwtype, width: 0, height: 0, bpp: 0, rotatebuffer: 0, colortable: [], busy: false };
			tagTypes[hwtype] = data;
			return data;
		}
		const jsonData = await response.json();
		let data = {
			name: jsonData.name,
			width: parseInt(jsonData.width),
			height: parseInt(jsonData.height),
			bpp: parseInt(jsonData.bpp),
			rotatebuffer: jsonData.rotatebuffer,
			colortable: Object.values(jsonData.colortable),
			contentids: Object.values(jsonData.contentids ?? []),
			busy: false
		};
		tagTypes[hwtype] = data;
		return data;

	} catch (error) {
		console.error('Error fetching data:', error);
		return null;
	}
}
