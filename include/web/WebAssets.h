#ifndef WEB_ASSETS_H
#define WEB_ASSETS_H

// WebUI do Mini Lele — 100% autocontida (sem CDN externo), funciona offline no
// modo AP. Servida por WebHandler em "/". Ver webui_preview.html para o fonte.

static const char index_html_raw[] = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Mini Lele</title>
<style>
:root{
  --bg:#070b12; --bg2:#0b1120; --panel:rgba(18,24,33,.72); --panel2:#121821;
  --border:rgba(122,138,160,.18); --green:#38ff9e; --purple:#b26bff; --cyan:#4bd6ff;
  --orange:#ffa53b; --red:#ff5a7a; --text:#e6f0ff; --dim:#8394ab;
  --shadow:0 10px 40px rgba(0,0,0,.45);
}
*{box-sizing:border-box;margin:0;padding:0}
html,body{height:100%}
body{
  font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,Ubuntu,sans-serif;
  color:var(--text);background:var(--bg);
  background-image:
    radial-gradient(900px 500px at 12% -10%,rgba(178,107,255,.18),transparent 60%),
    radial-gradient(800px 500px at 100% 0%,rgba(56,255,158,.12),transparent 55%),
    radial-gradient(700px 700px at 50% 120%,rgba(75,214,255,.10),transparent 60%);
  min-height:100%;
  -webkit-font-smoothing:antialiased;
}
.mono{font-family:"SF Mono",ui-monospace,"JetBrains Mono",Menlo,Consolas,monospace}
.wrap{max-width:1080px;margin:0 auto;padding:20px 16px 60px}

/* Header */
header{display:flex;align-items:center;gap:14px;padding:14px 4px 22px}
.logo{
  display:flex;align-items:center;gap:12px;font-weight:800;font-size:22px;letter-spacing:.3px;
}
.logo .dot{
  width:38px;height:38px;border-radius:12px;display:grid;place-items:center;font-size:20px;
  background:linear-gradient(135deg,var(--purple),var(--green));box-shadow:0 6px 22px rgba(178,107,255,.5)
}
.badge{font-size:11px;font-weight:700;padding:3px 8px;border-radius:999px;background:rgba(178,107,255,.18);
  color:var(--purple);border:1px solid rgba(178,107,255,.4)}
header .spacer{flex:1}
.conn{display:flex;align-items:center;gap:8px;font-size:13px;color:var(--dim)}
.conn .live{width:9px;height:9px;border-radius:50%;background:var(--red);box-shadow:0 0 10px var(--red)}
.conn.ok .live{background:var(--green);box-shadow:0 0 10px var(--green);animation:pulse 1.8s infinite}
@keyframes pulse{0%,100%{opacity:1}50%{opacity:.35}}

/* Tabs */
nav.tabs{display:flex;gap:6px;flex-wrap:wrap;margin-bottom:20px;position:sticky;top:0;z-index:5;
  padding:8px;background:rgba(7,11,18,.7);backdrop-filter:blur(12px);border:1px solid var(--border);
  border-radius:16px}
.tab{border:0;background:transparent;color:var(--dim);font-weight:600;font-size:14px;
  padding:10px 16px;border-radius:11px;cursor:pointer;transition:.18s;display:flex;align-items:center;gap:8px}
