# File System Implementation

## Overview

A project exposing information via Linux `/proc` virtual filesystem and implementing an in-memory filesystem based on FAT structure.

## Sub-projects

### 01_Proc_FS_Process_Info
Adding custom entries to the `/proc` filesystem

- Creating `/proc/[name]` file using kernel module
- Returning process information (PID, state, parent, etc.) upon file read

### 02_FAT_File_System
In-memory filesystem implementation based on FAT

- **Structure**: Directory Table + FAT Table + Data Blocks
- **Block Size**: 32 bytes, **Block Count**: 1024 (Total 32KB)
- Supports up to 100 files

Supported Commands:

| Command | Description |
|---------|-------------|
| `create <name>` | Create new file |
| `write <name> "<data>"` | Write data to file |
| `read <name>` | Read file content |
| `delete <name>` | Delete file |
| `list` | List all files |

- Auto-save to `kwufs.img` on exit, auto-load on start (Persistence)

## File Structure

```
05_File_System_Implementation/
├── 01_Proc_FS_Process_Info/
│   ├── proc_info.c
│   └── Makefile
├── 02_FAT_File_System/
│   ├── fat.c               — FAT filesystem implementation
│   └── Makefile
└── Report_FAT_FileSystem_Implementation.pdf
```
