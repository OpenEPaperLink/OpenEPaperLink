var repo = apConfig.repo || 'jjwbruijn/OpenEPaperLink';
var repoUrl = 'https://api.github.com/repos/' + repo + '/releases';

const $ = document.querySelector.bind(document);

let running = false;
let errors = 0;
let env = '', currentVer = '', currentBuildtime = 0;
let buttonState = false;

export async function initUpdate() {

    const response = await fetch("version.txt");
    let filesystemversion = await response.text();
    if (!filesystemversion) filesystemversion = "unknown";
    $('#repo').value = repo;

    const envBox = $('#environment');
    if (envBox?.tagName === 'SELECT') {
        const inputElement = document.createElement('input');
        inputElement.type = 'text';
        inputElement.id = 'environment';
        envBox.parentNode.replaceChild(inputElement, envBox);
    }
    $('#environment').value = '';
    $('#environment').setAttribute('readonly', true);
    $('#repo').removeAttribute('readonly');
    $('#confirmSelectRepo').style.display = 'none';
    $('#cancelSelectRepo').style.display = 'none';
    $('#selectRepo').style.display = 'inline-block';
    $('#repoWarning').style.display = 'none';

    const sysinfoPromise = fetch("sysinfo")
        .then(response => {
            if (response.status != 200) {
                print("Error fetching sysinfo: " + response.status, "red");
                if (response.status == 404) {
                    print("Your current firmware version is not yet capable of updating OTA.");
                    print("Update it manually one last time.");
                    disableButtons(true);
                }
                return {};
            } else {
                return response.json();
            }
        })
        .catch(error => {
            print('Error fetching sysinfo: ' + error, "red");
        });

    const repoPromise = fetch(repoUrl)
        .then(response => response.json())


    Promise.all([sysinfoPromise, repoPromise])
        .then(([sdata, rdata]) => {

            if (sdata.env) {
                let matchtest = '';
                if (sdata.buildversion != filesystemversion && filesystemversion != "custom" && sdata.buildversion != "custom") matchtest = " <- not matching!"
                print(`env:                ${sdata.env}`);
                print(`build date:         ${formatEpoch(sdata.buildtime)}`);
                print(`esp32 version:      ${sdata.buildversion}`);
                print(`filesystem version: ${filesystemversion}` + matchtest);
                print(`psram size:         ${sdata.psramsize}`);
                print(`flash size:         ${sdata.flashsize}`);
                print("--------------------------", "gray");
                env = sdata.env;
                currentVer = sdata.buildversion;
                currentBuildtime = sdata.buildtime;
                if (sdata.rollback) $("#rollbackOption").style.display = 'block';
                if (sdata.env == 'ESP32_S3_16_8_YELLOW_AP') $("#c6Option").style.display = 'block';
                $('#environment').value = env;
            }

            const releaseDetails = rdata.map(release => {
                const assets = release.assets;
                const filesJsonAsset = assets.find(asset => asset.name === 'filesystem.json');
                const binariesJsonAsset = assets.find(asset => asset.name === 'binaries.json');
                if (filesJsonAsset && binariesJsonAsset) {
                    return {
                        html_url: release.html_url,
                        tag_name: release.tag_name,
                        name: release.name,
                        date: formatDateTime(release.published_at),
                        author: release.author.login,
                        file_url: filesJsonAsset.browser_download_url,
                        bin_url: binariesJsonAsset.browser_download_url
                    }
                };
            });
            
            const easyupdate = $('#easyupdate');
            if (releaseDetails.length === 0) {
                easyupdate.innerHTML = ("No releases found.");
            } else {
                const release = releaseDetails[0];
                if (release?.tag_name) {
                    if (release.tag_name == currentVer) {
                        easyupdate.innerHTML = `Version ${currentVer}. You are up to date`;
                    } else if (release.date < formatEpoch(currentBuildtime)) {
                        easyupdate.innerHTML = `Your version is newer than the latest release date.<br>Are you the developer? :-)`;
                    } else {
                        easyupdate.innerHTML = `An update from version ${currentVer} to version ${release.tag_name} is available.<button onclick="otamodule.updateAll('${release.bin_url}','${release.file_url}','${release.tag_name}')">Update now!</button>`;
                    }
                }
            }

            const table = document.createElement('table');
            const tableHeader = document.createElement('tr');
            tableHeader.innerHTML = '<th>Release</th><th>Date</th><th>Name</th><th colspan="2">Update:</th><th>Remark</th>';
            table.appendChild(tableHeader);

            let rowCounter = 0;
            releaseDetails.forEach(release => {
                if (rowCounter < 4 && release?.html_url) {
                    const tableRow = document.createElement('tr');
                    let tablerow = `<td><a href="${release.html_url}" target="_new">${release.tag_name}</a></td><td>${release.date}</td><td>${release.name}</td><td><button type="button" onclick="otamodule.updateWebpage('${release.file_url}','${release.tag_name}', true)">Filesystem</button></td><td><button type="button" onclick="otamodule.updateESP('${release.bin_url}', true)">ESP32</button></td>`;
                    if (release.tag_name == currentVer) {
                        tablerow += "<td>current version</td>";
                    } else if (release.date < formatEpoch(currentBuildtime)) {
                        tablerow += "<td>older</td>";
                    } else {
                        tablerow += "<td>newer</td>";
                    }
                    tableRow.innerHTML = tablerow;
                    table.appendChild(tableRow);
                    rowCounter++;
                }
            });

            $('#releasetable').innerHTML = "";
            $('#releasetable').appendChild(table);
            disableButtons(buttonState);
        })
        .catch(error => {
            print('Error fetching releases:' + error, "red");
        });
}

