const $ = document.querySelector.bind(document);
let running = false;
let buttonState = false;

const WEBFLASH_ENABLE_AUTOFLASH = 1
const WEBFLASH_ENABLE_USBFLASHER = 2
const WEBFLASH_FOCUS = 3
export const WEBFLASH_BLUR = 4

export async function init() {
    wsCmd(WEBFLASH_FOCUS);
    checkTagFW();
}

export function wsCmd(command) {
    const dataToSend = {
        flashcmd: command,
    };
    const jsonData = JSON.stringify(dataToSend);
    socket.send(jsonData);
}

$('#doAutoflash').onclick = function () {
    if (running) return;
    disableButtons(true);
    running = true;

    wsCmd(WEBFLASH_ENABLE_AUTOFLASH);

    running = false;
    disableButtons(false);
}

$('#doUSBflash').onclick = function () {
    if (running) return;
    disableButtons(true);
    running = true;

    wsCmd(WEBFLASH_ENABLE_USBFLASHER);

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

const fetchAndPost = async (url, name, path) => {
    try {
        print("updating " + path);
        const response = await fetch(url);
        const fileContent = await response.blob();

        const formData = new FormData();
        formData.append('path', path);
        formData.append('file', fileContent, name);

        const uploadResponse = await fetch('/littlefs_put', {
            method: 'POST',
            body: formData
        });

        if (!uploadResponse.ok) {
            print(`${response.status} ${response.body}`, "red");
            errors++;
        } else {
            print(`Firmware file downloaded`, "green");
        }
    } catch (error) {
        print('error: ' + error, "red");
        errors++;
    }
};

async function checkTagFW() {
    const fwfile = "/Tag_FW_Pack.bin";
    const url = "/check_file?path=" + encodeURIComponent(fwfile);
    const response = await fetch(url);
    if (response.ok) {
        const data = await response.json();
        if (data.filesize > 0) {
            print(`File ${fwfile} found`, "green");
        } else {
            print(`File ${fwfile} not found. Downloading...`, "red");
            await fetchAndPost("https://raw.githubusercontent.com/jjwbruijn/OpenEPaperLink/master/binaries/Tag/Tag_FW_Pack.bin", "Tag_FW_Pack.bin", fwfile);
        }
    } else {
        print(`error checking file ${file.path}: ${response.status}`, "red");
        errors++;
    }
}
