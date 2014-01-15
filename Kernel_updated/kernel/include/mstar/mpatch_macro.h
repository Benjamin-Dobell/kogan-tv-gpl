//=========================== Module:PLATFORM=======================================
#ifdef CONFIG_MP_PLATFORM_ARCH_GENERAL
#define MP_PLATFORM_ARCH_GENERAL 1
#else
#define MP_PLATFORM_ARCH_GENERAL 0
#endif

#ifdef CONFIG_MP_PLATFORM_ARCH_GENERAL_DEBUG
#define MP_PLATFORM_ARCH_GENERAL_DEBUG 1
#else
#define MP_PLATFORM_ARCH_GENERAL_DEBUG 0
#endif


#ifdef CONFIG_MP_PLATFORM_ARM
#define MP_PLATFORM_ARM 1
#else
#define MP_PLATFORM_ARM 0
#endif

#ifdef CONFIG_MP_PLATFORM_ARM_DEBUG
#define MP_PLATFORM_ARM_DEBUG 1
#else
#define MP_PLATFORM_ARM_DEBUG 0
#endif


#ifdef CONFIG_MP_PLATFORM_MIPS
#define MP_PLATFORM_MIPS 1
#else
#define MP_PLATFORM_MIPS 0
#endif

#ifdef CONFIG_MP_PLATFORM_MIPS_DEBUG
#define MP_PLATFORM_MIPS_DEBUG 1
#else
#define MP_PLATFORM_MIPS_DEBUG 0
#endif


#ifdef CONFIG_MP_PLATFORM_FIXME
#define MP_PLATFORM_FIXME 1
#else
#define MP_PLATFORM_FIXME 0
#endif

#ifdef CONFIG_MP_PLATFORM_FIXME_DEBUG
#define MP_PLATFORM_FIXME_DEBUG 1
#else
#define MP_PLATFORM_FIXME_DEBUG 0
#endif


#ifdef CONFIG_MP_PLATFORM_ARM_PMU
#define MP_PLATFORM_ARM_PMU 1
#else
#define MP_PLATFORM_ARM_PMU 0
#endif

#ifdef CONFIG_MP_PLATFORM_ARM_PMU_DEBUG
#define MP_PLATFORM_ARM_PMU_DEBUG 1
#else
#define MP_PLATFORM_ARM_PMU_DEBUG 0
#endif


#ifdef CONFIG_MP_PLATFORM_PM
#define MP_PLATFORM_PM 1
#else
#define MP_PLATFORM_PM 0
#endif

#ifdef CONFIG_MP_PLATFORM_PM_DEBUG
#define MP_PLATFORM_PM_DEBUG 1
#else
#define MP_PLATFORM_PM_DEBUG 0
#endif


#ifdef CONFIG_MP_PLATFORM_ARM_ERRATA_775420
#define MP_PLATFORM_ARM_ERRATA_775420 1
#else
#define MP_PLATFORM_ARM_ERRATA_775420 0
#endif

#ifdef CONFIG_MP_PLATFORM_ARM_ERRATA_775420_DEBUG
#define MP_PLATFORM_ARM_ERRATA_775420_DEBUG 1
#else
#define MP_PLATFORM_ARM_ERRATA_775420_DEBUG 0
#endif


#ifdef CONFIG_MP_PLATFORM_MSTAR_LEGANCY_INTR
#define MP_PLATFORM_MSTAR_LEGANCY_INTR 1
#else
#define MP_PLATFORM_MSTAR_LEGANCY_INTR 0
#endif

#ifdef CONFIG_MP_PLATFORM_MSTAR_LEGANCY_INTR_DEBUG
#define MP_PLATFORM_MSTAR_LEGANCY_INTR_DEBUG 1
#else
#define MP_PLATFORM_MSTAR_LEGANCY_INTR_DEBUG 0
#endif


#ifdef CONFIG_MP_PLATFORM_SW_PATCH_L2_SRAM_RMA
#define MP_PLATFORM_SW_PATCH_L2_SRAM_RMA 1
#else
#define MP_PLATFORM_SW_PATCH_L2_SRAM_RMA 0
#endif

