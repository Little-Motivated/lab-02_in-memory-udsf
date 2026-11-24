import unittest
import subprocess
import time
import http.client
import sys
import httpx
import json

UDSF_ADDR = "127.0.0.1:8000"

class TestStringMethods(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.proc = subprocess.Popen(
            launch_line,
            shell=True,
            universal_newlines=True,
            stderr=subprocess.STDOUT,
            errors="backslashreplace",
        )
        time.sleep(1)
        cls.client = httpx.Client(base_url=f'http://{UDSF_ADDR}')

    @classmethod
    def tearDownClass(cls):
        cls.proc.terminate()
        cls.proc.communicate()
        cls.client.close()

    def test_record_creation(self):
        record = json.loads('{"meta": { "tags" : {"tag1" : ["value1"] } } }')
        record2 = json.loads('{"meta": { "tags" : {"tag2" : ["value2"] } } }')
        realmId = 'test-laboratory'
        storageId = 'AMF'
        recordId = 'supi-1234567890'
        uri = f'/nudsf-dr/v1/{realmId}/{storageId}/records/{recordId}'
        blocks_uri = uri + '/blocks'

        resp = self.client.get(uri)
        self.assertEqual(resp.status_code, 404)

        resp = self.client.get(blocks_uri)
        self.assertEqual(resp.status_code, 404)

        resp = self.client.put(uri, headers={'content-type': 'application/json'}, json=record)
        self.assertEqual(resp.status_code, 201)

        resp = self.client.get(uri)
        self.assertEqual(resp.status_code, 200)
        self.assertEqual(resp.json(), record)

        resp = self.client.get(blocks_uri)
        self.assertEqual(resp.status_code, 404)

        resp = self.client.put(uri, headers={'content-type': 'application/json'}, json=record2)
        self.assertEqual(resp.status_code, 204)

        resp = self.client.get(uri)
        self.assertEqual(resp.status_code, 200)
        self.assertEqual(resp.json(), record2)

        resp = self.client.get(blocks_uri)
        self.assertEqual(resp.status_code, 404)

        resp = self.client.delete(uri)
        self.assertEqual(resp.status_code, 204)

        resp = self.client.get(uri)
        self.assertEqual(resp.status_code, 404)

        resp = self.client.delete(uri)
        self.assertEqual(resp.status_code, 404)

    def test_record_with_block_creation(self):
        block_id = 'userDefBinaryBlob'
        block_data = 'QmxvY2sgY29udGVudA=='
        record = json.loads('{"meta": { "tags" : {"tag1" : ["value1"] } }, "blocks": [{"Content-Id": "userDefBinaryBlob", "Content-Type": "text/plain", "content": "QmxvY2sgY29udGVudA=="}] }')
        blocks = json.loads('[{"Content-Id": "userDefBinaryBlob", "Content-Type": "text/plain", "content": "QmxvY2sgY29udGVudA=="}]')
        realmId = 'test-laboratory'
        storageId = 'AMF'
        recordId = 'supi-1234567890'
        record_uri = f'/nudsf-dr/v1/{realmId}/{storageId}/records/{recordId}'
        blocks_uri = f'{record_uri}/blocks'
        block_uri = f'{record_uri}/blocks/{block_id}'

        resp = self.client.put(record_uri, headers={'content-type': 'application/json'}, json=record)
        self.assertEqual(resp.status_code, 201)

        resp = self.client.get(record_uri)
        self.assertEqual(resp.status_code, 200)
        self.assertEqual(resp.json(), record)

        resp = self.client.get(blocks_uri)
        self.assertEqual(resp.status_code, 200)
        self.assertEqual(resp.json(), blocks)

        resp = self.client.get(block_uri)
        self.assertEqual(resp.status_code, 200)
        self.assertEqual(resp.headers['Content-Type'], 'text/plain')
        self.assertEqual(resp.text, block_data)

        resp = self.client.delete(record_uri)
        self.assertEqual(resp.status_code, 204)

    def test_block_creation(self):
        block_id = 'userDefBinaryBlob'
        block_data = json.loads('{"key": "value"}')
        record = json.loads('{"meta": { "tags" : {"tag1" : ["value1"] } } }')
        blocks_data = json.loads('[{"Content-Id": "userDefBinaryBlob","Content-Type": "application/json","content": "{\\\"key\\\":\\\"value\\\"}"}]')
        realmId = 'test-laboratory'
        storageId = 'AMF'
        recordId = 'supi-1234567890'
        record_uri = f'/nudsf-dr/v1/{realmId}/{storageId}/records/{recordId}'
        blocks_uri = f'{record_uri}/blocks'
        block_uri = f'{record_uri}/blocks/{block_id}'

        resp = self.client.put(record_uri, headers={'content-type': 'application/json'}, json=record)
        self.assertEqual(resp.status_code, 201)

        resp = self.client.put(block_uri, headers={'content-type': 'application/json'}, json=block_data)
        self.assertEqual(resp.status_code, 201)

        resp = self.client.get(blocks_uri)
        self.assertEqual(resp.status_code, 200)
        self.assertEqual(resp.json(), blocks_data)

        resp = self.client.get(block_uri)
        self.assertEqual(resp.status_code, 200)
        self.assertEqual(resp.headers['Content-Type'], 'application/json')
        self.assertEqual(resp.json(), block_data)

        resp = self.client.delete(record_uri)
        self.assertEqual(resp.status_code, 204)


launch_line = "python3 example.py"

if __name__ == '__main__':
    if len(sys.argv) > 1:
        elements_to_remove = list()
        for i, arg in enumerate(sys.argv):
            if arg == '--cmd' and i+1 < len(sys.argv):
                launch_line = sys.argv[i+1]
                elements_to_remove.append(i)
                elements_to_remove.append(i+1)
            if arg == '--addr' and i+1 < len(sys.argv):
                UDSF_ADDR = sys.argv[i+1]
                elements_to_remove.append(i)
                elements_to_remove.append(i+1)
        for i in elements_to_remove[::-1]:
            sys.argv.pop(i)
    unittest.main()