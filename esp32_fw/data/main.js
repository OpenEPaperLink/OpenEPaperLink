const $ = document.querySelector.bind(document);

const contentModes = ["static image", "current date", "count days", "count hours","current weather","public transport","memo text"];
const models = ["unknown", "1.54\" 152x152px", "2.9\" 296x128px", "4.2\" 400x300px"];


let socket;
connect();
setInterval(updatecards, 1000);
window.addEventListener("load", function () {
	fetch("/get_db")
		.then(response => response.json())
		.then(data => {
			processTags(data.tags);
		})
		.catch(error => showMessage('Error: ' + error));
});


function connect() {
	socket = new WebSocket("ws://" + location.host + "/ws");

	socket.addEventListener("open", (event) => {
		showMessage("websocket connected");
	});

	socket.addEventListener("message", (event) => {
		console.log(event.data);
		const msg = JSON.parse(event.data);
		if (msg.logMsg) {
			showMessage(msg.logMsg);
		}
		if (msg.tags) {
			processTags(msg.tags);
		}
		if (msg.sys) {
			$('#sysinfo').innerHTML = 'free heap: ' + msg.sys.heap + ' bytes &#x2507; db size: ' + msg.sys.dbsize + ' bytes &#x2507; db record count: ' + msg.sys.recordcount + ' &#x2507; littlefs free: ' + msg.sys.littlefsfree + ' bytes';
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

		var img = $('#tag' + tagmac + ' .tagimg');
		img.style.display = 'block';
		img.src = '/edit?edit=current/' + tagmac + '.bmp&' + (new Date()).getTime();

		$('#tag' + tagmac + ' .contentmode').innerHTML = contentModes[element.contentmode];
		$('#tag' + tagmac + ' .model').innerHTML = models[element.model];

		var date = new Date(element.nextupdate * 1000);
		var options = { year: 'numeric', month: '2-digit', day: '2-digit', hour: '2-digit', minute: '2-digit', second: '2-digit', hour12: false };
		$('#tag' + tagmac + ' .nextupdate').innerHTML = date.toLocaleString('nl-NL', options).replace(',', '');

		date = new Date(element.lastseen * 1000);
		var options = {	year: 'numeric', month: '2-digit', day: '2-digit', hour: '2-digit', minute: '2-digit', second: '2-digit', hour12: false};
		$('#tag' + tagmac + ' .lastseen').innerHTML = date.toLocaleString('nl-NL', options).replace(',', '');

		div.dataset.lastseen = element.lastseen;
		div.dataset.lastseenlocaltime = Date.now();
		$('#tag' + tagmac + ' .warningicon').style.display = 'none';

		if (element.pending) $('#tag' + tagmac + ' .pending').innerHTML = "pending..."; else $('#tag' + tagmac + ' .pending').innerHTML = "";

	}
}

function updatecards() {
	document.querySelectorAll('[data-mac]').forEach(item => {
		var tagmac = item.dataset.mac;
		var idletime = Date.now() - item.dataset.lastseenlocaltime;
		$('#tag' + tagmac + ' .idletime').innerHTML = int(idletime);
		if (idletime > 300000) $('#tag' + tagmac + ' .warningicon').style.display='inline-block';
		if (idletime > 1800000) $('#tag' + tagmac).style.display = 'none';
	})
}

$('#send_image').onclick = function() {
	let formData = new FormData();
	formData.append("dst", $("#dstmac").value);
	formData.append("filename", $("#imgfile").value);
	formData.append("ttl", $("#ttl").value);
	fetch("/send_image", {
		method: "POST",
		body: formData
	})
		.then(response => response.text())
		.then(data => showMessage(data))
		.catch(error => showMessage('Error: ' + error));
}

$('#send_fw').onclick = function () {
	let formData = new FormData();
	formData.append("dst", $("#dstmac").value);
	formData.append("filename", $("#fwfile").value);
	fetch("/send_fw", {
		method: "POST",
		body: formData
	})
		.then(response => response.text())
		.then(data => showMessage(data))
		.catch(error => showMessage('Error: ' + error));
}

$('#req_checkin').onclick = function () {
	let formData = new FormData();
	formData.append("dst", $("#dstmac").value);
	fetch("/req_checkin", {
		method: "POST",
		body: formData
	})
		.then(response => response.text())
		.then(data => showMessage(data))
		.catch(error => showMessage('Error: ' + error));
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
	if (event.target.classList.contains("mac")) {
		$('#dstmac').value=mac;
	}
	if (event.target.classList.contains("configicon")) {
		$('#cfgmac').innerHTML = mac;
		$('#cfgmac').dataset.mac = mac;
		fetch("/get_db?mac=" + mac)
			.then(response => response.json())
			.then(data => {
				console.log(data);
				var tagdata = data.tags[0];
				$('#cfgalias').value = tagdata.alias;
				$('#cfgcontent').value = tagdata.contentmode;
				$('#cfgmodel').value = tagdata.model;
				$('#configbox').style.display = 'block';
			})
			.catch(error => showMessage('Error: ' + error));
	}
})

$('#cfgsave').onclick = function () {
	let formData = new FormData();
	formData.append("mac", $('#cfgmac').dataset.mac);
	formData.append("alias", $('#cfgalias').value);
	formData.append("contentmode", $('#cfgcontent').value);
	formData.append("model", $('#cfgmodel').value);
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

function showMessage(message) {
	const messages = $('#messages');
	var date = new Date(),
        time = date.toLocaleTimeString('en-US', {hour12: false, hour: '2-digit', minute:'2-digit', second:'2-digit'});
	messages.insertAdjacentHTML("afterbegin", '<li class="new">'+htmlEncode(time+' '+message)+'</li>');
}

function htmlEncode(input) {
	const textArea = document.createElement("textarea");
	textArea.innerText = input;
	return textArea.innerHTML.split("<br>").join("\n");
}