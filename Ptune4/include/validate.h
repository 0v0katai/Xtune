#define CPG SH7305_CPG
#define BSC SH7305_BSC

extern i32 settings[];

unsigned int best_rom_wait(i32 Bphi_f);
unsigned int best_ram_read(i32 Bphi_f);
unsigned int best_ram_write(i32 Bphi_f);
unsigned int best_TRC(i32 Bphi_f);
int TRC_equivalent(u8 reg_value);
int BCR_equivalent(u8 reg_value);
int WR_equivalent(u8 reg_value);
bool auto_up_PFC();
bool auto_down_PFC();
bool exceed_limit();
