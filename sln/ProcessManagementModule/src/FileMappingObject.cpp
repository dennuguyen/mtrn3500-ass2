#include <iostream>

#include "FileMappingObject.hpp"

FileMappingObject::FileMappingObject(std::wstring name, size_t size) : name(name), size(size) {
	handle = CreateFileMapping(INVALID_HANDLE_VALUE,
		                       NULL,
		                       PAGE_READWRITE,
		                       0,
		                       size,
		                       name.c_str());

	if (handle == INVALID_HANDLE_VALUE) {
		std::cerr << "ERROR " << INVALID_HANDLE_VALUE << std::endl;
		exit(EXIT_FAILURE);
	}
}

FileMappingObject::~FileMappingObject() { CloseHandle(handle); }

std::wstring FileMappingObject::getName() {
	return name;
}

size_t FileMappingObject::getSize() {
	return size;
}