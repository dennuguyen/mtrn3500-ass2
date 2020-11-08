/**
 * Kills all processes when ProcessManagementModule process dies
 */

#include <iostream>
#include <Windows.h>

#include "Process.hpp"

class JobManager {
   public:
	JobManager() {
		jobHandle = CreateJobObject(NULL, NULL);
		if (jobHandle == NULL) {
			std::cerr << "ERROR: Could not create job object" << std::endl;
			exit(EXIT_FAILURE);
		}

		JOBOBJECT_EXTENDED_LIMIT_INFORMATION info = {};

		// Tell the job object to kill all processes when job object dies
		info.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
		if (!SetInformationJobObject(jobHandle, JobObjectExtendedLimitInformation, &info, sizeof(info))) {
			std::cout << "ERROR: Could not set information in job object" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	~JobManager() {}

	// Gets the process handle and attaches it to the job object
	void attachProcess(Process process) {
		if (!AssignProcessToJobObject(jobHandle, process.pinfo.hProcess)) {
			std::cerr << "ERROR: Could not attach process to job object" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

   private:
	HANDLE jobHandle;  // handle to job object
};
