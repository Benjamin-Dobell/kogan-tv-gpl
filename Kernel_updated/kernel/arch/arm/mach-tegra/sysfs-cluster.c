/*
 * Copyright (c) 2010-2011 NVIDIA Corporation.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of the NVIDIA Corporation nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * This driver creates the /sys/kernel/cluster node and attributes for CPU
 * switch testing. Node attributes:
 *
 * active: currently active CPU (G or LP)
 *		write:	'g'	 = switch to G CPU
 *			'lp'	 = switch to LP CPU
 *			'toggle' = switch to the other CPU
 *		read: returns the currently active CPU (g or lp)
 *
 * force: force switch even if already on target CPU
 *		write:	'0' = do not perform switch if
 *			      active CPU == target CPU (default)
 *			'1' = force switch regardless of
 *			      currently active CPU
 *		read: returns the current status of the force flag
 *
 * immediate: request immediate wake-up from switch request
 *		write:	'0' = non-immediate wake-up on next interrupt (default)
 *			'1' = immediate wake-up
 *		read: returns the current status of the immediate flag
 *
 * power_mode: power mode to use for switch (LP1 or LP2)
 *		write:	'1' = use LP1 power mode
 *			'2' = use LP2 power mode (default)
 *		read: returns the current status of the immediate flag
 *
 * wake_ms: wake time (in milliseconds) -- ignored if immediate==1
 *		write:	'0' = wake up at the next non-timer interrupt
 *			'n' = (n > 0) wake-up after 'n' milliseconds or the
 *			      next non-timer interrupt (whichever comes first)
 *		read: returns the current wake_ms value
 *
 * Writing the force, immediate and wake_ms attributes simply updates the
 * state of internal variables that will be used for the next switch request.
 * Writing to the active attribute initates a switch request using the
 * current values of the force, immediate, and wake_ms attributes.
 *
 * The OS tick timer is not a valid interrupt source for waking up following
 * a switch request. This is because the kernel uses local timers that are
 * part of the CPU complex. These get shut down when the CPU complex is
 * placed into reset by the switch request. If you want a timed wake up
 * from a switch, you must specify a positive wake_ms value. This will
 * ensure that a non-local timer is programmed to fire an interrupt
 * after the desired interval.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/smp.h>
#include <linux/io.h>
#include <linux/clk.h>

#include <mach/iomap.h>
#include "clock.h"
#include "sleep.h"
#include "pm.h"

#define SYSFS_CLUSTER_PRINTS	   1	/* Nonzero: enable status prints */
#define SYSFS_CLUSTER_TRACE_PRINTS 0	/* Nonzero: enable trace prints */
#define SYSFS_CLUSTER_POWER_MODE   1	/* Nonzero: use power modes other than LP2*/

#if SYSFS_CLUSTER_TRACE_PRINTS
#define TRACE_CLUSTER(x) printk x
#else
#define TRACE_CLUSTER(x)
#endif

#if SYSFS_CLUSTER_PRINTS
#define PRINT_CLUSTER(x) printk x
#else
#define PRINT_CLUSTER(x)
#endif

static struct kobject *cluster_kobj;
static spinlock_t cluster_lock;
static unsigned int flags = 0;
static unsigned int wake_ms = 0;

static ssize_t sysfscluster_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf);

static ssize_t sysfscluster_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count);

/* Active CPU: "G", "LP", "toggle" */
static struct kobj_attribute cluster_active_attr =
		__ATTR(active, 0640, sysfscluster_show, sysfscluster_store);

/* Immediate wake-up when performing switch: 0, 1 */
static struct kobj_attribute cluster_immediate_attr =
		__ATTR(immediate, 0640, sysfscluster_show, sysfscluster_store);

/* Force power transition even if already on the desired CPU: 0, 1 */
static struct kobj_attribute cluster_force_attr =
		__ATTR(force, 0640, sysfscluster_show, sysfscluster_store);

