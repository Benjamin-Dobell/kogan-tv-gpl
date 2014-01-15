#ifndef TARGET_CORE_IBLOCK_H
#define TARGET_CORE_IBLOCK_H

#define IBLOCK_VERSION		"4.0"

#define IBLOCK_MAX_CDBS		16
#define IBLOCK_LBA_SHIFT	9

struct iblock_req {
	struct se_task ib_task;
	unsigned char ib_scsi_cdb[TCM_MAX_COMMAND_SIZE];
	atomic_t ib_bio_cnt;
	atomic_t ib_bio_err_cnt;
	struct bio *ib_bio;
} ____cacheline_aligned;

#define IBDF_HAS_UDEV_PATH		0x01

struct iblock_dev {
	unsigned char ibd_udev_path[SE_UDEV_PATH_LEN];
	u32	ibd_flags;
	struct bio_set	*ibd_bio_set;
	struct block_device *ibd_bd;
	struct iblock_hba *ibd_host;
} ____cacheline_aligned;

struct iblock_hba {
	int		iblock_host_id;
} ____cacheline_aligned;

#endif /* TARGET_CORE_IBLOCK_H */
