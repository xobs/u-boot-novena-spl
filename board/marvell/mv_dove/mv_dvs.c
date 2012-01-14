/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File in accordance with the terms and conditions of the General 
Public License Version 2, June 1991 (the "GPL License"), a copy of which is 
available along with the File in the license.txt file or by writing to the Free 
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or 
on the worldwide web at http://www.gnu.org/licenses/gpl.txt. 

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED 
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY 
DISCLAIMED.  The GPL License provides additional details about this warranty 
disclaimer.

*******************************************************************************/

#include <config.h>
#include <common.h>
#include <command.h>

#include "mvCommon.h"
#include "mvCtrlEnvLib.h"
#include "mvCpuIfRegs.h"

#include "pmu/mvPmuRegs.h"
#include "pmu/mvPmu.h"
#include "mv_dvs.h"

extern u32 dvs_values_param;
MV_STATUS mvPmuDvs(MV_U32 pSet, MV_U32 vSet, MV_U32 rAddr, MV_U32 sAddr);
MV_VOID mvPmuSelSDI (MV_U8 select);

MV_VOLTAGE_INFO voltStagesArr[VOLT_STAGES_NUM]= {
	/*value	volt	%		vSet	pSet*/
	{ 900,		100,	-1000,	0x8,	0x8},//0
	{ 925,		100,	-750,	0x8,	0x9},//1
	{ 950,		100,	-500,	0x8,	0xA},//2
	{ 975,		100,	-250,	0x8,	0xB},//3
	{1000,		100,	   0,	0x8,	0x0},//4
	{1025,		100,	 250,	0x8,	0xC},//5
	{1050,		100,	 500,	0x8,	0xD},//6
	{1075,		100,	 750,	0x8,	0xE},//7
	{1080,		120,	-1000,	0x9,	0x8},//8
	{1100,		100,	 1000,	0x8,	0xF},//9
	{1110,		120,	-750,	0x9,	0x9},//10
	{1140,		120,	-500,	0x9,	0xA},//11
	{1170,		120,	-250,	0x9,	0xB},//12
	{1200,		120,	   0,	0x9,	0x0},//13
	{1230,		120,	 250,	0x9,	0xC},//14
	{1260,		120,	 500,	0x9,	0xD},//15
	{1290,		120,	 750,	0x9,	0xE},//16
	{1320,		120,	 1000,	0x9,	0xF},//17
};

/* Standby blinking led timing */
#define PMU_STBY_LED_PERIOD		0x10000			/* RTC 32KHz cc ~2.0s */
#define PMU_STBY_LED_DUTY		0xE000			/* RTC 32KHz cc ~1.75s - 12.5%*/
/* Save L2 ratio at system power up */
static MV_U32 l2TurboRatio;

/* PMU uController code fixes <add,val> couples */
MV_U32	uCfix[][2] = {{0,0x01}, {38, 0x33}, {120,0xB0}, {121,0x00}, {122,0x80}, {123,0xC0}, {124,0x00}, \
	{125,0x00}, {126,0x00}, {127,0x00}, {214,0x00}, {239,0xB0}, {252,0x44}, {253,0x00}, {254,0xF3}, \
	{255,0xF0}, {284, 0x00}, {297, 0x7C}, {356, 0x00}, {388, 0x71}, {389, 0x30}, {390, 0x80}, {391, 0x28}, {392, 0x80}, \
	{393, 0xFF}, {394, 0x08}, {395, 0x49}, {396, 0x39}, {397, 0x20}, {398, 0xA8}, {399, 0x18}, \
	{400, 0x80}, {401, 0x69}, {402, 0x59}, {403, 0x08}, {404, 0x94}, {405 ,0x41}, {406 , 0x00}, \
	{407, 0x7D}, {408, 0x19}, {409, 0x94}, {410, 0x40}, {411, 0x94}, {412, 0x42}, {413, 0xFF}};

