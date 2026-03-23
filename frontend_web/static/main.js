let statsData = null;
let chartInstance = null;
let networkInstance = null;

const ind = document.getElementById('status-indicator');
const toastContainer = document.getElementById('toast-container');
const tBody = document.getElementById('table-body');
const tHead = document.getElementById('table-head');
const tEmpty = document.getElementById('table-empty-state');

function showStatus(msg) {
  ind.style.display = 'block';
  ind.innerText = '⏳ ' + msg;
}
function hideStatus() { ind.style.display = 'none'; }

function showToast(msg, type) {
  const t = document.createElement('div');
  t.className = 'toast-msg';
  t.style.borderLeft = type === 'error' ? '4px solid #f87171' : '4px solid #38bdf8';
  t.innerText = msg;
  toastContainer.appendChild(t);
  setTimeout(() => t.remove(), 4000);
}

// ─── TAB LOGIC ─────────────────────────────────────────
const views = {
  'overview': document.getElementById('view-overview'),
  'data':     document.getElementById('view-data'),
  'radar':    document.getElementById('view-radar'),
  'system':   document.getElementById('view-system')
};
const tabBtns = {
  'overview': document.getElementById('btnTabOverview'),
  'data':     document.getElementById('btnTabData'),
  'radar':    document.getElementById('btnTabRadar'),
  'system':   document.getElementById('btnTabSystem')
};

function switchTab(target) {
  Object.keys(views).forEach(k => {
    views[k].style.display = (k === target) ? 'block' : 'none';
    tabBtns[k].classList.toggle('active', k === target);
  });
}

Object.keys(tabBtns).forEach(k => tabBtns[k].onclick = () => switchTab(k));

document.getElementById('heroGoToRadar').onclick = () => switchTab('radar');
document.getElementById('btnRefreshHero').onclick = () => fetchDashboard();

