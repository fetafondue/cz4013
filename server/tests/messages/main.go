package main

import (
	"fmt"

	"github.com/cz4013/server/delete/messages"
)

// example
func deleteMessageTest() {
	// --- TEST UNMARSHAL REQUEST ---
	// paste marshalled request from client test here
	bytes := []byte("\x00\x00\x00\x16\x2f\x70\x61\x74\x68\x2f\x74\x6f\x2f\x73\x6f\x6d\x65\x2f\x66\x69\x6c\x65\x2e\x70\x64\x66\xff\xff\xff\xff\x00\x00\x00\x7b")
	unmarshalled, err := messages.UnmarshalRequest(bytes)
	if err != nil {
		fmt.Println("UnmarshalRequest error: ", err)
		return
	}
	fmt.Println("UnmarshalRequest: ", unmarshalled)
	// verify printed request is same as original on client side

	// --- TEST MARSHAL RESPONSE ---
	res := messages.DeleteResponse{
		Success:      false,
		ErrorMessage: "some error occurred >:()",
	}
	marshalled := messages.MarshalResponse(res)
	fmt.Println("marshalled response: ", marshalled)
	// past the printed value into client side test and see if the res unmarshals to the original one
}

func main() {
	deleteMessageTest()
}