#ifdef CONFIG_MP_PLATFORM_SW_PATCH_L2_SRAM_RMA_DEBUG
#define MP_PLATFORM_SW_PATCH_L2_SRAM_RMA_DEBUG 1
#else
#define MP_PLATFORM_SW_PATCH_L2_SRAM_RMA_DEBUG 0
#endif


//=========================== Module:NAND=======================================
#ifdef CONFIG_MP_NAND_UBI
#define MP_NAND_UBI 1
#else
#define MP_NAND_UBI 0
#endif

#ifdef CONFIG_MP_NAND_UBI_DEBUG
#define MP_NAND_UBI_DEBUG 1
#else
#define MP_NAND_UBI_DEBUG 0
#endif


#ifdef CONFIG_MP_NAND_MTD
#define MP_NAND_MTD 1
#else
#define MP_NAND_MTD 0
#endif

#ifdef CONFIG_MP_NAND_MTD_DEBUG
#define MP_NAND_MTD_DEBUG 1
#else
#define MP_NAND_MTD_DEBUG 0
#endif


#ifdef CONFIG_MP_NAND_UBIFS
#define MP_NAND_UBIFS 1
#else
#define MP_NAND_UBIFS 0
#endif

#ifdef CONFIG_MP_NAND_UBIFS_DEBUG
#define MP_NAND_UBIFS_DEBUG 1
#else
#define MP_NAND_UBIFS_DEBUG 0
#endif


#ifdef CONFIG_MP_NAND_Spansion
#define MP_NAND_Spansion 1
#else
#define MP_NAND_Spansion 0
#endif

#ifdef CONFIG_MP_NAND_Spansion_DEBUG
#define MP_NAND_Spansion_DEBUG 1
#else
#define MP_NAND_Spansion_DEBUG 0
#endif


#ifdef CONFIG_MP_NAND_BBT
#define MP_NAND_BBT 1
#else
#define MP_NAND_BBT 0
#endif

#ifdef CONFIG_MP_NAND_BBT_DEBUG
#define MP_NAND_BBT_DEBUG 1
#else
#define MP_NAND_BBT_DEBUG 0
#endif


#ifdef CONFIG_MP_NAND_BBT_SIZE
#define MP_NAND_BBT_SIZE 1
#else
#define MP_NAND_BBT_SIZE 0
#endif

#ifdef CONFIG_MP_NAND_BBT_SIZE_DEBUG
#define MP_NAND_BBT_SIZE_DEBUG 1
#else
#define MP_NAND_BBT_SIZE_DEBUG 0
#endif


//=========================== Module:USB=======================================
#ifdef CONFIG_MP_USB_MSTAR
#define MP_USB_MSTAR 1
#else
#define MP_USB_MSTAR 0
#endif

#ifdef CONFIG_MP_USB_MSTAR_DEBUG
#define MP_USB_MSTAR_DEBUG 1
#else
#define MP_USB_MSTAR_DEBUG 0
#endif


//=========================== Module:SD=======================================
#ifdef CONFIG_MP_SD_MSTAR
#define MP_SD_MSTAR 1
#else
#define MP_SD_MSTAR 0
#endif

#ifdef CONFIG_MP_SD_MSTAR_DEBUG
#define MP_SD_MSTAR_DEBUG 1
#else
#define MP_SD_MSTAR_DEBUG 0
#endif


#ifdef CONFIG_MP_SD_PLUG
#define MP_SD_PLUG 1
#else
#define MP_SD_PLUG 0
#endif

#ifdef CONFIG_MP_SD_PLUG_DEBUG
#define MP_SD_PLUG_DEBUG 1
#else
#define MP_SD_PLUG_DEBUG 0
#endif


//=========================== Module:EMMC=======================================
#ifdef CONFIG_MP_EMMC_PARTITION
#define MP_EMMC_PARTITION 1
#else
#define MP_EMMC_PARTITION 0
#endif