/* Wake time (in milliseconds) */
static struct kobj_attribute cluster_wake_ms_attr =
		__ATTR(wake_ms, 0640, sysfscluster_show, sysfscluster_store);

#if defined(CONFIG_PM_SLEEP) && SYSFS_CLUSTER_POWER_MODE
/* LPx power mode to use when switching CPUs: 1=LP1, 2=LP2 */
static unsigned int power_mode = 2;
static struct kobj_attribute cluster_powermode_attr =
		__ATTR(power_mode, 0640, sysfscluster_show, sysfscluster_store);
#endif

#if DEBUG_CLUSTER_SWITCH
unsigned int tegra_cluster_debug = 0;
static struct kobj_attribute cluster_debug_attr =
		__ATTR(debug, 0640, sysfscluster_show, sysfscluster_store);
#endif

typedef enum
{
	ClusterAttr_Invalid = 0,
	ClusterAttr_Active,
	ClusterAttr_Immediate,
	ClusterAttr_Force,
	ClusterAttr_WakeMs,
#if defined(CONFIG_PM_SLEEP) && SYSFS_CLUSTER_POWER_MODE
	ClusterAttr_PowerMode,
#endif
#if DEBUG_CLUSTER_SWITCH
	ClusterAttr_Debug
#endif
} ClusterAttr;

static ClusterAttr GetClusterAttr(const char *name)
{
	if (!strcmp(name, "active"))
		return ClusterAttr_Active;
	if (!strcmp(name, "immediate"))
		return ClusterAttr_Immediate;
	if (!strcmp(name, "force"))
		return ClusterAttr_Force;
	if (!strcmp(name, "wake_ms"))
		return ClusterAttr_WakeMs;
#if defined(CONFIG_PM_SLEEP) && SYSFS_CLUSTER_POWER_MODE
	if (!strcmp(name, "power_mode"))
		return ClusterAttr_PowerMode;
#endif
#if DEBUG_CLUSTER_SWITCH
	if (!strcmp(name, "debug"))
		return ClusterAttr_Debug;
#endif
	TRACE_CLUSTER(("GetClusterAttr(%s): invalid\n", name));
	return ClusterAttr_Invalid;
}

static ssize_t sysfscluster_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	ClusterAttr type;
	ssize_t len;

	TRACE_CLUSTER(("+sysfscluster_show\n"));

	type = GetClusterAttr(attr->attr.name);
	switch (type) {
	case ClusterAttr_Active:
		len = sprintf(buf, "%s\n", is_lp_cluster() ? "LP" : "G");
		break;

	case ClusterAttr_Immediate:
		len = sprintf(buf, "%d\n",
			      ((flags & TEGRA_POWER_CLUSTER_IMMEDIATE) != 0));
		break;

	case ClusterAttr_Force:
		len = sprintf(buf, "%d\n",
			      ((flags & TEGRA_POWER_CLUSTER_FORCE) != 0));
		break;

	case ClusterAttr_WakeMs:
		len = sprintf(buf, "%d\n", wake_ms);
		break;

#if defined(CONFIG_PM_SLEEP) && SYSFS_CLUSTER_POWER_MODE
	case ClusterAttr_PowerMode:
		len = sprintf(buf, "%d\n", power_mode);
		break;
#endif

#if DEBUG_CLUSTER_SWITCH
	case ClusterAttr_Debug:
		len = sprintf(buf, "%d\n", tegra_cluster_debug);
		break;
#endif

	default:
		len = sprintf(buf, "invalid\n");
		break;
	}

	TRACE_CLUSTER(("-sysfscluster_show\n"));
	return len;
}

static ssize_t sysfscluster_store(struct kobject *kobj,
	struct kobj_attribute *attr, const char *buf, size_t count)
{
	ClusterAttr type;
	ssize_t ret = count--;
	unsigned request;
	int e;
	int tmp;
	int cnt;
	struct clk *cpu_clk = tegra_get_clock_by_name("cpu");
	struct clk *cpu_g_clk = tegra_get_clock_by_name("cpu_g");
	struct clk *cpu_lp_clk = tegra_get_clock_by_name("cpu_lp");
	struct clk *new_parent = NULL;

