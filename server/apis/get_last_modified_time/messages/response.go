package messages

import (
	"encoding/binary"

	"github.com/cz4013/server/common"
)

// GetLastModifiedTimeResponse represents a server response message for the GetLastModifiedTime method
type GetLastModifiedTimeResponse struct {
	Success          bool
	LastModifiedUnixTime int64
	ErrorMessage     string
}

// MarshalSuccessResponse marshals a successful GetLastModifiedTimeResponse struct into a byte slice
func marshalSuccessResponse(lastModifiedUnixTime int64) []byte {
	// buffer consists of: Success (bool), LastModifiedUnixTime (uint64)
	buf := make([]byte, common.BoolByteLength+common.Uint64ByteLength)

	// insert Success
	buf[0] = 1 // success flag
	// insert LastModifiedUnixTime
	binary.BigEndian.PutUint64(buf[common.BoolByteLength:], uint64(lastModifiedUnixTime))

	return buf
}

// MarshalErrorResponse marshals an unsuccessful GetLastModifiedTimeResponse struct into a byte slice
func marshalErrorResponse(errMsg string) []byte {
	errMsgBytes := []byte(errMsg)
	errMsgLen := len(errMsgBytes)

	// buffer consists of: Success (bool), errMsgLen (uint32), errMsg (string)
	buf := make([]byte, common.BoolByteLength+common.Uint32ByteLength+errMsgLen)

	// insert Success
	buf[0] = 0 // failure flag
	// insert ErrorMessage
	binary.BigEndian.PutUint32(buf[common.BoolByteLength:common.BoolByteLength+common.Uint32ByteLength], uint32(errMsgLen))
	copy(buf[common.BoolByteLength+common.Uint32ByteLength:], errMsgBytes)

	return buf
}

// MarshalResponse marshals a ReadResponse struct into a byte slice
func MarshalResponse(res GetLastModifiedTimeResponse) []byte {
	if res.Success {
		return marshalSuccessResponse(res.LastModifiedUnixTime)
	}
	return marshalErrorResponse(res.ErrorMessage)
}
