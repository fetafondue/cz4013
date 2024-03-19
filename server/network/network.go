package network

import (
	"fmt"
	"net"
)

// global variable representing the server's UDP connection
var conn *net.UDPConn

func StartUdpServer(port *string) (*net.UDPConn, error) {
	addr := fmt.Sprintf(":%s", *port)
	udpAddr, err := net.ResolveUDPAddr("udp", addr)
	if err != nil {
		return nil, err
	}

	conn, err = net.ListenUDP("udp", udpAddr)
	if err != nil {
		return nil, err
	}

	return conn, nil
}

func ReadFromClient() (data []byte, addr *net.UDPAddr, err error){
	// buffer to hold incoming data
	buffer := make([]byte, 1024)
	var n int

	// read data from client
	n, addr, err = conn.ReadFromUDP(buffer)
	if err != nil {
		return
	}

	// process data
	data = buffer[:n]
	return
}

func SendToClient(clientAddr *net.UDPAddr, data []byte) (err error) {
	_, err = conn.WriteToUDP(data, clientAddr)
	if err != nil {
		return 
	}
	return 
}