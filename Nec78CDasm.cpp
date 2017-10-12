// Nec78CDasm.cpp : Nec D78C10 Disassembler.
//

#include "stdio.h"
#include "string.h"
#include "Nec78CDasm.h"

typedef bool(*IntFunctionWithOneParameter) (void);

IntFunctionWithOneParameter decode_inst_proc[] =
{
	decode_data_transfert8_inst ,
	decode_data_transfert16_inst,
	decode_arithmetic_reg8_inst,
	decode_arithmetic_mem8_inst,
	decode_immediate_inst,
	decode_arithmetic_16_inst,
	decode_working_reg_inst,
	decode_other_inst,
	decode_shift_rotate_inst,
	decode_jmp_inst,
	decode_call_inst,
	decode_return_inst,
	decode_skip_cpu_inst,
	unknown_inst
};

bool decode_data_transfert8_inst()
{
	// MOV r, A
	if ((inst & 0xF8) == 0x18)
	{
		sprintf(inst_asm, "MOV %s,A", T[inst & 0x7]);
		return true;
	}
	// MOV A, r
	if ((inst & 0xF8) == 0x08)
	{
		sprintf(inst_asm, "MOV %s,A", T[inst & 0x7]);
		return true;
	}
	// MOV sr, A
	if ((inst == 0x4D) && ((next_inst & 0xC0) == 0xC0) && (S[next_inst & 0x3F][0] != '!'))
	{
		sprintf(inst_asm, "MOV %s,A", S[next_inst & 0x3F]);
		sprintf(tmp_str, " %02X", next_inst); strcat(inst_hex, tmp_str); ptr++;
		return true;
	}
	// MOV A, sr
	if ((inst == 0x4C) && ((next_inst & 0xC0) == 0xC0) && (S[next_inst & 0x3F][0] != '!'))
	{
		sprintf(inst_asm, "MOV A,%s", S[next_inst & 0x3F]);
		sprintf(tmp_str, " %02X", next_inst); strcat(inst_hex, tmp_str); ptr++;
		return true;
	}
	// MOV r, Word
	if ((inst == 0x70) && ((next_inst & 0xF8) == 0x68))
	{
		sprintf(inst_asm, "MOV %s, [%02X%02X]", R[next_inst & 0x7], code[ptr + 3], code[ptr + 2]);
		sprintf(tmp_str, " %02X %02X %02X", next_inst, code[ptr + 2], code[ptr + 3]); strcat(inst_hex, tmp_str); ptr += 3;
		return true;
	}
	// MOV Word, r
	if ((inst == 0x70) && ((next_inst & 0xF8) == 0x78))
	{
		sprintf(inst_asm, "MOV [%02X%02X], %s", code[ptr + 3], code[ptr + 2], R[next_inst & 0x7]);
		sprintf(tmp_str, " %02X %02X %02X", next_inst, code[ptr + 2], code[ptr + 3]); strcat(inst_hex, tmp_str); ptr += 3;
		return true;
	}
	// MVI r, Byte
	if ((inst & 0xF8) == 0x68)
	{
		sprintf(inst_asm, "MVI %s, %02X", R[inst & 0x7], next_inst);
		sprintf(tmp_str, " %02X", next_inst); strcat(inst_hex, tmp_str); ptr++;
		return true;
	}
	// MVI sr, Byte
	if ((inst == 0x64) && ((next_inst & 0x78) == 0x0))
	{
		sprintf(inst_asm, "MVI %s, %02X", S[(next_inst & 0x7)|((next_inst>>4)&0x8)], code[ptr + 1]);
		sprintf(tmp_str, " %02X %02X", next_inst, code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
		return true;
	}
	// MVIW wa, Byte
	if (inst == 0x71)
	{
		sprintf(inst_asm, "MVIW %02X, %02X", next_inst, code[ptr + 1]);
		sprintf(tmp_str, " %02X %02X", next_inst, code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
		return true;
	}
	// MVIX rpa, Byte
	if (((inst & 0xFC) == 0x48) && (inst != 0x48))
	{
		sprintf(inst_asm, "MVIX %s, %02X", RP[inst & 0x3], next_inst);
		sprintf(tmp_str, " %02X", next_inst); strcat(inst_hex, tmp_str); ptr++;
		return true;
	}
	// STAW wa, Byte
	if (inst == 0x63)
	{
		sprintf(inst_asm, "STAW %02X", next_inst);
		sprintf(tmp_str, " %02X", next_inst); strcat(inst_hex, tmp_str); ptr++;
		return true;
	}
	// LDAW wa, Byte
	if (inst == 0x01)
	{
		sprintf(inst_asm, "LDAW %02X", next_inst);
		sprintf(tmp_str, " %02X", next_inst); strcat(inst_hex, tmp_str); ptr++;
		return true;
	}
	// STAX rp
	rp = (inst & 0x7) | ((inst >> 4) & 0x8);
	if (((inst & 0x78) == 0x38) && (RP[rp][0] != '!'))
	{
		
		sprintf(inst_asm, "STAX %s", RP[rp]);
		if (rp == 0xB || rp == 0xF)
		{
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			strcat(inst_asm, tmp_str); strcat(inst_asm, ")");
		}
		return true;
	}
	// LDAX
	if (((inst & 0x78) == 0x28) && (RP[rp][0] != '!'))
	{
		rp = (inst & 0x7) | ((inst >> 4) & 0x8);
		sprintf(inst_asm, "LDAX %s", RP[rp]);
		if (rp == 0xB || rp == 0xF)
		{
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			strcat(inst_asm, tmp_str); strcat(inst_asm, ")");
		}
		return true;
	}
	// EXX, EXA, EXH
	switch (inst)
	{
	case 0x11: // EXX
		sprintf(inst_asm, "EXX");
		return true;
	case 0x10: // EXA
		sprintf(inst_asm, "EXA");
		return true;
	case 0x50: // EXH
		sprintf(inst_asm, "EXH");
		return true;
	}
	return false;
}
bool decode_data_transfert16_inst()
{
	// BLOCK
	if (inst == 0x31)
	{
		sprintf(inst_asm, "BLOCK");
		return true;
	}
	// DMOV rp, EA
	if ((inst & 0xFC) == 0xB4)
	{
		sprintf(inst_asm, "DMOV %s, EA", P[inst & 0x3]);
		return true;
	}
	// DMOV EA, rp
	if ((inst & 0xFC) == 0xA4)
	{
		sprintf(inst_asm, "DMOV EA, %s", P[inst & 0x3]);
		return true;
	}
	// DMOV sr, EA
	if ((inst == 0x48) && ((next_inst & 0xFE) == 0xD2))
	{
		sprintf(inst_asm, "DMOV %s, EA", U[next_inst & 0x1]);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	}
	// DMOV EA, sr
	// Note See Document 1987 pp4-92 (inst format = 0100 1000 1100 00V1V2 ?!)
	// OK corrected in the next version
	if ((inst == 0x48) && ((next_inst & 0xFE) == 0xC0))
	{
		sprintf(inst_asm, "DMOV EA, %s", V[next_inst & 0x1]);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	}
	// SBCD, SDED, SHLD, SSPD, LBCD, LDED, LHLD, LSPD
	if (inst == 0x70)
		switch (next_inst)
		{
		case 0x1E: // SBCD
			sprintf(inst_asm, "SBCD [%02X%02X]", code[ptr + 1], code[ptr]);
			sprintf(tmp_str, " %02X %02X %02X", code[ptr], code[ptr + 1], code[ptr + 2]);
			ptr += 3; strcat(inst_hex, tmp_str);
			return true;
		case 0x2E: // SDED
			sprintf(inst_asm, "SDED [%02X%02X]", code[ptr + 1], code[ptr]);
			sprintf(tmp_str, " %02X %02X %02X", code[ptr], code[ptr + 1], code[ptr + 2]);
			ptr += 3; strcat(inst_hex, tmp_str);
			return true;
		case 0x3E: // SHLD
			sprintf(inst_asm, "SHLD [%02X%02X]", code[ptr + 1], code[ptr]);
			sprintf(tmp_str, " %02X %02X %02X", code[ptr], code[ptr + 1], code[ptr + 2]);
			ptr += 3; strcat(inst_hex, tmp_str);
			return true;
		case 0x0E: // SSPD
			sprintf(inst_asm, "SSPD [%02X%02X]", code[ptr + 1], code[ptr]);
			sprintf(tmp_str, " %02X %02X %02X", code[ptr], code[ptr + 1], code[ptr + 2]);
			ptr += 3; strcat(inst_hex, tmp_str);
			return true;
		case 0x1F: // LBCD
			sprintf(inst_asm, "LBCD [%02X%02X]", code[ptr + 1], code[ptr]);
			sprintf(tmp_str, " %02X %02X %02X", code[ptr], code[ptr + 1], code[ptr + 2]);
			ptr += 3; strcat(inst_hex, tmp_str);
			return true;
		case 0x2F: // LDED
			sprintf(inst_asm, "LDED [%02X%02X]", code[ptr + 1], code[ptr]);
			sprintf(tmp_str, " %02X %02X %02X", code[ptr], code[ptr + 1], code[ptr + 2]);
			ptr += 3; strcat(inst_hex, tmp_str);
			return true;
		case 0x3F: // LHLD
			sprintf(inst_asm, "LHLD [%02X%02X]", code[ptr + 1], code[ptr]);
			sprintf(tmp_str, " %02X %02X %02X", code[ptr], code[ptr + 1], code[ptr + 2]);
			ptr += 3; strcat(inst_hex, tmp_str);
			return true;
		case 0x0F: // LSPD
			sprintf(inst_asm, "LSPD [%02X%02X]", code[ptr + 1], code[ptr]);
			sprintf(tmp_str, " %02X %02X %02X", code[ptr], code[ptr + 1], code[ptr + 2]);
			ptr += 3; strcat(inst_hex, tmp_str);
			return true;
		}
	// STEAX rp
	if ((inst == 0x48) && ((next_inst & 0xF0) == 0x90))
	{
		rp = (next_inst & 0xF);
		sprintf(inst_asm, "STEAX %s", C[rp]);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		if (rp == 0xB || rp == 0xF)
		{
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			strcat(inst_asm, tmp_str); strcat(inst_asm, ")");
		}
		return true;
	}
	// LDEAX rp
	rp = (next_inst & 0xF);
	if ((inst == 0x48) && ((next_inst & 0xF0) == 0x80) && (C[rp][0] != '!'))
	{
		sprintf(inst_asm, "LDEAX %s", C[rp]);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		if (rp == 0xB || rp == 0xF)
		{
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			strcat(inst_asm, tmp_str); strcat(inst_asm, ")");
		}
		return true;
	}
	// PUSH
	if (((inst & 0xF8) == 0xB0) && (inst <= 0xB4))
	{
		sprintf(inst_asm, "PUSH %s", Q[inst & 0x7]);
		return true;
	}
	// POP
	if (((inst & 0xF8) == 0xA0) && (inst <= 0xB4))
	{
		sprintf(inst_asm, "POP %s", Q[inst & 0x7]);
		return true;
	}
	// LXI
	if (((inst & 0x8F) == 0x04) && ((inst & 0x70) <= 0x40))
	{
		sprintf(inst_asm, "LXI %s, %02X%02X", P[inst >> 4], code[ptr + 1], code[ptr]);
		sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]);
		strcat(inst_hex, tmp_str); ptr += 2;
		return true;
	}
	// TABLE
	if ((inst == 0x48) && (next_inst == 0xA8))
	{
		sprintf(inst_asm, "TABLE");
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	}
	return false;
}
bool decode_arithmetic_reg8_inst()
{
	// ADD, ADC, ADDNC, SUB, SBB, SUBNB, ANA, ORA, XRA, GTA, LTA, NEA, EQA, ONA, OFFA
	if (inst == 0x60)
		switch ((next_inst & 0xF8))
		{
		case 0xC0: // ADD A, r
			sprintf(inst_asm, "ADD A, %s", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x40: // ADD r, A
			sprintf(inst_asm, "ADD %s, A", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xD0: // ADC A, r
			sprintf(inst_asm, "ADC A, %s", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x50: // ADC r, A
			sprintf(inst_asm, "ADC %s, A", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xA0: // ADDNC A, r
			sprintf(inst_asm, "ADDNC A, %s", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x20: // ADDNC r, A
			sprintf(inst_asm, "ADDNC %s, A", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xE0: // SUB A, r
			sprintf(inst_asm, "SUB A, %s", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x60: // SUB r, A
			sprintf(inst_asm, "SUB %s, A", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xF0: // SBB A, r
			sprintf(inst_asm, "SBB A, %s", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x70: // SBB r, A
			sprintf(inst_asm, "SBB %s, A", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xB0: // SUBNB A, r
			sprintf(inst_asm, "SUBNB A, %s", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x30: // SUBNB r, A
			sprintf(inst_asm, "SUBNB %s, A", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x88: // ANA A, r
			sprintf(inst_asm, "ANA A, %s", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x08: // ANA r, A
			sprintf(inst_asm, "ANA %s, A", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x98: // ORA A, r
			sprintf(inst_asm, "ORA A, %s", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x18: // ORA r, A
			sprintf(inst_asm, "ORA %s, A", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x90: // XRA A, r
			sprintf(inst_asm, "XRA A, %s", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x10: // XRA r, A
			sprintf(inst_asm, "XRA %s, A", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xA8: // GTA A, r
			sprintf(inst_asm, "GTA A, %s", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x28: // GTA r, A
			sprintf(inst_asm, "GTA %s, A", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xB8: // LTA A, r
			sprintf(inst_asm, "LTA A, %s", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x38: // LTA r, A
			sprintf(inst_asm, "LTA %s, A", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xE8: // NEA A, r
			sprintf(inst_asm, "NEA A, %s", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x68: // NEA r, A
			sprintf(inst_asm, "NEA %s, A", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xF8: // EQA A, r
			sprintf(inst_asm, "EQA A, %s", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x78: // EQA r, A
			sprintf(inst_asm, "EQA %s, A", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xC8: // ONA r, A
			sprintf(inst_asm, "ONA A, %s", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xD8: // OFFA r, A
			sprintf(inst_asm, "OFFA A, %s", R[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		}
	return false;
}
bool decode_arithmetic_mem8_inst()
{
	// ADDX, ADCX, ADDNC, SUB, SBB, SUBNB, ANA, ORA, XRA, GTA, LTA, NEA, EQA, ONA, OFFA
	if (inst == 0x70)
		switch ((next_inst & 0xF8))
		{
		case 0xC0: // ADDX rp
			sprintf(inst_asm, "ADDX %s", RP[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xD0: // ADCX rp
			sprintf(inst_asm, "ADCX %s", RP[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xA0: // ADNCX rp
			sprintf(inst_asm, "ADNCX %s", RP[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xE0: // SUBX rp
			sprintf(inst_asm, "SUBX %s", RP[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xF0: // SBBX rp
			sprintf(inst_asm, "SBBX %s", RP[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xB0: // SUBNBX rp
			sprintf(inst_asm, "SUBNBX %s", RP[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x88: // ANAX rp
			sprintf(inst_asm, "ANAX %s", RP[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x98: // ORAX rp
			sprintf(inst_asm, "ORAX %s", RP[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x90: // XRAX rp
			sprintf(inst_asm, "XRAX %s", RP[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xA8: // GTAX rp
			sprintf(inst_asm, "GTAX %s", RP[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xB8: // LTAX rp
			sprintf(inst_asm, "LTAX %s", RP[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xE8: // NEAX rp
			sprintf(inst_asm, "NEAX %s", RP[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xF8: // EQAX rp
			sprintf(inst_asm, "EQAX %s", RP[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xC8: // ONAX rp
			sprintf(inst_asm, "ONAX %s", RP[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xD8: // OFFAX rp
			sprintf(inst_asm, "OFFAX %s", RP[next_inst & 0x7]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		}
	return false;
}
bool decode_immediate_inst()
{
	// ADI, ACI, ADINC, SUI, SBI
	switch (inst)
	{
	case 0x46: // ADI
		sprintf(inst_asm, "ADI A, %02X", next_inst);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	case 0x56: // ACI
		sprintf(inst_asm, "ACI A, %02X", next_inst);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	case 0x26: // ADINC
		sprintf(inst_asm, "ADINC A, %02X", next_inst);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	case 0x66: // SUI
		sprintf(inst_asm, "SUI A, %02X", next_inst);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	case 0x76: // SBI
		sprintf(inst_asm, "SBI A, %02X", next_inst);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	case 0x36: // SUINB
		sprintf(inst_asm, "SUINB A, %02X", next_inst);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	case 0x07: // ANI
		sprintf(inst_asm, "ANI A, %02X", next_inst);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	case 0x17: // ORI
		sprintf(inst_asm, "ORI A, %02X", next_inst);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	case 0x16: // XRI
		sprintf(inst_asm, "XRI A, %02X", next_inst);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	case 0x27: // GTI
		sprintf(inst_asm, "GTI A, %02X", next_inst);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	case 0x37: // LTI
		sprintf(inst_asm, "LTI A, %02X", next_inst);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	case 0x67: // NEI
		sprintf(inst_asm, "NEI A, %02X", next_inst);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	case 0x77: // EQI
		sprintf(inst_asm, "EQI A, %02X", next_inst);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	case 0x47: // ONI
		sprintf(inst_asm, "ONI A, %02X", next_inst);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	case 0x57: // OFFI
		sprintf(inst_asm, "OFFI A, %02X", next_inst);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	}
	if (inst == 0x74)
		switch (next_inst & 0xF8)
		{
		case 0x40: // ADI
			sprintf(inst_asm, "ADI %s, %02X", R[next_inst & 0x7], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x50: // ACI
			sprintf(inst_asm, "ACI %s, %02X", R[next_inst & 0x7], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x20: // ADINC
			sprintf(inst_asm, "ADINC %s, %02X", R[next_inst & 0x7], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x60: // SUI
			sprintf(inst_asm, "SUI %s, %02X", R[next_inst & 0x7], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x70: // SBI
			sprintf(inst_asm, "SBI %s, %02X", R[next_inst & 0x7], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x30: // SUINB
			sprintf(inst_asm, "SUINB %s, %02X", R[next_inst & 0x7], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x08: // ANI
			sprintf(inst_asm, "ANI %s, %02X", R[next_inst & 0x7], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x18: // ORI
			sprintf(inst_asm, "ORI %s, %02X", R[next_inst & 0x7], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x10: // XRI
			sprintf(inst_asm, "XRI %s, %02X", R[next_inst & 0x7], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x28: // GTI
			sprintf(inst_asm, "GTI %s, %02X", R[next_inst & 0x7], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x38: // LTI
			sprintf(inst_asm, "LTI %s, %02X", R[next_inst & 0x7], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x68: // NEI
			sprintf(inst_asm, "NEI %s, %02X", R[next_inst & 0x7], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x78: // EQI
			sprintf(inst_asm, "EQI %s, %02X", R[next_inst & 0x7], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x48: // ONI
			sprintf(inst_asm, "ONI %s, %02X", R[next_inst & 0x7], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x58: // OFFI
			sprintf(inst_asm, "OFFI %s, %02X", R[next_inst & 0x7], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		}
	rp = (next_inst & 0x7) | ((next_inst >> 4) & 0x8);
	if (inst == 0x64) //&& (S[rp][0] != '!'))
		switch (next_inst & 0x78)
		{
		case 0x40: // ADI
			sprintf(inst_asm, "ADI %s, %02X", S[rp], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x50: // ACI
			sprintf(inst_asm, "ACI %s, %02X", S[rp], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x20: // ADINC
			sprintf(inst_asm, "ADINC %s, %02X", S[rp], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x60: // SUI
			sprintf(inst_asm, "SUI %s, %02X", S[rp], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x70: // SBI
			sprintf(inst_asm, "SBI %s, %02X", S[rp], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x30: // SUINB
			sprintf(inst_asm, "SUINB %s, %02X", S[rp], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x08: // ANI
			sprintf(inst_asm, "ANI %s, %02X", S[rp], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x18: // ORI
			sprintf(inst_asm, "ORI %s, %02X", S[rp], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x10: // XRI
			sprintf(inst_asm, "XRI %s, %02X", S[rp], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x28: // GTI
			sprintf(inst_asm, "GTI %s, %02X", S[rp], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x38: // LTI
			sprintf(inst_asm, "LTI %s, %02X", S[rp], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x68: // NEI
			sprintf(inst_asm, "NEI %s, %02X", S[rp], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x78: // EQI
			sprintf(inst_asm, "EQI %s, %02X", S[rp], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x48: // ONI
			sprintf(inst_asm, "ONI %s, %02X", S[rp], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		case 0x58: // OFFI
			sprintf(inst_asm, "OFFI %s, %02X", S[rp], code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
			return true;
		}
	return false;
}

bool decode_arithmetic_16_inst()
{
	// EADD, ESUB
	if (inst == 0x70)
		switch (next_inst & 0xFC)
		{
		case 0x40: // EADD
			sprintf(inst_asm, "EADD EA, %s", R[next_inst & 0x3]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x60: // ESUB
			sprintf(inst_asm, "ESUB EA, %s", R[next_inst & 0x3]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		}
	if (inst == 0x74)
		switch (next_inst & 0xFC)
		{
		case 0xC4: // DADD
			sprintf(inst_asm, "DADD EA, %s", RP[next_inst & 0x3]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xD4: // DADC
			sprintf(inst_asm, "DADC EA, %s", RP[next_inst & 0x3]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xA4: // DADDNC
			sprintf(inst_asm, "DADDNC EA, %s", RP[next_inst & 0x3]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xE4: // DSUB
			sprintf(inst_asm, "DSUB EA, %s", RP[next_inst & 0x3]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xF4: // DSBB
			sprintf(inst_asm, "DSBB EA, %s", RP[next_inst & 0x3]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xB4: // DSUBNB
			sprintf(inst_asm, "DSUBNB EA, %s", RP[next_inst & 0x3]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x8C: // DAN
			sprintf(inst_asm, "DAN EA, %s", RP[next_inst & 0x3]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x9C: // DOR
			sprintf(inst_asm, "DOR EA, %s", RP[next_inst & 0x3]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x94: // DXR
			sprintf(inst_asm, "DXR EA, %s", RP[next_inst & 0x3]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xAC: // DGT
			sprintf(inst_asm, "DGT EA, %s", RP[next_inst & 0x3]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xBC: // DLT
			sprintf(inst_asm, "DLT EA, %s", RP[next_inst & 0x3]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xEC: // DNE
			sprintf(inst_asm, "DNE EA, %s", RP[next_inst & 0x3]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xFC: // DEQ
			sprintf(inst_asm, "DEQ EA, %s", RP[next_inst & 0x3]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xCC: // DON
			sprintf(inst_asm, "DON EA, %s", RP[next_inst & 0x3]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xDC: // DOFF
			sprintf(inst_asm, "DOFF EA, %s", RP[next_inst & 0x3]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		}
	return false;
}
bool decode_working_reg_inst()
{
	// ADDW, ADCW, ADDNCW, SUBW, SBBW, SBBNBW, ANAW, ORAW, XRAW, GTAW, LTAW, NEAW, EQAW, ONAW, OFFAW
	if (inst == 0x74)
		switch (next_inst)
		{
		case 0xC0: // ADDW
			sprintf(inst_asm, "ADDW %02X", code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr++;
			return true;
		case 0xD0: // ADCW
			sprintf(inst_asm, "ADCW %02X", code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr++;
			return true;
		case 0xA0: // ADDNCW
			sprintf(inst_asm, "ADDNCW %02X", code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr++;
			return true;
		case 0xE0: // SUBW
			sprintf(inst_asm, "SUBW %02X", code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr++;
			return true;
		case 0xF0: // SBBW
			sprintf(inst_asm, "SBBW %02X", code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr++;
			return true;
		case 0xB0: // SBBNBW
			sprintf(inst_asm, "SBBNBW %02X", code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr++;
			return true;
		case 0x88: // ANAW
			sprintf(inst_asm, "ANAW %02X", code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr++;
			return true;
		case 0x98: // ORAW 98
			sprintf(inst_asm, "ORAW %02X", code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr++;
			return true;
		case 0x90: // XRAW 90
			sprintf(inst_asm, "XRAW %02X", code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr++;
			return true;
		case 0xA8: // GTAW
			sprintf(inst_asm, "GTAW %02X", code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr++;
			return true;
		case 0xB8: // LTAW
			sprintf(inst_asm, "LTAW %02X", code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr++;
			return true;
		case 0xE8: // NEAW
			sprintf(inst_asm, "NEAW %02X", code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr++;
			return true;
		case 0xF8: // EQAW
			sprintf(inst_asm, "EQAW %02X", code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr++;
			return true;
		case 0xC8: // ONAW
			sprintf(inst_asm, "ONAW %02X", code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr++;
			return true;
		case 0xD8: // OFFAW D8
			sprintf(inst_asm, "OFFAW %02X", code[ptr + 1]);
			sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr++;
			return true;
		}
	switch (inst)
	{
	case 0x05: // ANIW
		sprintf(inst_asm, "ANIW %02X, %02X", code[ptr], code[ptr + 1]);
		sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr++;
		return true;
	case 0x15: // ORIW
		sprintf(inst_asm, "ORIW %02X, %02X", code[ptr], code[ptr + 1]);
		sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr++;
		return true;
	case 0x25: // GTIW
		sprintf(inst_asm, "GTIW %02X, %02X", code[ptr], code[ptr + 1]);
		sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr++;
		return true;
	case 0x35: // LTIW
		sprintf(inst_asm, "LTIW %02X, %02X", code[ptr], code[ptr + 1]);
		sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr++;
		return true;
	case 0x65: // NEIW
		sprintf(inst_asm, "NEIW %02X, %02X", code[ptr], code[ptr + 1]);
		sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr++;
		return true;
	case 0x75: // EQIW
		sprintf(inst_asm, "EQIW %02X, %02X", code[ptr], code[ptr + 1]);
		sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr++;
		return true;
	case 0x45: // ONIW
		sprintf(inst_asm, "ONIW %02X, %02X", code[ptr], code[ptr + 1]);
		sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr++;
		return true;
	case 0x55: // OFFIW
		sprintf(inst_asm, "OFFIW %02X, %02X", code[ptr], code[ptr + 1]);
		sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr++;
		return true;
	}
	return false;
}
bool decode_other_inst()
{
	// MUL
	if ((inst == 0x48) && ((next_inst & 0xFC) == 0x2C))
	{
		sprintf(inst_asm, "MUL %s", R[next_inst & 0x3]);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	}
	// DIV
	if ((inst == 0x48) && ((next_inst & 0xFC) == 0x3C))
	{
		sprintf(inst_asm, "DIV %s", R[next_inst & 0x3]);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	}
	// INR
	if (((inst & 0xFC) == 0x40) && (inst != 0x40))
	{
		sprintf(inst_asm, "INR %s", R[inst & 0x3]);
		return true;
	}
	// INRW
	if (inst == 0x20)
	{
		sprintf(inst_asm, "INRW %02X", next_inst);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	}
	// INX
	if ((inst & 0xCF) == 0x02)
	{
		sprintf(inst_asm, "INX %s", P[(inst >> 4)]);
		return true;
	}
	// INX
	if (inst == 0xA8)
	{
		sprintf(inst_asm, "INX EA");
		return true;
	}
	// DCR
	if ((inst & 0xFC) == 0x50)
	{
		sprintf(inst_asm, "DCR %s", R[inst & 0x3]);
		return true;
	}
	// DCRW
	if (inst == 0x30)
	{
		sprintf(inst_asm, "DCRW %02X", next_inst);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	}
	// DCX
	if ((inst & 0xCF) == 0x03)
	{
		sprintf(inst_asm, "DCX %s", P[(inst >> 4)]);
		return true;
	}
	// DCX
	if (inst == 0xA9)
	{
		sprintf(inst_asm, "DCX EA");
		return true;
	}
	// DAA
	if (inst == 0x61)
	{
		sprintf(inst_asm, "DAA");
		return true;
	}
	// STC
	if ((inst == 0x48) && (next_inst == 0x2B))
	{
		sprintf(inst_asm, "STC");
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	}
	// CLC
	if ((inst == 0x48) && (next_inst == 0x2A))
	{
		sprintf(inst_asm, "CLC");
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	}
	// NEGA
	if ((inst == 0x48) && (next_inst == 0x3A))
	{
		sprintf(inst_asm, "NEGA");
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	}
	return false;
}
bool decode_shift_rotate_inst()
{
	if (inst == 0x48)
	{
		switch (next_inst)
		{
		case 0x38: // RLD
			sprintf(inst_asm, "RLD");
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x39: // RRD
			sprintf(inst_asm, "RRD");
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xB4: // DRLL EA
			sprintf(inst_asm, "DRLL EA");
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xB0: // DRLR EA
			sprintf(inst_asm, "DRLR EA");
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xA4: // DSLL EA
			sprintf(inst_asm, "DSLL EA");
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0xA0: // DSLR EA
			sprintf(inst_asm, "DSLR EA");
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		}
		switch (next_inst & 0xFC)
		{
		case 0x34: // RLL r
			sprintf(inst_asm, "RLL %s", R[next_inst & 0x3]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x30: // RLR r
			sprintf(inst_asm, "RLR %s", R[next_inst & 0x3]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x24: // SLL r
			sprintf(inst_asm, "SLL %s", R[next_inst & 0x3]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x20: // SLR r
			sprintf(inst_asm, "SLR %s", R[next_inst & 0x3]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x04: // SLLC r
			sprintf(inst_asm, "SLLC %s", R[next_inst & 0x3]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		case 0x00: // SLRC r
			sprintf(inst_asm, "SLRC %s", R[next_inst & 0x3]);
			sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
			return true;
		}
	}
	return false;
}
bool decode_jmp_inst()
{
	// JMP
	if (inst == 0x54)
	{
		sprintf(inst_asm, "JMP %02X%02X", code[ptr + 1], code[ptr]);
		sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
		return true;
	}
	// JB
	if (inst == 0x21)
	{
		sprintf(inst_asm, "JB");
		return true;
	}
	// JR
	if ((inst & 0xC0) == 0xC0)
	{
		if ((inst & 0x20) == 0x20)
			sprintf(inst_asm, "JR %05X", org_code + ptr + (inst & 0x3F) - 0x40);
		else
			sprintf(inst_asm, "JR %05X", org_code + ptr + (inst & 0x3F));
		return true;
	}
	// JRE
	if ((inst & 0xFE) == 0x4E)
	{
		ptr++;
		if ((inst & 0x1) == 0x1)
			sprintf(inst_asm, "JRE %05X", org_code + ptr + next_inst - 0x100);
		else
			sprintf(inst_asm, "JRE %05X", org_code + ptr + next_inst);
		sprintf(tmp_str, " %02X", next_inst); strcat(inst_hex, tmp_str);
		return true;
	}
	// JEA
	if ((inst == 0x48) && (next_inst == 0x28))
	{
		sprintf(inst_asm, "JEA");
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	}
	return false;
}
bool decode_call_inst()
{
	// CALL
	if (inst == 0x40)
	{
		sprintf(inst_asm, "CALL %02X%02X", code[ptr + 1], code[ptr]);
		sprintf(tmp_str, " %02X %02X", code[ptr], code[ptr + 1]); strcat(inst_hex, tmp_str); ptr += 2;
		return true;
	}
	// CALB
	if ((inst == 0x48) && (next_inst == 0x29))
	{
		sprintf(inst_asm, "CALB");
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	}
	// CALF
	if ((inst & 0xF8) == 0x78)
	{
		sprintf(inst_asm, "CALF %02X%02X", ((inst & 7) | 0x8), next_inst);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	}
	// CALT
	if ((inst & 0xE0) == 0x80)
	{
		sprintf(inst_asm, "CALT %d", 128 + ((inst & 0x1F) << 1));
		return true;
	}
	// SOFTI
	if (inst == 0x72)
	{
		sprintf(inst_asm, "SOFTI");
		return true;
	}
	return false;
}
bool decode_return_inst()
{
	// RET
	switch(inst)
	{
	case 0xB8: // RET
		sprintf(inst_asm, "RET");
		return true;
	case 0xB9: // RETS
		sprintf(inst_asm, "RETS");
		return true;
	case 0x62: // RETI
		sprintf(inst_asm, "RETI");
		return true;
	}
	return false;
}
bool decode_skip_cpu_inst()
{
	// BIT
	if ((inst & 0xF8) == 0x58)
	{
		sprintf(inst_asm, "BIT %d, %02X", inst & 0x7, code[ptr]);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	}
	// SK
	if ((inst == 0x48) && ((next_inst & 0xF8) == 0x08) && (F[next_inst & 0xF8][0] != '!'))
	{
		sprintf(inst_asm, "SK %s", F[next_inst & 0x7]);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	}
	// SKN
	if ((inst == 0x48) && ((next_inst & 0xF8) == 0x18))
	{
		sprintf(inst_asm, "SKN %s", F[next_inst & 0x7]);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	}
	// SKIT
	if ((inst == 0x48) && ((next_inst & 0xE0) == 0x40) && (I[next_inst & 0x1F][0] != '!'))
	{
		sprintf(inst_asm, "SKIT %s", I[next_inst & 0x1F]);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	}
	// SKNIT
	if ((inst == 0x48) && ((next_inst & 0xE0) == 0x60))
	{
		sprintf(inst_asm, "SKNIT %s", I[next_inst & 0x1F]);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	}
	// NOP
	if (inst == 0x00)
	{
		sprintf(inst_asm, "NOP");
		return true;
	}
	// EI
	if (inst == 0xAA)
	{
		sprintf(inst_asm, "EI");
		return true;
	}
	// DI
	if (inst == 0xBA)
	{
		sprintf(inst_asm, "DI");
		return true;
	}
	// HLT
	if ((inst == 0x48)&& (next_inst == 0x3B))
	{
		sprintf(inst_asm, "HLT");
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return true;
	}
	return false;
}

bool unknown_inst()
{
	sprintf(inst_asm, "*");
	return true;
}
/*
void decode_inst()
{
	//
	//printf("\norg=%x ptr=%x", org, ptr);
	/*inst_asm[0] = inst_hex[0] = 0;
	inst = code[ptr];
	sprintf(inst_hex, "%05X: %02X", org + ptr++, inst);* /
	switch (inst & 0xF8)
	{
	case 0x68:// MVI
		sprintf(inst_asm, "MVI %s,%02X", R[inst & 0x7], code[ptr]);
		sprintf(tmp_str, " %02X", code[ptr++]); strcat(inst_hex, tmp_str);
		return;break;
	}
	if ((inst & 0xE8) == 0x08)
	{
		// MOV
		if (inst & 0x10)
			sprintf(inst_asm, "MOV %s, A", T[inst & 0x7]);
		else
			sprintf(inst_asm, "MOV A, %s", T[inst & 0x7]);
		return;
	}
	//decode_other_inst();
}
*/

bool read_next_inst()
{
	if (ptr >= len_code-1) return false;
	inst_asm[0] = inst_hex[0] = 0;
	inst = code[ptr];
	next_inst = code[ptr + 1];
	sprintf(inst_hex, "%05X: %02X", org_code + ptr++, inst);
	return true;
}

int main(int argc, char *argv[])
{
	printf("\nNEC D87C10 disassembler v 1.0 beta");
	printf("\nCopyLeft (c) 2016 by Farouk MEDDAH \n");
	if (argc >= 2)
		bin_file = fopen(argv[1], "rb");
	else
	    printf("\nNom de fichier bin manquant.\n");
		//bin_file = fopen("D:\\Projects\\CPP\\Nec78CDasm\\Debug\\d.bin", "rb");
	do {
		printf("\n>");
		gets(s);
		org_code = end_code = 0;
		sscanf(s, "%s %x %x", cmd, &org_code, &end_code);
		if (end_code != 0)
			len_code = (end_code - org_code);
		else
			len_code = 0x100;
		switch (s[0])
		{
		case 'h':
			printf("\nhelp:");
			printf("\n  h         this help");
			printf("\n  f file    open file");
			printf("\n  d addr    meory dump");
			printf("\n  u addr    disassemble");
			printf("\n  q         quit program");
			printf("\n");
			break;
		case 'f':
	        if (bin_file != NULL)
                fclose(bin_file);
		    sscanf(s, "%s %s", cmd, inst_asm);
		    if(strlen(inst_asm)!=0)
                bin_file = fopen(inst_asm, "rb");
		    break;
		case 'd':
			fseek( bin_file, org_code, SEEK_SET);
			fread(code, len_code, 1, bin_file);
			if ((org_code & 0xF) != 0)
			{
				printf("\n%05X:", (org_code & 0xFFFFF0));
				for (int j = 0; j < 16; j++)
				{
					if (j<(org_code & 0xF))
						if (j != 8)
							printf("   ", dig);
						else printf("-  ", dig);
					else
					{
						len_code--;
						dig = code[(org_code & 0xF) + j];
						if (j == 8) printf("-%02X", dig);
						else printf(" %02X", dig);
					}
				}
				org_code = (org_code & 0xFFFFF0) + 0x10;
			}
			for (int i = 0; (len_code>0); i++)
			{
				printf("\n%05X:", org_code+i*16);
				for (int j = 0;(len_code>0) && (j < 16); j++, len_code--)
				{
					dig = code[16 * i + j];
					if (j == 8) printf("-%02X", dig);
					else printf(" %02X", dig);
				}
			}
			break;
		case 'u':
			ptr = 0;
			fseek(bin_file, org_code, SEEK_SET);
			fread(code, len_code, 1, bin_file);
			do {
				if(!read_next_inst()) break;
				for (int i = 0; (i < 14) && (!decode_inst_proc[i]()); i++);
				for (int i = 24 - strlen(inst_hex); i > 0; i--)
					strcat(inst_hex, " ");
				printf("\n%s%s", inst_hex, inst_asm);
			} while (ptr < len_code);
			break;
		}
	} while (s[0]!='q');
	if (bin_file != NULL)
		fclose(bin_file);
    return 0;
}
