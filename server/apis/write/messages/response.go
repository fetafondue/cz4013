package messages

import (
	"encoding/binary"

	"github.com/cz4013/server/common"
)

// WriteResponse represents a server response message for the Write method
type WriteResponse struct {
	Success      bool
	ErrorMessage string
}

// MarshalResponse marshals a WriteResponse struct into a byte slice
func MarshalResponse(res WriteResponse) []byte {
	// no error message if successful
	if res.Success {
		return []byte{1} // success flag
	}

	// include ErrorMessage field if unsuccessful
	errMsgBytes := []byte(res.ErrorMessage)
	errMsgLen := len(errMsgBytes)

	// buffer consists of: Success (bool), errMsgLen (uint32), ErrorMessage (string)
	buf := make([]byte, common.BoolByteLength+common.Uint32ByteLength+errMsgLen)

	// insert Success
	buf[0] = 0 // failure flag
	// insert ErrorMessage
	binary.BigEndian.PutUint32(buf[common.BoolByteLength:common.BoolByteLength+common.Uint32ByteLength], uint32(errMsgLen))
	copy(buf[common.BoolByteLength+common.Uint32ByteLength:], errMsgBytes)

	return buf
}
