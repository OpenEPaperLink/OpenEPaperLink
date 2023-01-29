const $ = document.querySelector.bind(document);

let socket;

function connect() {
	socket = new WebSocket("ws://" + location.host + "/ws");

	socket.addEventListener("open", (event) => {
		showMessage("websocket connected");
	});

	socket.addEventListener("message", (event) => {
		console.log(event.data)
		const msg = JSON.parse(event.data);
		if (msg.logMsg) {
			showMessage(msg.logMsg);
		}
		if (msg.tags) {
			processTags(msg.tags);
		}
	});

	socket.addEventListener("close", (event) => {
		showMessage(`websocket closed ${event.code}`);
		setTimeout(connect, 5000);
	});
}

connect();

function processTags(tagArray) {
	var date = new Date(),
		dateString = date.toLocaleDateString('nl-NL', { day: '2-digit', month: '2-digit', year: 'numeric' }),
		time = date.toLocaleTimeString('nl-NL', { hour12: false, hour: '2-digit', minute: '2-digit', second: '2-digit' });
	for (const element of tagArray) {
		tagmac = element.mac;
		if ($('#tag' + tagmac) == null) {
			const div = $('#tagtemplate').cloneNode(true);
			div.setAttribute('id', 'tag'+tagmac);
			div.dataset.mac = tagmac;
			$('#taglist').appendChild(div);
			$('#tag' + tagmac + ' .mac').innerHTML = tagmac;
			$('#tag' + tagmac + ' .alias').innerHTML = tagmac;
			$('#tag' + tagmac + ' .lastseen').innerHTML = dateString+' '+time;
			var img = $('#tag' + tagmac + ' .tagimg')
			img.addEventListener('error', function handleError() {
				img.style.display = 'none';
			});	
			img.style.display = 'block';		
			img.src = '/edit?edit=current/' + tagmac +'.bmp';
			//+'<br>button: '+element.buttonstate 
			$('#taglist').appendChild(div);
		} else {
			$('#tag' + tagmac + ' .lastseen').innerHTML = dateString + ' ' + time;
			var img = $('#tag' + tagmac + ' .tagimg')
			img.style.display = 'block';		
			img.src = '/edit?edit=current/' + tagmac + '.bmp&' + date;
		}
		console.log(element.buttonstate);
	}
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
		console.log('config')
	}

});

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