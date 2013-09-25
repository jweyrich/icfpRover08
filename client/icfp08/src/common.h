#pragma once

#define SAFE_FREE(p) { if ((p)) { free((p)); (p) = NULL; } }
#define SAFE_DELETE(p) { if ((p)) { delete ((p)); (p) = NULL; } }
#define SAFE_DELETE_ARRAY(p) { if ((p)) { delete [] ((p)); (p) = NULL; } }
#define WaitForSingleObject(handle, time)) { pthread_mutex_lock(&(handle)) } // TODO what?
