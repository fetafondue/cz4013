package messages

import (
	"encoding/binary"
	"fmt"

	"github.com/cz4013/server/common"
)

// DeleteRequest represents a client request message for the Delete method
type DeleteRequest struct {
	Pathname string
	Offset   uint32
	NumBytes uint32
}

// todo this is for client side to implement
// MarshalRequest marshals a DeleteRequest struct into a byte slice
func MarshalRequest(req DeleteRequest) []byte {
	pathnameBytes := []byte(req.Pathname)
	pathnameLen := len(pathnameBytes)

	// buffer consists of: pathnameLen (uint32), Pathname (str), Offset (uint32), NumBytes (uint32)
	bufLen := pathnameLen + 3*common.Uint32ByteLength
	buf := make([]byte, bufLen)

	// insert Pathname length and itself into buf
	binary.BigEndian.PutUint32(buf[0:common.Uint32ByteLength], uint32(pathnameLen))
	copy(buf[common.Uint32ByteLength:common.Uint32ByteLength+pathnameLen], pathnameBytes)
	// insert Offset into buf
	binary.BigEndian.PutUint32(buf[common.Uint32ByteLength+pathnameLen:2*common.Uint32ByteLength+pathnameLen], req.Offset)
	// insert NumBytes into buf
	binary.BigEndian.PutUint32(buf[2*common.Uint32ByteLength+pathnameLen:bufLen], req.NumBytes)

	return buf
}

func validateMarshalledRequest(req []byte) (pathnameLen int, err error) {
	// ensure that the byte slice is at least its minimum length (3 uint32s)
	if len(req) < 3*common.Uint32ByteLength {
		return 0, fmt.Errorf("invalid request: byte array length is too short")
	}

	pathnameLen = int(binary.BigEndian.Uint32(req[0:common.Uint32ByteLength]))

	// ensure that the byte slice has the correct length
	if len(req) != 3*common.Uint32ByteLength+pathnameLen {
		return 0, fmt.Errorf("invalid request: incorrect byte array length")
	}

	return pathnameLen, nil
}

// UnmarshalRequest unmarshals a byte slice into a DeleteRequest struct
func UnmarshalRequest(req []byte) (*DeleteRequest, error) {
	pathnameLen, err := validateMarshalledRequest(req)
	if err != nil {
		return nil, err
	}

	// extract fields
	pathname := string(req[common.Uint32ByteLength : common.Uint32ByteLength+pathnameLen])
	offset := binary.BigEndian.Uint32(req[common.Uint32ByteLength+pathnameLen : 2*common.Uint32ByteLength+pathnameLen])
	numBytes := binary.BigEndian.Uint32(req[2*common.Uint32ByteLength+pathnameLen:])

	return &DeleteRequest{
		Pathname: pathname,
		Offset:   offset,
		NumBytes: numBytes,
	}, nil
}
