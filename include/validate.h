#define CPG SH7305_CPG
#define BSC SH7305_BSC

unsigned int best_rom_wait(u32 Bphi_f);
unsigned int best_ram_read(u32 Bphi_f);
unsigned int best_ram_write(u32 Bphi_f);
unsigned int best_TRC(u32 Bphi_f);
int TRC_equivalent(u8 reg_value);
int BCR_equivalent(u8 reg_value);
int WR_equivalent(u8 reg_value);
bool auto_up_PFC();
bool auto_down_PFC();
bool compute_limit(int PLL_add);
