from flask import Flask
from flask import request
from flask import make_response
from flask.views import MethodView

PORT = 8000

app = Flask(__name__)

def make_key(realmId, storageId, recordId):
    return '/'.join([realmId, storageId, recordId])

S = dict()

class RecordAPI(MethodView):
    init_every_request = False

    def get(self, realmId, storageId, recordId):
        key = make_key(realmId, storageId, recordId)
        old_record = S.get(key, None)
        if old_record:
            result = {"meta" : old_record['meta']}
            blocks = list()
            if 'blocks' in old_record:
                for k, v in old_record['blocks'].items():
                    blocks.append({
                        'Content-Id': k,
                        'Content-Type': v['Content-Type'],
                        'content': v['content'],
                    })
            if blocks:
                result['blocks'] = blocks
            resp = make_response(result, 200)
        else:
            resp = make_response('{"detail": "Record not found", "status": 404}', 404)
        resp.headers["Content-Type"] = "application/json"
        return resp

    def put(self, realmId, storageId, recordId):
        key = make_key(realmId, storageId, recordId)
        old_record = S.get(key, None)
        body = request.json
        meta = body.get('meta', None)
        if meta is None:
            resp = make_response('{"detail": "Meta data is required", "status": 400}', 400)
            resp.headers["Content-Type"] = "application/json"
        else:
            blocks = dict()
            if 'blocks' in body:
                for block in body['blocks']:
                    blocks[block['Content-Id']] = {"Content-Type":block['Content-Type'], "content":block['content']}
            S[key] = {"blocks":blocks, "meta": meta}
            if old_record:
                resp = make_response("", 204)
            else:
                resp = make_response("", 201)
                resp.headers["Location"] = request.path
        return resp

    def delete(self, realmId, storageId, recordId):
        key = make_key(realmId, storageId, recordId)
        old_record = S.get(key, None)
        if old_record:
            resp = make_response("", 204)
            S[key] = None
        else:
            resp = make_response('{"detail": "Record not found", "status": 404}', 404)
            resp.headers["Content-Type"] = "application/json"
        return resp

app.add_url_rule(
    "/nudsf-dr/v1/<realmId>/<storageId>/records/<recordId>",
    view_func=RecordAPI.as_view("record")
)

class BlocksAPI(MethodView):
    init_every_request = False

    def get(self, realmId, storageId, recordId):
        key = make_key(realmId, storageId, recordId)
        old_record = S.get(key, None)
        if old_record:
            blocks = list()
            if 'blocks' in old_record:
                for k, v in old_record['blocks'].items():
                    blocks.append({
                        'Content-Id': k,
                        'Content-Type': v['Content-Type'],
                        'content': v['content'],
                    })
            if blocks:
                resp = make_response(blocks, 200)
                resp.headers["Content-Type"] = "application/json"
            else:
                resp = make_response('{"detail": "Blocks not found", "status": 404}', 404)
                resp.headers["Content-Type"] = "application/json"
        else:
            resp = make_response('{"detail": "Record not found", "status": 404}', 404)
            resp.headers["Content-Type"] = "application/json"

        return resp

app.add_url_rule(
    "/nudsf-dr/v1/<realmId>/<storageId>/records/<recordId>/blocks",
    view_func=BlocksAPI.as_view("blocks")
)

class BlockAPI(MethodView):
    init_every_request = False

    def get(self, realmId, storageId, recordId, blockId):
        key = make_key(realmId, storageId, recordId)
        old_record = S.get(key, None)
        if old_record:
            if 'blocks' in old_record and blockId in old_record['blocks']:
                block = old_record['blocks'][blockId]
                resp = make_response(block['content'], 200)
                resp.headers["Content-Type"] = block['Content-Type']
            else:
                resp = make_response('{"detail": "Block not found", "status": 404}', 404)
                resp.headers["Content-Type"] = "application/json"
        else:
            resp = make_response('{"detail": "Record not found", "status": 404}', 404)
            resp.headers["Content-Type"] = "application/json"
        return resp

    def put(self, realmId, storageId, recordId, blockId):
        key = make_key(realmId, storageId, recordId)
        old_record = S.get(key, None)
        if old_record:
            if 'blocks' in old_record and blockId in old_record['blocks']:
                block = old_record['blocks'][blockId]
                old_record['blocks'][blockId] = {"Content-Type":request.headers['Content-Type'], "content":request.data.decode()}
                resp = make_response('', 204)
            else:
                if 'blocks' not in old_record:
                    old_record['blocks'] = dict()
                old_record['blocks'][blockId] = {"Content-Type":request.headers['Content-Type'], "content":request.data.decode()}
                resp = make_response('', 201)
                resp.headers["Location"] = request.path
        else:
            resp = make_response('{"detail": "Record not found", "status": 404}', 404)
            resp.headers["Content-Type"] = "application/json"
        return resp

    def delete(self, realmId, storageId, recordId, blockId):
        key = make_key(realmId, storageId, recordId)
        old_record = S.get(key, None)
        if old_record:
            if 'blocks' in old_record and blockId in old_record['blocks']:
                old_record['blocks'][blockId] = None
                resp = make_response('', 204)
            else:
                resp = make_response('{"detail": "Block not found", "status": 404}', 404)
                resp.headers["Content-Type"] = "application/json"
        else:
            resp = make_response('{"detail": "Record not found", "status": 404}', 404)
            resp.headers["Content-Type"] = "application/json"
        return resp

app.add_url_rule(
    "/nudsf-dr/v1/<realmId>/<storageId>/records/<recordId>/blocks/<blockId>",
    view_func=BlockAPI.as_view("block")
)

if __name__ == '__main__':
    app.run(debug=True, port=PORT)