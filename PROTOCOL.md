Clients will connect to a unix domain socket at /tmp/juici.sock

Clients will send a structure as a request:

    8 bit mask                : Reserved for message flags
    8 bit big endian type     : enum defined in socket.h (Temporarily)
    32 bit big endian integer : payload length
    [length] bit payload      : protobuf encoded payload

Clients will send only a single request per connection, but may recieve multiple responses when it makes sense.
