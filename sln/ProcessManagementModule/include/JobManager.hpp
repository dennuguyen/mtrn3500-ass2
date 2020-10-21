/**
 * Kills all processes when ProcessManagementModule process dies
 */

#include <Windows.h>

#include "Process.hpp"

class JobManager {
   public:
	JobManager();
	~JobManager();

	// Gets the process handle and attaches it to the job object
	void attachProcess(Process process);

   private:
	HANDLE jobHandle;  // handle to job object
};
