# Notes on the Project's implementation

The base code in the [repo](https://github.com/udacity/CppND-System-Monitor-Project-Updated.git) is cloned from Udacity's course on C++.

## Memory Utilization

### Result

A value [0-100] is presented in the Memory field, as well as a "load bar". The implementation for both of these items was already in place.

### Implementation

- `src/linux_parser.cpp`: `LinuxParser::MemoryUtilization()`

This implementation opens the `/proc/meminfo` file in the Linux system, where the values of Total memory and Free Memory are designated with `MemTotal` and `MemFree`. The value corresponding to each of these keys is stored in its respective variable, `total_mem` and `free_mem`. The returned value is the difference between these values over the total available memory.

- `src/system.cpp`: `System::MemoryUtilization()`

Function calls the `MemoryUtilization()` function from the `LinuxParser` namespace referred above.

## Kernel and OS

### Result

Text with the OS name as well as the kernel version are visible at the top of the tool

### Implementation

- `src/linux_parser.cpp`: `LinuxParser::Kernel()`

The implementation was already done, apart from the necessary addition of the `version` variable to use in the `linestream` utilization so that the correct information of the kernel version was fetched from the correct value.

- `src/linux_parser.cpp`: `LinuxParser::OperatingSystem()`

The implementation was already correctly done.

- `src/system.cpp`: `System::Kernel()` & `System::OperatingSystem()`

Simple call of the correct functions

## Total and Running Processes

### Result

A value of the number of total and running processes is present in the `Total Processes` and `Running Processes` field in the monitor tool, respectively.

### Implementation

- `src/linux_parser.cpp`: `LinuxParser::TotalProcesses()` & `LinuxParser::RunningProcesses()`

Implemented in the same way as the `MemoryUtilization`, `Kernel` and `OperatingSystem`, by searching for the `processes` and `procs_running` key, repectively, and returning it's `value`.

- `src/system.cpp`: `System::TotalProcesses()` & `System::RunningProcesses()`

Simple call of the correct functions

## System UpTime

### Result

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

The function builds a ostringstream `time` that takes the values calculated above and turns them to strings with a set width of 2 (`std:setw(2)`) and filled with `0`s (`std::setfill('0')`).

The return operation changes the operation from a `ostringstream` to a `string`.

(Implementation of permanent double digits in the format of the time written by Satyajeet S. [here](https://knowledge.udacity.com/questions/155686)).

## CPU Utilization

### Result

The field for `CPU` in the monitor tool will show a percentage of current CPU utilization, together with a visual percentage bar.

### Implementation

This implementation was based on how to calculate CPU utilization in this [link](https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux). The solution proposed in ithas been split into several of the functions described below.

- `src/linux_parser.cpp`: `LinuxParser::CpuUtilization()`

Implemented in a similar way as the previous functions to fetch information from a specific file; in this case, `/proc/stat`.

The required information from this file occurs on the first line, therefore there is no need to use a `while-loop` to go through all the lines, contrary to other functions in this project.

This line "aggregates the numbers in all the other cpuN lines. These numbers identify the amount of time the CPU has spent performing different kinds of work. Time units are in USER_HZ (typically hundredths of a second)".([chapter 1.8 of The /proc filesystem](https://github.com/torvalds/linux/blob/master/Documentation/filesystems/proc.rst)).

This function will return a vector of strings correspoding to the values of each and every one of the numbers in the line. The function is called by the following two functions.

- `src/linux_parser.cpp`: `LinuxParser::ActiveJiffies()`

Returns the sum of the "Jiffies" that represent time when the CPUs were doing something.

- `src/linux_parser.cpp`: `LinuxParser::IdleJiffies()`

Returns the sum of the "Jiffies" that represent time when the CPUs were idle.

- `src/linux_parser.cpp`: `LinuxParser::Jiffies()`

Returns the sum of both of the two previous functions.

- `src/processor.cpp`: `Processor::Utilization()`

*NOTE:* Given that the values in `/proc/stat` are from when the system started, the output of this function will be rather static if the system has been up for some time. This could be improved by calculating the delta of the cpu usage between `t` and `t - 1`.

This merely returns the fraction of `ActiveJiffies` over total `Jiffies`. As mentioned, it can be improved.

## Processes

### Result

A list of 10 processes is showed in the bottom part of the display, incluiding the information about each process `PID`, `User`, `CPU %`, `RAM [MB]`, `UpTime` and `Command`.

### Implementation

The information displayed in each field for each process was obtained in a similar way as the information of the system. The difference being that each process has a `/proc/[pid]/[file]` which will contain the necessary information that we are looking for.

#### PID

- `src/linux_parser.cpp`: `LinuxParser::Pids()`

This function was already suplied in the base code; it goes through the list of subdirectories in the `/proc` directory whose names are `int` numbers. It supplies a vector of processes later used by the `System::Processes()` function.

- `src/system.cpp`: `System::Processes()`

Returns a reference to a vector of type `Processes`. It fills this vector by going through the vector returned by `LinuxParser::Pids()`, creates a process of type `Process` with the `pid` of said process, and "pushing back" each process to the end of the vector to be returned. It also clears the private `processes_` vector, otherwise it would result in an error.

- `src/process.cpp`: `Process::Process(int pid)`

With each initialization of this object by the loop mentioned above, it will initiate private variables `pid_`, `cmd_`, etc. The initialization of most of these will be done through the call if the corresponding `LinuxParser::` function. The value is later returned by the `Process::[function]` functions when called by the application that runs the data output.

#### Command

- `src/linux_parser.cpp`: `LinuxParser::Command(int pid)`

In a similar way as done in previous functions in the `LinuxParser` area, it fetches the necessary information by opening the `/proc/[pid]/cmdline` file and attributing to the `cmd` variable the information inserted in the first item of the first line.

#### RAM

- `src/linux_parser.cpp`: `LinuxParser::Ram(int pid)`

Searches for the key `VmSize:` in `/proc/[pid]/status`(*NOTE*: not `/proc/[pid]/stat` - lost a lot of time on this one) and returns a string of the value found in MB, by dividing the `ram_value` which is represented in `kB` by 1000.

#### User

Obtaining the user per process is done in two parts: find the Uid of the process, and find the User corresponding to that Uid.

- `src/linux_parser.cpp`: `LinuxParser::Uid(int pid)`

Similar behaviour as previous functions: goes through the `/proc/[pid]/status` line by line searching for the `Uid:` key; when it finds it, returns the `uid_value`, used in the function below.

- `src/linux_parser.cpp`: `LinuxParser::User(int pid)`

Uses the `uid_value` obtained in the previous function as key to go through the `/etc/passwd` file in search of the user with the corresponding `uid`.

The format of this file `[user]:x:[uid]:[uid]...` requires the translation of `':'` to `' '` in order for the same process as the previous functions to be used. The functionality `std::replace(...)` is used, just like in the `LinuxParser::OperatingSystem()` function.

#### UpTime

The UpTime for each process can be found in the `/proc/{pid]/stat` file, in the 22nd value, corresponding to the `starttime` value, according to the `proc`[man page](http://man7.org/linux/man-pages/man5/proc.5.html). The `starttime` is the time the process started after system boot, represented in "clock ticks".

- `src/linux_parser.cpp`: `LinuxParser::UpTime(pid)`

The relevant value is fetched from the 22nd location in the first line, therefore a for-loop is used to place the value obtained in the `linestream` in the `up_time` variable over and over until the 22nd place is reached. This value is then turned to a `long int`, divided by `_SC_CLK_TCK` to turn into seconds.

The actual up-time of the process is obtained by subtracting the calculated value above from the system up-time, obtained by calling `LinuxParser::UpTime` (this last solution obtained from a Knowledge post from [here](https://knowledge.udacity.com/questions/155622)).
