# Installation Instructions

The installation program is made using [NSIS](https://nsis.sourceforge.io/Main_Page), and the documentation refers to the [NSIS Users Manual](https://nsis.sourceforge.io/Docs/).

## EXE Return Code

- 0 - Normal execution (no error)
- 1 - Installation aborted by user (cancel button)
- 2 - Installation aborted by script
- 129 - Another installation is already in progress
- 130 - Application already exists on the device
- 131 - Disk space is full

## Installer Usage

Generated installers and uninstallers accept a few options on the command line. These options give the user a bit more control over the installation process.

### Common Options

- /S Runs the installer or uninstaller silently.
- /D Sets the default installation directory ($INSTDIR), overriding InstallDir and InstallDirRegKey. It must be the last parameter used in the command line and must not contain any quotes, even if the path contains spaces. Only absolute paths are supported.
- /NoShortCut Do not create a desktop shortcut.
- /NoStartMenu Do not add a start menu.

### Uninstaller Specific Options

- _?= sets $INSTDIR. It also stops the uninstaller from copying itself to the temporary directory and running from there. It can be used along with ExecWait to wait for the uninstaller to finish. It must be the last parameter used in the command line and must not contain any quotes, even if the path contains spaces.

### Examples

`setup.exe /S`

`setup.exe /S /D=C:\Program Files (x86)\DeepLink`

`setup.exe /S /NoShortCut /NoStartMenu`

`uninst.exe /S _?=C:\Program Files (x86)\DeepLink`