.tab:hover{color:var(--text);background:rgba(255,255,255,.04)}
.tab.active{color:#07100a;background:linear-gradient(135deg,var(--green),#7bffc4);box-shadow:0 6px 18px rgba(56,255,158,.35)}
.tab svg{width:16px;height:16px}

.page{display:none;animation:fade .25s ease}
.page.active{display:block}
@keyframes fade{from{opacity:0;transform:translateY(6px)}to{opacity:1;transform:none}}

/* Cards */
.grid{display:grid;gap:16px}
.g-hero{grid-template-columns:1.3fr 1fr}
.g-stats{grid-template-columns:repeat(auto-fit,minmax(150px,1fr))}
.g-cfg{grid-template-columns:repeat(auto-fit,minmax(300px,1fr))}
@media(max-width:760px){.g-hero{grid-template-columns:1fr}}
.card{background:var(--panel);border:1px solid var(--border);border-radius:18px;padding:20px;
  box-shadow:var(--shadow);backdrop-filter:blur(14px)}
.card h3{font-size:12px;text-transform:uppercase;letter-spacing:1.4px;color:var(--dim);margin-bottom:14px;font-weight:700}

/* Hero pet */
.hero{display:flex;flex-direction:column;align-items:center;justify-content:center;gap:6px;min-height:230px;
  position:relative;overflow:hidden}
.hero .ring{position:absolute;inset:0;background:radial-gradient(circle at 50% 40%,rgba(56,255,158,.14),transparent 55%)}
.face{font-size:64px;font-weight:800;color:var(--green);text-shadow:0 0 26px rgba(56,255,158,.55);
  font-family:ui-monospace,monospace;z-index:1}
.pet-name{font-size:26px;font-weight:800;z-index:1}
.pet-mood{color:var(--purple);font-weight:700;letter-spacing:2px;font-size:13px;z-index:1}
.stage-pill{margin-top:8px;font-size:12px;color:var(--dim);border:1px solid var(--border);
  padding:5px 12px;border-radius:999px;z-index:1}

/* Gauges */
.gauges{display:flex;justify-content:space-around;align-items:center;flex-wrap:wrap;gap:10px}
.gauge{display:flex;flex-direction:column;align-items:center;gap:6px}
.gauge svg{transform:rotate(-90deg)}
.gauge .lbl{font-size:12px;color:var(--dim);text-transform:uppercase;letter-spacing:1px}
.gauge .val{position:absolute;font-size:20px;font-weight:800}
.gauge .gwrap{position:relative;display:grid;place-items:center}

/* Battery */
.batt-big{display:flex;align-items:center;gap:16px}
.batt-shell{position:relative;width:96px;height:44px;border:3px solid var(--dim);border-radius:8px;padding:4px}
.batt-shell:after{content:"";position:absolute;right:-9px;top:12px;width:6px;height:16px;background:var(--dim);border-radius:0 3px 3px 0}
.batt-fill{height:100%;border-radius:3px;background:linear-gradient(90deg,var(--green),#7bffc4);transition:width .6s,background .4s}
.batt-info{flex:1}
.batt-pct{font-size:30px;font-weight:800;line-height:1}
.batt-sub{color:var(--dim);font-size:13px;margin-top:4px}
.charging .batt-fill{background:linear-gradient(90deg,var(--orange),#ffd089);animation:chg 1.6s ease-in-out infinite}
@keyframes chg{0%,100%{filter:brightness(1)}50%{filter:brightness(1.4)}}
.bolt{color:var(--orange);font-weight:800}

/* Stat tiles */
.tile{background:var(--panel2);border:1px solid var(--border);border-radius:16px;padding:16px}
.tile .k{font-size:12px;color:var(--dim);text-transform:uppercase;letter-spacing:1px}
.tile .v{font-size:26px;font-weight:800;margin-top:6px}
.tile .v small{font-size:14px;color:var(--dim);font-weight:600}

/* Config */
.cfg-sec h3{color:var(--green)}
.field{display:flex;align-items:center;justify-content:space-between;gap:12px;padding:10px 0;border-bottom:1px solid rgba(122,138,160,.10)}
.field:last-child{border-bottom:0}
.field label{font-size:14px;color:var(--text)}
.field .hint{font-size:11px;color:var(--dim);display:block;margin-top:2px}
input[type=text],input[type=number],input[type=password],select{
  background:#0a0f18;border:1px solid var(--border);color:var(--text);border-radius:10px;
  padding:8px 11px;font-size:14px;width:150px;text-align:right}
input:focus,select:focus{outline:none;border-color:var(--green);box-shadow:0 0 0 3px rgba(56,255,158,.15)}
/* toggle */
.sw{position:relative;width:46px;height:26px;flex:0 0 auto}
.sw input{opacity:0;width:0;height:0}
.sw .track{position:absolute;inset:0;background:#26303f;border-radius:999px;transition:.25s;cursor:pointer}
.sw .track:before{content:"";position:absolute;left:3px;top:3px;width:20px;height:20px;border-radius:50%;
  background:#fff;transition:.25s}
.sw input:checked + .track{background:linear-gradient(135deg,var(--green),#7bffc4)}
.sw input:checked + .track:before{transform:translateX(20px)}

.btn{border:0;border-radius:12px;padding:12px 22px;font-weight:700;font-size:14px;cursor:pointer;transition:.18s}
.btn-primary{background:linear-gradient(135deg,var(--green),#7bffc4);color:#07100a;box-shadow:0 8px 22px rgba(56,255,158,.32)}
.btn-primary:hover{transform:translateY(-2px)}
.btn-ghost{background:transparent;border:1px solid var(--border);color:var(--text)}
.btn-ghost:hover{border-color:var(--purple);color:var(--purple)}
.btn-danger{background:transparent;border:1px solid rgba(255,90,122,.5);color:var(--red)}
.btn-danger:hover{background:rgba(255,90,122,.12)}
.savebar{position:sticky;bottom:12px;display:flex;justify-content:flex-end;gap:10px;margin-top:18px;
  padding:12px;background:rgba(7,11,18,.72);backdrop-filter:blur(12px);border:1px solid var(--border);border-radius:16px}

/* Files */
.file{display:flex;align-items:center;gap:12px;padding:12px 14px;border-radius:12px;transition:.15s}
.file:hover{background:rgba(255,255,255,.04)}
.file .ico{width:34px;height:34px;border-radius:9px;display:grid;place-items:center;background:var(--panel2);font-size:16px}
.file .nm{flex:1;font-weight:600;word-break:break-all}
.file .sz{color:var(--dim);font-size:12px;font-family:monospace}

/* Logs */
#log{height:360px;overflow-y:auto;background:#05080e;border:1px solid var(--border);border-radius:14px;
  padding:14px;font-family:ui-monospace,monospace;font-size:12.5px;line-height:1.6}
#log div{border-bottom:1px solid rgba(255,255,255,.03);padding:2px 0;color:#a9ffd6}
.toast{position:fixed;bottom:24px;left:50%;transform:translateX(-50%) translateY(120px);
  background:var(--panel2);border:1px solid var(--green);color:var(--text);padding:14px 24px;border-radius:14px;
  box-shadow:var(--shadow);transition:.35s;z-index:50}
.toast.show{transform:translateX(-50%) translateY(0)}
.row{display:flex;gap:10px;align-items:center;flex-wrap:wrap}
.muted{color:var(--dim);font-size:13px}
a{color:var(--cyan);text-decoration:none}
</style>
</head>
<body>
<div class="wrap">
  <header>
    <div class="logo"><span class="dot">🐣</span> Mini&nbsp;Lele <span class="badge">v2.0</span></div>
    <div class="spacer"></div>
    <div class="conn" id="conn"><span class="live"></span><span id="connTxt">conectando…</span></div>
  </header>

  <nav class="tabs" id="tabs">
    <button class="tab active" data-p="painel">◈ Painel</button>
    <button class="tab" data-p="config">⚙ Configurações</button>
    <button class="tab" data-p="arquivos">▤ Arquivos</button>
    <button class="tab" data-p="sistema">⌘ Sistema</button>
    <button class="tab" data-p="logs">≋ Logs</button>
  </nav>

  <!-- PAINEL -->
  <section class="page active" id="painel">
    <div class="grid g-hero" style="margin-bottom:16px">
      <div class="card hero">
        <div class="ring"></div>
        <div class="face" id="face">(o_o)</div>
        <div class="pet-name" id="petName">PwnBaby</div>
        <div class="pet-mood" id="petMood">FELIZ</div>
        <div class="stage-pill" id="petStage">Estágio: EGG</div>
      </div>
      <div class="card">
        <h3>Bateria &amp; Carga</h3>
        <div class="batt-big" id="battBox">
          <div class="batt-shell"><div class="batt-fill" id="battFill" style="width:60%"></div></div>
          <div class="batt-info">
            <div class="batt-pct" id="battPct">--%</div>
            <div class="batt-sub" id="battStatus">—</div>
            <div class="batt-sub mono" id="battDetail">—</div>
          </div>
        </div>
        <div class="gauges" style="margin-top:18px">
          <div class="gauge"><div class="gwrap">
            <svg width="96" height="96"><circle cx="48" cy="48" r="40" stroke="#1a2230" stroke-width="9" fill="none"/>
              <circle id="gHunger" cx="48" cy="48" r="40" stroke="#ffa53b" stroke-width="9" fill="none"
                stroke-linecap="round" stroke-dasharray="251" stroke-dashoffset="90"/></svg>
            <div class="val" id="hungerVal">--</div></div><div class="lbl">Fome</div></div>
          <div class="gauge"><div class="gwrap">
            <svg width="96" height="96"><circle cx="48" cy="48" r="40" stroke="#1a2230" stroke-width="9" fill="none"/>
              <circle id="gHappy" cx="48" cy="48" r="40" stroke="#38ff9e" stroke-width="9" fill="none"
                stroke-linecap="round" stroke-dasharray="251" stroke-dashoffset="40"/></svg>
            <div class="val" id="happyVal">--</div></div><div class="lbl">Felicidade</div></div>
        </div>
      </div>
    </div>

    <div class="grid g-stats">
      <div class="tile"><div class="k">Nível</div><div class="v" id="tLevel">1</div></div>
      <div class="tile"><div class="k">XP</div><div class="v" id="tXP">0</div></div>
      <div class="tile"><div class="k">Handshakes</div><div class="v" id="tHS">0</div></div>
      <div class="tile"><div class="k">Dispositivos</div><div class="v" id="tDev">0</div></div>
      <div class="tile"><div class="k">RAM livre</div><div class="v" id="tHeap">--<small> KB</small></div></div>
      <div class="tile"><div class="k">Uptime</div><div class="v" id="tUp">--</div></div>
    </div>
  </section>

  <!-- CONFIG -->
  <section class="page" id="config">
    <div class="grid g-cfg" id="cfgGrid"><div class="card"><p class="muted">Carregando configurações…</p></div></div>
    <div class="savebar">
      <button class="btn btn-ghost" onclick="loadConfig()">Recarregar</button>
      <button class="btn btn-primary" onclick="saveConfig()">Salvar &amp; Aplicar</button>
    </div>
  </section>

  <!-- ARQUIVOS -->
  <section class="page" id="arquivos">
    <div class="card">
      <h3>Cartão SD <span class="muted" id="filePath">/</span></h3>
      <div id="fileList"><p class="muted">Carregando…</p></div>
    </div>
  </section>

  <!-- SISTEMA -->
  <section class="page" id="sistema">
    <div class="grid g-cfg">
      <div class="card">
        <h3>Dispositivo</h3>
        <div class="field"><label>MAC WiFi</label><span class="mono muted" id="sMac">—</span></div>
        <div class="field"><label>Tensão bateria</label><span class="mono muted" id="sVolt">—</span></div>
        <div class="field"><label>Autonomia estimada</label><span class="mono muted" id="sHours">—</span></div>
        <div class="field"><label>RAM livre</label><span class="mono muted" id="sHeap">—</span></div>
        <div class="field"><label>Uptime</label><span class="mono muted" id="sUp">—</span></div>
      </div>
      <div class="card">
        <h3>Atualização de Firmware (OTA)</h3>
        <p class="muted" style="margin-bottom:14px">Envie um arquivo <b>.bin</b> compilado. O dispositivo reinicia ao concluir.</p>
        <form id="otaForm" class="row">
          <input type="file" name="update" id="otaFile" accept=".bin" style="width:auto">
          <button class="btn btn-primary" type="submit">Enviar</button>
        </form>
        <div id="otaProg" class="muted mono" style="margin-top:12px"></div>
      </div>
      <div class="card">
        <h3>Ações</h3>
        <p class="muted" style="margin-bottom:14px">Reiniciar o Mini Lele agora.</p>
        <button class="btn btn-danger" onclick="reboot()">Reiniciar dispositivo</button>
      </div>
    </div>
  </section>

  <!-- LOGS -->
  <section class="page" id="logs">
    <div class="card">
      <h3>Logs ao vivo (WebSocket)</h3>
      <div class="row" style="margin-bottom:12px">
        <button class="btn btn-ghost" id="pauseBtn" onclick="togglePause()">⏸ Pausar</button>
        <button class="btn btn-ghost" onclick="document.getElementById('log').innerHTML=''">🗑 Limpar</button>
        <span class="muted" id="logCount">0 linhas</span>
      </div>
      <div id="log"></div>
    </div>
  </section>
</div>
<div class="toast" id="toast">Salvo!</div>

<script>
// ---------- Grupos e rótulos amigáveis da config ----------
const GROUPS = {
  pet_:  {icon:"🐣", name:"Pet / Gameplay"},
  disp_: {icon:"🖥", name:"Display"},
  pwr_:  {icon:"🔋", name:"Energia & Bateria"},
  atk_:  {icon:"📡", name:"Ataques & WiFi"},
  sys_:  {icon:"⚙", name:"Sistema"},
  web_:  {icon:"🌐", name:"WebUI"},
};
const LABELS = {
  pet_name:"Nome do Pet", pet_voice_enabled:"Voz ativada", pet_hunger_rate:"Taxa de fome",
  disp_brightness:"Brilho (0-255)", disp_timeout_sec:"Timeout da tela (s)", disp_theme:"Tema",
  pwr_battery_capacity_mah:"Capacidade da bateria (mAh)", pwr_charge_current_ma:"Corrente de carga (mA)",
  pwr_charge_target_mv:"Tensão de corte (mV)", pwr_low_warn_pct:"Aviso bateria baixa (%)",
  pwr_low_shutdown_pct:"Desligar em (%)", pwr_cpu_freq_max:"Clock máx (MHz)",
  atk_deauth_enabled:"Deauth ativado", atk_evil_portal:"Evil Portal", atk_auto_scan:"Scan automático",
  sys_ap_ssid:"SSID do AP", sys_ap_pass:"Senha do AP", sys_web_user:"Usuário WebUI", sys_web_pass:"Senha WebUI",
};
const label = k => LABELS[k] || k.replace(/^[a-z]+_/,'').replace(/_/g,' ').replace(/\b\w/g,c=>c.toUpperCase());

let cfgData = {};

// ---------- Tabs ----------
document.getElementById('tabs').addEventListener('click', e=>{
  const b = e.target.closest('.tab'); if(!b) return;
  document.querySelectorAll('.tab').forEach(t=>t.classList.remove('active'));
  document.querySelectorAll('.page').forEach(p=>p.classList.remove('active'));
  b.classList.add('active');
  document.getElementById(b.dataset.p).classList.add('active');
  if(b.dataset.p==='arquivos') loadFiles();
});

// ---------- Helpers de rede (com fallback p/ preview offline) ----------
async function api(url,opt){ const r = await fetch(url,opt); if(!r.ok) throw new Error(r.status); return r; }

const MOCK = {
  status:{pet_name:"PwnBaby",pet_hunger:72,pet_happiness:88,pet_level:7,pet_xp:240,pet_stage:2,
    pet_mood:"FELIZ",pet_handshakes:34,pwr_battery:63,pwr_current:82,pwr_voltage:3.81,
    pwr_charging:true,pwr_vbus:true,pwr_charge_status:"Carregando (CC)",pwr_hours:1.9,
    wifi_mac:"A0:B7:65:12:34:56",wifi_devices:12,heap_free:210000,uptime:5127},
  config:{pet_name:"PwnBaby",pet_voice_enabled:true,pet_hunger_rate:1,disp_brightness:200,
    disp_timeout_sec:30,disp_theme:"cyber_favela",pwr_battery_capacity_mah:250,pwr_charge_current_ma:100,
    pwr_charge_target_mv:4200,pwr_low_warn_pct:10,pwr_low_shutdown_pct:5,pwr_cpu_freq_max:240,
    atk_auto_scan:true,atk_deauth_enabled:false,atk_evil_portal:false,atk_channels:"1,6,11",
    sys_ap_ssid:"Mini-Lele",sys_ap_pass:"********",sys_web_user:"admin",sys_web_pass:"********",
    web_live_logs:true,web_theme:"cyber_favela"},
  files:[{name:"config.json",size:4210,isDir:false},{name:"capturas",size:0,isDir:true},
    {name:"macs_detectados.txt",size:88123,isDir:false},{name:"pwn_pet_save.json",size:512,isDir:false}]
};
const setRing=(id,pct)=>{const c=document.getElementById(id);const len=251;c.style.strokeDashoffset=len-(len*pct/100);};
const fmtUp=s=>{const h=(s/3600|0),m=((s%3600)/60|0);return h>0?h+"h "+m+"m":m+"m "+(s%60)+"s";};

// ---------- Painel ----------
async function loadStatus(){
  let s;
  try{ s = await (await api('/api/status')).json(); setConn(true);}
  catch(e){ s = MOCK.status; setConn(false); }
  const face = s.pet_charging||s.pwr_charging ? "(^u^)" : s.pet_hunger<20?"(T_T)":s.pet_happiness>80?"(^_^)":"(o_o)";
  document.getElementById('face').textContent = face;
  document.getElementById('petName').textContent = s.pet_name||"PwnBaby";
  document.getElementById('petMood').textContent = s.pet_mood||"—";
  const stages=["EGG","BABY","KID","TEEN","ADULT","PWNLORD","PWNGOD"];
  document.getElementById('petStage').textContent = "Estágio: "+(stages[s.pet_stage]||"—");

  setRing('gHunger',s.pet_hunger); document.getElementById('hungerVal').textContent=s.pet_hunger;
  setRing('gHappy',s.pet_happiness); document.getElementById('happyVal').textContent=s.pet_happiness;

  document.getElementById('battPct').textContent = s.pwr_battery+"%";
  document.getElementById('battFill').style.width = s.pwr_battery+"%";
  document.getElementById('battStatus').innerHTML = (s.pwr_charging?'<span class="bolt">⚡ </span>':'')+(s.pwr_charge_status||"");
  document.getElementById('battDetail').textContent = (s.pwr_voltage?.toFixed?s.pwr_voltage.toFixed(2):s.pwr_voltage)+" V • "+
     (s.pwr_vbus?"USB-C":"bateria")+(s.pwr_hours&&!s.pwr_vbus?(" • ~"+s.pwr_hours.toFixed(1)+"h"):"");
  document.getElementById('battBox').classList.toggle('charging', !!s.pwr_charging);

  document.getElementById('tLevel').textContent=s.pet_level;
  document.getElementById('tXP').textContent=s.pet_xp;
  document.getElementById('tHS').textContent=s.pet_handshakes;
  document.getElementById('tDev').textContent=s.wifi_devices;
  document.getElementById('tHeap').innerHTML=(s.heap_free/1024|0)+'<small> KB</small>';
  document.getElementById('tUp').textContent=fmtUp(s.uptime);
  // Sistema
  document.getElementById('sMac').textContent=s.wifi_mac;
  document.getElementById('sVolt').textContent=(s.pwr_voltage?.toFixed?s.pwr_voltage.toFixed(2):s.pwr_voltage)+" V";
  document.getElementById('sHours').textContent=s.pwr_hours?("~"+s.pwr_hours.toFixed(1)+" h"):"—";
  document.getElementById('sHeap').textContent=(s.heap_free/1024|0)+" KB";
  document.getElementById('sUp').textContent=fmtUp(s.uptime);
}
function setConn(ok){
  const c=document.getElementById('conn'); c.classList.toggle('ok',ok);
  document.getElementById('connTxt').textContent = ok?"online":"offline (preview)";
}

// ---------- Config ----------
async function loadConfig(){
  try{ cfgData = await (await api('/api/config')).json(); setConn(true);}
  catch(e){ cfgData = MOCK.config; setConn(false); }
  const grid=document.getElementById('cfgGrid'); grid.innerHTML='';
  const secs={};
  for(const k in cfgData){
    const pfx=Object.keys(GROUPS).find(p=>k.startsWith(p))||'sys_';
    (secs[pfx]=secs[pfx]||[]).push(k);
  }
  for(const pfx in GROUPS){
    if(!secs[pfx]) continue;
    const g=GROUPS[pfx];
    const card=document.createElement('div'); card.className='card cfg-sec';
    card.innerHTML=`<h3>${g.icon} ${g.name}</h3>`;
    secs[pfx].forEach(k=>{
      const v=cfgData[k]; const f=document.createElement('div'); f.className='field';
      let ctrl;
      if(typeof v==='boolean'){
        ctrl=`<label class="sw"><input type="checkbox" data-k="${k}" ${v?'checked':''}><span class="track"></span></label>`;
      }else if(typeof v==='number'){
        ctrl=`<input type="number" data-k="${k}" value="${v}">`;
      }else{
        const isPass=/pass/.test(k);
        ctrl=`<input type="${isPass?'password':'text'}" data-k="${k}" value="${(''+v).replace(/"/g,'&quot;')}">`;
      }
      f.innerHTML=`<div><label>${label(k)}</label><span class="hint mono">${k}</span></div>${ctrl}`;
      card.appendChild(f);
    });
    grid.appendChild(card);
  }
}
async function saveConfig(){
  const out={};
  document.querySelectorAll('#cfgGrid [data-k]').forEach(el=>{
    out[el.dataset.k] = el.type==='checkbox'?el.checked : el.type==='number'?Number(el.value):el.value;
  });
  try{ await api('/api/config',{method:'POST',body:JSON.stringify(out)}); toast('Configuração salva e aplicada ✓'); }
  catch(e){ toast('Preview: salvamento simulado ✓'); }
}

// ---------- Arquivos ----------
async function loadFiles(){
  let files;
  try{ files = await (await api('/api/files')).json(); setConn(true);}
  catch(e){ files = MOCK.files; setConn(false); }
  const el=document.getElementById('fileList');
  if(!files.length){ el.innerHTML='<p class="muted">Vazio.</p>'; return; }
  el.innerHTML = files.map(f=>{
    const kb=f.isDir?'—':(f.size/1024).toFixed(1)+' KB';
    const ico=f.isDir?'📁':/\.pcap|\.hccapx/.test(f.name)?'🎯':/\.json|\.txt/.test(f.name)?'📄':'📦';
    const dl=f.isDir?'':`<a href="/api/download?file=/${encodeURIComponent(f.name)}">⬇ baixar</a>`;
    return `<div class="file"><div class="ico">${ico}</div><div class="nm">${f.name}</div><div class="sz">${kb}</div>${dl}</div>`;
  }).join('');
}

// ---------- OTA ----------
document.getElementById('otaForm').addEventListener('submit',e=>{
  e.preventDefault();
  const file=document.getElementById('otaFile').files[0];
  if(!file){ toast('Selecione um arquivo .bin'); return; }
  const fd=new FormData(); fd.append('update',file,file.name);
  const xhr=new XMLHttpRequest(); xhr.open('POST','/update');
  const prog=document.getElementById('otaProg');
  xhr.upload.onprogress=ev=>{ if(ev.lengthComputable) prog.textContent='Enviando… '+(ev.loaded/ev.total*100|0)+'%'; };
  xhr.onload=()=>{ prog.textContent = xhr.responseText.includes('OK')?'✓ Concluído. Reiniciando…':'✗ Falhou'; };
  xhr.onerror=()=>prog.textContent='✗ Erro de conexão';
  xhr.send(fd);
});
function reboot(){ if(confirm('Reiniciar o Mini Lele?')) fetch('/api/reboot',{method:'POST'}).catch(()=>{}); }

// ---------- Logs (WebSocket) ----------
let paused=false, lines=0;
function togglePause(){ paused=!paused; document.getElementById('pauseBtn').textContent=paused?'▶ Retomar':'⏸ Pausar'; }
function initWS(){
  try{
    const ws=new WebSocket('ws://'+location.hostname+'/ws');
    ws.onopen=()=>setConn(true);
    ws.onclose=()=>setTimeout(initWS,3000);
    ws.onmessage=e=>{
      if(paused) return;
      const log=document.getElementById('log');
      const d=document.createElement('div');
      d.textContent='› '+e.data;
      log.appendChild(d); lines++;
      document.getElementById('logCount').textContent=lines+' linhas';
      if(lines>500) log.removeChild(log.firstChild);
      log.scrollTop=log.scrollHeight;
    };
  }catch(e){}
}

function toast(msg){ const t=document.getElementById('toast'); t.textContent=msg; t.classList.add('show');
  setTimeout(()=>t.classList.remove('show'),2400); }

// ---------- Boot ----------
loadStatus(); loadConfig(); initWS();
setInterval(loadStatus, 3000);
</script>
</body>
</html>

)rawliteral";

#endif