#ifdef CONFIG_MP_EMMC_PARTITION_DEBUG
#define MP_EMMC_PARTITION_DEBUG 1
#else
#define MP_EMMC_PARTITION_DEBUG 0
#endif


//=========================== Module:MSTAR_STR=======================================
#ifdef CONFIG_MP_MSTAR_STR_BASE
#define MP_MSTAR_STR_BASE 1
#else
#define MP_MSTAR_STR_BASE 0
#endif

#ifdef CONFIG_MP_MSTAR_STR_BASE_DEBUG
#define MP_MSTAR_STR_BASE_DEBUG 1
#else
#define MP_MSTAR_STR_BASE_DEBUG 0
#endif


//=========================== Module:Android=======================================
#ifdef CONFIG_MP_Android_LOW_MEM_KILLER_INCLUDE_CACHEMEM
#define MP_Android_LOW_MEM_KILLER_INCLUDE_CACHEMEM 1
#else
#define MP_Android_LOW_MEM_KILLER_INCLUDE_CACHEMEM 0
#endif

#ifdef CONFIG_MP_Android_LOW_MEM_KILLER_INCLUDE_CACHEMEM_DEBUG
#define MP_Android_LOW_MEM_KILLER_INCLUDE_CACHEMEM_DEBUG 1
#else
#define MP_Android_LOW_MEM_KILLER_INCLUDE_CACHEMEM_DEBUG 0
#endif


#ifdef CONFIG_MP_Android_DUMMY_MSTAR_RTC
#define MP_Android_DUMMY_MSTAR_RTC 1
#else
#define MP_Android_DUMMY_MSTAR_RTC 0
#endif

#ifdef CONFIG_MP_Android_DUMMY_MSTAR_RTC_DEBUG
#define MP_Android_DUMMY_MSTAR_RTC_DEBUG 1
#else
#define MP_Android_DUMMY_MSTAR_RTC_DEBUG 0
#endif


#ifdef CONFIG_MP_Android_ALARM_CLOCK_BOOTTIME_PATCH
#define MP_Android_ALARM_CLOCK_BOOTTIME_PATCH 1
#else
#define MP_Android_ALARM_CLOCK_BOOTTIME_PATCH 0
#endif

#ifdef CONFIG_MP_Android_ALARM_CLOCK_BOOTTIME_PATCH_DEBUG
#define MP_Android_ALARM_CLOCK_BOOTTIME_PATCH_DEBUG 1
#else
#define MP_Android_ALARM_CLOCK_BOOTTIME_PATCH_DEBUG 0
#endif


#ifdef CONFIG_MP_Android_MSTAR_RC_MAP_DEFINE
#define MP_Android_MSTAR_RC_MAP_DEFINE 1
#else
#define MP_Android_MSTAR_RC_MAP_DEFINE 0
#endif

#ifdef CONFIG_MP_Android_MSTAR_RC_MAP_DEFINE_DEBUG
#define MP_Android_MSTAR_RC_MAP_DEFINE_DEBUG 1
#else
#define MP_Android_MSTAR_RC_MAP_DEFINE_DEBUG 0
#endif


#ifdef CONFIG_MP_Android_COREDUMP_PATH
#define MP_Android_COREDUMP_PATH 1
#else
#define MP_Android_COREDUMP_PATH 0
#endif

#ifdef CONFIG_MP_Android_COREDUMP_PATH_DEBUG
#define MP_Android_COREDUMP_PATH_DEBUG 1
#else
#define MP_Android_COREDUMP_PATH_DEBUG 0
#endif


//=========================== Module:COMPILER=======================================
#ifdef CONFIG_MP_COMPILER_ERROR
#define MP_COMPILER_ERROR 1
#else
#define MP_COMPILER_ERROR 0
#endif

#ifdef CONFIG_MP_COMPILER_ERROR_DEBUG
#define MP_COMPILER_ERROR_DEBUG 1
#else
#define MP_COMPILER_ERROR_DEBUG 0
#endif


