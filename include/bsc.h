#include <gint/mpu/bsc.h>

enum CSn_option
{
    SELECT_CS0,
    SELECT_CS2,
    SELECT_CS3,
    SELECT_CS4,
    SELECT_CS5A,
    SELECT_CS5B,
    SELECT_CS6A,
    SELECT_CS6B
};

enum MODE_option
{
    SELECT_BCR,
    SELECT_WCR
};

enum BCR_REG_option
{
    SELECT_IWW,
    SELECT_IWRWD,
    SELECT_IWRWS,
    SELECT_IWRRD,
    SELECT_IWRRS
};

enum CSnWCR_REG_option
{
    SELECT_WW,
    SELECT_SW,
    SELECT_HW,
    SELECT_WR
};

enum CS3WCR_REG_option
{
    SELECT_TRP,
    SELECT_TRCD,
    SELECT_A3CL,
    SELECT_TRWL,
    SELECT_TRC
};

typedef byte_union(BSC_option,
                   u8 : 1;
                   u8 MODE : 1;
                   u8 CSn : 3;
                   u8 REG : 3;);

extern BSC_option const CS0WCR_WR_ptr;
extern BSC_option const CS2WCR_WR_ptr;
extern BSC_option const CS2WCR_WW_ptr;
extern BSC_option const CS3WCR_CL_ptr;
extern BSC_option const CS3WCR_TRC_ptr;

void bsc_modify(BSC_option select, i8 modify);