static MV_VOID mvPmuCodeFix (MV_VOID)
{
	MV_U32 reg;
	MV_U32 i;

	/* Initialize the uCode PCs of the different PMU operations */
	MV_REG_WRITE(PMU_DFS_PROC_PC_0_REG, ((MV_REG_READ(PMU_DFS_PROC_PC_0_REG) & ~PMU_DFS_PRE_PC_MASK) | PMU_DFS_PRE_PC_VAL));
	MV_REG_WRITE(PMU_DFS_PROC_PC_0_REG, ((MV_REG_READ(PMU_DFS_PROC_PC_0_REG) & ~PMU_DFS_CPU_PC_MASK) | PMU_DFS_CPU_PC_VAL));
	MV_REG_WRITE(PMU_DFS_PROC_PC_1_REG, ((MV_REG_READ(PMU_DFS_PROC_PC_1_REG) & ~PMU_DFS_DDR_PC_MASK) | PMU_DFS_DDR_PC_VAL));
	MV_REG_WRITE(PMU_DFS_PROC_PC_1_REG, ((MV_REG_READ(PMU_DFS_PROC_PC_1_REG) & ~PMU_DFS_L2_PC_MASK) | PMU_DFS_L2_PC_VAL));
	MV_REG_WRITE(PMU_DFS_PROC_PC_2_REG, ((MV_REG_READ(PMU_DFS_PROC_PC_2_REG) & ~PMU_DFS_PLL_PC_MASK) | PMU_DFS_PLL_PC_VAL));
	MV_REG_WRITE(PMU_DFS_PROC_PC_2_REG, ((MV_REG_READ(PMU_DFS_PROC_PC_2_REG) & ~PMU_DFS_POST_PC_MASK) | PMU_DFS_POST_PC_VAL));
	MV_REG_WRITE(PMU_DVS_PROC_PC_REG, ((MV_REG_READ(PMU_DVS_PROC_PC_REG) & ~PMU_DVS_PRE_PC_MASK) | PMU_DVS_PRE_PC_VAL));
	MV_REG_WRITE(PMU_DVS_PROC_PC_REG, ((MV_REG_READ(PMU_DVS_PROC_PC_REG) & ~PMU_DVS_POST_PC_MASK) | PMU_DVS_POST_PC_VAL));
	MV_REG_WRITE(PMU_DEEPIDLE_STBY_PROC_PC_REG, ((MV_REG_READ(PMU_DEEPIDLE_STBY_PROC_PC_REG) & ~PMU_DPIDL_PC_MASK) | PMU_DPIDL_PC_VAL));
	MV_REG_WRITE(PMU_DEEPIDLE_STBY_PROC_PC_REG, ((MV_REG_READ(PMU_DEEPIDLE_STBY_PROC_PC_REG) & ~PMU_STBY_PRE_PC_MASK) | PMU_STBY_PRE_PC_VAL));
	MV_REG_WRITE(PMU_STBY_PROC_PC_REG, ((MV_REG_READ(PMU_STBY_PROC_PC_REG) & ~PMU_STBY_DDR_PC_MASK) | PMU_STBY_DDR_PC_VAL));
	MV_REG_WRITE(PMU_STBY_PROC_PC_REG, ((MV_REG_READ(PMU_STBY_PROC_PC_REG) & ~PMU_STBY_POST_PC_MASK) | PMU_STBY_POST_PC_VAL));

	/* Update uCode */
	for (i=0; i< (sizeof(uCfix) / sizeof(uCfix[0])); i++)
	{
		reg = MV_REG_READ(PMU_PROGRAM_REG(uCfix[i][0]));
		reg &= ~PMU_PROGRAM_MASK(uCfix[i][0]);
		reg |= (uCfix[i][1] << PMU_PROGRAM_OFFS(uCfix[i][0]));
		MV_REG_WRITE(PMU_PROGRAM_REG(uCfix[i][0]), reg);
	}
}

