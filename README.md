# cz4013

## Setup

### Server
1. Install Go
2. Go to server directory `cd server`
3. Add files as desired under /disk
3. Run `go run main.go -port <port> -is <alo/amo>` (port is 8080 by default, is: invocation semantics to use, either at-least-once (alo) or at-most-once (amo))
4. Use `ifconfig` | `ipconfig` to find the IP address of the server

### Client
1. Install GCC
2. Go to client directory `cd client`
2. Compile `./compile.sh` (if this does not work, try `chmod +x compile.sh` first)
3. Run `./client_udp <ipaddress> <socket> <t (refresh interval)>` (server runs on 8080 by default)