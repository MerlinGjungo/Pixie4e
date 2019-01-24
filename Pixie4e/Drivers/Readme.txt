Driver installation:

1. The file "Pixie.xop" needs to be copied from "x86" or "amd64"
into the following directory: 
C:\Program Files\WaveMetrics\Igor Pro Folder\Igor Extensions

This links the C driver library to the Igor user interface.

Note: All .xop files are loaded at Igor startup. If the same function is
defined in multiple xops, it is not clear which one is actually in use. 
Therefore, when temporarily saving an older version of Pixie.xop, the
_extension_ must be renamed, e.g. into pixie.xopsave, not pixiesave.xop. 


2. Device drivers for the  Pixie-4 PCI interface chip are the following files:
- PLXSDK.INF
- x86\PLX9054.SYS  and amd64\PLX9054.SYS
- PLXAPI650.dll

When Windows detects Pixie-4 or Pixie-500 modules as new hardware, point it to the above 
.INF file. The windows installation process should then copy the .sys and .dll
file into Windows\System32 (32 bit) or Windows\SysWOW64 (64 bit). The installation process
should recognize 32 vs 64 bit Windows and pick the .sys file from the correct folder. 
If not, copy them manually.

3. The file Pixie4DLL.dll in "x86", LV64, or "amd64" is used by custom user programs accessing 
the API, for example a LabVIEW interface. There is no need to copy this file anywhere 
unless such a program is used. For LabVIEW, it can reside in the same folder as the VI. 

4. Manual installation of WinDriver for PCIe (Pixie-500 Express and Pixie-4 Express)
All the needed files are in the "x86" or "amd64" folders. Choose the appropriate
version for the PC's operating system: The files are
windrvr6.sys, windrvr6.inf, wd1120.cat, wdapi1120.dll, 
wdreg.exe, difxapi.dll, and Pixie500e.inf.
It is recommended to use the included wdreg.exe utility (which depends on 
difxapi.dll) to install the kernel module and the device-specific driver.
(The script should be executed from command prompt running as Administrator)

a) To install the kernel module. In the directory containing windrvr6.sys, 
windrvr6.inf, and wd1120.cat, execute
wdreg -inf windrvr6.inf install
b) To install the INF file for Pixie-500 Express and Pixie-4 Express 
(register the Plug-and-Play device with windrvr6.sys), execute
wdreg -inf Pixie500e.inf install
To pre-install the device on the computer not currently connected to Pixie500e, 
execute
wdreg -inf Pixie500e.inf preinstall
(If the installation fails with an ERROR_FILE_NOT_FOUND error, inspect the 
Windows registry to see if the RunOnce key exists in 
HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion. The registry key 
is required by Windows Plug-and-Play in order to properly install drivers using 
INF files. If the RunOnce key is missing, create it; then try installing the INF 
file again.)
c) To install wdapi1120.dll, copy it from the appropriate folder 
to %windir%\system32 (32 bit) or to %windir%\sysWOW64 (64 bit).

5. To uninstall drivers:
Uninstall.exe will remove WinDriver from your system, as well as the 
WinDriver API library and pixie.xop.
To manually unstall drivers, use wdreg.exe utility and execute
wdreg -inf Pixie500e.inf uninstall
wdreg -inf windrvr6.inf unintall
Confirm that windrvr6.sys not present anymore in %windir%\System32\drivers directory, 
as well as windrvr6.inf* directory is removed from 
%windir%\System32\DriverStore\FileRepository. If uninstaller 
failed to remove them, delete those items manually.

6. LabVIEW DLLs:
In all cases, begin by running installer to copy files and 
install I/O drivers. Then, depending on variants:
a) 32bit LabVIEW on 32bit Windows
- copy Drivers\PlxApi650.dll to Windows\System         (should be already copied)
- copy Drivers\x86\wdapi1120.dll to Windows\System32   (should be already copied)
- link LabVIEW VIs to Drivers\x86\Pixie4DLL.dll        (recommend copy to LabVIEW folder)
b) 32bit LabVIEW on 64bit Windows
- copy Drivers\PlxApi650.dll to Windows\System         (should be already copied)
- copy Drivers\amd64\wdapi1120.dll to Windows\System32 (should be already copied)
- link LabVIEW VIs to Drivers\amd64\Pixie4DLL.dll      (recommend copy to LabVIEW folder)
c) 64bit LabVIEW on 64bit Windows
- copy Drivers\LV64\PlxApi650_x64.dll to Windows\System 
- copy Drivers\LV64\wdapi1120_6464.dll to Windows\System32 and rename to wdapi1120.dll
- link LabVIEW VIs to Drivers\LV64\Pixie4DLL.dll       (recommend copy to LabVIEW folder)

