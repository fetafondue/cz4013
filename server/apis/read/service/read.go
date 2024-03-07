package service

import (
	"os"

	"github.com/cz4013/server/apis/read/messages"
	"github.com/cz4013/server/common"
)

func Read(fileStorePath string, req *messages.ReadRequest) messages.ReadResponse {
	// read the contents of the file into a byte slice
	filePath := common.ConcatenatePaths(fileStorePath, req.Pathname)
	fileContent, err := os.ReadFile(filePath)
	if err != nil {
		return messages.ReadResponse{
			Success:      false,
			ErrorMessage: err.Error(),
		}
	}

	// sanity check: range of bytes to read in file should be within file
	fileLen := len(fileContent)
	if req.Offset > uint32(fileLen) {
		return messages.ReadResponse{
			Success:      false,
			ErrorMessage: "offset exceeds file length",
		}
	}
	if req.Offset+req.NumBytes > uint32(fileLen) {
		return messages.ReadResponse{
			Success:      false,
			ErrorMessage: "segment to read exceeds file length",
		}
	}

	// read the specified content
	content := string(fileContent[req.Offset : req.Offset+req.NumBytes])
	return messages.ReadResponse{
		Success: true,
		Content: content,
	}
}
