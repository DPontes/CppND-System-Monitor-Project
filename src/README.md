# Notes on the Project's implementation

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

Simple call of the correct functions

## Total and Running Processes

### Result:
A value of the number of total and running processes is present in the `Total Processes` and `Running Processes` field in the monitor tool, respectively.

### Implementation

- `src/linux_parser.cpp`: `LinuxParser::TotalProcesses()` & `LinuxParser::RunningProcesses()`

Implemented in the same way as the `MemoryUtilization`, `Kernel` and `OperatingSystem`, by searching for the `processes` and `procs_running` key, repectively, and returning it's `value`.

- `src/system.cpp`: `System::TotalProcesses()` & `System::RunningProcesses()`

Simple call of the correct functions

## System UpTime

### Result:

The field for system `Up Time` contains the time that the system has been up in the HH:MM:SS format.

### Implementation

- `src/linux_parser.cpp`: `LinuxParser::UpTime()`

Implemented in a similar way as the previous implementations, except for the UpTime the file where we fetch the information is `/proc/uptime`
This simple file contains only two long numbers (values in seconds):
    - the uptime of the system (including time spent in suspend)
    - the amount of time spent in the idle process
We are only interested in the first value, therefore do not use the value extracted of the second number.
The `long int` that is extracted is later used in the function `Format::ElapsedTime(long seconds)` for formatation.


- `src/system.cpp`: `System::Uptime()`

Simple call of the correct function

- `src/format.cpp`: `Format::ElapsedTime(long seconds)`

The function in the `Format` namespace takes in the `long int` returned by the `UpTime()` functionand converts it to the required HH:MM:SS format. This is done by:

  1. divide the seconds by 3600 to get the number of hours. Given that the results is saved in an `int hours` it becomes automatically truncated,
  2. obtain the rest value `%` from dividing for 3600 to get the number of seconds that do not fit in a whole hour; divide then by 60 to obtain the number of minutes,
  3. same reasoning, but this time instead of dividing the rest by 60, we do the rest by 60 again, to get the remaining seconds

The function builds a string `time` that takes the values calculates above and turns them to strings (`to_string`) before concatenating them into the desired format


