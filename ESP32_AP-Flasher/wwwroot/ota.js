var repo = apConfig.repo || 'OpenEPaperLink/OpenEPaperLink';
var repoUrl = 'https://api.github.com/repos/' + repo + '/releases';

const $ = document.querySelector.bind(document);

let running = false;
let errors = 0;
let env = '', currentVer = '', currentBuildtime = 0;
let buttonState = false;
let gIsC6 = false;
let gIsH2 = false;
let gIsTLSR = false;
let gBinDir = '';
let gModuleType = '';
let gShortName = '';
let gCurrentRfVer = 0;
// What this board can actually flash. A radio co-processor is either an H2 or
// a C6, but an SWS wired TLSR is independent of that - a board with both can
// flash either, so this is a list rather than a single choice. The TLSR has two
// firmwares that are not interchangeable and each gets its own entry.
let gTargets = [];

// GitHub release assets carry no CORS headers, so the browser cannot read the
// firmware json from the release. The release workflow copies it from the repo
// at the tag, and raw.githubusercontent.com does allow cross-origin reads.
const radioFwTimeout = 5000;
let gRadioFwVersionFailed = false;

// The file lists of a release are only release assets: the AP fetches those for the page.
// Firmware from before github_proxy answers 404. It is still updated through the external
// proxy, and also gets its firmware from there: it downloads over http only.
let gLegacyProxy = false;

async function fetchReleaseFile(url) {
    if (!gLegacyProxy) {
        const response = await fetch('github_proxy?url=' + encodeURIComponent(url));
        if (response.status != 404) return response;
        gLegacyProxy = true;
        print("This firmware cannot fetch release files itself yet, using openepaperlink.eu", "yellow");
    }
    return fetch("//openepaperlink.eu/getupdate/?url=" + url);
}

async function fetchRadioFwVersion(tag, jsonName) {
    const url = `https://raw.githubusercontent.com/${repo}/${tag}/binaries/${gBinDir}/${jsonName}`;
    try {
        const response = await fetch(url, { signal: AbortSignal.timeout(radioFwTimeout) });
        if (!response.ok) throw new Error('HTTP ' + response.status);
        const files = await response.json();
        return files[2]['version'];
    } catch (error) {
        console.log(`Radio firmware version from ${url} failed: ${error}`);
        gRadioFwVersionFailed = true;
        return null;
    }
}

