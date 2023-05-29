const repoUrl = 'https://api.github.com/repos/nlimper/OpenEPaperLink/releases';

const $ = document.querySelector.bind(document);

let running = false;
let errors = 0;
let env = '', currentVer = '', currentBuildtime = 0;
let buttonState = false;

export async function initUpdate() {
    if (!$("#updateconsole")) {
        const consoleDiv = document.createElement('div');
        consoleDiv.classList.add('console');
        consoleDiv.id = "updateconsole";
        $('#apupdatebox').appendChild(consoleDiv);
    }
    $("#updateconsole").innerHTML = "";

    const response = await fetch("/version.txt");
    let filesystemversion = await response.text();
    if (!filesystemversion) filesystemversion = "unknown";

    fetch("/sysinfo")
        .then(response => {
            if (response.status != 200) {
                print("Error fetching sysinfo: " + response.status, "red");
                if (response.status == 404) {
                    print("Your current firmware version is not yet capable of updating OTA.");
                    print("Update it manually one last time.");
                    disableButtons(true);
                }
                return "{}";
            } else {
                return response.json();
            }
        })
        .then(data => {
            if (data.env) {
                print(`env:        ${data.env}`);
                print(`build date: ${formatEpoch(data.buildtime)}`);
                print(`version:    ${data.buildversion}`);
                print(`filesystem: ${filesystemversion}`);
                print(`sha:        ${data.sha}`);
                print(`psram size: ${data.psramsize}`);
                print(`flash size: ${data.flashsize}`);
                print("--------------------------","gray");
                env = data.env;
                currentVer = data.buildversion;
                currentBuildtime = data.buildtime;
                if (data.rollback) $("#rollbackOption").display = 'block';
            }
        })
        .catch(error => {
            print('Error fetching sysinfo: ' + error, "red");
        });

    fetch(repoUrl)
        .then(response => response.json())
        .then(data => {
            const releaseDetails = data.map(release => {
                const assets = release.assets;
                let fileUrl = null;
                const filesJsonAsset = assets.find(asset => asset.name === 'files.json');
                if (filesJsonAsset) {
                    fileUrl = filesJsonAsset.browser_download_url;
                    return {
                        html_url: release.html_url,
                        tag_name: release.tag_name,
                        name: release.name,
                        date: formatDateTime(release.published_at),
                        author: release.author.login,
                        file_url: fileUrl
                    }
                };
            });

            const easyupdate = $('#easyupdate');

            if (releaseDetails.length === 0) {
                easyupdate.innerHTML = ("No releases found.");
            } else {
                const release = releaseDetails[0];
                if (release.tag_name == currentVer) {
                    easyupdate.innerHTML = `Version ${currentVer}. You are up to date`;
                } else if (release.date < formatEpoch(currentBuildtime)) {
                    easyupdate.innerHTML = `Your version is newer than the latest release date. Are you the developer? :-)`;
                } else {
                    easyupdate.innerHTML = `Update from version ${currentVer} to version ${release.tag_name} available.<button onclick="otamodule.updateall('${release.file_url}','${release.tag_name}')">Update now!</button>`;
                }

                tableRow.innerHTML = tablerow;
                table.appendChild(tableRow);

                return release;
            }

            const table = document.createElement('table');
            const tableHeader = document.createElement('tr');
            tableHeader.innerHTML = '<th>Release</th><th>Date</th><th>Name</th><th>Author</th><th colspan="2">Update:</th><th>Remark</th>';
            table.appendChild(tableHeader);

            releaseDetails.forEach(release => {
                const tableRow = document.createElement('tr');
                let tablerow = `<td><a href="${release.html_url}" target="_new">${release.tag_name}</a></td><td>${release.date}</td><td>${release.name}</td><td>${release.author}</td><td><button onclick="otamodule.updateESP('${release.file_url}')">ESP32</button></td><td><button onclick="otamodule.updateWebpage('${release.file_url}','${release.tag_name}', true)">Filesystem</button></td>`;
                if (release.tag_name == currentVer) {
                    tablerow += "<td>current version</td>";
                } else if (release.date < formatEpoch(currentBuildtime)) {
                    tablerow += "<td>older</td>";
                } else {
                    tablerow += "<td>newer</td>";
                }
                tableRow.innerHTML = tablerow;
                table.appendChild(tableRow);
            });

            $('#releasetable').innerHTML = "";
            $('#releasetable').appendChild(table);
            disableButtons(buttonState);
        })
        .catch(error => {
            print('Error fetching releases:' + error, "red");
        });
}

export function updateAll(fileUrl, tagname) {
    otamodule.updateWebpage(fileUrl, tagname, false);
    otamodule.updateESP(fileUrl)
}

