package main

import (
	"fmt"
	"time"

	"github.com/cz4013/server/apis"

	"github.com/cz4013/server/apis/delete/messages"
	getlastmodtime "github.com/cz4013/server/apis/get_last_modified_time/messages"
)

// example
func deleteMessageRequestTest() {
	// paste marshalled request from client test here (remove first byte -- the message type)
	bytes := []byte("\x00\x00\x00\x16\x2f\x70\x61\x74\x68\x2f\x74\x6f\x2f\x73\x6f\x6d\x65\x2f\x66\x69\x6c\x65\x2e\x70\x64\x66\xff\xff\xff\xff\x00\x00\x00\x7b")
	unmarshalled, err := messages.UnmarshalRequest(bytes)
	if err != nil {
		fmt.Println("UnmarshalRequest error: ", err)
		return
	}
	fmt.Println("UnmarshalRequest: ", unmarshalled)
	// verify printed request is same as original on client side
}

// example
func getlastmodtimeMessageRequestTest() {
	// paste marshalled request from client test here (no need to remove first byte)
	bytes := []byte("\x20\x00\x00\x00\x08\x66\x69\x6c\x65\x2e\x74\x78\x74")

	// get message type
	messageType := apis.MessageType(bytes[0])
	data := bytes[1:]

	fmt.Println("Message type: ", messageType)

	unmarshalled, err := getlastmodtime.UnmarshalRequest(data)
	if err != nil {
		fmt.Println("UnmarshalRequest error: ", err)
		return
	}
	fmt.Println("UnmarshalRequest: ", unmarshalled)
	// verify printed request is same as original on client side
}

// example
func deleteMessageResponseTest() {
	res := messages.DeleteResponse{
		Success:      false,
		ErrorMessage: "some error occurred >:()",
	}
	marshalled := messages.MarshalResponse(res)
	fmt.Println("marshalled response: ", marshalled)
	// past the printed value into client side test and see if the res unmarshals to the original one
}

func getlastmodtimeResponseTest() {
	res := getlastmodtime.GetLastModifiedTimeResponse{
		Success:      false,
		LastModifiedUnixTime: time.Now().Unix(),
		ErrorMessage: "this is an error!!",
	}
	marshalled := getlastmodtime.MarshalResponse(res)
	fmt.Println("marshalled response: ", marshalled)
	// past the printed value into client side test and see if the res unmarshals to the original one
}

func main() {
	getlastmodtimeResponseTest()
}
