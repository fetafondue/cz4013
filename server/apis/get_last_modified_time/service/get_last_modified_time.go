package service

import (
	"os"

	"github.com/cz4013/server/apis/get_last_modified_time/messages"
	"github.com/cz4013/server/common"
)

func GetLastModifiedTime(fileStorePath string, req *messages.GetLastModifiedTimeRequest) messages.GetLastModifiedTimeResponse {
	// get file info
	filePath := common.ConcatenatePaths(fileStorePath, req.Pathname)
	info, err := os.Stat(filePath)
	if err != nil {
		return messages.GetLastModifiedTimeResponse{
			Success:      false,
			ErrorMessage: err.Error(),
		}
	}

	// get last modified time
	lastModifiedUnixTime := info.ModTime().Unix()
	return messages.GetLastModifiedTimeResponse{
		Success:          true,
		LastModifiedUnixTime: lastModifiedUnixTime,
	}
}
