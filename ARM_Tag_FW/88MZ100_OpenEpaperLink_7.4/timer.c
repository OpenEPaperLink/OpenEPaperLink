#include "timer.h"
#include "mz100.h"
#include "mz100_gpt.h"
#include "mz100_driver.h"

static volatile uint32_t mTicksHi = 0;

void __attribute__((interrupt)) GPT1_IRQHandler(void)
{
	GPT1->INT_RAW_REG.WORDVAL = 1 << 16;
	(void)GPT1->INT_RAW_REG.WORDVAL; // readback - errata for irq double-triggering
	mTicksHi++;
}

#define MZ_PMU_CLK_SRC_GPT1_SRC_MASK 0x0000000c
#define MZ_PMU_CLK_SRC_GPT1_SRC_32M_XTAL 0x00000000
void timerInit(void)
{
	// set GPT1 timer source to 32MHz
	PMU->CLK_SRC.WORDVAL = (PMU->CLK_SRC.WORDVAL & ~MZ_PMU_CLK_SRC_GPT1_SRC_MASK) | MZ_PMU_CLK_SRC_GPT1_SRC_32M_XTAL;
	PMU->CLK_SRC.WORDVAL |= (MZ_PMU_CLK_SRC_GPT1_SRC_32M_XTAL << 2);

	GPT1->CNT_EN_REG.WORDVAL = 2; // stop it
	NVIC_EnableIRQ(20);
	NVIC_ClearPendingIRQ(20);

	GPT1->INT_MSK_REG.WORDVAL = ~(1 << 16);
	GPT1->CNT_CNTL_REG.WORDVAL = 0x100; // count up, slow value updates
	GPT1->CNT_UPP_VAL_REG.WORDVAL = 0xffffffff;
	GPT1->CLK_CNTL_REG.WORDVAL = 0x000; // Fclk / 1. Fclk is rc32K - 32KHz-ish. PMU can change it, somehow..haha
	GPT1->DMA_CNTL_EN_REG.WORDVAL = 0;
	GPT1->CNT_EN_REG.WORDVAL = 5; // reset and start
}

void timerStop(void)
{
	GPT1->CNT_EN_REG.WORDVAL = 0;
}

uint64_t timerGet(void)
{
	uint32_t hi, lo;

	do
	{
		hi = mTicksHi;
		lo = GPT1->CNT_VAL_REG.WORDVAL;
	} while (hi != mTicksHi);

	return (((uint64_t)hi) << 32) + lo;
	return 0;
}

void timerDelay(uint64_t cycles)
{
	uint64_t t;

	t = timerGet();
	while (timerGet() - t < cycles)
		;
}