export function updateAll(binUrl, fileUrl, tagname) {
    updateWebpage(fileUrl, tagname, false)
        .then(() => {
            updateESP(binUrl, false);
        })
        .catch(error => {
            console.error(error);
        });
}

export async function updateWebpage(fileUrl, tagname, showReload) {
    return new Promise((resolve, reject) => {
        (async function () {
            try {
                if (running) return;
                if (showReload) {
                    if (!confirm("Confirm updating the filesystem")) return;
                } else {
                    if (!confirm("Confirm updating the esp32 and filesystem")) return;
                }

                disableButtons(true);
                running = true;
                errors = 0;
                const consoleDiv = document.getElementById('updateconsole');
                consoleDiv.scrollTop = consoleDiv.scrollHeight;

                print("Updating littleFS partition...");

                fetch("//openepaperlink.eu/getupdate/?url=" + fileUrl)
                    .then(response => response.json())
                    .then(data => {
                        checkfiles(data);
                    })
                    .catch(error => {
                        print('Error fetching data:' + error, "red");
                    });

                const checkfiles = async (files) => {
                    const updateactions = files.find(files => files.name === "update_actions.json");
                    if (updateactions) {
                        await fetchAndPost(updateactions.url, updateactions.name, updateactions.path);
                        try {
                            const response = await fetch("update_actions", {
                                method: "POST",
                                body: ''
                            });
                            if (response.ok) {
                                await response.text();
                            } else {
                                print(`error performing update actions: ${response.status}`, "red");
                                errors++;
                            }
                        } catch (error) {
                            console.error(`error calling update actions:` + error, "red");
                            errors++;
                        }
                    }

                    for (const file of files) {
                        try {
                            if (file.name != "update_actions.json") {
                                const url = "check_file?path=" + encodeURIComponent(file.path);
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
                        reject(error);
                    } else {
                        print("------", "gray");
                        print("Update succesful.");
                        resolve();
                    }
                    disableButtons(false);

                    if (showReload) {
                        const newLine = document.createElement('div');
                        newLine.innerHTML = "<button onclick=\"location.reload()\">Reload this page</button>";
                        consoleDiv.appendChild(newLine);
                        consoleDiv.scrollTop = consoleDiv.scrollHeight;
                    }
                };
            } catch (error) {
                print('Error: ' + error, "red");
                errors++;
                reject(error);
            }
        })();
    });
}

export async function updateESP(fileUrl, showConfirm) {
    if (running) return;
    if (showConfirm) {
        if (!confirm("Confirm updating the esp32")) return;
    }

    disableButtons(true);
    running = true;
    errors = 0;
    const consoleDiv = document.getElementById('updateconsole');
    consoleDiv.scrollTop = consoleDiv.scrollHeight;

    print("Updating firmware...");

    let binurl, binmd5, binsize;

    let retryCount = 0;
    const maxRetries = 5;

    while (retryCount < maxRetries) {
        try {
            const response = await fetch("//openepaperlink.eu/getupdate/?url=" + fileUrl + "&env=" + env);
            const responseBody = await response.text();
            if (!response.ok) {
                throw new Error("Network response was not OK: " + responseBody);
            }

            if (!responseBody.trim().startsWith("[")) {
                throw new Error("Failed to fetch the release info file");
            }

            const data = JSON.parse(responseBody);
            const file = data.find((entry) => entry.name == env + '.bin');
            if (file) {
                binurl = "http://openepaperlink.eu/getupdate/?url=" + encodeURIComponent(file.url);
                binmd5 = file.md5;
                binsize = file.size;
                console.log(`URL for "${file.name}": ${binurl}`);

                try {
                    const response = await fetch('update_ota', {
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
                        await response.text();
                        print('OTA update initiated.');
                    } else {
                        print('Failed to initiate OTA update: ' + response.status, "red");
                    }
                } catch (error) {
                    print('Error during OTA update: ' + error, "red");
                }
                break;
            } else {
                print(`No info about "${env}" found in the release.`, "red");
            }
        } catch (error) {
            print('Error: ' + error.message, "yellow");
            retryCount++;
            print(`Retrying... attempt ${retryCount}`);
            await new Promise((resolve) => setTimeout(resolve, 3000));
        }
    }

    if (retryCount === maxRetries) {
        print("Reached maximum retry count. Failed to execute the update.", "red");
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

    fetch("rollback", {
        method: "POST",
        body: ''
    })

    running = false;
    disableButtons(false);
}

$('#updateC6Btn').onclick = function () {
    if (running) return;
    disableButtons(true);
    running = true;
    errors = 0;
    const consoleDiv = document.getElementById('updateconsole');
    consoleDiv.scrollTop = consoleDiv.scrollHeight;

    print("Flashing ESP32-C6...");

    const isChecked = $('#c6download').checked;
    const formData = new FormData();
    formData.append('download', isChecked ? '1' : '0'); // Convert to '1' or '0'

    fetch("update_c6", {
        method: "POST",
        body: formData
    })

    running = false;
    disableButtons(false);
}

$('#selectRepo').onclick = function (event) {
    event.preventDefault();
    $('#updateconsole').innerHTML = '';

    let repoUrl = 'https://api.github.com/repos/' + $('#repo').value + '/releases';
    fetch(repoUrl)
        .then(response => response.json())
        .then(data => {
            if (Array.isArray(data) && data.length > 0) {
                const release = data[0];
                print("Repo found! Latest release: " + release.name + " created " + release.created_at);
                const assets = release.assets;
                const filesJsonAsset = assets.find(asset => asset.name === 'filesystem.json');
                const binariesJsonAsset = assets.find(asset => asset.name === 'binaries.json');
                if (filesJsonAsset && binariesJsonAsset) {
                    const updateUrl = "//openepaperlink.eu/getupdate/?url=" + binariesJsonAsset.browser_download_url + "&env=" + $('#repo').value;
                    return fetch(updateUrl);
                } else {
                    throw new Error("Json file binaries.json and/or filesystem.json not found in the release assets");
                }            
            };
        })
        .then(updateResponse => {
            if (!updateResponse.ok) {
                throw new Error("Network response was not OK");
            }
            return updateResponse.text();
        })
        .then(responseBody => {
            if (!responseBody.trim().startsWith("[")) {
                throw new Error("Failed to fetch the release info file");
            }
            const updateData = JSON.parse(responseBody).filter(item => !item.name.endsWith('_full.bin'));

            const inputParent = $('#environment').parentNode;
            const selectElement = document.createElement('select');
            selectElement.id = 'environment';
            updateData.forEach(item => {
                const option = document.createElement('option');
                option.value = item.name.replace('.bin', '');
                option.text = item.name.replace('.bin', '');
                selectElement.appendChild(option);
            });
            inputParent.replaceChild(selectElement, $('#environment'));
            $('#environment').value = env;
            $('#confirmSelectRepo').style.display = 'inline-block';
            $('#cancelSelectRepo').style.display = 'inline-block';
            $('#selectRepo').style.display = 'none';
            $('#repo').setAttribute('readonly', true);
            $('#repoWarning').style.display = 'block';
        })
        .catch(error => {
            print('Error fetching releases:' + error, "red");
        });    
}

$('#cancelSelectRepo').onclick = function (event) {
    event.preventDefault();
    $('#updateconsole').innerHTML = '';
    initUpdate();
}

$('#confirmSelectRepo').onclick = function (event) {
    event.preventDefault();

    repo = $('#repo').value;
    let formData = new FormData();
    formData.append("repo", repo);
    formData.append("env", $('#environment').value);
    fetch("save_apcfg", {
        method: "POST",
        body: formData
    })
        .then(response => response.text())
        .then(data => {
            window.dispatchEvent(loadConfig);
            print('OK, Saved');
        })
        .catch(error => print('Error: ' + error));
    $('#updateconsole').innerHTML = '';
    repoUrl = 'https://api.github.com/repos/' + repo + '/releases';
    initUpdate();
}

export function print(line, color = "white") {
    const consoleDiv = document.getElementById('updateconsole');
    if (consoleDiv) {
        const isScrolledToBottom = consoleDiv.scrollHeight - consoleDiv.clientHeight <= consoleDiv.scrollTop;
        const newLine = document.createElement('div');
        newLine.style.color = color;
        if (line == "[reboot]") {
            newLine.innerHTML = "<button onclick=\"otamodule.reboot()\">Reboot</button>";
        } else {
            newLine.textContent = line;
        }
        consoleDiv.appendChild(newLine);
        if (isScrolledToBottom) {
            consoleDiv.scrollTop = consoleDiv.scrollHeight;
        }
    }
}

export function reboot() {
    print("Rebooting now... Reloading webpage in 5 seconds...", "yellow");
    fetch("reboot", { method: "POST" });
    setTimeout(() => {
        location.reload();
    }, 5000);
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

        const uploadResponse = await fetch('littlefs_put', {
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

        const uploadResponse = await fetch('littlefs_put', {
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
    $("#configtab").querySelectorAll('button').forEach(button => {
        button.disabled = active;
    });
    buttonState = active;
}