MV_STATUS mvPmuInit (MV_PMU_INFO * pmu)
{
	MV_U32 reg;
	MV_32 i;
	MV_32 cke_mpp_num = -1;

	/* Set the DeepIdle and Standby power delays */
	MV_REG_WRITE(PMU_STANDBY_PWR_DELAY_REG, pmu->standbyPwrDelay);

	/* Configure the Battery Management Control register */
	reg = 0;
	if (pmu->batFltMngDis) reg |= PMU_BAT_FLT_DISABLE_MASK;
	if (pmu->exitOnBatFltDis) reg |= PMU_BAT_FLT_STBY_EXIT_DIS_MASK;
	MV_REG_WRITE(PMU_BAT_MNGMT_CTRL_REG, reg);

	/* Invert the CPU and whole chip Power Down Polarity */
	reg = MV_REG_READ(PMU_PWR_SUPLY_CTRL_REG);
	reg &= ~PMU_PWR_CPU_OFF_LEVEL_MASK;	/* 0 - CPU is powered down (DEEP IDLE) */
	reg |= PMU_PWR_CPU_ON_LEVEL_MASK;	/* 1 - CPU is powered up */
	reg &= ~PMU_PWR_STBY_ON_LEVEL_MASK;	/* 0 - Dove is powered down (STANDBY) */
	reg |= PMU_PWR_STBY_OFF_LEVEL_MASK;	/* 1 - Dove is powered up */
	MV_REG_WRITE(PMU_PWR_SUPLY_CTRL_REG, reg);

	/* Enable the PMU Control on the CPU reset) */
	reg = MV_REG_READ(CPU_CONTROL_REG);
	reg |= CPU_CTRL_PMU_CPU_RST_EN_MASK;
	MV_REG_WRITE(CPU_CONTROL_REG, reg);

	/* Configure the PMU Signal selection */
	reg = 0;
	for (i=7; i>=0; i--)
		reg |= ((pmu->sigSelctor[i] & 0xF) << (i*4));
	MV_REG_WRITE(PMU_SIG_SLCT_CTRL_0_REG, reg);

	reg = 0;
	for (i=7; i>=0; i--)
		reg |= ((pmu->sigSelctor[i+8] & 0xF) << (i*4));
	MV_REG_WRITE(PMU_SIG_SLCT_CTRL_1_REG, reg);

	/* Configure the CPU Power Good if used */
	for (i=0; i<16; i++)
	{
		if (pmu->sigSelctor[i] == PMU_SIGNAL_CPU_PWRGOOD)
		{
			reg = MV_REG_READ(PMU_PWR_SUPLY_CTRL_REG);
			reg |= PMU_PWR_GOOD_PIN_EN_MASK;
			MV_REG_WRITE(PMU_PWR_SUPLY_CTRL_REG, reg);
			break;
		}
	}

	/* Check if CKE workaround is applicable */
	for (i=0; i<8; i++)
	{
		if (pmu->sigSelctor[i] == PMU_SIGNAL_CKE_OVRID)
		{
			cke_mpp_num = i;
			break;
		}
	}

	/* Configure the blinking led timings */
	MV_REG_WRITE(PMU_BLINK_PERIOD_CTRL_REG, PMU_STBY_LED_PERIOD);
	MV_REG_WRITE(PMU_BLINK_DUTY_CYCLE_CTRL_REG, PMU_STBY_LED_DUTY);

	/* Configure the DVS delay */
	MV_REG_WRITE(PMU_DVS_DELAY_REG, pmu->dvsDelay);

	/* Apply uCode Fixes */
	mvPmuCodeFix();

	/* Save L2 ratio at reset - Needed in DFS scale up */
	reg = MV_REG_READ(PMU_CLK_DIVIDER_0_REG);
	l2TurboRatio = (((reg & PMU_CLK_DIV_XPRATIO_MASK) >> PMU_CLK_DIV_XPRATIO_OFFS) << PMU_DFS_CTRL_L2_RATIO_OFFS);

	return MV_OK;
}

