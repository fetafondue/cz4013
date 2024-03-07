package messages

import (
	"encoding/binary"

	"github.com/cz4013/server/common"
)

// ReadResponse represents a server response message for the Read method
type ReadResponse struct {
	Success      bool
	Content      string
	ErrorMessage string
}

func marshalSingleStringResponse(success byte, str string) []byte {
	strBytes := []byte(str)
	strLen := len(strBytes)

	// buffer consists of: success (bool), strLen (uint32), str (str)
	buf := make([]byte, common.BoolByteLength+common.Uint32ByteLength+strLen)

	// insert success into buf
	buf[0] = success
	// insert str length and itself into buf
	binary.BigEndian.PutUint32(buf[common.BoolByteLength:common.BoolByteLength+common.Uint32ByteLength], uint32(strLen))
	copy(buf[common.BoolByteLength+common.Uint32ByteLength:common.BoolByteLength+common.Uint32ByteLength+strLen], strBytes)

	return buf
}

// MarshalResponse marshals a ReadResponse struct into a byte slice
func MarshalResponse(res ReadResponse) []byte {
	if res.Success {
		return marshalSingleStringResponse(1, res.Content)
	}
	return marshalSingleStringResponse(0, res.ErrorMessage)
}
