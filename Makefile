CC = clang
CFLAGS += "-Isrc"
PROTOC_OPTS = --c_out src

PROTOBUF_CFLAGS = -lprotobuf-c
PROTOBUFS = src/proto/build_payload.pb-c.o

all: $(PROTOBUFS)
src/proto/%.pb-c.c: proto/%.proto
	protoc-c ${PROTOC_OPTS} $^

src/proto/%.pb-c.o: src/proto/%.pb-c.c
	$(CC) $(CFLAGS) -c $(CFLAGS) -o $@ $^