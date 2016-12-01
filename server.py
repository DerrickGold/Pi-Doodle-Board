#!/usr/bin/env python3

from flask import Flask, request, jsonify, redirect, url_for, render_template, send_file, Response
from werkzeug.datastructures import Headers
from flask_cors import CORS, cross_origin
from flask_compress import Compress

import os

app = Flask(__name__)
CORS(app)
Compress(app)
Port = 80
imgDir = './DrawingPi/images'
historyCount = 25


def scan_dir(directory, lastfile, page):
    if lastfile is None: lastfile = ""
    if page is None: page = 0
    
    listing = list()
    
    for root,dirs,files in os.walk(os.path.normpath(directory)):
        del(dirs[:])        
        for f in files:
            if f <= str(lastfile): continue
            listing.append(os.path.join(directory, f))

    res = sorted(listing)[page:historyCount]
    return res


@app.route('/all', methods=['GET', 'POST'])
def getAll():
    lastfile = request.values.get('last')
    flip = request.values.get('newest')
    offset = request.values.get('offset')
    if offset is None: offset = 0
    
    resp = {
        'files': scan_dir(imgDir, lastfile, int(offset)),
        'count': 0,
        'offset': offset,
        'limit': historyCount
    }
    resp['count'] = len(resp['files'])
    if flip is not None: resp['files'].reverse()
    return jsonify(**resp)

@app.route('/<path:filename>')
def serve(filename):
    return send_file(filename)


def main():
    app.run(host='0.0.0.0', threaded=True, port=Port)


if __name__ == '__main__':
    main()