// ─── TABLE PARSER ──────────────────────────────────────
// Handles ANY pipe-separated output from C backend, no hard-coded header detection
function parseTable(text) {
  tHead.innerHTML = '';
  tBody.innerHTML = '';

  if (!text || !text.trim()) {
    tEmpty.style.display = 'block';
    return;
  }

  // Strip ANSI color codes
  const clean = text.replace(/\u001b\[[0-9;]*m/g, '').replace(/\r/g, '');
  const lines = clean.split('\n').filter(x => x.trim());
  
  // Check if any line has pipe characters
  const pipeLines = lines.filter(l => l.includes('|') && !l.match(/^[-+| ]+$/));
  
  if (pipeLines.length === 0) {
    // No table data — show raw text as notification
    tEmpty.style.display = 'block';
    tEmpty.innerText = lines[0] || 'No data found.';
    if (lines[0] && lines[0].length < 120) showToast(lines[0]);
    return;
  }

  tEmpty.style.display = 'none';

  // First pipe line = header
  let isFirst = true;
  pipeLines.forEach(line => {
    const cols = line.split('|').map(c => c.trim()).filter(c => c !== '');
    if (cols.length === 0) return;

    const tr = document.createElement('tr');
    if (isFirst) {
      cols.forEach(c => {
        const th = document.createElement('th');
        th.innerText = c;
        tr.appendChild(th);
      });
      tHead.appendChild(tr);
      isFirst = false;
    } else {
      cols.forEach(c => {
        const td = document.createElement('td');
        td.innerText = c;
        tr.appendChild(td);
      });
      tBody.appendChild(tr);
    }
  });
}

// ─── API HELPER ────────────────────────────────────────
async function api(path, gotoTab) {
  showStatus('Loading...');
  try {
    const res = await fetch(path);
    if (!res.ok) {
      const err = await res.json().catch(() => ({}));
      showToast('Error: ' + (err.error || res.status), 'error');
      hideStatus(); return null;
    }
    const json = await res.json();
    if (json.output) parseTable(json.output);
    hideStatus();
    if (gotoTab) switchTab(gotoTab);
    return json;
  } catch (e) {
    showToast('Cannot reach backend server.', 'error');
    hideStatus(); return null;
  }
}

// ─── DASHBOARD ─────────────────────────────────────────
async function fetchDashboard() {
  showStatus('Syncing stats...');
  try {
    const res = await fetch('/api/dashboard_json');
    if (!res.ok) { hideStatus(); return; }
    const json = await res.json();
    if (!json.output) { hideStatus(); return; }

    const raw = json.output;
    const s = raw.indexOf('{'), e = raw.lastIndexOf('}');
    if (s === -1 || e === -1) { hideStatus(); return; }

    statsData = JSON.parse(raw.substring(s, e + 1));
    renderDashboard(statsData);
  } catch (err) {
    console.error('Dashboard fetch failed:', err);
  }
  hideStatus();
}

function renderDashboard(d) {
  if (!d) return;

  const safeSet = (id, val) => {
    const el = document.getElementById(id);
    if (el) el.innerText = val;
  };

  safeSet('kpi-avg-val',   d.avg_temp  ? d.avg_temp.toFixed(1) : '--');
  safeSet('kpi-total-val', d.total || '--');
  safeSet('kpi-hot-val',   d.hottest ? d.hottest.temp.toFixed(1) : '--');
  safeSet('kpi-hot-city',  d.hottest ? d.hottest.city : '--');
  safeSet('kpi-cold-val',  d.coldest ? d.coldest.temp.toFixed(1) : '--');
  safeSet('kpi-cold-city', d.coldest ? d.coldest.city : '--');

  // The chart logic is moved to plotTrend() below!
}

// ─── DYNAMIC 24-HOUR TREND CHART ────────────────────────
async function plotTrend(city, date) {
  const defaultData = Array(24).fill(0);
  let plotData = defaultData;
  let label = `Loading data...`;

  showStatus('Fetching trend...');
  try {
      const res = await fetch(`/api/trend?city=${encodeURIComponent(city)}&date=${encodeURIComponent(date)}`);
      if (res.ok) {
          const j = await res.json();
          const raw = j.output || '';
          const s = raw.indexOf('{');
          const e = raw.lastIndexOf('}');
          if (s !== -1) {
              const data = JSON.parse(raw.substring(s, e + 1));
              if (!data.error && data.hourly) {
                  plotData = data.hourly;
                  label = `24-Hour Trend for ${data.city} (${data.date})`;
                  
                  // Update Dashboard Hero Card
                  const avgEl = document.getElementById('kpi-avg-val');
                  if(avgEl && data.avg_temp !== undefined) avgEl.innerText = data.avg_temp.toFixed(1);
                  const titleEl = document.getElementById('hero-title');
                  if(titleEl) titleEl.innerText = `${data.city} - ${data.date}`;
                  
              } else {
                  label = `No data found for ${city} on ${date}`;
                  showToast(data.error || 'No data found', 'warning');
              }
          }
      } else {
          label = `Server Error (Code ${res.status})`;
          showToast(`Please restart the Node server. API route missing/error.`, 'error');
      }
  } catch (e) {
      label = `Network Error`;
      showToast('Error fetching trend: ' + e.message, 'error');
  }
  hideStatus();

  const ctx = document.getElementById('tempChart');
  if (!ctx) return;
  if (chartInstance) chartInstance.destroy();

  const labels = Array.from({length: 24}, (_, i) => `${i.toString().padStart(2, '0')}:00`);

  chartInstance = new Chart(ctx, {
    type: 'line',
    data: {
      labels: labels,
      datasets: [{
        label: label,
        data: plotData,
        borderColor: '#38bdf8',
        backgroundColor: 'rgba(56, 189, 248, 0.15)',
        borderWidth: 3, fill: true, tension: 0.4,
        pointBackgroundColor: '#fbbf24', pointRadius: 4
      }]
    },
    options: {
      responsive: true, maintainAspectRatio: false,
      plugins: { legend: { display: true, labels: { color: '#f8fafc' } } },
      scales: {
        y: { grid: { color: 'rgba(255,255,255,0.05)' }, ticks: { color: '#94a3b8' } },
        x: { grid: { display: false }, ticks: { color: '#94a3b8', maxTicksLimit: 12 } }
      }
    }
  });
}

document.getElementById('btnFetchTrend')?.addEventListener('click', () => {
  const c = document.getElementById('trendCityInput').value.trim();
  const d = document.getElementById('trendDateInput').value.trim();
  if (c && d) plotTrend(c, d);
  else showToast('Enter both City and Date', 'warning');
});

// Init chart at start
setTimeout(() => plotTrend('Mumbai', '2025-12-11'), 100);

// ─── ACTIONS ───────────────────────────────────────────
document.getElementById('btnSysLoad').onclick = () => {
  api('/api/load');
  showToast('Dataset loaded from CSV');
};
document.getElementById('btnSysSave').onclick = () => {
  api('/api/save');
  showToast('Dataset saved to CSV');
};

document.getElementById('btnTopHotCard').onclick = () => {
  const city = document.getElementById('top5CityInput').value.trim();
  const url = '/api/top5?type=hottest' + (city ? '&city=' + encodeURIComponent(city) : '');
  api(url, 'data');
};
document.getElementById('btnTopColdCard').onclick = () => {
  const city = document.getElementById('top5CityInput').value.trim();
  const url = '/api/top5?type=coldest' + (city ? '&city=' + encodeURIComponent(city) : '');
  api(url, 'data');
};
document.getElementById('btnTopHotCityCard').onclick = () => api('/api/top5percity?type=hottest', 'data');
document.getElementById('btnTopColdCityCard').onclick = () => api('/api/top5percity?type=coldest', 'data');

document.getElementById('btnSearchDateCard').onclick = () => {
  const q = document.getElementById('dateSearchInput').value.trim();
  if (!q) return showToast('Enter date as YYYY-MM-DD', 'error');
  api('/api/search_date?d=' + encodeURIComponent(q), 'data');
};

document.getElementById('btnSearchNav').onclick = () => {
  const q = document.getElementById('citySearchbar').value.trim();
  if (!q) return showToast('Enter a city name', 'error');
  api('/api/search_city?q=' + encodeURIComponent(q), 'data');
};

document.getElementById('citySearchbar').addEventListener('keypress', e => {
  if (e.key === 'Enter') document.getElementById('btnSearchNav').click();
});
document.getElementById('dateSearchInput').addEventListener('keypress', e => {
  if (e.key === 'Enter') document.getElementById('btnSearchDateCard').click();
});

// ─── RADAR / STORM GRAPH ───────────────────────────────
document.getElementById('btnSimulateStorm').onclick = async () => {
  const val = document.getElementById('stormCityInput').value.trim();
  if (!val) return showToast('Enter a city name or index (e.g. 5)', 'error');

  showStatus('Building radar...');
  try {
    const res = await fetch('/api/storm_json?idx=' + encodeURIComponent(val));
    if (!res.ok) {
      showToast('Storm API returned ' + res.status, 'error');
      hideStatus(); return;
    }
    const j = await res.json();
    const raw = j.output || '';
    const s = raw.indexOf('{'), e = raw.lastIndexOf('}');
    if (s === -1) {
      showToast('No graph data returned from backend', 'error');
      hideStatus(); return;
    }
    const data = JSON.parse(raw.substring(s, e + 1));
    if (data.error) { showToast(data.error, 'error'); hideStatus(); return; }
    drawRadar(data);
    showToast('Storm radar generated!');
  } catch (err) {
    showToast('Failed to connect to storm API', 'error');
  }
  hideStatus();
};

function drawRadar(data) {
  const cont = document.getElementById('network-container');
  if (!cont) return;

  const nodes = [], edges = [];
  nodes.push({
    id: 0, label: (data.epicenter || 'Epicenter') + '\n(Impact)',
    color: '#ef4444', font: { color: '#fff', size: 16, bold: true },
    size: 35, shape: 'hexagon'
  });

  if (data.connected && data.connected.length > 0) {
    data.connected.forEach((city, i) => {
      nodes.push({
        id: i + 1, label: city,
        color: '#f59e0b', font: { color: '#fff', size: 13 },
        size: 20, shape: 'dot'
      });
      edges.push({ from: 0, to: i + 1, color: { color: 'rgba(245,158,11,0.5)' }, width: 2, arrows: 'to' });
    });
  } else {
    showToast('No connected cities impacted from this epicenter.');
  }

  const netData = { nodes: new vis.DataSet(nodes), edges: new vis.DataSet(edges) };
  const opts = {
    physics: { barnesHut: { gravitationalConstant: -3000, springLength: 200 } },
    interaction: { hover: true, dragNodes: true },
    nodes: { borderWidth: 2 },
    width: '100%',
    height: '100%',
    autoResize: true
  };

  if (networkInstance) networkInstance.destroy();
  networkInstance = new vis.Network(cont, netData, opts);
}

// ─── INIT ──────────────────────────────────────────────
fetchDashboard();
