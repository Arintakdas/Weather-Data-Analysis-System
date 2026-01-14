let currentView = 'data'; 
let statsData = null;
let chartInstance = null;

const tableHead = document.getElementById('table-head');
const tableBody = document.getElementById('table-body');
const statusMsg = document.getElementById('status-message');
const dataView = document.getElementById('data-view');
const visualView = document.getElementById('visual-view');
const btnData = document.getElementById('btnViewData');
const btnVisual = document.getElementById('btnViewVisual');

function showToast(msg, type = 'success') {
  const toast = document.createElement('div');
  toast.className = `toast ${type}`;
  toast.innerHTML = `<span>${type === 'success' ? '✔' : '✖'}</span> ${msg}`;
  document.body.appendChild(toast);

  
  toast.offsetHeight;
  toast.classList.add('show');

  setTimeout(() => {
    toast.classList.remove('show');
    setTimeout(() => toast.remove(), 300);
  }, 3000);
}

function parseToTable(text) {

  tableHead.innerHTML = '';
  tableBody.innerHTML = '';

  if (!text) {
    statusMsg.style.display = 'block';
    statusMsg.innerText = 'No Data Available';
    return;
  }

 
  const cleanText = text.replace(/\u001b\[[0-9;]*m/g, '');
  const lines = cleanText.split('\n').filter(l => l.trim() !== '');


  const isTable = lines.some(l => l.includes('|'));

  if (!isTable) {
 
    statusMsg.style.display = 'block';
    statusMsg.innerText = cleanText;
    if (cleanText.length < 100) showToast(cleanText, 'success');
    return;
  }

  statusMsg.style.display = 'none';

 
  let headersFound = false;

  lines.forEach(line => {
   
    if (line.match(/^[-+\| ]+$/)) return;

    const cols = line.split('|').map(c => c.trim()).filter(c => c !== '');
    if (cols.length === 0) return;

    if (!headersFound && (cols[0].toLowerCase() === 'id' || cols[0].toLowerCase() === 'recordid' || line.includes('ID'))) {
     
      cols.forEach(h => {
        const th = document.createElement('th');
        th.innerText = h;
        tableHead.appendChild(th);
      });
      headersFound = true;
    } else {
     
      const tr = document.createElement('tr');
      cols.forEach(d => {
        const td = document.createElement('td');
        td.innerText = d;
        tr.appendChild(td);
      });
      tableBody.appendChild(tr);
    }
  });

  }

function switchView(view) {
  currentView = view;
  if (view === 'data') {
    dataView.style.display = 'flex';
    visualView.style.display = 'none';
    btnData.classList.add('active');
    btnVisual.classList.remove('active');
  } else {
    dataView.style.display = 'none';
    visualView.style.display = 'flex';
    btnData.classList.remove('active');
    btnVisual.classList.add('active');
    if (statsData) renderDashboard(statsData);
  }
}

btnData.onclick = () => switchView('data');
btnVisual.onclick = () => switchView('visual');

async function api(path) {

  statusMsg.style.display = 'block';
  statusMsg.innerHTML = '<span class="dot"></span> Processing...';

  try {
    const res = await fetch(path);
    if (!res.ok) {
      const err = await res.json().catch(() => ({ error: 'unknown' }));
      showToast(`Error: ${JSON.stringify(err)}`, 'error');
      statusMsg.innerText = 'Request Failed';
      return null;
    }
    const j = await res.json();

    // Parse output to Table
    if (j.output) parseToTable(j.output);

    if (path.includes('dashboard')) {
      fetchDashboardStats();
    }
    return j;
  } catch (e) {
    showToast('Connection Failed', 'error');
    statusMsg.innerText = 'Connection Error';
  }
}

async function fetchDashboardStats() {
  try {
    const res = await fetch('/api/dashboard_json');
    if (res.ok) {
      const json = await res.json();
      try {
        const raw = json.output;
        const start = raw.indexOf('{');
        const end = raw.lastIndexOf('}');

        if (start !== -1 && end !== -1) {
          const jsonStr = raw.substring(start, end + 1);
          statsData = JSON.parse(jsonStr);
          if (currentView === 'visual') renderDashboard(statsData);
        }
      } catch (err) {
        console.error("Failed to parse dashboard JSON", err);
      }
    }
  } catch (e) { console.error(e); }
}

function renderDashboard(data) {
  if (!data) return;

  document.getElementById('kpi-hot-val').innerText = data.hottest ? `${data.hottest.temp.toFixed(1)}°C` : '--';
  document.getElementById('kpi-hot-city').innerText = data.hottest ? data.hottest.city : '--';

  document.getElementById('kpi-cold-val').innerText = data.coldest ? `${data.coldest.temp.toFixed(1)}°C` : '--';
  document.getElementById('kpi-cold-city').innerText = data.coldest ? data.coldest.city : '--';

  document.getElementById('kpi-avg-val').innerText = data.avg_temp ? `${data.avg_temp.toFixed(1)}°C` : '--';
  document.getElementById('kpi-total-val').innerText = data.total || '--';

  const ctx = document.getElementById('tempChart').getContext('2d');

  if (chartInstance) chartInstance.destroy();

  chartInstance = new Chart(ctx, {
    type: 'bar',
    data: {
      labels: ['Coldest', 'Average', 'Hottest'],
      datasets: [{
        label: 'Temperature (°C)',
        data: [
          data.coldest ? data.coldest.temp : 0,
          data.avg_temp || 0,
          data.hottest ? data.hottest.temp : 0
        ],
        backgroundColor: [
          'rgba(0, 242, 254, 0.6)',
          'rgba(241, 250, 140, 0.6)',
          'rgba(255, 85, 85, 0.6)'
        ],
        borderColor: [
          'rgba(0, 242, 254, 1)',
          'rgba(241, 250, 140, 1)',
          'rgba(255, 85, 85, 1)'
        ],
        borderWidth: 2,
        borderRadius: 8
      }]
    },
    options: {
      responsive: true,
      maintainAspectRatio: false,
      plugins: {
        legend: { display: false },
        title: { display: true, text: 'Temperature Extremes Overview', color: '#888', font: { size: 16 } }
      },
      scales: {
        y: { grid: { color: 'rgba(255,255,255,0.05)' }, ticks: { color: '#888' } },
        x: { grid: { display: false }, ticks: { color: '#fff' } }
      },
      animation: { duration: 1000, easing: 'easeOutQuart' }
    }
  });
}

document.getElementById('btnLoad').onclick = () => api('/api/load');
document.getElementById('btnDashboard').onclick = async () => {
  await api('/api/dashboard');
  switchView('visual');
};
document.getElementById('btnTopHot').onclick = () => {
  api('/api/top5?type=hottest');
  switchView('data');
};
document.getElementById('btnTopCold').onclick = () => {
  api('/api/top5?type=coldest');
  switchView('data');
};
document.getElementById('btnTopHotPerCity').onclick = () => {
  api('/api/top5percity?type=hottest');
  switchView('data');
};
document.getElementById('btnTopColdPerCity').onclick = () => {
  api('/api/top5percity?type=coldest');
  switchView('data');
};
document.getElementById('btnSave').onclick = () => api('/api/save');

document.getElementById('btnSearchCity').onclick = () => {
  const q = document.getElementById('cityQ').value.trim();
  if (!q) return showToast('Please enter a city name.', 'error');
  api('/api/search_city?q=' + encodeURIComponent(q));
  switchView('data');
};

document.getElementById('btnSearchDate').onclick = () => {
  const d = document.getElementById('dateQ').value.trim();
  if (!d) return showToast('Please enter a date.', 'error');
  api('/api/search_date?d=' + encodeURIComponent(d));
  switchView('data');
};

document.getElementById('btnStorm').onclick = () => {
  const idx = document.getElementById('stormIdx').value.trim();
  if (idx === '') return showToast('Please enter a city index.', 'error');
  api('/api/storm?idx=' + encodeURIComponent(idx));
  switchView('data');
};
