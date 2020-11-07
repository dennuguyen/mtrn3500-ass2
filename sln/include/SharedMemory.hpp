/**
 * A file mapping object (section object) is a file where its contents are
 * associated with a virtual address space of a process.
 */

#ifndef SM_HPP_
#define SM_HPP_

#include <Windows.h>

#include <string>
#include <vector>

namespace sm {

size_t SIZE = 1024;  // size of maps

class FileMappingObject {
   public:
    FileMappingObject(std::wstring name, size_t size)
        : name(name), size(size), createHandle(NULL), accessHandle(NULL), baseAddress(NULL) {}

    FileMappingObject(const FileMappingObject& obj)
        : name(obj.name), size(obj.size), createHandle(obj.createHandle), accessHandle(obj.accessHandle), baseAddress(obj.baseAddress) {}

    FileMappingObject(FileMappingObject&& obj) noexcept
        : name(std::move(obj.name)), size(std::move(obj.size)), createHandle(std::move(obj.createHandle)), accessHandle(std::move(obj.accessHandle)), baseAddress(std::move(obj.baseAddress)) {}

    ~FileMappingObject() {}

    HANDLE createFileMapping() {
        createHandle = CreateFileMapping(INVALID_HANDLE_VALUE,
                                         NULL,
                                         PAGE_READWRITE,
                                         0,
                                         size,
                                         name.c_str());

        if (createHandle == INVALID_HANDLE_VALUE) {
            std::wcerr << L"ERROR: " << INVALID_HANDLE_VALUE << std::endl;
            exit(EXIT_FAILURE);
        }

        return createHandle;
    }

    HANDLE openFileMapping() {
        accessHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS,
                                       FALSE,
                                       name.c_str());

        if (accessHandle == INVALID_HANDLE_VALUE) {
            std::wcerr << L"ERROR: " << INVALID_HANDLE_VALUE << std::endl;
            exit(EXIT_FAILURE);
        }

        return accessHandle;
    }

    LPVOID mappedViewAddr() {
        baseAddress = MapViewOfFile(accessHandle != NULL ? accessHandle : createHandle,
                                    FILE_MAP_ALL_ACCESS,
                                    0,
                                    0,
                                    size);
        return baseAddress;
    }

    /**
	 * Invalidate handles in method as temp object destructors can invalidate handles
	 */
    void close() {
        if (createHandle != NULL)
            if (CloseHandle(createHandle) == false)
                std::wcerr << L"WARNING: Could not close handle" << createHandle << std::endl;

        if (accessHandle != NULL)
            if (CloseHandle(accessHandle) == false)
                std::wcerr << L"WARNING: Could not close handle" << accessHandle << std::endl;

        if (UnmapViewOfFile(baseAddress) == 0)
            std::wcerr << L"WARNING: Could not unmap " << name << std::endl;
    }

    const std::wstring getName() { return name; }
    const size_t getSize() { return size; }
    const HANDLE getCreateHandle() { return createHandle; }
    const HANDLE getAccessHandle() { return accessHandle; }
    const void* getBaseAddress() { return baseAddress; }

   private:
    std::wstring name;    // name of file mapping object
    size_t size;          // size of file mapping object
    HANDLE createHandle;  // unique handle to file mapping object
    HANDLE accessHandle;  // unique handle to access file mapping object
    LPVOID baseAddress;   // base address of map view
};
}  // namespace sm

#endif  // SM_HPP_
