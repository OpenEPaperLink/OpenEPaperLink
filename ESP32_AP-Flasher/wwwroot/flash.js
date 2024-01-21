const $ = document.querySelector.bind(document);
let running = false;
let buttonState = false;

const WEBFLASH_ENABLE_AUTOFLASH = 1
const WEBFLASH_PINTEST = 2
const WEBFLASH_FOCUS = 3
export const WEBFLASH_BLUR = 4

export async function init() {
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
        if (color == "clear") {
            consoleDiv.innerHTML = "";
            return;
        }

        const isScrolledToBottom = consoleDiv.scrollHeight - consoleDiv.clientHeight <= consoleDiv.scrollTop;
        const newLine = document.createElement('div');
        newLine.style.color = color;

        if (line.startsWith("<")) {
            const existingLines = consoleDiv.getElementsByTagName('div');
            let lastLine;

            for (let i = existingLines.length - 1; i >= 0; i--) {
                const lineText = existingLines[i].textContent;
                if (lineText.startsWith("  ")) {
                    lastLine = existingLines[i];
                    break;
                }
            }
            if (lastLine) {
                lastLine.innerHTML = line.substring(1) + lastLine.innerHTML.substring(line.length - 1);
                lastLine.style.color = color;
            }
        } else if (line.startsWith("\r")) {
            const existingLines = consoleDiv.getElementsByTagName('div');
            if (existingLines.length > 0) {
                const lastLine = existingLines[existingLines.length - 1];
                lastLine.innerHTML = line.substring(1);
            }
        } else {
            newLine.textContent = line;
            consoleDiv.appendChild(newLine);
        }

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
