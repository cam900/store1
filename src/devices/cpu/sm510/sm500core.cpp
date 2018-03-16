// license:BSD-3-Clause
// copyright-holders:hap
/*

  Sharp SM500 MCU core implementation

  TODO:
  - EXKSA, EXKFA opcodes
  - unknown which O group is which W output, guessed for now (segments and H should be ok)

*/

#include "emu.h"
#include "sm500.h"
#include "sm510d.h"
#include "debugger.h"


// MCU types
DEFINE_DEVICE_TYPE(SM500, sm500_device, "sm500", "Sharp SM500") // 1.2K ROM, 4x10x4 RAM, shift registers for LCD


// internal memory maps
ADDRESS_MAP_START(sm500_device::program_1_2k)
	AM_RANGE(0x000, 0x4bf) AM_ROM
ADDRESS_MAP_END

ADDRESS_MAP_START(sm500_device::data_4x10x4)
	AM_RANGE(0x00, 0x09) AM_RAM
	AM_RANGE(0x10, 0x19) AM_RAM
	AM_RANGE(0x20, 0x29) AM_RAM
	AM_RANGE(0x30, 0x39) AM_RAM
ADDRESS_MAP_END


// device definitions
sm500_device::sm500_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
	: sm500_device(mconfig, SM500, tag, owner, clock, 1 /* stack levels */, 7 /* o group pins */, 11 /* prg width */, address_map_constructor(FUNC(sm500_device::program_1_2k), this), 6 /* data width */, address_map_constructor(FUNC(sm500_device::data_4x10x4), this))
{
}

sm500_device::sm500_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, u32 clock, int stack_levels, int o_pins, int prgwidth, address_map_constructor program, int datawidth, address_map_constructor data)
	: sm510_base_device(mconfig, type, tag, owner, clock, stack_levels, prgwidth, program, datawidth, data),
	m_write_o(*this),
	m_o_pins(o_pins)
{
}


// disasm
util::disasm_interface *sm500_device::create_disassembler()
{
	return new sm500_disassembler;
}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void sm500_device::device_start()
{
	// common init (not everything is used though)
	sm510_base_device::device_start();

	// resolve callbacks
	m_write_o.resolve_safe();

	// init/zerofill
	memset(m_ox, 0, sizeof(m_ox));
	memset(m_o, 0, sizeof(m_o));
	m_cn = 0;
	m_mx = 0;
	m_cb = 0;
	m_s = 0;
	m_rsub = false;

	// register for savestates
	save_item(NAME(m_ox));
	save_item(NAME(m_o));
	save_item(NAME(m_cn));
	save_item(NAME(m_mx));
	save_item(NAME(m_cb));
	save_item(NAME(m_s));
	save_item(NAME(m_rsub));
}



//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void sm500_device::device_reset()
{
	// common reset
	sm510_base_device::device_reset();

	// SM500 specific
	push_stack();
	op_idiv();
	m_1s = true;
	m_cb = 0;
	m_rsub = false;
	m_r = 0xf;
}



//-------------------------------------------------
//  lcd driver
//-------------------------------------------------

void sm500_device::lcd_update()
{
	// 2 columns
	for (int h = 0; h < 2; h++)
	{
		for (int o = 0; o < m_o_pins; o++)
		{
			// 4 segments per group
			u8 seg = h ? m_ox[o] : m_o[o];
			m_write_o(o << 1 | h, m_bp ? seg : 0, 0xff);
		}
	}
}



//-------------------------------------------------
//  buzzer controller
//-------------------------------------------------

void sm500_device::clock_melody()
{
	// R1 from divider or direct control, R2-R4 generic outputs
	u8 mask = (m_r_mask_option == SM510_R_CONTROL_OUTPUT) ? 1 : (m_div >> m_r_mask_option & 1);
	u8 out = (mask & ~m_r) | (~m_r & 0xe);

	// output to R pins
	if (out != m_r_out)
	{
		m_write_r(0, out, 0xff);
		m_r_out = out;
	}
}



//-------------------------------------------------
//  execute
//-------------------------------------------------

void sm500_device::get_opcode_param()
{
	// LBL and prefix opcodes are 2 bytes
	if (m_op == 0x5e || m_op == 0x5f)
	{
		m_icount--;
		m_param = m_program->read_byte(m_pc);
		increment_pc();
	}
}

void sm500_device::execute_one()
{
	switch (m_op & 0xf0)
	{
		case 0x20: op_lax(); break;
		case 0x30:
			if (m_op == 0x30) op_ats(); // !
			else op_adx();
			break;
		case 0x40: op_lb(); break;
		case 0x70: op_ssr(); break;

		case 0x80: case 0x90: case 0xa0: case 0xb0:
			op_tr(); break;
		case 0xc0: case 0xd0: case 0xe0: case 0xf0:
			op_trs(); break;

		default:
			switch (m_op & 0xfc)
			{
		case 0x04: op_rm(); break;
		case 0x0c: op_sm(); break;
		case 0x10: op_exc(); break;
		case 0x14: op_exci(); break;
		case 0x18: op_lda(); break;
		case 0x1c: op_excd(); break;
		case 0x54: op_tmi(); break; // TM

		default:
			switch (m_op)
			{
		case 0x00: op_skip(); break;
		case 0x01: op_atr(); break;
		case 0x02: op_exksa(); break;
		case 0x03: op_atbp(); break;
		case 0x08: op_add(); break;
		case 0x09: op_add11(); break; // ADDC
		case 0x0a: op_coma(); break;
		case 0x0b: op_exbla(); break;

		case 0x50: op_tal(); break; // TA
		case 0x51: op_tb(); break;
		case 0x52: op_tc(); break;
		case 0x53: op_tam(); break;
		case 0x58: op_tis(); break; // TG
		case 0x59: op_ptw(); break;
		case 0x5a: op_ta0(); break;
		case 0x5b: op_tabl(); break;
		case 0x5c: op_tw(); break;
		case 0x5d: op_dtw(); break;
		case 0x5f: op_lbl(); break;

		case 0x60: op_comcn(); break;
		case 0x61: op_pdtw(); break;
		case 0x62: op_wr(); break;
		case 0x63: op_ws(); break;
		case 0x64: op_incb(); break;
		case 0x65: op_idiv(); break;
		case 0x66: op_rc(); break;
		case 0x67: op_sc(); break;
		case 0x68: op_rmf(); break;
		case 0x69: op_smf(); break;
		case 0x6a: op_kta(); break;
		case 0x6b: op_exkfa(); break;
		case 0x6c: op_decb(); break;
		case 0x6d: op_comcb(); break;
		case 0x6e: op_rtn0(); break; // RTN
		case 0x6f: op_rtn1(); break; // RTNS

		// extended opcodes
		case 0x5e:
			m_op = m_op << 8 | m_param;
			switch (m_param)
			{
		case 0x00: op_cend(); break;
		case 0x04: op_dta(); break;

		default: op_illegal(); break;
			}
			break; // 0x5e

		default: op_illegal(); break;
			}
			break; // 0xff

			}
			break; // 0xfc

	} // big switch
}
