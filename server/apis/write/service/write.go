package service

import (
	"os"

	"github.com/cz4013/server/apis/write/messages"
	"github.com/cz4013/server/common"
)

func Write(fileStorePath string, req *messages.WriteRequest) messages.WriteResponse {
	// read the contents of the file into a byte slice
	filePath := common.ConcatenatePaths(fileStorePath, req.Pathname)
	fileContent, err := os.ReadFile(filePath)
	if err != nil {
		return messages.WriteResponse{
			Success:      false,
			ErrorMessage: err.Error(),
		}
	}

	// sanity check: offset should be within file
	fileLen := len(fileContent)
	if req.Offset > uint32(fileLen) {
		return messages.WriteResponse{
			Success:      false,
			ErrorMessage: "offset exceeds file length",
		}
	}

	writeContentBytes := []byte(req.Content)
	writeContentLen := len(writeContentBytes)

	// allocate new buffer with space for content to be written
	buf := make([]byte, fileLen+writeContentLen)

	// copy original file content up to and before Offset
	copy(buf[0:req.Offset], fileContent[0:req.Offset])
	// copy content to be written
	copy(buf[req.Offset:req.Offset+uint32(writeContentLen)], writeContentBytes)
	// copy original file content from and after Offset
	copy(buf[req.Offset+uint32(writeContentLen):], fileContent[req.Offset:])

	// perform write
	err = os.WriteFile(filePath, buf, 0644)
	if err != nil {
		return messages.WriteResponse{
			Success:      false,
			ErrorMessage: err.Error(),
		}
	}

	return messages.WriteResponse{Success: true}
}
