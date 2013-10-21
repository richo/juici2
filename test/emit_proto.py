import sys
import struct
import argparse
from proto.build_request_pb2 import BuildRequest

parser = argparse.ArgumentParser()
parser.add_argument("--workspace", help="workspace to build in")
parser.add_argument("--command", help="command to run")
parser.add_argument("--priority", type=int, help="priority for the build", default=1)
args = parser.parse_args()


b = BuildRequest()

b.workspace = args.workspace
b.command = args.command
b.priority = args.priority

request = b.SerializeToString()
length = len(request)
sys.stdout.write(struct.pack(">I", length))
sys.stdout.write(request)
