# cz4013

## Setup

### Server
1. Install Go
2. Go to server directory `cd server`
3. Add files as desired under /disk
3. Run `go run main.go`
4. Use `ifconfig` | `ipconfig` to find the IP address of the server

### Client
1. Install GCC
2. Go to client directory `cd client`
2. Compile `./compile.sh` (if this does not work, try `chmod +x compile.sh` first)
3. Run `./client_udp <ipaddress> <socket>` (server runs on 8080 by default)