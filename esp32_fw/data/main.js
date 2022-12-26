$.fn.htmlTo = function(elem) {
	return this.each(function() {
		$(elem).html($(this).html());
	});
}

let socket = new WebSocket("ws://" + location.host + "/ws");

socket.onmessage = function(event) {
	let incomingMessage = event.data;
	showMessage(incomingMessage);
};

socket.onclose = event => console.log(`Closed ${event.code}`);

function showMessage(message) {
$( "<li/>", {
"class": "new",
html: message
}).prependTo( "ul.messages" );
}


