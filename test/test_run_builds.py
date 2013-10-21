import time
import struct
import tempfile

from juici_testcase import JuiciTestCase
from proto.build_payload_pb2 import BuildPayload

class TestRunBuilds(JuiciTestCase):
    def test_can_run_builds(self):
        directory = tempfile.mkdtemp()

        bp = BuildPayload()
        bp.workspace = "test"
        bp.command = "echo butts > %s/file" % directory
        bp.priority = 10

        payload = bp.SerializeToString()
        length = len(payload)
        payload = struct.pack(">I", length) + payload
        sock = self.socket()
        sock.send(payload)
        status = sock.recv(16)

        with open("%s/file" % directory, "r") as fh:
            self.assertEqual(fh.read(), "butts\n")

    def test_runs_in_worktree(self):
        bp = BuildPayload()
        bp.workspace = "test_butts_lol"
        bp.command = "echo butts > file"
        bp.priority = 10

        payload = bp.SerializeToString()
        length = len(payload)
        payload = struct.pack(">I", length) + payload
        sock = self.socket()
        sock.send(payload)
        status = sock.recv(16)

        with open("worktree/test_butts_lol/file", "r") as fh:
            self.assertEqual(fh.read(), "butts\n")
