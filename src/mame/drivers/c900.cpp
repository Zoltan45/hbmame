// license:BSD-3-Clause
// copyright-holders:Curt Coder, Robbbert
/******************************************************************************************

Commodore C900
UNIX prototype

http://www.zimmers.net/cbmpics/c900.html
http://www.zimmers.net/cbmpics/cbm/900/c900-chips.txt

Chips: Z8001 CPU, Z8010 MMU, Z8030 SCC, Z8036 CIO. Crystal: 12MHz

The Z8030 runs 2 serial ports. The Z8036 runs the IEEE interface and the speaker.

The FDC is an intelligent device that communicates with the main board via the MMU.
It has a 6508 CPU.

Disk drive is a Matsushita JA-560-012

Increasing the amount of RAM stops the error message, however it still keeps running
into the weeds (jumps to 00000). Due to lack of banking, the stack is pointing at rom.

To Do:
- Banking
- Pretty much everything
- Need schematics, technical manuals and so on.
- Eventually, will need software.
- Disassembler needs fixing

*******************************************************************************************/


#include "emu.h"
#include "cpu/z8000/z8000.h"
#include "machine/z80scc.h"
#include "bus/rs232/rs232.h"
#include "machine/z8536.h"
#include "emupal.h"


class c900_state : public driver_device
{
public:
	c900_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
	{ }

	void c900(machine_config &config);

private:
	void data_map(address_map &map);
	void io_map(address_map &map);
	void mem_map(address_map &map);
	required_device<cpu_device> m_maincpu;
};

void c900_state::mem_map(address_map &map)
{
	map(0x00000, 0x07fff).rom().region("roms", 0);
}

void c900_state::data_map(address_map &map)
{
	map(0x00000, 0x07fff).rom().region("roms", 0);
	map(0x08000, 0x6ffff).ram();
}

void c900_state::io_map(address_map &map)
{
	map(0x0000, 0x007f).rw("cio", FUNC(z8036_device::read), FUNC(z8036_device::write)).umask16(0x00ff);
	map(0x0100, 0x013f).rw("scc", FUNC(scc8030_device::zbus_r), FUNC(scc8030_device::zbus_w)).umask16(0x00ff);
}

static INPUT_PORTS_START( c900 )
INPUT_PORTS_END

/* F4 Character Displayer */
static const gfx_layout c900_charlayout =
{
	8, 16,                   /* 8 x 16 characters */
	256,                    /* 256 characters */
	1,                  /* 1 bits per pixel */
	{ 0 },                  /* no bitplanes */
	/* x offsets */
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	/* y offsets */
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	8*16                    /* every char takes 16 bytes */
};

static GFXDECODE_START( gfx_c900 )
	GFXDECODE_ENTRY( "chargen", 0x0000, c900_charlayout, 0, 1 )
GFXDECODE_END

MACHINE_CONFIG_START(c900_state::c900)
	/* basic machine hardware */
	MCFG_DEVICE_ADD("maincpu", Z8001, XTAL(12'000'000) / 2)
	MCFG_DEVICE_PROGRAM_MAP(mem_map)
	MCFG_DEVICE_DATA_MAP(data_map)
	MCFG_DEVICE_IO_MAP(io_map)

	GFXDECODE(config, "gfxdecode", "palette", gfx_c900);
	PALETTE(config, "palette", palette_device::MONOCHROME);

	Z8036(config, "cio", 6'000'000);

	scc8030_device& scc(SCC8030(config, "scc", 6'000'000)); // 5'850'000 is the ideal figure
	/* Port B */
	scc.out_txdb_callback().set("rs232", FUNC(rs232_port_device::write_txd));
	scc.out_dtrb_callback().set("rs232", FUNC(rs232_port_device::write_dtr));
	scc.out_rtsb_callback().set("rs232", FUNC(rs232_port_device::write_rts));
	//scc.out_int_callback().set("rs232", FUNC(c900_state::scc_int));

	rs232_port_device &rs232(RS232_PORT(config, "rs232", default_rs232_devices, "terminal"));
	rs232.rxd_handler().set("scc", FUNC(scc8030_device::rxb_w));
	rs232.cts_handler().set("scc", FUNC(scc8030_device::ctsb_w));
MACHINE_CONFIG_END

ROM_START( c900 )
	ROM_REGION16_LE( 0x8000, "roms", 0 )
	ROM_LOAD16_BYTE( "c 900 boot-h v 1.0.bin.u17", 0x0001, 0x4000, CRC(c3aa7fc1) SHA1(ff12dd100fa7b1e7e931e9a8ef4c4f5cc056e099) )
	ROM_LOAD16_BYTE( "c 900 boot-l v 1.0.bin.u18", 0x0000, 0x4000, CRC(0aa39272) SHA1(b2c5da4586d38fc66bb33aafeae4dbda36080f1e) )

	ROM_REGION( 0x2000, "fdc", 0 )
	ROM_LOAD( "s41_6-20-85.bin", 0x0000, 0x2000, CRC(ec245721) SHA1(4cc19014b4887833a56b1236dc5fe39cc5d7b5c3) )

	ROM_REGION( 0x1000, "chargen", 0 ) // this must be for the c900 terminal as the mainframe has no video output
	ROM_LOAD( "380217-01.u2", 0x0000, 0x1000, CRC(64cb4171) SHA1(e60d796170addfd27e2c33090f9c512c7e3f99f5) )
ROM_END

/*    YEAR  NAME  PARENT  COMPAT  MACHINE  INPUT  CLASS       INIT        COMPANY      FULLNAME         FLAGS */
COMP( 1985, c900, 0,      0,      c900,    c900,  c900_state, empty_init, "Commodore", "Commodore 900", MACHINE_IS_SKELETON )
