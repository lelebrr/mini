#ifndef WEB_ASSETS_H
#define WEB_ASSETS_H

// Minimal Web UI assets served as plain HTML.
// Gzipped support can be added later by replacing index_html_gz and length.

static const uint8_t index_html_gz[] = { 0x1f, 0x8b, 0x08, 0x00 };
static const size_t index_html_gz_len = sizeof(index_html_gz);

static const char* index_html_raw = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR" data-bs-theme="dark">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>PwnTamagotchi BR</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet">
    <style>
        :root { --neon-green: #00ff00; --neon-purple: #bc13fe; --bg-black: #050505; }
        body { background-color: var(--bg-black); color: var(--neon-green); font-family: 'Courier New', monospace; }
        .card { border: 1px solid var(--neon-purple); background: #111; }
        .nav-tabs .nav-link.active { background-color: var(--neon-purple); color: #fff; border-color: var(--neon-purple); }
        .nav-link { color: var(--neon-green); }
        .btn-neon { background: transparent; border: 1px solid var(--neon-green); color: var(--neon-green); }
        .btn-neon:hover { background: var(--neon-green); color: black; }
        #log-container { height: 200px; overflow-y: scroll; background: #000; border: 1px solid #333; font-size: 12px; }
    </style>
</head>
<body>
    <div class="container py-4">
        <h1 class="text-center mb-4" style="text-shadow: 0 0 10px var(--neon-purple);">
            PwnTamagotchi BR <span class="badge bg-danger">v2.0</span>
        </h1>

        <div class="row mb-4">
            <div class="col-md-3">
                <div class="card p-3">
                    <h5>Fome</h5>
                    <div class="progress">
                        <div id="bar-hunger" class="progress-bar bg-danger" style="width: 50%"></div>
                    </div>
                </div>
            </div>
            <div class="col-md-3">
                <div class="card p-3">
                    <h5>Felicidade</h5>
                    <div class="progress">
                        <div id="bar-happy" class="progress-bar bg-success" style="width: 80%"></div>
                    </div>
                </div>
            </div>
            <div class="col-md-3">
                <div class="card p-3">
                    <h5>Bateria</h5>
                    <h3 id="val-batt">---%</h3>
                </div>
            </div>
            <div class="col-md-3">
                <div class="card p-3">
                    <h5>Level</h5>
                    <h3 id="val-lvl">Lvl 1</h3>
                </div>
            </div>
        </div>

        <ul class="nav nav-tabs mb-3" role="tablist">
            <li class="nav-item"><button class="nav-link active" data-bs-target="#tab-config" data-bs-toggle="tab">Configurações</button></li>
            <li class="nav-item"><button class="nav-link" data-bs-target="#tab-files" data-bs-toggle="tab">Arquivos</button></li>
            <li class="nav-item"><button class="nav-link" data-bs-target="#tab-logs" data-bs-toggle="tab">Logs</button></li>
            <li class="nav-item"><button class="nav-link" data-bs-target="#tab-system" data-bs-toggle="tab">Sistema</button></li>
        </ul>

        <div class="tab-content">
            <div class="tab-pane fade show active" id="tab-config">
                <div class="card p-3">
                    <form id="config-form">
                        <div id="config-container" class="row g-3">Loading...</div>
                        <button type="button" class="btn btn-neon mt-3" onclick="saveConfig()">Salvar & Aplicar</button>
                    </form>
                </div>
            </div>

            <div class="tab-pane fade" id="tab-files">
                <div class="card p-3">
                    <ul class="list-group" id="file-list"></ul>
                </div>
            </div>

            <div class="tab-pane fade" id="tab-logs">
                <div id="log-container" class="p-2"></div>
            </div>
        </div>
    </div>

    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js"></script>
    <script>
        const ws = new WebSocket('ws://' + window.location.hostname + '/ws');
        ws.onmessage = (e) => {
            const logDiv = document.getElementById('log-container');
            logDiv.innerHTML += '<div>' + e.data + '</div>';
            logDiv.scrollTop = logDiv.scrollHeight;
        };

        async function loadConfig() {
            const res = await fetch('/api/config');
            const data = await res.json();
            const container = document.getElementById('config-container');
            container.innerHTML = '';
            for (const [key, val] of Object.entries(data)) {
                let input = '';
                if (typeof val === 'boolean') {
                    input = `<div class="form-check form-switch"><input class="form-check-input" type="checkbox" id="${key}" ${val ? 'checked' : ''}></div>`;
                } else {
                    input = `<input type="text" class="form-control bg-dark text-light" id="${key}" value="${val}">`;
                }
                container.innerHTML += `<div class="col-md-6"><label class="form-label">${key}</label>${input}</div>`;
            }
        }

        async function saveConfig() {
            const inputs = document.querySelectorAll('#config-container input');
            const data = {};
            inputs.forEach(el => {
                data[el.id] = el.type === 'checkbox' ? el.checked : el.value;
            });
            await fetch('/api/config', { method: 'POST', body: JSON.stringify(data) });
            alert('Salvo!');
        }

        async function loadFiles() {
            const res = await fetch('/api/files');
            const files = await res.json();
            const list = document.getElementById('file-list');
            list.innerHTML = files.map(f =>
                `<li class="list-group-item bg-dark text-light d-flex justify-content-between">
                    ${f.name}
                    <a href="/api/download?file=/${f.name}" class="btn btn-sm btn-outline-info">Download</a>
                 </li>`
            ).join('');
        }

        loadConfig();
        loadFiles();

        setInterval(async () => {
            const res = await fetch('/api/status');
            const s = await res.json();
            document.getElementById('val-batt').innerText = s.pwr_battery + '%';
            document.getElementById('val-lvl').innerText = 'Lvl ' + s.pet_level;
        }, 5000);
    </script>
</body>
</html>
)rawliteral";

#endif
