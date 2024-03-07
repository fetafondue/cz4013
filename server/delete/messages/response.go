package messages

import (
	"encoding/binary"
	"fmt"

	"github.com/cz4013/server/common"
)

// DeleteResponse represents a server response message for the Delete method
type DeleteResponse struct {
	Success      bool
	ErrorMessage string
}

// MarshalResponse marshals a DeleteResponse struct into a byte slice
func MarshalResponse(res DeleteResponse) []byte {
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

func validateMarshalledResponse(req []byte) (success bool, err error) {
	// ensure that the byte slice is at least its minimum length (single bool)
	if len(req) == 0 {
		return false, fmt.Errorf("invalid request: byte array length is too short")
	}

	success = req[0] == 1

	// case of only bool present
	if len(req) == common.BoolByteLength {
		// should not have Success = False
		if !success {
			return success, fmt.Errorf("invalid request: unsuccessful without error message")
		}
		return success, nil
	}

	// should not have Success = true
	if success {
		return success, fmt.Errorf("invalid request: successful with additional unknown bytes")
	}

	errMsgLen := int(binary.BigEndian.Uint32(req[common.BoolByteLength : common.BoolByteLength+common.Uint32ByteLength]))

	// ensure that the byte slice has the correct length if it includes ErrorMessage
	if len(req) != common.BoolByteLength+common.Uint32ByteLength+errMsgLen {
		return success, fmt.Errorf("invalid request: incorrect byte array length")
	}

	return success, nil
}

// todo this is for client side
// UnmarshalResponse unmarshals a byte slice into a DeleteResponse struct
func UnmarshalResponse(req []byte) (*DeleteResponse, error) {
	success, err := validateMarshalledResponse(req)
	if err != nil {
		return nil, err
	}

	// successful
	if success {
		return &DeleteResponse{Success: true}, nil
	}

	// extract ErrorMessage if unsuccessful
	errMsg := string(req[common.BoolByteLength+common.Uint32ByteLength:])

	return &DeleteResponse{Success: false, ErrorMessage: errMsg}, nil
}
