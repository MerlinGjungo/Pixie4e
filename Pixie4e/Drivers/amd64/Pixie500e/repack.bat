:: Update the catalog file from the .inf and sign it.
::
:: You should manually update the DriverVer date to accommodate manual Device
:: Manager update detection. I suppose there's no reason to bump DriverVer
:: unless the kernel module changes.
::
:: Requires the Windows Driver Kit installed.
::
:: Requires the environment variable XIA_CERT_DIR set to the directory containing
:: the XIA code signing certificate and DigiCert cross-certificate.
::
:: Requires the environment variable XIA_CERT_PASS set to the password on the
:: code signing certificate.
::
:: You may have to install the DigiCert Root CA. The docs say you can't use a .pfx
:: file for code signing with a cross-certificate; so far it seems to work, but you
:: may have to install the XIA cert, too.
::
:: MSDN Kernel-Mode Code Signing Walkthrough:
:: https://msdn.microsoft.com/en-us/library/windows/hardware/Dn653569(v=VS.85).aspx

@setlocal

@set catalog=pixie500e.cat

@if not defined XIA_CERT_DIR (
  @echo Variable XIA_CERT_DIR is required.
  @exit /b 1
)

@if not defined XIA_CERT_PASS (
  @echo Variable XIA_CERT_PASS is required.
  @exit /b 1
)

:: For inf2cat and signtool
@set PATH=%PATH%;%ProgramFiles(x86)%\Windows Kits\10\bin\x86

:: List all the x64 operating systems
@set os=10_x64,8_x64,7_x64,vista_x64,6_3_x64,server6_3_x64,server8_x64,server2008r2_x64,server2008_x64,server2003_x64,xp_x64

inf2cat /os:%os% /driver:.
@if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

:: /ac CROSS_CERT is critical for driver installation on offline systems.
:: It comes from https://www.digicert.com/digicert-root-certificates.htm.
signtool sign /p %XIA_CERT_PASS% /f "%XIA_CERT_DIR%\xia_digicert_2018.pfx" /ac "%XIA_CERT_DIR%\DigiCert Assured ID Root CA.crt" /d "XIA USB2 Driver" /du "http://www.xia.com" /t http://timestamp.digicert.com /v %catalog%

@if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

@endlocal
