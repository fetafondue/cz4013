package main

import (
	"flag"
	"fmt"
	"log"
	"net"
	"os"

	"github.com/cz4013/server/apis"
	"github.com/cz4013/server/common"
)

// parse command-line arguments flags
func parseFlags() (port *string) {
	// define flags
	port = flag.String("port", "8080", "Port for server to listen on")
	// b := flag.String("b", "", "Description")

	flag.Parse()

	return port
}

func getFullFileStorePath() (string, error) {
	currentDir, err := os.Getwd()
	if err != nil {
		return "", fmt.Errorf("unable to get working directory")
	}

	return currentDir + common.FilesDir, nil
}

func startUdpServer(port *string) (*net.UDPConn, error) {
	addr := fmt.Sprintf(":%s", *port)
	udpAddr, err := net.ResolveUDPAddr("udp", addr)
	if err != nil {
		return nil, err
	}

	conn, err := net.ListenUDP("udp", udpAddr)
	if err != nil {
		return nil, err
	}

	return conn, nil
}

func handlePacket(conn *net.UDPConn, fileStorePath string) {
	// buffer to hold incoming data
	buffer := make([]byte, 1024)

	// read data from client
	n, addr, err := conn.ReadFromUDP(buffer)
	if err != nil {
		fmt.Println("Error reading data:", err)
		return
	}

	// process data
	data := buffer[:n]
	response := apis.RouteRequest(fileStorePath, addr, data)

	// respond to client
	_, err = conn.WriteToUDP(response, addr)
	if err != nil {
		fmt.Println("Error sending response:", err)
		return
	}
}

func main() {
	port := parseFlags()

	// the absolute path to the server's file store directory
	fileStorePath, err := getFullFileStorePath()
	if err != nil {
		log.Fatal("Unable to obtain server's file store path:", err)
	}

	conn, err := startUdpServer(port)
	if err != nil {
		log.Fatal("Unable to start UDP server:", err)
	}
	defer conn.Close()

	fmt.Println("Started UDP server.\n",
		"File directory:", fileStorePath, "\n",
		"Listening on:", conn.LocalAddr(), "(use ifconfig/ipconfig to get the IP address)")

	// continuously handle incoming messages
	for {
		handlePacket(conn, fileStorePath)
	}
}
