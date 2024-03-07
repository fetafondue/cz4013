package messages

import (
	"encoding/binary"
	"fmt"

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

func validateMarshalledResponse(req []byte) (success bool, err error) {
	// ensure that the byte slice is at least its minimum length (1 bool + 1 uint32)
	if len(req) < common.BoolByteLength+common.Uint32ByteLength {
		return false, fmt.Errorf("invalid request: byte array length is too short")
	}

	success = req[0] == 1
	strLen := int(binary.BigEndian.Uint32(req[common.BoolByteLength : common.BoolByteLength+common.Uint32ByteLength]))

	// ensure that the byte slice has the correct length if it includes ErrorMessage
	if len(req) != common.BoolByteLength+common.Uint32ByteLength+strLen {
		return success, fmt.Errorf("invalid request: incorrect byte array length")
	}

	return success, nil
}

// todo this is for client side
// UnmarshalResponse unmarshals a byte slice into a ReadResponse struct
func UnmarshalResponse(req []byte) (*ReadResponse, error) {
	success, err := validateMarshalledResponse(req)
	if err != nil {
		return nil, err
	}

	// extract str
	str := string(req[common.BoolByteLength+common.Uint32ByteLength:])

	if success {
		return &ReadResponse{Success: true, Content: str}, nil
	}
	return &ReadResponse{Success: false, ErrorMessage: str}, nil
}
