function startPainter(mac, width, height, tagtype) {
	let isDrawing = false;
	let lastX = 0;
	let lastY = 0;
	let color = 'black';
	let linewidth = 3;
	let cursor = 'auto';
	let isAddingText = false;
	let layerDiv, intervalId, showCursor, input, textX, textY, font, sizeSelect, isDragging;
	
	var fonts = ['Roboto', 'Open Sans', 'Lato', 'Montserrat', 'PT Sans', 'Barlow Condensed', 'Headland One', 'Sofia Sans Extra Condensed', 'Mynerve', 'Lilita One', 'Passion One', 'Big Shoulders Display'];

	loadGoogleFonts(fonts);

	const canvas = document.createElement('canvas');
	canvas.width = width;
	canvas.height = height;

	const ctx = canvas.getContext('2d');
	
	ctx.fillStyle = 'white';
	ctx.fillRect(0, 0, canvas.width, canvas.height);
	ctx.imageSmoothingEnabled = false;

	$("#canvasdiv").appendChild(canvas);
	canvas.style.imageRendering = 'pixelated';

	canvas.addEventListener('mousedown', startDrawing);
	window.addEventListener('mouseup', stopDrawing);
	canvas.addEventListener('mousemove', draw);

	canvas.addEventListener('touchstart', startDrawing, { passive: false });
	window.addEventListener('touchend', stopDrawing);
	canvas.addEventListener('touchmove', draw, { passive: false });

	const bgCanvas = document.createElement('canvas');
	bgCanvas.width = canvas.width;
	bgCanvas.height = canvas.height;
	const bgCtx = bgCanvas.getContext('2d');

	bgCtx.fillStyle = '#ffffff'; 
	bgCtx.fillRect(0, 0, bgCanvas.width, bgCanvas.height);

	const txtButton = document.createElement('button');
	txtButton.innerHTML = 'tT';
	txtButton.style.fontStyle = 'italic';
	txtButton.addEventListener('click', addText);

	console.log(tagtype.colortable);
	const rgbToCSSColor = (rgbArray) => `rgb(${rgbArray[0]}, ${rgbArray[1]}, ${rgbArray[2]})`;

	let activeButton = null;

	tagtype.colortable.forEach((thiscolor, index) => {
		if (thiscolor[0] === 255 && thiscolor[1] === 255 && thiscolor[2] === 255) return;

		const colorButton = document.createElement('button');
		colorButton.innerHTML = '&#65103;&#128396';
		colorButton.style.color = rgbToCSSColor(thiscolor);
		colorButton.classList.add('colorbutton');

		colorButton.addEventListener('click', () => {
			color = rgbToCSSColor(thiscolor);
			linewidth = 3;
			cursor = 'url("data:image/svg+xml;utf8,<svg xmlns=\'http://www.w3.org/2000/svg\'><circle cx=\'2\' cy=\'2\' r=\'2\' opacity=\'0.5\'/></svg>") 2 2, auto';

			if (activeButton) activeButton.classList.remove('active');
			colorButton.classList.add('active');
			activeButton = colorButton;
		});

		if (!activeButton) {
			colorButton.classList.add('active');
			activeButton = colorButton;
			color = rgbToCSSColor(thiscolor);
		}

		$("#buttonbar").appendChild(colorButton);
	});

	const whiteButton = document.createElement('button');
	whiteButton.innerHTML = '&#11044;';
	whiteButton.style.color = 'white';
	whiteButton.addEventListener('click', () => {
		color = 'white';
		linewidth = 20;
		cursor = 'url("data:image/svg+xml;utf8,<svg xmlns=\'http://www.w3.org/2000/svg\'><circle cx=\'10\' cy=\'10\' r=\'10\' opacity=\'0.5\'/></svg>") 10 10, auto';
		if (activeButton) activeButton.classList.remove('active');
		whiteButton.classList.add('active');
		activeButton = whiteButton;
	});

	const clearButton = document.createElement('button');
	clearButton.innerHTML = '&#128437;';
	clearButton.addEventListener('click', () => {
		if (isAddingText) handleFinish(false);
		if (window.confirm("Are you sure you want to clear the canvas?")) {
			ctx.fillStyle = 'white';
			ctx.fillRect(0, 0, canvas.width, canvas.height);
		}
	});

	const uploadButton = document.createElement('button');
	uploadButton.innerHTML = 'Upload';
	uploadButton.addEventListener('click', () => {
		if (isAddingText) handleFinish(true);
		const dataURL = canvas.toDataURL('image/jpeg');
		const binaryImage = dataURLToBlob(dataURL);
		const formData = new FormData();
		formData.append('mac', mac);
		formData.append('dither', '0');
		formData.append('contentmode', '22');
		formData.append('file', binaryImage, 'image.jpg');
		const xhr = new XMLHttpRequest();
		xhr.open('POST', '/imgupload');
		xhr.send(formData);
		$('#configbox').close();
	});

	$("#buttonbar").appendChild(whiteButton);
	$("#buttonbar").appendChild(txtButton);
	$("#buttonbar").appendChild(clearButton);
	$("#savebar").appendChild(uploadButton);

	canvas.addEventListener('mouseenter', function () {
		if (!isAddingText) {
			canvas.style.cursor = cursor;
		} else {
			canvas.style.cursor = 'move';
		}
	});

	canvas.addEventListener('mouseleave', function () {
		canvas.style.cursor = 'auto';
	});

	function startDrawing(e) {
		e.stopPropagation();
		e.preventDefault();
		if (isAddingText) return;
		isDrawing = true;
		var rect = canvas.getBoundingClientRect();
		lastX = e.pageX - rect.left - window.scrollX;
		lastY = e.pageY - rect.top - window.scrollY;
	}

	function stopDrawing(e) {
		if (isAddingText) return;
		isDrawing = false;
	}

	function draw(e) {
		e.stopPropagation();
		e.preventDefault();
		if (isAddingText) return;
		if (!isDrawing) return;
		var rect = canvas.getBoundingClientRect();
		ctx.beginPath();
		ctx.moveTo(lastX, lastY);
		ctx.lineTo(e.pageX - rect.left - window.scrollX, e.pageY - rect.top - window.scrollY);
		ctx.strokeStyle = color;
		ctx.lineWidth = linewidth;
		ctx.lineCap = "round";
		ctx.stroke();
		lastX = e.pageX - rect.left - window.scrollX;
		lastY = e.pageY - rect.top - window.scrollY;
	}

	function addText() {
		if (isAddingText) {
			handleFinish(true);
			return;
		}
		txtButton.classList.add('active');
		bgCtx.drawImage(canvas, 0, 0);

		const defaultX = 5;
		const defaultY = 40;
		isDragging = false;
		let startX, startY;
		showCursor = true;

		textX = defaultX;
		textY = defaultY;
		font = '24px ' + fonts[0];

		input = document.createElement('textarea');
		input.type = 'text';
		input.placeholder = 'Type text here';
		input.style.opacity = '0';
		input.style.position = 'absolute';
		input.style.left = '-200px'

		input.addEventListener('input', () => {
			drawText(input.value, textX, textY);
		});
		input.addEventListener('keyup', () => {
			input.selectionStart = input.selectionEnd = input.value.length;
		});

		input.addEventListener('blur', function () {
			input.focus();
		});

		intervalId = setInterval(function () {
			showCursor = !showCursor;
			drawText(input.value, textX, textY);
		}, 300);

		canvas.addEventListener('mouseup', handleMouseUp);
		canvas.addEventListener('mousedown', handleMouseDown);
		canvas.addEventListener('mousemove', handleMouseMove);

		canvas.addEventListener('touchstart', handleTouchStart, { passive: true });
		canvas.addEventListener('touchend', handleTouchEnd);
		canvas.addEventListener('touchmove', handleTouchMove, { passive: true });

		var sizes = [10,11,12,13,14,16,18,20,24,28,32,36,40,48,56,64,72,84];

		const fontSelect = document.createElement('select');
		fontSelect.id = 'font-select';
		for (var i = 0; i < fonts.length; i++) {
			const option = document.createElement('option');
			option.value = fonts[i];
			option.text = fonts[i];
			option.style.fontFamily = fonts[i];
			fontSelect.appendChild(option);
		}

		sizeSelect = document.createElement('select');
		sizeSelect.id = 'size-select';
		for (var i = 0; i < sizes.length; i++) {
			const option = document.createElement('option');
			option.value = sizes[i];
			option.text = sizes[i] + ' px';
			sizeSelect.appendChild(option);
		}

		function updateFont() {
			var selectedFont = fontSelect.value;
			var selectedSize = sizeSelect.value;
			fontSelect.style.fontFamily = selectedFont;
			font = selectedSize + 'px ' + selectedFont;
			drawText(input.value, textX, textY);
		}

		fontSelect.value = fonts[0];
		sizeSelect.value = '24';
		fontSelect.addEventListener('change', updateFont);
		sizeSelect.addEventListener('change', updateFont);

		const finishButton = document.createElement('button');
		finishButton.innerHTML = '&#10004;';
		finishButton.addEventListener('click', clickHandleFinish);

		layerDiv = document.createElement('div');

		layerDiv.appendChild(input);
		layerDiv.appendChild(fontSelect);
		layerDiv.appendChild(sizeSelect);
		layerDiv.appendChild(finishButton);
		$("#layersdiv").appendChild(layerDiv);
		input.focus();

		isAddingText = true;
		//cursor = 'move';
		//blackButton.innerHTML = 'aA'
		//redButton.innerHTML = 'aA'
		const colorButtons = document.querySelectorAll('.colorbutton');
		colorButtons.forEach(button => {
			button.innerHTML = 'aA';
		});		
		if (color=='white') {
			//whiteButton.classList.remove('active');
			//blackButton.classList.add('active');
			//color='black';
			document.querySelector('.colorbutton').click();
		}
	}

	function handleFinish(apply) {
		canvas.removeEventListener('mousedown', handleMouseDown);
		canvas.removeEventListener('mouseup', handleMouseUp);
		canvas.removeEventListener('mousemove', handleMouseMove);

		canvas.removeEventListener('touchstart', handleTouchStart);
		canvas.removeEventListener('touchend', handleTouchEnd);
		canvas.removeEventListener('touchmove', handleTouchMove);
		isAddingText = false;
		cursor = 'auto';
		layerDiv.remove();
		clearInterval(intervalId);
		showCursor = false;
		if (apply) drawText(input.value, textX, textY);
		txtButton.classList.remove('active');
		//blackButton.innerHTML = '&#65103;&#128396'
		//redButton.innerHTML = '&#65103;&#128396'
		const colorButtons = document.querySelectorAll('.colorbutton');
		colorButtons.forEach(button => {
			button.innerHTML = '&#65103;&#128396';
		});		
	}

	function drawText(text, x, y) {
		ctx.drawImage(bgCanvas, 0, 0);
		ctx.save();
		ctx.translate(x, y);
		ctx.font = font;
		ctx.fillStyle = color;
		const lines = text.split('\n');
		lines.forEach((line, index) => {
			ctx.fillText(line + (showCursor && index === lines.length - 1 ? '|' : ''), 0, index * (sizeSelect.value * 1.1));
		});
		ctx.restore();
	}

	function handleMouseDown(e) {
		isDragging = true;
		startX = textX;
		startY = textY;
		({ clientX: lastMouseX, clientY: lastMouseY } = e);
	}

	function handleMouseMove(e) {
		if (isDragging) {
			const { clientX, clientY } = e;
			textX = startX + clientX - lastMouseX;
			textY = startY + clientY - lastMouseY;
			drawText(input.value, textX, textY);
		}
	}

	function handleTouchStart(e) {
		isDragging = true;
		startX = textX;
		startY = textY;
		({ clientX: lastTouchX, clientY: lastTouchY } = e.touches[0]);
	}

	function handleTouchMove(e) {
		if (isDragging) {
			const { clientX, clientY } = e.touches[0];
			textX = startX + clientX - lastTouchX;
			textY = startY + clientY - lastTouchY;
			drawText(input.value, textX, textY);
		}
	}

	function handleMouseUp(e) {
		isDragging = false;
	}

	function handleTouchEnd(e) {
		isDragging = false;
	}

	function clickHandleFinish() {
		handleFinish(true);
	}

}

function loadGoogleFonts(fonts) {
	var link = document.createElement('link');
	link.rel = 'stylesheet';
	link.href = 'https://fonts.googleapis.com/css?family=' + fonts.join('|');
	document.head.appendChild(link);
}

function dataURLToBlob(dataURL) {
	const byteString = atob(dataURL.split(',')[1]);
	const mimeString = dataURL.split(',')[0].split(':')[1].split(';')[0];
	const arrayBuffer = new ArrayBuffer(byteString.length);
	const uint8Array = new Uint8Array(arrayBuffer);
	for (let i = 0; i < byteString.length; i++) {
		uint8Array[i] = byteString.charCodeAt(i);
	}
	return new Blob([arrayBuffer], { type: mimeString });
}

paintLoaded = true;
