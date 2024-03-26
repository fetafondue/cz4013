package main

import (
	"flag"
	"fmt"
	"log"
	"math/rand"
	"net"
	"os"
	"time"

	"github.com/cz4013/server/apis"
	"github.com/cz4013/server/common"
	"github.com/cz4013/server/network"
)

var Conn *net.UDPConn // global variable representing the server's UDP connection

// parse command-line arguments flags
func parseFlags() (port *string, sem common.InvocationSemantic) {
	// define flags
	port = flag.String("port", "8080", "Port for server to listen on")

	inv_semantics := flag.String("inv_semantics", "alo", "Invocation semantics to use. [at-least-once (alo), at-most-once (amo)]")
	sem = common.AT_LEAST_ONCE
	if *inv_semantics == "amo" {
		sem = common.AT_MOST_ONCE
	}

	flag.Parse()

	return port, sem
}

func getFullFileStorePath() (string, error) {
	currentDir, err := os.Getwd()
	if err != nil {
		return "", fmt.Errorf("unable to get working directory")
	}

	return currentDir + common.FilesDir, nil
}

func handlePacket(sem common.InvocationSemantic, fileStorePath string) {
	// Randomly decide what to do with the client's request
	rand.Seed(time.Now().UnixNano())
	randomChoice := rand.Intn(10) // Generate a random number between 0 and 9
	if randomChoice < 1 {         // 10% chance of loss of request
		log.Println("Simulating loss of request message")
		time.Sleep(11 * time.Second)
		return
	} else if randomChoice < 2 { // Additional 10% chance to loss of reply
		log.Println("Simulating loss of reply message")
		time.Sleep(11 * time.Second)
		return
	}
	// Remaining 40% chance to proceed as usual => read from client
	data, clientAddress, err := network.ReadFromClient()
	if err != nil {
		log.Println("Error reading data:", err)
		return
	}
	log.Println("Simulating proceeding as usual")

	// process request
	response := apis.RouteRequest(sem, fileStorePath, clientAddress, data)

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
