#pragma once

FILE *bin_file;
char s[100], cmd[20], inst_asm[200], inst_hex[200], tmp_str[10];
unsigned char dig, code[40000], inst, next_inst, rp;
long org_code, end_code;
int ptr, len_code;
char U[2][5]={"ETM0", "ETM1"},
V[2][5] = { "ECNT", "ECPT" },
R[9][2] = { "V",  "A",  "B",  "C",  "D",  "E",  "H",  "L" },
Q[5][3] = { "VA",  "BC",    "DE",    "HL",     "EA" },
P[5][3] = { "SP",  "BC",    "DE",    "HL",     "EA" },
RP[16][8] = { "!",  "(BC)",  "(DE)",  "(HL)",  "(DE)+",  "(HL)+",  "(DE)-", "(HL)-",
			"!",  "!",  "!",  "(DE+",  "(HL+A)",  "(HL+B)",  "(HL+EA)",  "(HL+" },
T[8][4] = { "EAH", "EAL",   "B",   "C",   "D",    "E",   "H",   "L" },
S[41][5] = { "PA",  "PB",    "PC",  "PD",  "!",    "PF",  "MKH", "MKL",
			"ANM", "SMH",   "SML", "EOM", "ETMM", "TMM", "!",   "!",
			"MM",  "MCC",   "MA",  "MB",  "MC",  "!", "!",   "MF",  "TXB",
			"RXB", "TM0",   "TM1", "!",   "!",    "!",   "!",   "CR0",
			"CR1", "CR2",   "CR3", "!",   "!",    "!",   "!",   "ZCM" },
C[16][8] = { "!",  "!",  "(DE)",  "(HL)",  "(DE)++",  "(HL)++",  "!",  "!",
			"!",  "!",  "!",  "(DE+",  "(HL+A)",  "(HL+B)",  "(HL+EA)",  "(HL+" },
F[8][3] = { "-", "!", "CY",  "HC",  "Z",  "!", "!", "!" },
I[32][5]={ "NMI","FT0","FT1","F1","F2","FE0","FE1","FEIN","FAD","FSR","FST","ER","OV",
"!","!","!","AN4","AN5","AN6","AN7","SB","!","!","!","!","!","!","!","!","!","!","!" }
	;

bool decode_data_transfert8_inst();
bool decode_data_transfert16_inst();
bool decode_arithmetic_reg8_inst();
bool decode_arithmetic_mem8_inst();
bool decode_immediate_inst();
bool decode_arithmetic_16_inst();
bool decode_working_reg_inst();
bool decode_other_inst();
bool decode_shift_rotate_inst();
bool decode_jmp_inst();
bool decode_call_inst();
bool decode_return_inst();
bool decode_skip_cpu_inst();
bool unknown_inst();
