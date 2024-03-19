package service

import (
	"os"

	"github.com/cz4013/server/apis/replace/messages"
	"github.com/cz4013/server/apis/subscribe"
	"github.com/cz4013/server/common"
)

func Replace(fileStorePath string, req *messages.ReplaceRequest) messages.ReplaceResponse {
	// read the contents of the file into a byte slice
	filePath := common.ConcatenatePaths(fileStorePath, req.Pathname)
	fileContent, err := os.ReadFile(filePath)
	if err != nil {
		return messages.ReplaceResponse{
			Success:      false,
			ErrorMessage: err.Error(),
		}
	}

	// sanity check: offset should be within file
	fileLen := len(fileContent)
	if req.Offset > uint32(fileLen) {
		return messages.ReplaceResponse{
			Success:      false,
			ErrorMessage: "offset exceeds file length",
		}
	}

	replaceContentBytes := []byte(req.Content)
	replaceContentLen := len(replaceContentBytes)

	// allocate new buffer with space for content to be replaced
	buf := make([]byte, fileLen)

	// copy original file content up to and before Offset
	copy(buf[0:req.Offset], fileContent[0:req.Offset])
	// copy content to be replaced
	copy(buf[req.Offset:req.Offset+uint32(replaceContentLen)], replaceContentBytes)
	// copy original file content from and after Offset
	copy(buf[req.Offset+uint32(replaceContentLen):], fileContent[req.Offset+uint32(replaceContentLen):])


	// perform write
	err = os.WriteFile(filePath, buf, 0644)
	if err != nil {
		return messages.ReplaceResponse{
			Success:      false,
			ErrorMessage: err.Error(),
		}
	}

	// Notify subscribers of the update
	subscribe.HandleNotification(req.Pathname, buf)

	return messages.ReplaceResponse{Success: true}
}