export async function initUpdate() {
    gTargets = [];
    gRadioFwVersionFailed = false;
    initFlashPins();
    if (apConfig?.H2 && apConfig.H2 == 1) {
        gIsH2 = true;
        gTargets.push({ name: "ESP32-H2", short: "H2", dir: "ESP32-H2", ep: "update_c6" });
    } else if (apConfig.C6 == 1) {
        gIsC6 = true;
        gTargets.push({ name: "ESP32-C6", short: "C6", dir: "ESP32-C6", ep: "update_c6" });
    }
    if (apConfig?.TLSR && apConfig.TLSR == 1) {
        gIsTLSR = true;
        // Zigbee first: it is what most tags in the field run, so it is the
        // one offered by default. The GFSK build is the special case.
        gTargets.push({ name: "TLSR Zigbee 802.15.4", short: "TLSR", dir: "TLSR", ep: "flash_tlsr", aptype: "F8" });
        gTargets.push({ name: "TLSR GFSK", short: "TLSR", dir: "TLSR", ep: "flash_tlsr", aptype: "F9" });
    }

    // The first entry stays the "primary" one so the release table below keeps
    // working unchanged; the per-target buttons are rendered separately.
    const primary = gTargets.length ? gTargets[0] : null;
    gModuleType = primary ? primary.name : "Unknown";
    gShortName = primary ? primary.short : "";
    gBinDir = primary ? primary.dir : "";
    $('#radio_release_title').innerHTML =
        (gTargets.length ? gTargets.map(t => t.name).join(" &middot; ") : gModuleType) + " Firmware";

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

    const sdata = await fetch("sysinfo")
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

    if (sdata.env) {
        print(`current env:        ${sdata.env}`);
        print(`build date:         ${formatEpoch(sdata.buildtime)}`);
        print(`esp32 version:      ${sdata.buildversion}`);
        print(`filesystem version: ${filesystemversion}`);
        print(`psram size:         ${sdata.psramsize}`);
        print(`flash size:         ${sdata.flashsize}`);
        if (gModuleType !== '') {
            let hex_ver = sdata.ap_version && !isNaN(sdata.ap_version)
                ? ('0000' + sdata.ap_version.toString(16)).slice(-4)
                : 'unknown';
            // Prefer what the module reported over the label the build guessed:
            // a board that can carry either radio has the latter fixed to one.
            const label = sdata.ap_type ? sdata.ap_typename : gModuleType;
            const type = sdata.ap_type
                ? ` (type ${('00' + sdata.ap_type.toString(16).toUpperCase()).slice(-2)})` : '';
            print(`${label} version:   ${hex_ver}${type}`);
        }
        print("--------------------------", "gray");
        env = apConfig.env || sdata.env;
        if (sdata.env != env) {
            print(`Warning: you selected a build environment ${env} which is\ndifferent than the currently used ${sdata.env}.\nOnly update the firmware with a mismatched build environment if\nyou know what you're doing.`, "yellow");
        }
        currentVer = sdata.buildversion;
        currentBuildtime = sdata.buildtime;
        gCurrentRfVer = sdata.ap_version;
        if (sdata.rollback) $("#rollbackOption").style.display = 'block';
        // Without an SWS flasher this button has nothing to talk to: the
        // firmware behind /flash_tlsr is not even compiled in on such a
        // board, so pressing it could only ever fail.
        if (sdata.has_sws_flasher) $("#flashTlsrOption").style.display = 'block';
        if (sdata.hasC6 || sdata.hasH2) $("#radioTestOption").style.display = 'block';
        $('#environment').value = env;
    }

    const rdata = await fetch(repoUrl).then(response => response.json())
    const JsonName = 'firmware_' + gShortName + '.json';
    const releaseDetails = rdata.map(release => {
        const assets = release.assets;
        const filesJsonAsset = assets.find(asset => asset.name === 'filesystem.json');
        const binariesJsonAsset = assets.find(asset => asset.name === 'binaries.json');
        const containsEnv = assets.find(asset => asset.name === env + '.bin');
        const firmwareAsset = assets.find(asset => asset.name === JsonName);
        if (filesJsonAsset && binariesJsonAsset && containsEnv) {
            return {
                html_url: release.html_url,
                tag_name: release.tag_name,
                name: release.name,
                date: formatDateTime(release.published_at),
                author: release.author.login,
                file_url: filesJsonAsset.browser_download_url,
                bin_url: binariesJsonAsset.browser_download_url,
                firmware_url: firmwareAsset?.browser_download_url,
            }
        };
    })

    if (releaseDetails.length === 0) {
        easyupdate.innerHTML = ("No releases found.");
    } else {
        const release = releaseDetails[0];
        if (release?.tag_name) {
            if (sameVersion(release.tag_name, currentVer)) {
                easyupdate.innerHTML = `Version ${currentVer}. You are up to date`;
            } else if (release.date < formatEpoch(currentBuildtime - 30 * 60)) {
                easyupdate.innerHTML = `Your version is newer than the latest release date.<br>Are you the developer? :-)`;
            } else {
                easyupdate.innerHTML = `An update from version ${currentVer} to version ${release.tag_name} is available.<button onclick="otamodule.updateAll('${release.bin_url}','${release.file_url}','${release.tag_name}')">Update now!</button>`;
            }
        }
    }

    const table = document.createElement('table');
    const tableHeader = document.createElement('tr');
    tableHeader.innerHTML = '<th>Release</th><th>Date</th><th>Name</th><th colspan="2"><center>Update</center></th><th>Remark</th>';
    table.appendChild(tableHeader);

    let rowCounter = 0;
    let radioFwCounter = 0;
    releaseDetails.forEach(release => {
        if (rowCounter < 4 && release?.html_url) {
            const tableRow = document.createElement('tr');
            let tablerow = `<td><a href="${release.html_url}" target="_new">${release.tag_name}</a></td><td>${release.date}</td><td>${release.name}</td><td><button type="button" onclick="otamodule.updateESP('${release.bin_url}', true)">ESP32</button></td><td><button type="button" onclick="otamodule.updateWebpage('${release.file_url}','${release.tag_name}', true)">Filesystem</button></td>`;
            if (sameVersion(release.tag_name, currentVer)) {
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
        if (release?.firmware_url) {
            radioFwCounter++;
        }
    });
    $('#releasetable').innerHTML = "";
    $('#releasetable').appendChild(table);

    if (radioFwCounter > 0) {
        const table1 = document.createElement('table');
        const tableHeader1 = document.createElement('tr');

        tableHeader1.innerHTML = '<th>Release</th><th>Date</th><th>Name</th><th><center>Update</center></th><th>Version</th><th>Remark</th>';
        table1.appendChild(tableHeader1);

        rowCounter = 0;
        for (const release of releaseDetails) {
            if (rowCounter < 4 && release?.firmware_url) {
                const tableRow = document.createElement('tr');
                var tablerow;
                var firmwareVer = "unknown";
                var release_url = release.firmware_url;

                tablerow = `<td><a href="${release.html_url}" target="_new">${release.tag_name}</a></td><td>${release.date}</td><td>${release.name}</td>`;
                tablerow += `<td><button type="button" onclick="otamodule.updateC6H2('${release_url}')">${gModuleType}</button></td>`;
                firmwareVer = await fetchRadioFwVersion(release.tag_name, JsonName) ?? 'unknown';
                tablerow += '<td>' + firmwareVer + '</td><td>';
                if (firmwareVer != 'unknown') {
                    let Ver = Number('0x' + firmwareVer);
                    if (Ver > gCurrentRfVer) {
                        tablerow += 'newer';
                    }
                    else if (Ver < gCurrentRfVer) {
                        tablerow += 'older';
                    }
                    else if (!Number.isNaN(Ver)) {
                        tablerow += 'current version';
                    }
                }
                tablerow += '</td>';
                tableRow.innerHTML = tablerow;
                table1.appendChild(tableRow);
                rowCounter++;
            }
        };

        $('#radio_releasetable').innerHTML = "";
        $('#radio_releasetable').appendChild(table1);
        if (gRadioFwVersionFailed) {
            print(`Could not read the ${gModuleType} firmware version of one or more releases\nfrom raw.githubusercontent.com. Updating still works, only the\nVersion column shows 'unknown'.`, "yellow");
        }
    }

    const table2 = document.createElement('table');
    {
        const tableHeader2 = document.createElement('tr');
        tableHeader2.innerHTML = '<th>Firmware</th><th><center>Update</center></th>';
        table2.appendChild(tableHeader2);
    }

    // One row per target the board can reach. A TLSR appears twice, because
    // its two firmwares are different radios and switching between them is the
    // point: GFSK on one side, 802.15.4/Zigbee on the other.
    for (const t of gTargets) {
        const arg = t.aptype ? `,'${t.aptype}'` : '';
        {
            const tableRow = document.createElement('tr');
            tableRow.innerHTML =
                `<td title="manual upload to the file system">${t.name} from ` +
                `<a href="/edit" target="littlefs">file system</a></td>` +
                `<td><button type="button" onclick="otamodule.updateC6H2(''${arg})">Flash</button></td>`;
            table2.appendChild(tableRow);
        }
        {
            const Url = "https://raw.githubusercontent.com/" + repo +
                "/master/binaries/" + t.dir + "/firmware_" + t.short + ".json";
            const tableRow = document.createElement('tr');
            tableRow.innerHTML =
                `<td>${t.name} from <a href="https://github.com/${repo}/tree/master/binaries/${t.dir}/" ` +
                `target="_new">repo</a></td>` +
                `<td><button type="button" onclick="otamodule.updateC6H2('${Url}'${arg})">Flash</button></td>`;
            table2.appendChild(tableRow);
        }
    }
    if (!gTargets.length) {
        const tableRow = document.createElement('tr');
        tableRow.innerHTML = '<td colspan="2">No flashable radio module on this board</td>';
        table2.appendChild(tableRow);
    }
    $('#radio_releasetable1').innerHTML = "";
    $('#radio_releasetable1').appendChild(table2);

    disableButtons(buttonState);
}

export async function updateAll(binUrl, fileUrl, tagname) {
    if (running) return;
    if (!confirm("Confirm updating the esp32 and filesystem")) return;

    // Each step waits for the previous one. The firmware is only flashed when
    // the file system is complete: new firmware with half of its files missing
    // is worse than staying on the old version. A retry is cheap, files that
    // are already up to date are skipped.
    if (!await updateWebpage(fileUrl, tagname, false)) {
        print("The firmware was not updated. Press 'Update now!' again to retry.", "red");
        return;
    }
    // update the tagtypes, but delete nothing: cleaning up is a choice on the tagtype button
    await fetchAndCheckTagtypes(false);
    await updateESP(binUrl, false);
}

// Returns true when every file is up to date. Never throws: whatever goes
// wrong is printed, and the buttons are always enabled again afterwards.
export async function updateWebpage(fileUrl, tagname, showReload) {
    if (running) return false;
    if (showReload && !confirm("Confirm updating the filesystem")) return false;

    disableButtons(true);
    running = true;
    errors = 0;
    const consoleDiv = document.getElementById('updateconsole');
    consoleDiv.scrollTop = consoleDiv.scrollHeight;

    try {
        print("Updating littleFS partition...");

        let files;
        try {
            const response = await fetchReleaseFile(fileUrl);
            if (!response.ok) throw new Error("HTTP " + response.status);
            files = await response.json();
            if (!Array.isArray(files)) throw new Error("unexpected response");
        } catch (error) {
            print("Error fetching the list of files: " + error.message, "red");
            errors++;
            return false;
        }

        const updateactions = files.find(file => file.name === "update_actions.json");
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
                print(`error calling update actions: ${error}`, "red");
                errors++;
            }
        }

        for (const file of files) {
            if (file.name == "update_actions.json") continue;
            try {
                const response = await fetch("check_file?path=" + encodeURIComponent(file.path));
                if (!response.ok) {
                    print(`error checking file ${file.path}: ${response.status}`, "red");
                    errors++;
                    continue;
                }
                const data = await response.json();
                if (data.filesize == file.size && data.md5 == file.md5) {
                    print(`file ${file.path} is up to date`, "green");
                } else {
                    await fetchAndPost(file.url, file.name, file.path);
                }
            } catch (error) {
                print(`error checking file ${file.path}: ${error}`, "red");
                errors++;
            }
        }

        // version.txt claims the release, so only write it when the release is
        // actually complete.
        if (!errors) await writeVersion(tagname, "version.txt", "/www/version.txt");

        print("------", "gray");
        if (errors) {
            print(`Finished updating with ${errors} errors.`, "red");
            return false;
        }
        print("Update succesful.");
        return true;
    } finally {
        running = false;
        disableButtons(false);
        if (showReload) {
            const newLine = document.createElement('div');
            newLine.innerHTML = "<button onclick=\"location.reload()\">Reload this page</button>";
            consoleDiv.appendChild(newLine);
            consoleDiv.scrollTop = consoleDiv.scrollHeight;
        }
    }
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
            const response = await fetchReleaseFile(fileUrl);
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
                binurl = gLegacyProxy ? "http://openepaperlink.eu/getupdate/?url=" + encodeURIComponent(file.url) : file.url;
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
                // Not a network error, retrying won't help. Without this break
                // the loop printed this line forever and froze the page.
                print(`No info about "${env}" found in the release.`, "red");
                break;
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

function target_name(apType) {
    const t = gTargets.find(x => (x.aptype || '') === (apType || ''));
    return t ? t.name : gModuleType;
}

export async function updateC6H2(Url, apType) {
    if (running) return;
    disableButtons(true);
    running = true;
    errors = 0;
    const ReleaseUrl = Url.substring(0, Url.lastIndexOf('/'));
    const consoleDiv = document.getElementById('updateconsole');
    consoleDiv.scrollTop = consoleDiv.scrollHeight;
    const formData = new FormData();

    print("Flashing " + (target_name(apType)) + " ...");
    formData.append('url', ReleaseUrl);
    if (apType) formData.append('aptype', apType);

    // Which endpoint depends on the target, not on the board: an ESP
    // co-processor goes through the serial loader, a TLSR over SWS.
    const target = gTargets.find(t => (t.aptype || '') === (apType || '')) || gTargets[0];
    fetch(target && target.ep ? target.ep : "update_c6", {
        method: "POST",
        body: formData
    })

    running = false;
    disableButtons(false);
}

$('#radioTestBtn').onclick = async function () {
    if (running) return;
    if (!confirm("Test the wiring to the radio? The radio is offline for about 10 seconds.")) return;
    disableButtons(true);
    running = true;
    print("Radio wiring test...");
    try {
        const response = await fetch("radio_selftest", { method: "POST" });
        if (!response.ok) {
            print("Radio wiring test not started: " + await response.text(), "red");
        } else {
            // progress and findings arrive on the console through the websocket; wait for the end
            let result = null;
            for (let i = 0; i < 30; i++) {
                await new Promise(r => setTimeout(r, 1000));
                const r = await fetch("radio_selftest");
                result = await r.json();
                if (!result.running) break;
            }
            if (result && !result.running) {
                print(result.ok ? "Radio wiring OK" : "Radio wiring: " + result.messages.join("; "), result.ok ? "green" : "red");
            }
        }
    } catch (error) {
        print("Radio wiring test: " + error, "red");
    }
    running = false;
    disableButtons(false);
}

$('#updateTagtypeBtn').onclick = function () {
    const cleanup = $('#tagtype_clean').checked;
    fetchAndCheckTagtypes(cleanup);
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
                    return fetchReleaseFile(binariesJsonAsset.browser_download_url);
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
            const updateData = JSON.parse(responseBody).filter(item => !item.name.endsWith('_full.bin') && !item.name.includes('_H2.') && !item.name.includes('_C6.'));

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

        if (!response.ok) {
            print(`download error ${path}: ${response.status}`, "red");
            errors++;
        } else {
            const fileContent = await response.blob();

            const formData = new FormData();
            formData.append('path', path);
            formData.append('file', fileContent, name);

            const uploadResponse = await fetch('littlefs_put', {
                method: 'POST',
                body: formData
            });

            if (!uploadResponse.ok) {
                print(`upload error ${path}: ${uploadResponse.status}`, "red");
                errors++;
            }
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
            print(`upload error ${path}: ${uploadResponse.status}`, "red");
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

async function fetchAndCheckTagtypes(cleanup) {
    print("Updating tagtype definitions...");
    const sortableGrid = $('#taglist');
    const gridItems = Array.from(sortableGrid.querySelectorAll('.tagcard:not(#tagtemplate)'));
    try {
        const response = await fetch('/edit?list=%2Ftagtypes&nosize=1');
        if (!response.ok) {
            print("Failed to fetch tagtypes list", "red");
            throw new Error('Failed to fetch tagtypes list');
        }
        const fileList = await response.json();

        for (const file of fileList) {
            const filename = file.name;
            print(filename, "green");
            let check = filename.endsWith('.json');
            let hwtype = parseInt(filename, 16);

            if (check && cleanup) {
                let isInUse = Array.from(gridItems).some(element => element.dataset.hwtype == hwtype);
                if (!isInUse) {
                    isInUse = Array.from(gridItems).some(element => element.dataset.usetemplate == hwtype);
                }
                if (!isInUse) {
                    print("not in use, deleting", "yellow");
                    const formData = new FormData();
                    formData.append('path', '/tagtypes/' + filename);
                    await fetch('/edit', {
                        method: 'DELETE',
                        body: formData
                    });
                    check = false;
                }
            }

            // Per file: a type that is not on GitHub (a custom one) or a broken
            // local copy must not stop the check for the types after it.
            if (check) {
                try {
                    let githubUrl = "https://raw.githubusercontent.com/" + repo + "/master/resources/tagtypes/" + filename;

                    const localResponse = await fetch(`/tagtypes/${filename}`);
                    const localJson = localResponse.ok ? await localResponse.json() : {};
                    const localVersion = localJson.version || 0;

                    const githubResponse = await fetch(githubUrl);
                    if (!githubResponse.ok) {
                        print("not in the repo, kept as it is", "gray");
                        continue;
                    }
                    const githubJson = await githubResponse.json();
                    const githubVersion = githubJson.version || 0;

                    if (githubVersion > localVersion) {
                        print("update from version " + localVersion + " to " + githubVersion);
                        await fetchAndPost(githubUrl, filename, "/tagtypes/" + filename);
                    }
                } catch (error) {
                    print(`Error checking ${filename}: ${error}`, "red");
                }
            }
        }
        print("Finished.");
    } catch (error) {
        print("Error: " + error, "red");
    }
}

function normalizeVersion(version) {
    return String(version).replace(/(\.\d*?)0+$/, '$1').replace(/\.$/, '');
}

// "2.9" and "2.90" are the same release: the 2.90 release was tagged twice,
// and the firmware built from either tag must not keep offering an update.
function sameVersion(a, b) {
    return normalizeVersion(a) == normalizeVersion(b);
}

// --- Co-processor flashing pins -------------------------------------------
// These apply only while flashing a C6/H2 or a TLSR. Normal operation always
// keeps the pinout the firmware was built with, so a wrong choice here costs a
// flash attempt and never the running access point.
const FPIN_FIELDS = ['sws', 'reset', 'txd', 'rxd', 'prog', 'dbgtxd', 'dbgrxd'];
let gFpinDefaults = null;

// ESP32-S3 GPIOs that may be offered. 22-25 do not exist, 26-37 carry the SPI
// flash and the octal PSRAM, 19/20 are USB - offering any of those is a way to
// produce a board that no longer boots or can no longer be reached. Strapping
// pins are offered but marked; some boards legitimately use them.
const FPIN_STRAPPING = [0, 3, 45, 46];

function fpinChoices(current) {
    const out = [-1];
    for (let g = 0; g <= 21; g++) if (g !== 19 && g !== 20) out.push(g);
    for (let g = 38; g <= 48; g++) out.push(g);
    // Never make an existing value unrepresentable: a board may be wired to a
    // pin this list does not offer, and dropping it would rewrite the
    // configuration behind the user's back on the next save.
    if (current !== null && current !== undefined && !out.includes(current)) {
        out.push(current);
        out.sort((a, b) => a - b);
    }
    return out;
}

function fpinFill(sel, current) {
    sel.innerHTML = '';
    for (const g of fpinChoices(current)) {
        const o = document.createElement('option');
        o.value = g;
        o.textContent = g < 0 ? 'not present'
            : 'GPIO ' + g + (FPIN_STRAPPING.includes(g) ? ' (strapping)' : '');
        sel.appendChild(o);
    }
    if (current !== null && current !== undefined) sel.value = current;
}

async function initFlashPins() {
    const box = document.getElementById('flashpins');
    if (!box) return;
    let sys = {};
    try {
        sys = await (await fetch('sysinfo')).json();
    } catch (e) {
        return;
    }
    gFpinDefaults = sys.flashpin_defaults || null;
    let any = false;
    for (const f of FPIN_FIELDS) {
        const sel = document.getElementById('fpin_' + f);
        if (!sel) continue;

        // A build default of -1 means this board has no such pin, and the code
        // that would use it is not even compiled in - the SWS pin on a board
        // without TLSR support, for instance. Offering the row anyway would
        // invite setting something that can never take effect, so hide it.
        const def = gFpinDefaults ? parseInt(gFpinDefaults[f], 10) : -1;
        const row = sel.closest('p');
        // The SWS row goes by whether the firmware has an SWS flasher at all,
        // not by whether a pin happens to be configured. Without the flasher
        // there is nothing the value could ever reach.
        const unavailable = (f === 'sws') ? !sys.has_sws_flasher : (def < 0);
        if (unavailable) {
            if (row) row.style.display = 'none';
            continue;
        }
        if (row) row.style.display = '';

        let v = apConfig['flashpin_' + f];
        if (v === undefined) v = def;
        v = parseInt(v, 10);
        fpinFill(sel, v);
        any = true;
    }
    // A board without a flashable co-processor has nothing to configure.
    box.style.display = any ? '' : 'none';

    const saveBtn = document.getElementById('fpinSave');
    if (saveBtn) saveBtn.onclick = function () {
        const fd = new FormData();
        for (const f of FPIN_FIELDS) {
            const sel = document.getElementById('fpin_' + f);
            if (sel && sel.value !== '') fd.append('flashpin_' + f, sel.value);
        }
        fetch('save_apcfg', { method: 'POST', body: fd })
            .then(r => r.text())
            .then(() => print('Flashing pins saved.'))
            .catch(e => print('Error saving pins: ' + e, 'red'));
    };
    const resetBtn = document.getElementById('fpinReset');
    if (resetBtn) resetBtn.onclick = function () {
        if (!gFpinDefaults) return;
        for (const f of FPIN_FIELDS) {
            const sel = document.getElementById('fpin_' + f);
            if (sel && gFpinDefaults[f] !== undefined) fpinFill(sel, parseInt(gFpinDefaults[f], 10));
        }
        print('Back to build defaults - press "Save pins" to keep it.');
    };
}

