const $ = document.querySelector.bind(document);
let running = false;
let buttonState = false;

const WEBFLASH_ENABLE_AUTOFLASH = 1
const WEBFLASH_PINTEST = 2
const WEBFLASH_FOCUS = 3
export const WEBFLASH_BLUR = 4

export async function init() {
    print("init");
    wsCmd(WEBFLASH_FOCUS);
}

export function wsCmd(command) {
    const dataToSend = {
        flashcmd: command,
    };
    const jsonData = JSON.stringify(dataToSend);
    socket.send(jsonData);
}

$('#testButton').onclick = function () {
    if (running) return;
    disableButtons(true);
    running = true;

    wsCmd(WEBFLASH_PINTEST);

    running = false;
    disableButtons(false);
}

export function print(line, color = "white") {
    const consoleDiv = document.getElementById('flashconsole');
    if (consoleDiv) {
        const isScrolledToBottom = consoleDiv.scrollHeight - consoleDiv.clientHeight <= consoleDiv.scrollTop;
        const newLine = document.createElement('div');
        newLine.style.color = color;
        newLine.textContent = line;
        consoleDiv.appendChild(newLine);
        if (isScrolledToBottom) {
            consoleDiv.scrollTop = consoleDiv.scrollHeight;
        }
    }
}

function disableButtons(active) {
    $("#flashtab").querySelectorAll('button').forEach(button => {
        button.disabled = active;
    });
    buttonState = active;
}
