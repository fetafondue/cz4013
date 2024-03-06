package messages

import (
	"encoding/binary"
	"fmt"

	"github.com/cz4013/server/common"
)

// ReplaceRequest represents a client request message for the Replace method
type ReplaceRequest struct {
	Pathname string
	Offset   uint32
	Content  string
}

// todo this is for client side to implement
// MarshalRequest marshals a ReplaceRequest struct into a byte slice
func MarshalRequest(req ReplaceRequest) []byte {
	pathnameBytes := []byte(req.Pathname)
	pathnameLen := len(pathnameBytes)
	contentBytes := []byte(req.Content)
	contentLen := len(contentBytes)

	// buffer consists of: pathnameLen (uint32), Pathname (str), Offset (uint32), contentLen (uint32), Content (str)
	bufLen := pathnameLen + contentLen + 3*common.Uint32ByteLength
	buf := make([]byte, bufLen)

	// insert Pathname length and itself into buf
	binary.BigEndian.PutUint32(buf[0:common.Uint32ByteLength], uint32(pathnameLen))
	copy(buf[common.Uint32ByteLength:common.Uint32ByteLength+pathnameLen], pathnameBytes)
	// insert Offset into buf
	binary.BigEndian.PutUint32(buf[common.Uint32ByteLength+pathnameLen:2*common.Uint32ByteLength+pathnameLen], req.Offset)
	// insert Content length and itself into buf
	binary.BigEndian.PutUint32(buf[2*common.Uint32ByteLength+pathnameLen:3*common.Uint32ByteLength+pathnameLen], uint32(contentLen))
	copy(buf[3*common.Uint32ByteLength+pathnameLen:bufLen], contentBytes)

	return buf
}

func validateMarshalledRequest(req []byte) (pathnameLen int, err error) {
	// ensure that the byte slice is at least its minimum length (3 uint32s)
	if len(req) < 3*common.Uint32ByteLength {
		return 0, fmt.Errorf("invalid request: byte array length is too short")
	}

	pathnameLen = int(binary.BigEndian.Uint32(req[0:common.Uint32ByteLength]))
	contentLen := int(binary.BigEndian.Uint32(req[2*common.Uint32ByteLength+pathnameLen : 3*common.Uint32ByteLength+pathnameLen]))

	// ensure that the byte slice has the correct length
	if len(req) != 3*common.Uint32ByteLength+pathnameLen+contentLen {
		return 0, fmt.Errorf("invalid request: incorrect byte array length")
	}

	return pathnameLen, nil
}

// UnmarshalRequest unmarshals a byte slice into a ReplaceRequest struct
func UnmarshalRequest(req []byte) (*ReplaceRequest, error) {
	pathnameLen, err := validateMarshalledRequest(req)
	if err != nil {
		return nil, err
	}

	// extract fields
	pathname := string(req[common.Uint32ByteLength : common.Uint32ByteLength+pathnameLen])
	offset := binary.BigEndian.Uint32(req[common.Uint32ByteLength+pathnameLen : 2*common.Uint32ByteLength+pathnameLen])
	content := string(req[3*common.Uint32ByteLength+pathnameLen:])

	return &ReplaceRequest{
		Pathname: pathname,
		Offset:   offset,
		Content: content,
	}, nil
}