export function updateWebpage(fileUrl, tagname, showReload) {
    if (running) return;
    if (!confirm("Confirm updating the littleFS storage")) return;

    disableButtons(true);
    running = true;
    errors = 0;
    const consoleDiv = document.getElementById('updateconsole');
    consoleDiv.scrollTop = consoleDiv.scrollHeight;

    print("Updating littleFS partition...");

    fetch("/getexturl?url=" + fileUrl)
        .then(response => response.json())
        .then(data => {
            checkfiles(data.files);
        })
        .catch(error => {
            print('Error fetching data:' + error, "red");
        });

    const checkfiles = async (files) => {
        for (const file of files) {
            try {
                const url = "/check_file?path=" + encodeURIComponent(file.path);
                const response = await fetch(url);
                if (response.ok) {
                    const data = await response.json();
                    if (data.filesize == file.size && data.md5 == file.md5) {
                        print(`file ${file.path} is up to date`, "green");
                    } else if (data.filesize == 0) {
                        await fetchAndPost(file.url, file.name, file.path);
                    } else {
                        await fetchAndPost(file.url, file.name, file.path);
                    }
                } else {
                    print(`error checking file ${file.path}: ${response.status}`, "red");
                    errors++;
                }
            } catch (error) {
                console.error(`error checking file ${file.path}:` + error, "red");
                errors++;
            }
        }
        writeVersion(tagname, "version.txt", "/www/version.txt")
        running = false;
        if (errors) {
            print("------", "gray");
            print(`Finished updating with ${errors} errors.`, "red");
        } else {
            print("------", "gray");
            print("Update succesful.");
        }
        disableButtons(false);

        if (showReload) {
            const newLine = document.createElement('div');
            newLine.innerHTML = "<button onclick=\"window.reload()\">Reload this page</button>";
            consoleDiv.appendChild(newLine);
            consoleDiv.scrollTop = consoleDiv.scrollHeight;
        }
    }; 
}

export async function updateESP(fileUrl) {
    if (running) return;
    if (!confirm("Confirm updating the microcontroller")) return;

    disableButtons(true);
    running = true;
    errors = 0;
    const consoleDiv = document.getElementById('updateconsole');
    consoleDiv.scrollTop = consoleDiv.scrollHeight;

    print("Updating firmware...");

    let binurl, binmd5, binsize;

    try {
        const response = await fetch("/getexturl?url=" + fileUrl);
        const data = await response.json();
        const file = data.binaries.find((entry) => entry.name == env + '.bin');
        if (file) {
            binurl = file.url;
            binmd5 = file.md5;
            binsize = file.size;
            console.log(`URL for "${file.name}": ${binurl}`);

            try {
                const response = await fetch('/update_ota', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/x-www-form-urlencoded'
                    },
                    body: new URLSearchParams({
                        url: binurl,
                        md5: binmd5,
                        size: binsize
                    })
                });

                if (response.ok) {
                    const result = await response.text();
                    print('OTA update initiated.');
                } else {
                    print('Failed to initiate OTA update: ' + response.status, "red");
                }
            } catch (error) {
                print('Error during OTA update: ' + error, "red");
            }
        } else {
            print(`File "${fileName}" not found.`, "red");
        }
    } catch (error) {
        print('Error: ' + error, "red");
    }

    running = false;
    disableButtons(false);
}

$('#rollbackBtn').onclick = function () {
    if (running) return;

    disableButtons(true);
    running = true;
    errors = 0;
    const consoleDiv = document.getElementById('updateconsole');
    consoleDiv.scrollTop = consoleDiv.scrollHeight;
    
    print("Rolling back...");
    
    fetch("/rollback", {
        method: "POST",
        body: formData
    })

    running = false;
    disableButtons(false);
    
}

export function print(line, color = "white") {
    const consoleDiv = document.getElementById('updateconsole');
    if (consoleDiv) {
        const isScrolledToBottom = consoleDiv.scrollHeight - consoleDiv.clientHeight <= consoleDiv.scrollTop;
        const newLine = document.createElement('div');
        newLine.style.color = color;
        if (line == "[reboot]") {
            newLine.innerHTML = "<button onclick=\"fetch(\"/reboot\",{method: \"POST\"}); \">Reboot</button>";
        } else {
            newLine.textContent = line;
        }
        consoleDiv.appendChild(newLine);
        if (isScrolledToBottom) {
            consoleDiv.scrollTop = consoleDiv.scrollHeight;
        }
    }
}

function formatEpoch(epochTime) {
    const date = new Date(epochTime * 1000); // Convert seconds to milliseconds

    const year = date.getFullYear();
    const month = String(date.getMonth() + 1).padStart(2, '0'); // Months are zero-based
    const day = String(date.getDate()).padStart(2, '0');
    const hours = String(date.getHours()).padStart(2, '0');
    const minutes = String(date.getMinutes()).padStart(2, '0');

    return `${year}-${month}-${day} ${hours}:${minutes}`;
}

function formatDateTime(utcDateString) {
    const localTimeZoneOffset = new Date().getTimezoneOffset();
    const date = new Date(utcDateString);
    date.setMinutes(date.getMinutes() - localTimeZoneOffset);
    const year = date.getUTCFullYear();
    const month = String(date.getUTCMonth() + 1).padStart(2, '0');
    const day = String(date.getUTCDate()).padStart(2, '0');
    const hours = String(date.getUTCHours()).padStart(2, '0');
    const minutes = String(date.getUTCMinutes()).padStart(2, '0');

    const formattedDate = `${year}-${month}-${day} ${hours}:${minutes}`;
    return formattedDate;
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
        }
    } catch (error) {
        print('error: ' + error, "red");
        errors++;
    }
};

const writeVersion = async (content, name, path) => {
    try {
        print("uploading " + path);

        const formData = new FormData();
        formData.append('path', path);
        const blob = new Blob([content]);
        formData.append('file', blob, name);

        const uploadResponse = await fetch('/littlefs_put', {
            method: 'POST',
            body: formData
        });

        if (!uploadResponse.ok) {
            print(`${response.status} ${response.body}`, "red");
            errors++;
        }
    } catch (error) {
        print('error: ' + error, "red");
        errors++;
    }
};

function disableButtons(active) {
    $("#apupdatebox").querySelectorAll('button').forEach(button => {
        button.disabled = active;
    });
    buttonState = active;
}
