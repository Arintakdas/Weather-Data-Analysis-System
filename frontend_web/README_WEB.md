Web Frontend (HTML/CSS/JS) connected to C backend

Steps:

1) Build the C backend (Windows, MinGW):

```
gcc -o weather.exe main.c dsa.c -std=c99
```

2) Install Node dependencies and start server:

```
cd frontend_web
npm install
npm start
```

3) Open browser: http://localhost:3000/

Notes:
- The Node server calls the `weather.exe` executable via CLI. Keep `weather.exe` in the project root (one level up from `frontend_web`).
- The server exposes endpoints under `/api/*` that the frontend calls; responses are the raw stdout from the C program.
- If you want a tighter integration (native HTTP in C), I can implement a small C HTTP server instead, but the Node proxy approach is faster to build and keeps the backend logic in C.