void optimize_powerrails(void)
{
	/* Following is extremely dangerous to play with. Don't touch if you don't know what you are doing !!!! */
	/* Set core (perihperals to 1.0 -5% */
	printf ("Modifying CPU/CORE/DDR power rails to 1.0(-2.5%) / 1.0(-5%) / 1.5(-5%)\n");
	mvPmuSelSDI(9);
	mvPmuDvs(0xa, 0x9, 0x2, 0x5);
	udelay(100*1000); /* 100mSec */
	/* Set DDR to 1.5V -5% */
	mvPmuDvs(0xa, 0xb, 0x0, 0x5);
	udelay(100*1000); /* 100 mSec*/
	/* Set CPU to 1.0V -2.5% */
	mvPmuSelSDI(10);
	mvPmuDvs(0xb, 0x9, 0x2, 0x5);

}
MV_STATUS mvPmuWakeupEventSet (MV_U32 wkupEvents)
{
	MV_U32 reg;

	/* Read and modify the Wakeup events */
	reg = MV_REG_READ(PMU_STBY_WKUP_EVENT_CTRL_REG);
	reg &= ~(PMU_STBY_WKUP_CTRL_ALL_EV_MASK);
	reg |= (wkupEvents & PMU_STBY_WKUP_CTRL_ALL_EV_MASK);
	MV_REG_WRITE(PMU_STBY_WKUP_EVENT_CTRL_REG, reg);
	return MV_OK;
}
MV_STATUS mvSysPmuInit(void)
{
	MV_PMU_INFO pmuInitInfo;
	u32 dev, rev;
	MV_U32 reg;

	/* selecting signal 10 as SDI */
	reg = MV_REG_READ(PMU_SIG_SLCT_CTRL_1_REG);
	reg |= 0x300;
	MV_REG_WRITE(PMU_SIG_SLCT_CTRL_1_REG, reg);

	/* Setting MPP 9 and 10 as PMU MPP */
	reg = MV_REG_READ(MPP_GENERAL_CONTROL_REG);
	reg |= 0x600;
	MV_REG_WRITE(MPP_GENERAL_CONTROL_REG, reg);

	/* Open PMU DVS int in main cause */
	reg = MV_REG_READ(CPU_MAIN_IRQ_MASK_HIGH_REG);
	reg = 0x2;
	MV_REG_WRITE(CPU_MAIN_IRQ_MASK_HIGH_REG, reg);

	pmuInitInfo.batFltMngDis = MV_FALSE;				/* Keep battery fault enabled */
	pmuInitInfo.exitOnBatFltDis = MV_FALSE;				/* Keep exit from STANDBY on battery fail enabled */


	pmuInitInfo.sigSelctor[0] = PMU_SIGNAL_NC;
	pmuInitInfo.sigSelctor[1] = PMU_SIGNAL_NC;
	pmuInitInfo.sigSelctor[2] = PMU_SIGNAL_SLP_PWRDWN;		/* STANDBY => 0: I/O off, 1: I/O on */
	pmuInitInfo.sigSelctor[3] = PMU_SIGNAL_EXT0_WKUP;		/* power on push button */
	pmuInitInfo.sigSelctor[4] = PMU_SIGNAL_CKE_OVRID;	/* CKE controlled by Dove */

	pmuInitInfo.sigSelctor[5] = PMU_SIGNAL_CPU_PWRDWN;		/* DEEP-IdLE => 0: CPU off, 1: CPU on */

	pmuInitInfo.sigSelctor[6] = PMU_SIGNAL_MRESET_OVRID;		/* M_RESET is pulled up - always HI */

	pmuInitInfo.sigSelctor[7] = PMU_SIGNAL_1;			/* Standby Led - inverted */
	pmuInitInfo.sigSelctor[8] = PMU_SIGNAL_NC;
	pmuInitInfo.sigSelctor[9] = PMU_SIGNAL_NC;			/* CPU power good  - not used */
	pmuInitInfo.sigSelctor[10] = PMU_SIGNAL_SDI;			/* Voltage regulator control */
	pmuInitInfo.sigSelctor[11] = PMU_SIGNAL_NC;
	pmuInitInfo.sigSelctor[12] = PMU_SIGNAL_NC;
	pmuInitInfo.sigSelctor[13] = PMU_SIGNAL_NC;
	pmuInitInfo.sigSelctor[14] = PMU_SIGNAL_NC;
	pmuInitInfo.sigSelctor[15] = PMU_SIGNAL_NC;
	pmuInitInfo.dvsDelay = 0x4200;				/* ~100us in 166MHz cc - delay for DVS change */
	pmuInitInfo.ddrTermGpioNum = 2;			/* GPIO 2 used to disable terminations */

	pmuInitInfo.standbyPwrDelay = 0x2000;		/* 250ms delay to wait for complete powerup */

	/* Initialize the PMU HAL */
	if (mvPmuInit(&pmuInitInfo) != MV_OK)
	{
		printf("ERROR: Failed to initialise the PMU!\n");
		return MV_ERROR;
	}

	/* Configure wakeup events */
	mvPmuWakeupEventSet(PMU_STBY_WKUP_CTRL_EXT0_FALL | PMU_STBY_WKUP_CTRL_RTC_MASK);

	return MV_OK;
}