	if (!cpu_clk || !cpu_g_clk || !cpu_lp_clk) {
		ret = -ENOSYS;
		goto fail;
	}

	TRACE_CLUSTER(("+sysfscluster_store: %p, %d\n", buf, count));

	/* The count includes data bytes follow by a line feed character. */
	if (!buf || (count < 1)) {
		ret = -EINVAL;
		goto fail;
	}

	type = GetClusterAttr(attr->attr.name);

	spin_lock(&cluster_lock);

	switch (type) {
	case ClusterAttr_Active:
		if (!strncasecmp(buf, "g", count)) {
			flags &= ~TEGRA_POWER_CLUSTER_MASK;
			flags |= TEGRA_POWER_CLUSTER_G;
		} else if (!strncasecmp(buf, "lp", count)) {
			flags &= ~TEGRA_POWER_CLUSTER_MASK;
			flags |= TEGRA_POWER_CLUSTER_LP;
		} else if (!strncasecmp(buf, "toggle", count)) {
			flags &= ~TEGRA_POWER_CLUSTER_MASK;
			if (is_lp_cluster())
				flags |= TEGRA_POWER_CLUSTER_G;
			else
				flags |= TEGRA_POWER_CLUSTER_LP;
		} else {
			PRINT_CLUSTER(("cluster/active: '%*.*s' invalid, "
				" must be g, lp, or toggle\n",
				count, count, buf));
			ret = -EINVAL;
			break;
		}
		PRINT_CLUSTER(("cluster/active -> %s\n",
			(flags & TEGRA_POWER_CLUSTER_G) ? "G" : "LP"));

		request = flags;
#if defined(CONFIG_PM_SLEEP) && SYSFS_CLUSTER_POWER_MODE
		if (power_mode == 1) {
			request |= TEGRA_POWER_SDRAM_SELFREFRESH;
		}
#endif
		tegra_cluster_switch_set_parameters(wake_ms * 1000, request);
		new_parent = (flags & TEGRA_POWER_CLUSTER_LP) ?
			cpu_lp_clk : cpu_g_clk;
		break;

	case ClusterAttr_Immediate:
		if ((count == 1) && (*buf == '0'))
			flags &= ~TEGRA_POWER_CLUSTER_IMMEDIATE;
		else if ((count == 1) && *buf == '1')
			flags |= TEGRA_POWER_CLUSTER_IMMEDIATE;
		else {
			PRINT_CLUSTER(("cluster/immediate: '%*.*s' invalid, "
				"must be 0 or 1\n", count, count, buf));
			ret = -EINVAL;
			break;
		}
		PRINT_CLUSTER(("cluster/immediate -> %c\n",
			(flags & TEGRA_POWER_CLUSTER_IMMEDIATE) ? '1' : '0'));
		break;

	case ClusterAttr_Force:
		if ((count == 1) && (*buf == '0'))
			flags &= ~TEGRA_POWER_CLUSTER_FORCE;
		else if ((count == 1) && (*buf == '1'))
			flags |= TEGRA_POWER_CLUSTER_FORCE;
		else {
			PRINT_CLUSTER(("cluster/force: '%*.*s' invalid, "
				"must be 0 or 1\n", count, count, buf));
			ret = -EINVAL;
			break;
		}
		PRINT_CLUSTER(("cluster/force -> %c\n",
			(flags & TEGRA_POWER_CLUSTER_FORCE) ? '1' : '0'));
		break;

	case ClusterAttr_WakeMs:
		tmp = 0;
		cnt = sscanf(buf, "%d\n", &tmp);
		if ((cnt != 1) || (tmp < 0)) {
			PRINT_CLUSTER(("cluster/wake_ms: '%*.*s' is invalid\n",
				count, count, buf));
			ret = -EINVAL;
			break;
		}
		wake_ms = tmp;
		PRINT_CLUSTER(("cluster/wake_ms -> %d\n", wake_ms));
		break;

#if defined(CONFIG_PM_SLEEP) && SYSFS_CLUSTER_POWER_MODE
	case ClusterAttr_PowerMode:
		if ((count == 1) && (*buf == '2'))
			power_mode = 2;
		else if ((count == 1) && *buf == '1')
			power_mode = 1;
		else {
			PRINT_CLUSTER(("cluster/power_mode: '%*.*s' invalid, "
				"must be 2 or 1\n", count, count, buf));
			ret = -EINVAL;
			break;
		}
		PRINT_CLUSTER(("cluster/power_mode -> %d\n", power_mode));
		break;
#endif

#if DEBUG_CLUSTER_SWITCH
	case ClusterAttr_Debug:
		if ((count == 1) && (*buf == '0'))
			tegra_cluster_debug = 0;
		else if ((count == 1) && (*buf == '1'))
			tegra_cluster_debug = 1;
		else {
			PRINT_CLUSTER(("cluster/debug: '%*.*s' invalid, "
				"must be 0 or 1\n", count, count, buf));
			ret = -EINVAL;
			break;
		}
		PRINT_CLUSTER(("cluster/debug -> %d\n",tegra_cluster_debug));
		break;
#endif

	default:
		ret = -ENOENT;
		break;
	}