//=========================== Module:DEBUG_TOOL=======================================
#ifdef CONFIG_MP_DEBUG_TOOL_WATCHDOG
#define MP_DEBUG_TOOL_WATCHDOG 1
#else
#define MP_DEBUG_TOOL_WATCHDOG 0
#endif

#ifdef CONFIG_MP_DEBUG_TOOL_WATCHDOG_DEBUG
#define MP_DEBUG_TOOL_WATCHDOG_DEBUG 1
#else
#define MP_DEBUG_TOOL_WATCHDOG_DEBUG 0
#endif


#ifdef CONFIG_MP_DEBUG_TOOL_CODEDUMP
#define MP_DEBUG_TOOL_CODEDUMP 1
#else
#define MP_DEBUG_TOOL_CODEDUMP 0
#endif

#ifdef CONFIG_MP_DEBUG_TOOL_CODEDUMP_DEBUG
#define MP_DEBUG_TOOL_CODEDUMP_DEBUG 1
#else
#define MP_DEBUG_TOOL_CODEDUMP_DEBUG 0
#endif


#ifdef CONFIG_MP_DEBUG_TOOL_KDEBUG
#define MP_DEBUG_TOOL_KDEBUG 1
#else
#define MP_DEBUG_TOOL_KDEBUG 0
#endif

#ifdef CONFIG_MP_DEBUG_TOOL_KDEBUG_DEBUG
#define MP_DEBUG_TOOL_KDEBUG_DEBUG 1
#else
#define MP_DEBUG_TOOL_KDEBUG_DEBUG 0
#endif


#ifdef CONFIG_MP_DEBUG_TOOL_CHANGELIST
#define MP_DEBUG_TOOL_CHANGELIST 1
#else
#define MP_DEBUG_TOOL_CHANGELIST 0
#endif

#ifdef CONFIG_MP_DEBUG_TOOL_CHANGELIST_DEBUG
#define MP_DEBUG_TOOL_CHANGELIST_DEBUG 1
#else
#define MP_DEBUG_TOOL_CHANGELIST_DEBUG 0
#endif


//=========================== Module:REMOTE_CONTROL=======================================
#ifdef CONFIG_MP_REMOTE_CONTROL_RC_REGISTER_RETRY
#define MP_REMOTE_CONTROL_RC_REGISTER_RETRY 1
#else
#define MP_REMOTE_CONTROL_RC_REGISTER_RETRY 0
#endif

#ifdef CONFIG_MP_REMOTE_CONTROL_RC_REGISTER_RETRY_DEBUG
#define MP_REMOTE_CONTROL_RC_REGISTER_RETRY_DEBUG 1
#else
#define MP_REMOTE_CONTROL_RC_REGISTER_RETRY_DEBUG 0
#endif


//=========================== Module:SCSI=======================================
#ifdef CONFIG_MP_SCSI_MSTAR_SD_CARD_HOTPLUG
#define MP_SCSI_MSTAR_SD_CARD_HOTPLUG 1
#else
#define MP_SCSI_MSTAR_SD_CARD_HOTPLUG 0
#endif

#ifdef CONFIG_MP_SCSI_MSTAR_SD_CARD_HOTPLUG_DEBUG
#define MP_SCSI_MSTAR_SD_CARD_HOTPLUG_DEBUG 1
#else
#define MP_SCSI_MSTAR_SD_CARD_HOTPLUG_DEBUG 0
#endif


//=========================== Module:MM=======================================
#ifdef CONFIG_MP_MM_MSTAR_3DALLOC_MIU0_1
#define MP_MM_MSTAR_3DALLOC_MIU0_1 1
#else
#define MP_MM_MSTAR_3DALLOC_MIU0_1 0
#endif

#ifdef CONFIG_MP_MM_MSTAR_3DALLOC_MIU0_1_DEBUG
#define MP_MM_MSTAR_3DALLOC_MIU0_1_DEBUG 1
#else
#define MP_MM_MSTAR_3DALLOC_MIU0_1_DEBUG 0
#endif


