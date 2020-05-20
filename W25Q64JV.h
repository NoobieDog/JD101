#ifndef _W25Q64JV_H
#define _W25Q64JV_H

#define WRITE_ENABLE              0x06
#define VOLATILE_SR_WRITE_ENABLE  0x50
#define WRITE_DISABLE             0x04
#define RELEASE_PWR_DWN_ID        0xAB
#define MANUFACT_DEVICE_ID        0x90
#define JEDEC_ID                  0x9F
#define READ_UNIQUE_ID            0x4B
#define READ_DATA                 0x03
#define FAST_READ                 0x0B
#define PAGE_PROGRAM              0x02
#define SECTOR_ERASE_4KB          0x20
#define BLOCK_ERASE_32KB          0x52
#define BLOCK_ERASE_64KB          0xD8
#define CHIP_ERASE                0xC7
#define READ_STATUS_REG_1         0x05
#define WRITE_STATUS_REG_1        0x01
#define READ_STATUS_REG_2         0x35
#define WRITE_STATUS_REG_2        0x31
#define READ_STATUS_REG_3         0x15
#define WRITE_STATUS_REG_3        0x11
#define READ_SFDP_REG             ??
#define ERASE_SECURITY_REG        0x44
#define PROGRAM_SECURITY_REG      0x42
#define READ_SECURITY_REG         0x48
#define GLOBAL_BLOCK_LOCK         0x7E
#define GLOBAL_BLOCK_UNLOCK       0x98
#define READ_BLOCK_LOCK           0x3D
#define INDIVIDUAL_BLOCK_LOCK     0x36
#define INDIVIDUAL_BLOCK_UNLOCK   0x39
#define ERASE_PROGRAM_SUSPEND     0x75
#define ERASE_PROGRAM_RESUME      0x7A
#define POWER_DOWN                0xB9
#define ENABLE_RESET              0x66
#define RESET_DEVICE              0x99
#define STATUS_REG_1_BUSY_BIT   (1<<0)

#endif // _W25Q64JV_H
