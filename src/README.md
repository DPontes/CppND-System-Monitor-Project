#Notes on the Project's implementation

The base code in the [repo](https://github.com/udacity/CppND-System-Monitor-Project-Updated.git) is cloned from Udacity's course on C++.

## Memory Utilization

### Result:
A value [0-100] is presented in the Memory field, as well as a "load bar". The implementation for both of these items was already in place.

### Implementation:

- `src/linux_parser.cpp`: `LinuxParser::MemoryUtilization()`

This implementation opens the `/proc/meminfo` file in the Linux system, where the values of Total memory and Free Memory are designated with `MemTotal` and `MemFree`. The value corresponding to each of these keys is stored in its respective variable, `total_mem` and `free_mem`. The returned value is the difference between these values over the total available memory.

- `src/system.cpp`: `System::MemoryUtilization()`

Function calls the `MemoryUtilization()` function from the `LinuxParser` namespace referred above.

## Kernel and OS

### Result:
Text with the OS name as well as the kernel version are visible at the top of the tool

### Implementation:

- `src/linux_parser.cpp`: `LinuxParser::Kernel()`

The implementation was already done, apart from the necessary addition of the `version` variable to use in the `linestream` utilization so that the correct information of the kernel version was fetched from the correct value.

- `src/linux_parser.cpp`: `LinuxParser::OperatingSystem()`

The implementation was already correctly done.

- `src/system.cpp`: `System::Kernel()` & `System::OperatingSystem()`

Simple call of the correct function

## Total and Running Processes

### Result:
A value of the number of total and running processes is present in the `Total Processes` and `Running Processes` field in the monitor tool, respectively.

### Implementation

- `src/linux_parser.cpp`: `LinuxParser::TotalProcesses()` & `LinuxParser::RunningProcesses()`

Implemented in the same way as the `MemoryUtilization`, `Kernel` and `OperatingSystem`, by searching for the `processes` and `procs_running` key, repectively, and returning it's `value`.