/******************************************************************************
* Category     - General
* Functionality- Display temperature from sensor.
* Need modifications (Yes/No) - no
*****************************************************************************/
#define SENSOR_TEMP 1
#define SENSOR_VOLT_CPU 2
#define SENSOR_VOLT_CORE 3
//static MV_U8 current_mode = 0;
int temperature_cmd( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	MV_U32 reg = 0, reg1 = 0;
	MV_U32 value = 0;
	int i = 0;


	reg = MV_REG_READ(PMU_TEMP_DIOD_CTRL0_REG);
	reg &= ~(PMU_TDC0_SEL_IP_MODE_MASK);
	MV_REG_WRITE(PMU_TEMP_DIOD_CTRL0_REG, reg);

	reg = MV_REG_READ(PMU_TEMP_DIOD_CTRL1_REG);
	reg ^= PMU_TDC1_STRT_CAL_MASK;
	MV_REG_WRITE(PMU_TEMP_DIOD_CTRL1_REG, reg);
	//current_mode = SENSOR_TEMP;
	udelay(5000);


	/* Verify that the temperature is valid */
	reg = MV_REG_READ(PMU_TEMP_DIOD_CTRL1_REG);
	if ((reg & PMU_TDC1_TEMP_VLID_MASK) == 0x0)
	{
		printf("Error reading temperature\n");
	}
	else
	{
		for (i = 0; i < 16; i++)
		{
			/* Read the thermal sensor looking for two successive readings that differ in LSB only */
			reg = MV_REG_READ(PMU_THERMAL_MNGR_REG);
			reg = ((reg >> 1) & 0x1FF);
			value = ((2281638 - (7298*reg)) / 10000);
			if (((reg ^ reg1) & 0x1FE) == 0x0)
				break;

			/* save the current reading for the next iteration */
			reg1 = reg;
		}
		if (i == 16)
			printf("Thermal sensor is unstable: could not get two identical successive readings\n");
		else
			printf("\tTemprature (Tj): %d\n", value);
	}

	return 0;
}

U_BOOT_CMD(
	temp,      1,     1,      temperature_cmd,
	"temp	- Display the device temprature.\n",
	" \n"
	"\tDisplay the device temprature as read from the internal sensor.\n"
);

MV_STATUS mvVsenRead(char *cmd)
{
	MV_U32 reg = 0, reg1 = 0;
	MV_U32 value = 0;
	int i = 0;
	char *s;

	if (strncmp(cmd, "cpu", 3) != 0 && strncmp(cmd, "core", 4) != 0)
		return -1;

	if (strncmp(cmd, "cpu", 3) == 0) {
		reg = MV_REG_READ(PMU_TEMP_DIOD_CTRL0_REG);
		reg |= PMU_TDC0_SEL_IP_MODE_MASK;
		reg &= ~(PMU_TDC0_SEL_VSEN_MASK);
		MV_REG_WRITE(PMU_TEMP_DIOD_CTRL0_REG, reg);

		reg = MV_REG_READ(PMU_TEMP_DIOD_CTRL1_REG);
		reg ^= PMU_TDC1_STRT_CAL_MASK;
		MV_REG_WRITE(PMU_TEMP_DIOD_CTRL1_REG, reg);
		udelay(5000);
	}
	if (strncmp(cmd, "core", 3) == 0) {
		reg = MV_REG_READ(PMU_TEMP_DIOD_CTRL0_REG);
		reg |= PMU_TDC0_SEL_IP_MODE_MASK;
		reg |= PMU_TDC0_SEL_VSEN_MASK;
		MV_REG_WRITE(PMU_TEMP_DIOD_CTRL0_REG, reg);

		reg = MV_REG_READ(PMU_TEMP_DIOD_CTRL1_REG);
		reg ^= PMU_TDC1_STRT_CAL_MASK;
		MV_REG_WRITE(PMU_TEMP_DIOD_CTRL1_REG, reg);
		udelay(5000);
	}

	/* Verify that the temperature is valid */
	reg = MV_REG_READ(PMU_TEMP_DIOD_CTRL1_REG);
	if ((reg & PMU_TDC1_TEMP_VLID_MASK) == 0x0) {
		printf("Error");
		return MV_ERROR;
	} else {
		reg1 = MV_REG_READ(PMU_THERMAL_MNGR_REG);
		reg1 = ((reg1 >> 1) & 0x1FF);
		for (i = 0; i < 16; i++) {
			/* Read the thermal sensor looking for two successive readings that differ in LSB only */
			reg = MV_REG_READ(PMU_THERMAL_MNGR_REG);
			reg = ((reg >> 1) & 0x1FF);
			if (((reg ^ reg1) & 0x1FE) == 0x0)
				break;
			/* save the current reading for the next iteration */
			reg1 = reg;
		}

		value = ((reg + 241)*100000/39619);
		if (i == 16) {
			printf("Error");
			return MV_ERROR;
		}
		else
			printf("%d.%03d", value/1000, value%1000);
	}
	return value;
}
/******************************************************************************
* Category     - General
* Functionality- Display temperature from sensor.
* Need modifications (Yes/No) - no
*****************************************************************************/
int volt_cmd( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char *cmd;

	if (argc < 2)
		goto usage;

	cmd = argv[1];

	if (strncmp(cmd, "cpu", 3) != 0 && strncmp(cmd, "core", 4) != 0)
		goto usage;

	if (strncmp(cmd, "cpu", 3) == 0)
		printf("\tCPU: ");
	if (strncmp(cmd, "core", 4) == 0)
		printf("\tCore: ");
		
	if (mvVsenRead(cmd) == MV_ERROR){
		printf("Error reading VSEN sensor\n");
		return 1;
	}
	printf("\n");

	return 0;
usage:
	printf("Usage:\n%s\n", cmdtp->usage);
	return 1;
}

