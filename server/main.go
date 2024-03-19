package main

import (
	"flag"
	"fmt"
	"log"
	"net"
	"os"

	"github.com/cz4013/server/apis"
	"github.com/cz4013/server/common"
	"github.com/cz4013/server/network"
)

var Conn *net.UDPConn // global variable representing the server's UDP connection

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

func handlePacket(fileStorePath string) {
	// read to client
	data, clientAddress, err := network.ReadFromClient()
	if err != nil {
		log.Println("Error reading data:", err)
		return
	}
	// process request
	response := apis.RouteRequest(fileStorePath, clientAddress, data)

	// respond to client
	err = network.SendToClient(clientAddress, response)
	if err != nil {
		log.Println("Error sending response:", err)
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

	conn, err := network.StartUdpServer(port)
	if err != nil {
		log.Fatal("Unable to start UDP server:", err)
	}
	defer conn.Close()

	log.Println("Started UDP server.\n",
		"File directory:", fileStorePath, "\n",
		"Listening on:", conn.LocalAddr(), "(use ifconfig/ipconfig to get the IP address)")

	// continuously handle incoming messages
	for {
		handlePacket(fileStorePath)
	}
}
