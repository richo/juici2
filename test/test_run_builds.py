import time
import struct
import tempfile

from juici_testcase import JuiciTestCase
from proto.build_request_pb2 import BuildRequest

class TestRunBuilds(JuiciTestCase):
    def test_can_run_builds(self):
        directory = tempfile.mkdtemp()

        bp = BuildRequest()
        bp.workspace = "test"
        bp.command = "echo butts > %s/file" % directory
        bp.priority = 10

        request = bp.SerializeToString()
        length = len(request)
        request = struct.pack(">I", length) + request
        sock = self.socket()
        sock.send(request)
        status = sock.recv(16)

        with open("%s/file" % directory, "r") as fh:
            self.assertEqual(fh.read(), "butts\n")

    def test_runs_in_worktree(self):
        bp = BuildRequest()
        bp.workspace = "test_butts_lol"
        bp.command = "echo butts > file"
        bp.priority = 10

        request = bp.SerializeToString()
        length = len(request)
        request = struct.pack(">I", length) + request
        sock = self.socket()
        sock.send(request)
        status = sock.recv(16)

        with open("worktree/test_butts_lol/file", "r") as fh:
            self.assertEqual(fh.read(), "butts\n")