U_BOOT_CMD(
	volt,      2,     1,      volt_cmd,
	"volt	- Display the cpu / core voltage.\n",
	"volt cpu	- display the cpu voltage\n"
	"volt core	- display the core voltage\n"
);

MV_VOID mvPmuSelSDI (MV_U8 select)
{
        MV_U32 reg0, regw0, reg1, regw1;
        int i;
        regw0 = reg0 = MV_REG_READ(PMU_SIG_SLCT_CTRL_0_REG);
        for (i = 0 ; i < 8 ; i++ ) {
                if ((reg0 & (0xf << (i*4))) == (PMU_SIGNAL_SDI << (i*4))) {
                        regw0 &= ~(0xf << (i*4));
                        regw0 |= PMU_SIGNAL_0 << (i*4);
                }
        }
        regw1 = reg1 = MV_REG_READ(PMU_SIG_SLCT_CTRL_1_REG);
        for (i = 0 ; i < 8 ; i++ ) {
                if ((reg1 & (0xf << (i*4))) == (PMU_SIGNAL_SDI << (i*4))) {
                        regw1 &= ~(0xf << (i*4));
                        regw1 |= PMU_SIGNAL_0 << (i*4);
                }
        }
        if (select < 8) 
                regw0 = regw0 | (PMU_SIGNAL_SDI << (select * 4));
        if ((select >=8) && (select < 16)) 
        regw1 = regw1 | (PMU_SIGNAL_SDI << ((select-8) * 4));
done:   if (regw0 != reg0) {
                MV_REG_WRITE(PMU_SIG_SLCT_CTRL_0_REG, regw0);
        }
        if (regw1 != reg1)
        {
                MV_REG_WRITE(PMU_SIG_SLCT_CTRL_1_REG, regw1);
        }
}

#define PMU_DVS_POLL_DLY		100000			/* Poll DVS done count */
MV_STATUS mvPmuDvs(MV_U32 pSet, MV_U32 vSet, MV_U32 rAddr, MV_U32 sAddr)
{
	MV_U32 reg, reg1;
	MV_U32 i;

	/* Clean the PMU cause register */
	MV_REG_WRITE(PMU_INT_CAUSE_REG, 0x0);

	/* Trigger the DVS */
	reg = ((sAddr << PMU_DVS_CTRL_PMIC_SADDR_OFFS) & PMU_DVS_CTRL_PMIC_SADDR_MASK);
	reg |= ((rAddr << PMU_DVS_CTRL_PMIC_RADDR_OFFS) & PMU_DVS_CTRL_PMIC_RADDR_MASK);
	reg |= ((vSet << PMU_DVS_CTRL_PMIC_VSET_OFFS) & PMU_DVS_CTRL_PMIC_VSET_MASK);
	reg |= ((pSet << PMU_DVS_CTRL_PMIC_PSET_OFFS) & PMU_DVS_CTRL_PMIC_PSET_MASK);
	MV_REG_WRITE(PMU_CPU_DVS_CTRL_REG, reg);

	/* Disable all PMU interrupts and enable DVS done */
	reg1 = MV_REG_READ(PMU_INT_MASK_REG);
	MV_REG_WRITE(PMU_INT_MASK_REG, PMU_INT_DVS_DONE_MASK);

	reg |= PMU_DVS_CTRL_DVS_EN_MASK;

	MV_REG_WRITE(PMU_CPU_DVS_CTRL_REG, reg);
	mvOsDelay(20); /*wait 10 ms */

	for (i=0; i<PMU_DVS_POLL_DLY; i++)
		if ((MV_REG_READ(0x20210) & 0x2) == 0x0)
			break;

	/* return the original PMU MASK */
	MV_REG_WRITE(PMU_INT_MASK_REG, reg1);

	/* WA to return PC to 0 */
	MV_REG_WRITE(PMU_PC_FORCE_CTRL_REG, 0x1);
	MV_REG_WRITE(PMU_PC_FORCE_CTRL_REG, 0x0);

	if (i == PMU_DVS_POLL_DLY)
		return MV_TIMEOUT;

	return MV_OK;
}