#ifdef CONFIG_MP_MM_MALI_MMU_NOTIFIER
#define MP_MM_MALI_MMU_NOTIFIER 1
#else
#define MP_MM_MALI_MMU_NOTIFIER 0
#endif

#ifdef CONFIG_MP_MM_MALI_MMU_NOTIFIER_DEBUG
#define MP_MM_MALI_MMU_NOTIFIER_DEBUG 1
#else
#define MP_MM_MALI_MMU_NOTIFIER_DEBUG 0
#endif


//=========================== Module:MMAP=======================================
#ifdef CONFIG_MP_MMAP_BUFFERABLE
#define MP_MMAP_BUFFERABLE 1
#else
#define MP_MMAP_BUFFERABLE 0
#endif

#ifdef CONFIG_MP_MMAP_BUFFERABLE_DEBUG
#define MP_MMAP_BUFFERABLE_DEBUG 1
#else
#define MP_MMAP_BUFFERABLE_DEBUG 0
#endif


//=========================== Module:JIFFIES=======================================
#ifdef CONFIG_MP_JIFFIES_PRINT
#define MP_JIFFIES_PRINT 1
#else
#define MP_JIFFIES_PRINT 0
#endif

#ifdef CONFIG_MP_JIFFIES_PRINT_DEBUG
#define MP_JIFFIES_PRINT_DEBUG 1
#else
#define MP_JIFFIES_PRINT_DEBUG 0
#endif


//=========================== Module:MIU=======================================
#ifdef CONFIG_MP_MIU_MAPPING
#define MP_MIU_MAPPING 1
#else
#define MP_MIU_MAPPING 0
#endif

#ifdef CONFIG_MP_MIU_MAPPING_DEBUG
#define MP_MIU_MAPPING_DEBUG 1
#else
#define MP_MIU_MAPPING_DEBUG 0
#endif


//=========================== Module:MIPS_L2=======================================
#ifdef CONFIG_MP_MIPS_L2_CATCH
#define MP_MIPS_L2_CATCH 1
#else
#define MP_MIPS_L2_CATCH 0
#endif

#ifdef CONFIG_MP_MIPS_L2_CATCH_DEBUG
#define MP_MIPS_L2_CATCH_DEBUG 1
#else
#define MP_MIPS_L2_CATCH_DEBUG 0
#endif


//=========================== Module:WDT=======================================
#ifdef CONFIG_MP_WDT_OFF_DBG
#define MP_WDT_OFF_DBG 1
#else
#define MP_WDT_OFF_DBG 0
#endif

#ifdef CONFIG_MP_WDT_OFF_DBG_DEBUG
#define MP_WDT_OFF_DBG_DEBUG 1
#else
#define MP_WDT_OFF_DBG_DEBUG 0
#endif


//=========================== Module:CAMERA=======================================
#ifdef CONFIG_MP_CAMERA_PLUG_OUT
#define MP_CAMERA_PLUG_OUT 1
#else
#define MP_CAMERA_PLUG_OUT 0
#endif

#ifdef CONFIG_MP_CAMERA_PLUG_OUT_DEBUG
#define MP_CAMERA_PLUG_OUT_DEBUG 1
#else
#define MP_CAMERA_PLUG_OUT_DEBUG 0
#endif


//=========================== Module:SYSATTR=======================================
#ifdef CONFIG_MP_SYSATTR_SHOW
#define MP_SYSATTR_SHOW 1
#else
#define MP_SYSATTR_SHOW 0
#endif

#ifdef CONFIG_MP_SYSATTR_SHOW_DEBUG
#define MP_SYSATTR_SHOW_DEBUG 1
#else
#define MP_SYSATTR_SHOW_DEBUG 0
#endif


//=========================== Module:MIPS_HIGHMEM=======================================
#ifdef CONFIG_MP_MIPS_HIGHMEM_CACHE_ALIAS_PATCH
#define MP_MIPS_HIGHMEM_CACHE_ALIAS_PATCH 1
#else
#define MP_MIPS_HIGHMEM_CACHE_ALIAS_PATCH 0
#endif

