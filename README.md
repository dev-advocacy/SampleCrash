# Usage
The SampleCrash project is a C++ application designed to demonstrate various functionalities related to crash handling and local dump management.

# Build

## VCPKG Requirements and Installation

### Requirements
To use VCPKG, ensure your system meets the following requirements:
- **Operating System**: Windows
- **C++ Compiler**: Visual Studio 2022
- **Git**: Version control system to clone the VCPKG repository

### Step 1: Installation Steps

### Clone the VCPKG Repository
Open a terminal or command prompt and run the following command to clone the VCPKG repository:
```powershell
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
bootstrap-vcpkg
vcpkg integrate install
```
## Step 2: Build the Project
1. Open the `SampleCrash.sln` file in Visual Studio 2022.
2. Select the configuration (Release) and platform (x64).
3. Build the solution by clicking on `Build` > `Build Solution` or pressing `Ctrl + Shift + B`.

## Step 3: Run the Project

## Command Line Options

The `SampleCrash` application supports the following command line options:

- `--help`: Displays the help message with the list of available options.
- `--install`: Installs the local dump folder for the application.
               If the application is started with administrator privileges, install the local dump folder for all users, otherwise, install the local dump folder for the current user.
- `--uninstall`: Uninstalls the local dump folder for the application.
                If the application is started with administrator privileges, install the local dump folder for all users, otherwise, install the local dump folder for the current user.
- `--info`: Displays information about the local dump configuration for current user. and all users.
- `--monitor` : Monitor the registry for changes, create and input file to the folder C:\Users\<Username>\AppData\Roaming\temp\localdump\registry.log
- `--crash`: Crashes the application intentionally for testing purposes, produce the memory dump file on the folder C:\Users\<Username>\AppData\Roaming\temp\localdump


Example usage:
```powershell
SampleCrash.exe --install
SampleCrash.exe --info
SampleCrash.exe --Crash
SampleCrash.exe --monitor
SampleCrash.exe --uninstall
```