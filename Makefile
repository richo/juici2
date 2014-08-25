CC = gcc
CFLAGS += -g -Isrc -D_DEBUG
PROTOC_OPTS = --c_out src

PROTOBUF_CFLAGS = -lprotobuf-c
PROTOBUFS = src/proto/build_request.pb-c.o \
			src/proto/build_started.pb-c.o \
			src/proto/build_complete.pb-c.o
TEST_PROTOBUFS = test/proto/build_request_pb2.py \
				 test/proto/build_started_pb2.py \
				 test/proto/build_complete_pb2.py
BINS = bin/juici
OBJS = src/build.o src/socket.o src/work.o src/notification.o src/worktree.o

.PHONY: test clean

all: $(BINS)

bin/juici: src/main.c $(PROTOBUFS) $(OBJS)
	$(CC) -o $@ $(CFLAGS) $< $(PROTOBUFS) $(PROTOBUF_CFLAGS) $(OBJS)

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $<

src/proto/%.pb-c.c: proto/%.proto
	protoc-c ${PROTOC_OPTS} $^

src/proto/%.pb-c.o: src/proto/%.pb-c.c
	$(CC) -c $(CFLAGS) -o $@ $^

clean:
	rm $(OBJS)
	rm $(BINS)
	rm $(PROTOBUFS)
	rm $(TEST_PROTOBUFS)

## TESTS

test/proto/%_pb2.py: proto/%.proto
	protoc --python_out test $^

test: all $(TEST_PROTOBUFS)
	python -m unittest discover test
