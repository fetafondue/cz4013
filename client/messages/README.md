## Notes
* Use `g++ -o output_file_name srccode_file_name.cpp -lnetwork` to compile because the `htonl` function, which allows translation of bytes to transfer between different machines, requires linking to the network library.

* To use messages, include them at the top of your file, e.g.:
```
#include "messages/delete.h"
#include "messages/read.h"
#include "messages/replace.h"
#include "messages/subscribe.h"
#include "messages/write.h"
```
