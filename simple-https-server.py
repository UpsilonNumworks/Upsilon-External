# taken from http://www.piware.de/2011/01/creating-an-https-server-in-python/
# generate server.xml with the following command:
#    openssl req -new -x509 -keyout server.pem -out server.pem -days 365 -nodes
# run as follows:
#    python3 simple-https-server.py
# then in your browser, visit:
#    https://localhost:4443

import http.server
import ssl

httpd = http.server.HTTPServer(('0.0.0.0', 8443),
                               http.server.SimpleHTTPRequestHandler)
httpd.socket = ssl.wrap_socket(httpd.socket, certfile='./server.pem',
                               server_side=True)
httpd.serve_forever()
