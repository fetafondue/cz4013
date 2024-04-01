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
func parseFlags() (port *string, sem common.InvocationSemantic) {
	// define flags
	port = flag.String("port", "8080", "Port for server to listen on")
	is := flag.String("is", "alo", "Invocation semantics to use. [at-least-once (alo), at-most-once (amo)]")
	flag.Parse()

	sem = common.AT_LEAST_ONCE
	if *is == "amo" {
		sem = common.AT_MOST_ONCE
	}

	return
}

func getFullFileStorePath() (string, error) {
	currentDir, err := os.Getwd()
	if err != nil {
		return "", fmt.Errorf("unable to get working directory")
	}

	return currentDir + common.FilesDir, nil
}

func handlePacket(sem common.InvocationSemantic, fileStorePath string) {
	data, clientAddress, err := network.ReadFromClient()
	if err != nil {
		log.Println("Error reading data:", err)
		return
	}

	// simulating loss of request
	var simulateReqLoss string
	fmt.Println("\nPacket received from a client, simulate loss of REQUEST? (y/n)")
	fmt.Scanf("%v", &simulateReqLoss)
	if simulateReqLoss == "y" {
		log.Println("Simulating loss of request message")
		return
	}

	// process request
	response := apis.RouteRequest(sem, fileStorePath, clientAddress, data)

	// simulating loss of reply
	var simulateReplyLoss string
	fmt.Println("\nReply available for client, simulate loss of REPLY? (y/n)")
	fmt.Scanf("%v", &simulateReplyLoss)
	if simulateReplyLoss == "y" {
		log.Println("Simulating loss of reply message")
		return
	}

	// respond to client
	err = network.SendToClient(clientAddress, response)
	if err != nil {
		log.Println("Error sending response:", err)
		return
	}
}

func main() {
	port, sem := parseFlags()

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
		handlePacket(sem, fileStorePath)
	}
}
