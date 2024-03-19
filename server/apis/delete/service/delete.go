package service

import (
	"os"

	"github.com/cz4013/server/apis/delete/messages"
	"github.com/cz4013/server/apis/subscribe"
	"github.com/cz4013/server/common"
)

func Delete(fileStorePath string, req *messages.DeleteRequest) messages.DeleteResponse {
	// read the contents of the file into a byte slice
	filePath := common.ConcatenatePaths(fileStorePath, req.Pathname)
	fileContent, err := os.ReadFile(filePath)
	if err != nil {
		return messages.DeleteResponse{
			Success:      false,
			ErrorMessage: err.Error(),
		}
	}

	// sanity check: range of bytes to delete in file should be within file
	fileLen := len(fileContent)
	if req.Offset > uint32(fileLen) {
		return messages.DeleteResponse{
			Success:      false,
			ErrorMessage: "offset exceeds file length",
		}
	}
	if req.Offset+req.NumBytes > uint32(fileLen) {
		return messages.DeleteResponse{
			Success:      false,
			ErrorMessage: "segment to delete exceeds file length",
		}
	}

	// delete by concatenating parts before and after the specified segment
	updatedContent := append(fileContent[:req.Offset], fileContent[req.Offset+req.NumBytes:]...)

	// perform write
	err = os.WriteFile(filePath, updatedContent, 0644)
	if err != nil {
		return messages.DeleteResponse{
			Success:      false,
			ErrorMessage: err.Error(),
		}
	}

	// Notify subscribers of the update
	subscribe.HandleNotification(req.Pathname, updatedContent)

	return messages.DeleteResponse{Success: true}
}
