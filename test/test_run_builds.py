import time
import struct
import tempfile

import juici_server

from juici_testcase import JuiciTestCase
from proto.build_request_pb2 import BuildRequest
from proto.build_complete_pb2 import BuildComplete

class TestRunBuilds(JuiciTestCase):
    def test_can_run_builds(self):
        directory = tempfile.mkdtemp()

        bp = BuildRequest()
        bp.workspace = "test"
        bp.command = "echo butts > %s/file" % directory
        bp.priority = 10

        request = bp.SerializeToString()
        length = len(request)
        msg_type = juici_server.MSG_BUILD_REQUEST
        request = struct.pack(">II", msg_type, length) + request
        sock = self.socket()
        sock.send(request)
        status = sock.recv(1024)

        with open("%s/file" % directory, "r") as fh:
            self.assertEqual(fh.read(), "butts\n")

    def test_runs_in_worktree(self):
        bp = BuildRequest()
        bp.workspace = "test_butts_lol"
        bp.command = "echo butts > file"
        bp.priority = 10

        request = bp.SerializeToString()
        length = len(request)
        msg_type = juici_server.MSG_BUILD_REQUEST
        request = struct.pack(">II", msg_type, length) + request
        sock = self.socket()
        sock.send(request)
        status = sock.recv(1024)

        with open("worktree/test_butts_lol/file", "r") as fh:
            self.assertEqual(fh.read(), "butts\n")

    def test_gets_exit_status(self):
        directory = tempfile.mkdtemp()

        bp = BuildRequest()
        bp.workspace = "test"
        bp.command = "exit 2"
        bp.priority = 10

        request = bp.SerializeToString()
        length = len(request)
        msg_type = juici_server.MSG_BUILD_REQUEST
        request = struct.pack(">II", msg_type, length) + request
        sock = self.socket()
        sock.send(request)
        status = sock.recv(1024)

        msg_type, size = struct.unpack(">II", status[0:8])

        self.assertEqual(msg_type, juici_server.MSG_BUILD_COMPLETE)

        resp = BuildComplete()
        resp.ParseFromString(status[8:])

        self.assertEqual(resp.status, 2)