	spin_unlock(&cluster_lock);

	if (new_parent) {
		e = clk_set_parent(cpu_clk, new_parent);
		if (e) {
			PRINT_CLUSTER(("cluster/active: request failed (%d)\n",
				       e));
			ret = e;
		}
	}
fail:
	TRACE_CLUSTER(("-sysfscluster_store: %d\n", count));
	return ret;
}

#define CREATE_FILE(x) \
	do { \
		e = sysfs_create_file(cluster_kobj, &cluster_##x##_attr.attr); \
		if (e) { \
			TRACE_CLUSTER(("cluster/" __stringify(x) \
				": sysfs_create_file failed!\n")); \
			goto fail; \
		} \
	} while (0)

static int __init sysfscluster_init(void)
{
	int e;

	TRACE_CLUSTER(("+sysfscluster_init\n"));

	spin_lock_init(&cluster_lock);
	cluster_kobj = kobject_create_and_add("cluster", kernel_kobj);

	CREATE_FILE(active);
	CREATE_FILE(immediate);
	CREATE_FILE(force);
	CREATE_FILE(wake_ms);
#if defined(CONFIG_PM_SLEEP) && SYSFS_CLUSTER_POWER_MODE
	CREATE_FILE(powermode);
#endif
#if DEBUG_CLUSTER_SWITCH
	CREATE_FILE(debug);
#endif

	spin_lock(&cluster_lock);
	if (is_lp_cluster())
		flags |= TEGRA_POWER_CLUSTER_LP;
	else
		flags |= TEGRA_POWER_CLUSTER_G;
	spin_unlock(&cluster_lock);

fail:
	TRACE_CLUSTER(("-sysfscluster_init\n"));
	return e;
}

#define REMOVE_FILE(x) \
		sysfs_remove_file(cluster_kobj, &cluster_##x##_attr.attr)

static void __exit sysfscluster_exit(void)
{
	TRACE_CLUSTER(("+sysfscluster_exit\n"));
#if DEBUG_CLUSTER_SWITCH
	REMOVE_FILE(debug);
#endif
#if defined(CONFIG_PM_SLEEP) && SYSFS_CLUSTER_POWER_MODE
	REMOVE_FILE(powermode);
#endif
	REMOVE_FILE(wake_ms);
	REMOVE_FILE(force);
	REMOVE_FILE(immediate);
	REMOVE_FILE(active);
	kobject_del(cluster_kobj);
	TRACE_CLUSTER(("-sysfscluster_exit\n"));
}

module_init(sysfscluster_init);
module_exit(sysfscluster_exit);
MODULE_LICENSE("GPL");
