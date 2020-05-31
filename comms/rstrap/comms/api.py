"""
To access with the network:

flask run --host=192.168.1.69 --port 5000

from any computer on the network (watch for vpn) - curl http://192.168.1.69:5000/
"""

from flask import Flask
app = Flask(__name__)

@app.route("/")
def hello_world():
	return "Hello, World!"