MV_STATUS mvDvsToTarget(char *cmd, int target)
{
	MV_U32 reg;
	int voltCPU, voltCore, i, timeout;

	int voltMax = target +20;
	int voltMin = target -20;
	
	if (strncmp(cmd, "cpu", 3) == 0) {
		if ((voltMin < 980) || (voltMax > 1220)) {
			printf("target voltage not in allowed range, 1000 - 1200\n");
			return MV_ERROR;
		}
		printf("\tCPU: ");

#if defined(RD_88AP510A0_AVNG)
		/* selecting signal 10 as SDI */
		MV_REG_WRITE(PMU_SIG_SLCT_CTRL_1_REG, 0x300);
		/* Setting MPP 10 as PMU MPP */
		reg = MV_REG_READ(MPP_GENERAL_CONTROL_REG);
		reg |= 0x400;  //toggle MPP 9 and 10,0:General MPP ,1:PMU MPP	
		reg &= ~0x200; //toggle MPP 9 and 10,0:General MPP ,1:PMU MPP		
		MV_REG_WRITE(MPP_GENERAL_CONTROL_REG, reg);
#else
		/* selecting signal 10 as SDI */
		MV_REG_WRITE(PMU_SIG_SLCT_CTRL_1_REG, 0x300);
		/* Setting MPP 10 as PMU MPP */
		reg = MV_REG_READ(MPP_GENERAL_CONTROL_REG);
		reg |= 0x400;
		reg &= ~0x2000;
		MV_REG_WRITE(MPP_GENERAL_CONTROL_REG, reg);
#endif

		i = VOLT_CPU_DEFAULT_INDEX;
		mvPmuDvs(voltStagesArr[i].pSet, voltStagesArr[i].vSet, 0x2, 0x5);
		voltCPU = mvVsenRead("cpu");

		timeout = 0;
		while ((voltCPU > voltMax) || (voltCPU < voltMin)) {
			if (voltCPU > voltMax) {
				i--;
				printf("-");
			}
			if (voltCPU < voltMin) {
				i++;
				printf("+");
			}
			mvPmuDvs(voltStagesArr[i].pSet, voltStagesArr[i].vSet, 0x2, 0x5);
			voltCPU = mvVsenRead("cpu");
			timeout ++;
			if (timeout > 10)
				break;
		}
		printf(" -> pmic set to [%d.%d]\n", voltStagesArr[i].value/100, voltStagesArr[i].value%100);
		/* passing value to Linux as default DVS calibration value */
		dvs_values_param =  (voltStagesArr[i].vSet << 8) |  voltStagesArr[i].pSet;
	}

	if (strncmp(cmd, "core", 4) == 0) {
		if ((voltMin < 880) || (voltMax > 1120)) {
			printf("target voltage not in allowed range 900 - 1100\n");
			return MV_ERROR;
		}
		printf("\tCore: ");

#if defined(RD_88AP510A0_AVNG)
		/* selecting signal 9 as SDI */
		MV_REG_WRITE(PMU_SIG_SLCT_CTRL_1_REG, 0x30);
		/* Setting MPP 9 as PMU MPP */
		reg = MV_REG_READ(MPP_GENERAL_CONTROL_REG);
		reg &= ~0x400;  //toggle MPP 9 and 10,0:General MPP ,1:PMU MPP
		reg |= 0x200; 	//toggle MPP 9 and 10,0:General MPP ,1:PMU MPP
		MV_REG_WRITE(MPP_GENERAL_CONTROL_REG, reg);
#else
		/* selecting signal 13 as SDI */
		MV_REG_WRITE(PMU_SIG_SLCT_CTRL_1_REG, 0x300000);
		/* Setting MPP 13 as PMU MPP */
		reg = MV_REG_READ(MPP_GENERAL_CONTROL_REG);
		reg &= ~0x400;
		reg |= 0x2000;
		MV_REG_WRITE(MPP_GENERAL_CONTROL_REG, reg);
#endif

		i = VOLT_CORE_DEFAULT_INDEX;
		mvPmuDvs(voltStagesArr[i].pSet, voltStagesArr[i].vSet, 0x2, 0x5);
		voltCore = mvVsenRead("core");
		timeout = 0;
		while ((voltCore > voltMax) || (voltCore < voltMin)) {
			if (voltCore > voltMax) {
				i--;
				printf("-");
			}
			if (voltCore < voltMin) {
				i++;
				printf("+");
			}
			mvPmuDvs(voltStagesArr[i].pSet, voltStagesArr[i].vSet, 0x2, 0x5);
			voltCore = mvVsenRead("core");
			timeout ++;
			if (timeout > 10)
				break;
		}
		printf(" -> pmic set to [%d.%d]\n", voltStagesArr[i].value/100, voltStagesArr[i].value%100);
		/* Switching back to CPU SDI configuration to supprt Linux CPU only DVS */\
#if defined(RD_88AP510A0_AVNG)
		/* selecting signal 10 as SDI */
		MV_REG_WRITE(PMU_SIG_SLCT_CTRL_1_REG, 0x300);
		/* Setting MPP 10 as PMU MPP */
		reg = MV_REG_READ(MPP_GENERAL_CONTROL_REG);
		reg |= 0x400;  //toggle MPP 9 and 10,0:General MPP ,1:PMU MPP
		reg &= ~0x200; //toggle MPP 9 and 10,0:General MPP ,1:PMU MPP
		MV_REG_WRITE(MPP_GENERAL_CONTROL_REG, reg);
#else
		/* selecting signal 10 as SDI */
		MV_REG_WRITE(PMU_SIG_SLCT_CTRL_1_REG, 0x300);
		/* Setting MPP 10 as PMU MPP */
		reg = MV_REG_READ(MPP_GENERAL_CONTROL_REG);
		reg |= 0x400;
		reg &= ~0x2000;
		MV_REG_WRITE(MPP_GENERAL_CONTROL_REG, reg);
#endif
	}
	return MV_OK;
}
#if 0
int dvs_cmd( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char *cmd;
	int volt;

	if (argc < 3)
		goto usage;

	cmd = argv[1];

	if (strncmp(cmd, "cpu", 3) != 0 && strncmp(cmd, "core", 4) != 0)
		goto usage;
	
	volt = (int)simple_strtoul(argv[2], NULL, 10);
	if (mvDvsToTarget(cmd, volt) == MV_ERROR) {
		printf("Error perfoming DVS\n");
		return 1;
	}
	
	return 0;
usage:
		printf("Usage:\n%s\n", cmdtp->usage);
	return 1;
}
U_BOOT_CMD(
	dvs,      3,     1,      dvs_cmd,
	"     dvs   - set the cpu voltage.\n",
	"dvs cpu <target> - dvs cpu to target voltage, <target> in mili-voltage \n"
	"					for example: dvs cpu 1160\n"
	"dvs core <target> - dvs core to target voltage, <target> in mili-voltage \n"
	"					for example: dvs core 960\n"
);
#else
int dvs_cmd( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char *cmd;
	int pset;
	if (argc < 3)
		goto usage;

	cmd = argv[1];

	if (strncmp(cmd, "cpu", 3) != 0 && strncmp(cmd, "core", 4) != 0 && strncmp(cmd, "ddr", 3) != 0 )
		goto usage;
	
	pset = (int)simple_strtoul(argv[2], NULL, 10);
	if ((pset < 8) || (pset > 15)) goto usage;
	printf ("Was asked to set %s power rail to pset %d\n",argv[1],pset);
	if (!strncmp(cmd, "cpu", 3)) {
		mvPmuSelSDI(10);
		mvPmuDvs(pset, 0x9, 0x2, 0x5);
	}
	if (!strncmp(cmd, "core", 4)) {
		mvPmuSelSDI(9);
		mvPmuDvs(pset, 0x9, 0x2, 0x5);
	}
	if (!strncmp(cmd, "ddr", 3)) {
		if (pset > 11) goto usage;
		mvPmuSelSDI(9);
		mvPmuDvs(pset, 0xb, 0x0, 0x5);
	}

	return 0;
usage:
		printf("Usage:\n%s\n", cmdtp->usage);
	return 1;
}
U_BOOT_CMD(
	dvs,      3,     1,      dvs_cmd,
	"     dvs   - set the cpu/core(perihperals)/ddr pset value of the power rails voltages.\n",
	"dvs cpu <pset>  - dvs cpu to pset (vset=1.0V)\n"
	"dvs core <pset> - dvs core to pset (vset=1.0V)\n"
	"dvs ddr <pset>  - dvs ddr to pset (vset=1.5V)\n"
);
#endif
