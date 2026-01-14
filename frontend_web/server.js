const express = require('express');
const { execFile } = require('child_process');
const path = require('path');

const EXE = path.join(__dirname, '..', 'backend', 'weather.exe');
const app = express();
const port = process.env.PORT || 3000;

app.use(express.static(path.join(__dirname, 'static')));

function runExe(args, res) {
  execFile(EXE, args, { timeout: 10000, cwd: path.join(__dirname, '..', 'backend') }, (err, stdout, stderr) => {
    if (err) {
      res.status(500).json({ error: err.message, stdout: stdout, stderr: stderr });
    } else {
      res.json({ output: stdout });
    }
  });
}

app.get('/api/dashboard', (req, res) => runExe(['dashboard'], res));
app.get('/api/dashboard_json', (req, res) => runExe(['dashboard_json'], res));
app.get('/api/load', (req, res) => runExe(['load'], res));
app.get('/api/save', (req, res) => runExe(['save'], res));
app.get('/api/search_city', (req, res) => {
  const q = req.query.q || '';
  runExe(['search_city', q], res);
});
app.get('/api/search_date', (req, res) => {
  const d = req.query.d || '';
  runExe(['search_date', d], res);
});
app.get('/api/top5', (req, res) => {
  const type = req.query.type || 'hottest';
  runExe(['top5', type], res);
});
app.get('/api/top5percity', (req, res) => {
  const type = req.query.type || 'hottest';
  runExe(['top5percity', type], res);
});
app.get('/api/storm', (req, res) => {
  const idx = req.query.idx || '0';
  runExe(['storm', idx], res);
});

app.listen(port, () => {
  console.log(`Frontend server running: http://localhost:${port}/`);
});
