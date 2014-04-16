/*
 * Copyright (C) 2014 NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef MEMORY_TEGRA_MC_H
#define MEMORY_TEGRA_MC_H

#include <linux/io.h>
#include <linux/types.h>

struct page;

struct tegra_smmu_enable {
	unsigned int reg;
	unsigned int bit;
};

/* latency allowance */
struct tegra_mc_la {
	unsigned int reg;
	unsigned int shift;
	unsigned int mask;
	unsigned int def;
};

struct tegra_mc_client {
	unsigned int id;
	const char *name;
	unsigned int swgroup;

	unsigned int fifo_size;

	struct tegra_smmu_enable smmu;
	struct tegra_mc_la la;
};

struct tegra_smmu_swgroup {
	unsigned int swgroup;
	unsigned int reg;
};

struct tegra_smmu_ops {
	void (*flush_dcache)(struct page *page, unsigned long offset,
			     size_t size);
};

struct tegra_smmu_soc {
	const struct tegra_mc_client *clients;
	unsigned int num_clients;

	const struct tegra_smmu_swgroup *swgroups;
	unsigned int num_swgroups;

	bool supports_round_robin_arbitration;
	bool supports_request_limit;

	unsigned int num_asids;

	const struct tegra_smmu_ops *ops;
};

struct tegra_mc_soc {
	const struct tegra_mc_client *clients;
	unsigned int num_clients;

	unsigned int num_address_bits;
	unsigned int atom_size;

	const struct tegra_smmu_soc *smmu;
};

struct tegra_smmu;

struct tegra_mc {
	struct device *dev;
	struct tegra_smmu *smmu;
	void __iomem *regs;
	struct clk *clk;
	int irq;

	const struct tegra_mc_soc *soc;
	unsigned long tick;
};

static inline u32 mc_readl(struct tegra_mc *mc, unsigned long offset)
{
	return readl(mc->regs + offset);
}

static inline void mc_writel(struct tegra_mc *mc, u32 value,
			     unsigned long offset)
{
	writel(value, mc->regs + offset);
}

#ifdef CONFIG_TEGRA_IOMMU_SMMU
struct tegra_smmu *tegra_smmu_probe(struct device *dev,
				    const struct tegra_smmu_soc *soc,
				    struct tegra_mc *mc);
#else
static inline struct tegra_smmu *
tegra_smmu_probe(struct device *dev, const struct tegra_smmu_soc *soc,
		 struct tegra_mc *mc)
{
	return NULL;
}
#endif

#ifdef CONFIG_ARCH_TEGRA_3x_SOC
extern const struct tegra_mc_soc tegra30_mc_soc;
#endif

#ifdef CONFIG_ARCH_TEGRA_114_SOC
extern const struct tegra_mc_soc tegra114_mc_soc;
#endif

#ifdef CONFIG_ARCH_TEGRA_124_SOC
extern const struct tegra_mc_soc tegra124_mc_soc;
#endif

#endif /* MEMORY_TEGRA_MC_H */
