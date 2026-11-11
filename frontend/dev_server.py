#!/usr/bin/env python3
"""Tiny local frontend server; forwards /api requests to C++ backend localhost:8080."""
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from urllib.request import Request, urlopen
from urllib.error import HTTPError, URLError
import os

class Handler(SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=os.path.dirname(os.path.abspath(__file__)), **kwargs)
    def do_GET(self):
        if self.path.startswith("/api/"):
            self.proxy()
        else:
            super().do_GET()
    def do_POST(self):
        if self.path.startswith("/api/"):
            self.proxy()
        else:
            self.send_error(404)
    def proxy(self):
        length=int(self.headers.get("Content-Length","0"))
        body=self.rfile.read(length) if length else None
        req=Request("http://127.0.0.1:8080"+self.path,data=body,method=self.command,
                   headers={"Content-Type":self.headers.get("Content-Type","application/json")})
        try:
            with urlopen(req,timeout=30) as res:
                data=res.read(); self.send_response(res.status)
                self.send_header("Content-Type",res.headers.get("Content-Type","application/json"))
                self.send_header("Content-Length",str(len(data))); self.end_headers(); self.wfile.write(data)
        except HTTPError as e:
            data=e.read(); self.send_response(e.code); self.send_header("Content-Type","text/plain")
            self.send_header("Content-Length",str(len(data))); self.end_headers(); self.wfile.write(data)
        except URLError as e:
            data=("C++ backend is not running: "+str(e)).encode()
            self.send_response(502); self.send_header("Content-Type","text/plain")
            self.send_header("Content-Length",str(len(data))); self.end_headers(); self.wfile.write(data)

if __name__=="__main__":
    print("Frontend at http://localhost:5500 (proxying /api to :8080)")
    ThreadingHTTPServer(("127.0.0.1",5500),Handler).serve_forever()