#ifdef CONFIG_MP_MIPS_HIGHMEM_CACHE_ALIAS_PATCH_DEBUG
#define MP_MIPS_HIGHMEM_CACHE_ALIAS_PATCH_DEBUG 1
#else
#define MP_MIPS_HIGHMEM_CACHE_ALIAS_PATCH_DEBUG 0
#endif


//=========================== Module:CHECKPT=======================================
#ifdef CONFIG_MP_CHECKPT_BOOT
#define MP_CHECKPT_BOOT 1
#else
#define MP_CHECKPT_BOOT 0
#endif

#ifdef CONFIG_MP_CHECKPT_BOOT_DEBUG
#define MP_CHECKPT_BOOT_DEBUG 1
#else
#define MP_CHECKPT_BOOT_DEBUG 0
#endif


//=========================== Module:CORE=======================================
#ifdef CONFIG_MP_CORE_PATH
#define MP_CORE_PATH 1
#else
#define MP_CORE_PATH 0
#endif

#ifdef CONFIG_MP_CORE_PATH_DEBUG
#define MP_CORE_PATH_DEBUG 1
#else
#define MP_CORE_PATH_DEBUG 0
#endif


//=========================== Module:WEBCAM=======================================
#ifdef CONFIG_MP_WEBCAM_INIT
#define MP_WEBCAM_INIT 1
#else
#define MP_WEBCAM_INIT 0
#endif

#ifdef CONFIG_MP_WEBCAM_INIT_DEBUG
#define MP_WEBCAM_INIT_DEBUG 1
#else
#define MP_WEBCAM_INIT_DEBUG 0
#endif


//=========================== Module:BDI=======================================
#ifdef CONFIG_MP_BDI_MSTAR_BDI_PATCH
#define MP_BDI_MSTAR_BDI_PATCH 1
#else
#define MP_BDI_MSTAR_BDI_PATCH 0
#endif

#ifdef CONFIG_MP_BDI_MSTAR_BDI_PATCH_DEBUG
#define MP_BDI_MSTAR_BDI_PATCH_DEBUG 1
#else
#define MP_BDI_MSTAR_BDI_PATCH_DEBUG 0
#endif


//=========================== Module:NTFS=======================================
#ifdef CONFIG_MP_NTFS_IOCTL
#define MP_NTFS_IOCTL 1
#else
#define MP_NTFS_IOCTL 0
#endif

#ifdef CONFIG_MP_NTFS_IOCTL_DEBUG
#define MP_NTFS_IOCTL_DEBUG 1
#else
#define MP_NTFS_IOCTL_DEBUG 0
#endif


#ifdef CONFIG_MP_NTFS_VOLUME_LABEL
#define MP_NTFS_VOLUME_LABEL 1
#else
#define MP_NTFS_VOLUME_LABEL 0
#endif

#ifdef CONFIG_MP_NTFS_VOLUME_LABEL_DEBUG
#define MP_NTFS_VOLUME_LABEL_DEBUG 1
#else
#define MP_NTFS_VOLUME_LABEL_DEBUG 0
#endif


#ifdef CONFIG_MP_NTFS_VOLUME_ID
#define MP_NTFS_VOLUME_ID 1
#else
#define MP_NTFS_VOLUME_ID 0
#endif

#ifdef CONFIG_MP_NTFS_VOLUME_ID_DEBUG
#define MP_NTFS_VOLUME_ID_DEBUG 1
#else
#define MP_NTFS_VOLUME_ID_DEBUG 0
#endif


//=========================== Module:USB=======================================
#ifdef CONFIG_MP_USB_STR_PATCH
#define MP_USB_STR_PATCH 1
#else
#define MP_USB_STR_PATCH 0
#endif

#ifdef CONFIG_MP_USB_STR_PATCH_DEBUG
#define MP_USB_STR_PATCH_DEBUG 1
#else
#define MP_USB_STR_PATCH_DEBUG 0
#endif


