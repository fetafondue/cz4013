package common

import "net"

var Conn *net.UDPConn // global variable representing the server's UDP connection

func ConcatenatePaths(path1 string, path2 string) string {
	return path1 + "/" + path2
}