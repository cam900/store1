// license:BSD-3-Clause
// copyright-holders:Nicola Salmoria
/***************************************************************************

 Scramble hardware

NOTE:  Eventually to be merged into GALAXIAN.C

Interesting tidbit:

There is a bug in Amidars and Triple Punch. Look at the loop at 0x2715.
It expects DE to be saved during the call to 0x2726, but it can be destroyed,
causing the loop to read all kinds of bogus memory locations.


To Do:

- Mariner has discrete sound circuits connected to the 8910's output ports


Notes:

- While Atlantis has a cabinet switch, it doesn't use the 2nd player controls
  in cocktail mode.
- DIP locations have been verified from manuals for:
  800fath
  scramble

***************************************************************************/

#include "emu.h"
#include "includes/scramble.h"

#include "cpu/s2650/s2650.h"
#include "cpu/z80/z80.h"
#include "machine/gen_latch.h"
#include "machine/i8255.h"
#include "machine/watchdog.h"
#include "sound/ay8910.h"
#include "speaker.h"


void scramble_state::scramble_map(address_map &map)
{
	map(0x0000, 0x3fff).rom();
	map(0x4000, 0x47ff).ram();
	map(0x4800, 0x4bff).ram().w(this, FUNC(scramble_state::galaxold_videoram_w)).share("videoram");
	map(0x4c00, 0x4fff).rw(this, FUNC(scramble_state::galaxold_videoram_r), FUNC(scramble_state::galaxold_videoram_w)); /* mirror address */
	map(0x5000, 0x503f).ram().w(this, FUNC(scramble_state::galaxold_attributesram_w)).share("attributesram");
	map(0x5040, 0x505f).ram().share("spriteram");
	map(0x5060, 0x507f).ram().share("bulletsram");
	map(0x5080, 0x50ff).ram();
	map(0x6801, 0x6801).w(this, FUNC(scramble_state::galaxold_nmi_enable_w));
	map(0x6802, 0x6802).w(this, FUNC(scramble_state::galaxold_coin_counter_w));
	map(0x6804, 0x6804).w(this, FUNC(scramble_state::galaxold_stars_enable_w));
	map(0x6806, 0x6806).w(this, FUNC(scramble_state::galaxold_flip_screen_x_w));
	map(0x6807, 0x6807).w(this, FUNC(scramble_state::galaxold_flip_screen_y_w));
	map(0x7000, 0x7000).r("watchdog", FUNC(watchdog_timer_device::reset_r));
	map(0x7800, 0x7800).r("watchdog", FUNC(watchdog_timer_device::reset_r));
	map(0x8100, 0x8103).rw(m_ppi8255_0, FUNC(i8255_device::read), FUNC(i8255_device::write));
	map(0x8110, 0x8113).r(m_ppi8255_0, FUNC(i8255_device::read));  /* mirror for Frog */
	map(0x8200, 0x8203).rw(m_ppi8255_1, FUNC(i8255_device::read), FUNC(i8255_device::write));
}

READ8_MEMBER(scramble_state::scramble_soundram_r)
{
	return m_soundram[offset & 0x03ff];
}

WRITE8_MEMBER(scramble_state::scramble_soundram_w)
{
	m_soundram[offset & 0x03ff] = data;
}

void scramble_state::scramble_sound_map(address_map &map)
{
	map(0x0000, 0x2fff).rom();
	map(0x8000, 0x8fff).rw(this, FUNC(scramble_state::scramble_soundram_r), FUNC(scramble_state::scramble_soundram_w)).share("soundram");
	map(0x9000, 0x9fff).w(this, FUNC(scramble_state::scramble_filter_w));
}

void scramble_state::scramble_sound_io_map(address_map &map)
{
	map.global_mask(0xff);
	map(0x10, 0x10).w("8910.1", FUNC(ay8910_device::address_w));
	map(0x20, 0x20).rw("8910.1", FUNC(ay8910_device::data_r), FUNC(ay8910_device::data_w));
	map(0x40, 0x40).w("8910.2", FUNC(ay8910_device::address_w));
	map(0x80, 0x80).rw("8910.2", FUNC(ay8910_device::data_r), FUNC(ay8910_device::data_w));
}


void scramble_state::ckongs_map(address_map &map)
{
	map(0x0000, 0x5fff).rom();
	map(0x6000, 0x6bff).ram();
	map(0x7000, 0x7003).rw(m_ppi8255_0, FUNC(i8255_device::read), FUNC(i8255_device::write));
	map(0x7800, 0x7803).rw(m_ppi8255_1, FUNC(i8255_device::read), FUNC(i8255_device::write));
	map(0x9000, 0x93ff).ram().w(this, FUNC(scramble_state::galaxold_videoram_w)).share("videoram");
	map(0x9800, 0x983f).ram().w(this, FUNC(scramble_state::galaxold_attributesram_w)).share("attributesram");
	map(0x9840, 0x985f).ram().share("spriteram");
	map(0x9860, 0x987f).ram().share("bulletsram");
	map(0x9880, 0x98ff).ram();
	map(0xa801, 0xa801).w(this, FUNC(scramble_state::galaxold_nmi_enable_w));
	map(0xa802, 0xa802).w(this, FUNC(scramble_state::galaxold_coin_counter_w));
	map(0xa806, 0xa806).w(this, FUNC(scramble_state::galaxold_flip_screen_x_w));
	map(0xa807, 0xa807).w(this, FUNC(scramble_state::galaxold_flip_screen_y_w));
	map(0xb000, 0xb000).r("watchdog", FUNC(watchdog_timer_device::reset_r));
}



READ8_MEMBER(scramble_state::mars_ppi8255_0_r)
{
	return m_ppi8255_0->read(space, ((offset >> 2) & 0x02) | ((offset >> 1) & 0x01));
}

READ8_MEMBER(scramble_state::mars_ppi8255_1_r)
{
	return m_ppi8255_1->read(space, ((offset >> 2) & 0x02) | ((offset >> 1) & 0x01));
}

WRITE8_MEMBER(scramble_state::mars_ppi8255_0_w)
{
	m_ppi8255_0->write(space, ((offset >> 2) & 0x02) | ((offset >> 1) & 0x01), data);
}

WRITE8_MEMBER(scramble_state::mars_ppi8255_1_w)
{
	m_ppi8255_1->write(space, ((offset >> 2) & 0x02) | ((offset >> 1) & 0x01), data);
}

void scramble_state::mars_map(address_map &map)
{
	map(0x0000, 0x3fff).rom();
	map(0x4000, 0x47ff).ram();
	map(0x4800, 0x4bff).ram().w(this, FUNC(scramble_state::galaxold_videoram_w)).share("videoram");
	map(0x4c00, 0x4fff).r(this, FUNC(scramble_state::galaxold_videoram_r));
	map(0x5000, 0x503f).ram().w(this, FUNC(scramble_state::galaxold_attributesram_w)).share("attributesram");
	map(0x5040, 0x505f).ram().share("spriteram");
	map(0x5060, 0x507f).ram().share("bulletsram");
	map(0x5080, 0x50ff).ram();
	map(0x6800, 0x6800).w(this, FUNC(scramble_state::galaxold_coin_counter_1_w));
	map(0x6801, 0x6801).w(this, FUNC(scramble_state::galaxold_stars_enable_w));
	map(0x6802, 0x6802).w(this, FUNC(scramble_state::galaxold_nmi_enable_w));
	map(0x6808, 0x6808).w(this, FUNC(scramble_state::galaxold_coin_counter_0_w));
	map(0x6809, 0x6809).w(this, FUNC(scramble_state::galaxold_flip_screen_x_w));
	map(0x680b, 0x680b).w(this, FUNC(scramble_state::galaxold_flip_screen_y_w));
	map(0x7000, 0x7000).r("watchdog", FUNC(watchdog_timer_device::reset_r));
	map(0x7000, 0x7000).nopr();
	map(0x8100, 0x810f).rw(this, FUNC(scramble_state::mars_ppi8255_0_r), FUNC(scramble_state::mars_ppi8255_0_w));
	map(0x8200, 0x820f).rw(this, FUNC(scramble_state::mars_ppi8255_1_r), FUNC(scramble_state::mars_ppi8255_1_w));
}



void scramble_state::newsin7_map(address_map &map)
{
	map(0x0000, 0x3fff).rom();
	map(0x4000, 0x47ff).ram();
	map(0x4800, 0x4bff).ram().w(this, FUNC(scramble_state::galaxold_videoram_w)).share("videoram");
	map(0x4c00, 0x4fff).r(this, FUNC(scramble_state::galaxold_videoram_r));
	map(0x5000, 0x503f).ram().w(this, FUNC(scramble_state::galaxold_attributesram_w)).share("attributesram");
	map(0x5040, 0x505f).ram().share("spriteram");
	map(0x5060, 0x507f).ram().share("bulletsram");
	map(0x5080, 0x50ff).ram();
	map(0x6800, 0x6800).w(this, FUNC(scramble_state::galaxold_coin_counter_1_w));
	map(0x6801, 0x6801).w(this, FUNC(scramble_state::galaxold_stars_enable_w));
	//AM_RANGE(0x6802, 0x6802) AM_WRITE(galaxold_nmi_enable_w)
	map(0x6808, 0x6808).w(this, FUNC(scramble_state::galaxold_coin_counter_0_w));
	map(0x6809, 0x6809).w(this, FUNC(scramble_state::galaxold_flip_screen_x_w));
	map(0x680b, 0x680b).w(this, FUNC(scramble_state::galaxold_flip_screen_y_w));
	map(0x7000, 0x7000).r("watchdog", FUNC(watchdog_timer_device::reset_r));
	map(0x8200, 0x820f).rw(this, FUNC(scramble_state::mars_ppi8255_1_r), FUNC(scramble_state::mars_ppi8255_1_w));
	map(0xa000, 0xafff).rom();
	map(0xc100, 0xc10f).rw(this, FUNC(scramble_state::mars_ppi8255_0_r), FUNC(scramble_state::mars_ppi8255_0_w));
}



void scramble_state::mrkougar_map(address_map &map)
{
	map(0x0000, 0x3fff).rom();
	map(0x4000, 0x47ff).ram();
	map(0x4800, 0x4bff).ram().w(this, FUNC(scramble_state::galaxold_videoram_w)).share("videoram");
	map(0x4c00, 0x4fff).rw(this, FUNC(scramble_state::galaxold_videoram_r), FUNC(scramble_state::galaxold_videoram_w));
	map(0x5000, 0x503f).ram().w(this, FUNC(scramble_state::galaxold_attributesram_w)).share("attributesram");
	map(0x5040, 0x505f).ram().share("spriteram");
	map(0x5060, 0x507f).ram().share("bulletsram");
	map(0x5080, 0x50ff).ram();
	map(0x6800, 0x6800).w(this, FUNC(scramble_state::galaxold_coin_counter_1_w));
	map(0x6801, 0x6801).w(this, FUNC(scramble_state::galaxold_nmi_enable_w));
	map(0x6808, 0x6808).w(this, FUNC(scramble_state::galaxold_coin_counter_0_w));
	map(0x6809, 0x6809).w(this, FUNC(scramble_state::galaxold_flip_screen_x_w));
	map(0x680b, 0x680b).w(this, FUNC(scramble_state::galaxold_flip_screen_y_w));
	map(0x7000, 0x7000).r("watchdog", FUNC(watchdog_timer_device::reset_r));
	map(0x8100, 0x810f).rw(this, FUNC(scramble_state::mars_ppi8255_0_r), FUNC(scramble_state::mars_ppi8255_0_w));
	map(0x8200, 0x820f).rw(this, FUNC(scramble_state::mars_ppi8255_1_r), FUNC(scramble_state::mars_ppi8255_1_w));
}



void scramble_state::hotshock_map(address_map &map)
{
	map(0x0000, 0x3fff).rom();
	map(0x4000, 0x47ff).ram();
	map(0x4800, 0x4bff).ram().w(this, FUNC(scramble_state::galaxold_videoram_w)).share("videoram");
	map(0x4c00, 0x4fff).r(this, FUNC(scramble_state::galaxold_videoram_r));
	map(0x5000, 0x503f).ram().w(this, FUNC(scramble_state::galaxold_attributesram_w)).share("attributesram");
	map(0x5040, 0x505f).ram().share("spriteram");
	map(0x5060, 0x507f).ram().share("bulletsram");
	map(0x5080, 0x50ff).ram();
	map(0x6000, 0x6000).w(this, FUNC(scramble_state::galaxold_coin_counter_2_w));
	map(0x6002, 0x6002).w(this, FUNC(scramble_state::galaxold_coin_counter_1_w));
	map(0x6004, 0x6004).w(this, FUNC(scramble_state::hotshock_flip_screen_w));
	map(0x6005, 0x6005).w(this, FUNC(scramble_state::galaxold_coin_counter_0_w));
	map(0x6006, 0x6006).w(this, FUNC(scramble_state::galaxold_gfxbank_w));
	map(0x6801, 0x6801).w(this, FUNC(scramble_state::galaxold_nmi_enable_w));
	map(0x7000, 0x7000).w("watchdog", FUNC(watchdog_timer_device::reset_w));
	map(0x8000, 0x8000).portr("IN0").w(m_soundlatch, FUNC(generic_latch_8_device::write));
	map(0x8001, 0x8001).portr("IN1");
	map(0x8002, 0x8002).portr("IN2");
	map(0x8003, 0x8003).portr("IN3");
	map(0x9000, 0x9000).w(this, FUNC(scramble_state::hotshock_sh_irqtrigger_w));
}



void scramble_state::hunchbks_map(address_map &map)
{
	map(0x0000, 0x0fff).rom();
	map(0x1210, 0x1213).rw(m_ppi8255_1, FUNC(i8255_device::read), FUNC(i8255_device::write));
	map(0x1400, 0x143f).ram().w(this, FUNC(scramble_state::galaxold_attributesram_w)).share("attributesram");
	map(0x1440, 0x145f).ram().share("spriteram");
	map(0x1460, 0x147f).ram().share("bulletsram");
	map(0x1480, 0x14ff).ram();
	map(0x1500, 0x1503).rw(m_ppi8255_0, FUNC(i8255_device::read), FUNC(i8255_device::write));
	map(0x1606, 0x1606).w(this, FUNC(scramble_state::galaxold_flip_screen_x_w));
	map(0x1607, 0x1607).w(this, FUNC(scramble_state::galaxold_flip_screen_y_w));
	map(0x1680, 0x1680).r("watchdog", FUNC(watchdog_timer_device::reset_r));
	map(0x1780, 0x1780).r("watchdog", FUNC(watchdog_timer_device::reset_r));
	map(0x1800, 0x1bff).ram().w(this, FUNC(scramble_state::galaxold_videoram_w)).share("videoram");
	map(0x1c00, 0x1fff).ram();
	map(0x2000, 0x2fff).rom();
	map(0x3000, 0x3fff).rw(this, FUNC(scramble_state::hunchbks_mirror_r), FUNC(scramble_state::hunchbks_mirror_w));
	map(0x4000, 0x4fff).rom();
	map(0x5000, 0x5fff).rw(this, FUNC(scramble_state::hunchbks_mirror_r), FUNC(scramble_state::hunchbks_mirror_w));
	map(0x6000, 0x6fff).rom();
	map(0x7000, 0x7fff).rw(this, FUNC(scramble_state::hunchbks_mirror_r), FUNC(scramble_state::hunchbks_mirror_w));
}



void scramble_state::mimonscr_map(address_map &map)
{
	map(0x0000, 0x3fff).rom();
	map(0x4000, 0x43ff).rw(this, FUNC(scramble_state::galaxold_videoram_r), FUNC(scramble_state::galaxold_videoram_w)); /* mirror address?, probably not */
	map(0x4400, 0x47ff).ram();
	map(0x4800, 0x4bff).ram().w(this, FUNC(scramble_state::galaxold_videoram_w)).share("videoram");
	map(0x5000, 0x503f).ram().w(this, FUNC(scramble_state::galaxold_attributesram_w)).share("attributesram");
	map(0x5040, 0x505f).ram().share("spriteram");
	map(0x5060, 0x507f).ram().share("bulletsram");
	map(0x5080, 0x50ff).ram();
	map(0x6800, 0x6802).w(this, FUNC(scramble_state::galaxold_gfxbank_w));
	map(0x6801, 0x6801).w(this, FUNC(scramble_state::galaxold_nmi_enable_w));
	map(0x6806, 0x6806).w(this, FUNC(scramble_state::galaxold_flip_screen_x_w));
	map(0x6807, 0x6807).w(this, FUNC(scramble_state::galaxold_flip_screen_y_w));
	map(0x7000, 0x7000).r("watchdog", FUNC(watchdog_timer_device::reset_r));
	map(0x8100, 0x8103).rw(m_ppi8255_0, FUNC(i8255_device::read), FUNC(i8255_device::write));
	map(0x8200, 0x8203).rw(m_ppi8255_1, FUNC(i8255_device::read), FUNC(i8255_device::write));
	map(0xc000, 0xffff).rom();
}



void scramble_state::ad2083_map(address_map &map)
{
	map(0x0000, 0x3fff).rom();
	map(0x4000, 0x47ff).ram();
	map(0x4800, 0x4bff).rw(this, FUNC(scramble_state::galaxold_videoram_r), FUNC(scramble_state::galaxold_videoram_w)).share("videoram");
	map(0x5000, 0x503f).ram().w(this, FUNC(scramble_state::galaxold_attributesram_w)).share("attributesram");
	map(0x5040, 0x505f).ram().share("spriteram");
	map(0x5060, 0x507f).ram().share("bulletsram");
	map(0x6004, 0x6004).w(this, FUNC(scramble_state::hotshock_flip_screen_w));
	map(0x6800, 0x6800).w(this, FUNC(scramble_state::galaxold_coin_counter_2_w));
	map(0x6801, 0x6801).w(this, FUNC(scramble_state::galaxold_nmi_enable_w));
	map(0x6802, 0x6802).w(this, FUNC(scramble_state::galaxold_coin_counter_0_w));
	map(0x6803, 0x6803).w(this, FUNC(scramble_state::scrambold_background_blue_w));
	map(0x6805, 0x6805).w(this, FUNC(scramble_state::galaxold_coin_counter_1_w));
	map(0x6806, 0x6806).w(this, FUNC(scramble_state::scrambold_background_red_w));
	map(0x6807, 0x6807).w(this, FUNC(scramble_state::scrambold_background_green_w));
	map(0x8000, 0x8000).w(m_soundlatch, FUNC(generic_latch_8_device::write));
	map(0x9000, 0x9000).w(this, FUNC(scramble_state::hotshock_sh_irqtrigger_w));
	map(0x7000, 0x7000).r("watchdog", FUNC(watchdog_timer_device::reset_r));
	map(0x8000, 0x8000).portr("IN0");
	map(0x8001, 0x8001).portr("IN1");
	map(0x8002, 0x8002).portr("IN2");
	map(0x8003, 0x8003).portr("IN3");
	map(0xa000, 0xdfff).rom();
	map(0xe800, 0xebff).ram();
}



void scramble_state::triplep_map(address_map &map)
{
	map(0x0000, 0x3fff).rom();
	map(0x4000, 0x47ff).ram();
	map(0x4800, 0x4bff).ram().w(this, FUNC(scramble_state::galaxold_videoram_w)).share("videoram");
	map(0x4c00, 0x4fff).rw(this, FUNC(scramble_state::galaxold_videoram_r), FUNC(scramble_state::galaxold_videoram_w)); /* mirror address */
	map(0x5000, 0x503f).ram().w(this, FUNC(scramble_state::galaxold_attributesram_w)).share("attributesram");
	map(0x5040, 0x505f).ram().share("spriteram");
	map(0x5060, 0x507f).ram().share("bulletsram");
	map(0x5080, 0x50ff).ram();
	map(0x5800, 0x67ff).rom();
	map(0x6801, 0x6801).w(this, FUNC(scramble_state::galaxold_nmi_enable_w));
	map(0x6802, 0x6802).w(this, FUNC(scramble_state::galaxold_coin_counter_w));
	map(0x6804, 0x6804).w(this, FUNC(scramble_state::galaxold_stars_enable_w));
	map(0x6806, 0x6806).w(this, FUNC(scramble_state::galaxold_flip_screen_x_w));
	map(0x6807, 0x6807).w(this, FUNC(scramble_state::galaxold_flip_screen_y_w));
	map(0x7000, 0x7000).r("watchdog", FUNC(watchdog_timer_device::reset_r));
	map(0x8100, 0x8103).rw(m_ppi8255_0, FUNC(i8255_device::read), FUNC(i8255_device::write));
}

void scramble_state::triplep_io_map(address_map &map)
{
	map.global_mask(0xff);
	map(0x00, 0x01).w("8910.1", FUNC(ay8910_device::data_address_w));
	map(0x01, 0x01).r("8910.1", FUNC(ay8910_device::data_r));
	map(0x02, 0x02).r(this, FUNC(scramble_state::triplep_pip_r));
	map(0x03, 0x03).r(this, FUNC(scramble_state::triplep_pap_r));
}

void scramble_state::hotshock_sound_io_map(address_map &map)
{
	map.global_mask(0xff);
	map(0x10, 0x10).w("8910.1", FUNC(ay8910_device::address_w));
	map(0x20, 0x20).rw("8910.1", FUNC(ay8910_device::data_r), FUNC(ay8910_device::data_w));
	map(0x40, 0x40).rw("8910.2", FUNC(ay8910_device::data_r), FUNC(ay8910_device::data_w));
	map(0x80, 0x80).w("8910.2", FUNC(ay8910_device::address_w));
}



READ8_MEMBER(scramble_state::hncholms_prot_r)
{
	if(m_maincpu->pc() == 0x2b || m_maincpu->pc() == 0xa27)
		return 1;
	else
		return 0;
}

void scramble_state::hunchbks_readport(address_map &map)
{
	map(0x00, 0x00).r(this, FUNC(scramble_state::hncholms_prot_r));
}


// Harem

void scramble_state::harem_map(address_map &map)
{
	map(0x0000, 0x1fff).rom();

	map(0x2000, 0x27ff).ram();

	map(0x4000, 0x403f).ram().w(this, FUNC(scramble_state::galaxold_attributesram_w)).share("attributesram");
	map(0x4040, 0x405f).ram().share("spriteram");
	map(0x4060, 0x407f).ram().share("bulletsram");
	map(0x4080, 0x47ff).ram();

	map(0x4800, 0x4bff).rw(this, FUNC(scramble_state::galaxold_videoram_r), FUNC(scramble_state::galaxold_videoram_w)).share("videoram");
	map(0x4c00, 0x4fff).rw(this, FUNC(scramble_state::galaxold_videoram_r), FUNC(scramble_state::galaxold_videoram_w)); // mirror address

	map(0x5000, 0x5000).ram().w(this, FUNC(scramble_state::racknrol_tiles_bank_w)).share("racknrol_tbank"); // high bits of tiles, 1 bit every 4 columns
	map(0x5800, 0x5800).r("watchdog", FUNC(watchdog_timer_device::reset_r)).w(this, FUNC(scramble_state::galaxold_nmi_enable_w));

	map(0x5801, 0x5801).w(this, FUNC(scramble_state::harem_decrypt_clk_w));          // run-time bitswap selection
	map(0x5802, 0x5802).w(this, FUNC(scramble_state::harem_decrypt_bit_w));
	map(0x5803, 0x5803).w(this, FUNC(scramble_state::harem_decrypt_rst_w));

	map(0x5804, 0x5804).w(this, FUNC(scramble_state::galaxold_coin_counter_w));
	map(0x5805, 0x5805).w(this, FUNC(scramble_state::galaxold_gfxbank_w));           // bit 0 = sprite tiles high bit
	map(0x5806, 0x5806).w(this, FUNC(scramble_state::galaxold_flip_screen_x_w));     // maybe (0 at boot)
	map(0x5807, 0x5807).w(this, FUNC(scramble_state::galaxold_flip_screen_y_w));     // ""

	map(0x6100, 0x6103).rw(m_ppi8255_0, FUNC(i8255_device::read), FUNC(i8255_device::write));
	map(0x6200, 0x6203).rw(m_ppi8255_1, FUNC(i8255_device::read), FUNC(i8255_device::write));

	map(0x8000, 0x9fff).bankr("rombank");                  // bitswapped rom
}

void scramble_state::decrypted_opcodes_map(address_map &map)
{
	map(0x0000, 0x1fff).rom().region("maincpu", 0);
	map(0x8000, 0x9fff).bankr("rombank_decrypted");
}

void scramble_state::harem_sound_map(address_map &map)
{
	map(0x0000, 0x2fff).rom();
	map(0x6000, 0x6000).r(this, FUNC(scramble_state::harem_digitalker_intr_r));
	map(0x8000, 0x83ff).ram();
	map(0xa000, 0xafff).w(this, FUNC(scramble_state::scramble_filter_w));
}

void scramble_state::harem_sound_io_map(address_map &map)
{
	map.global_mask(0xff);

	// ports->speech:
	map(0x04, 0x04).w("8910.3", FUNC(ay8910_device::address_w));
	map(0x08, 0x08).rw("8910.3", FUNC(ay8910_device::data_r), FUNC(ay8910_device::data_w));
	// same as scramble:
	map(0x10, 0x10).w("8910.1", FUNC(ay8910_device::address_w));
	map(0x20, 0x20).rw("8910.1", FUNC(ay8910_device::data_r), FUNC(ay8910_device::data_w));
	map(0x40, 0x40).w("8910.2", FUNC(ay8910_device::address_w));
	map(0x80, 0x80).rw("8910.2", FUNC(ay8910_device::data_r), FUNC(ay8910_device::data_w));  // read soundlatch
}



/**************************************************************************/

static INPUT_PORTS_START( scramble )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )

	PORT_START("IN1")
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Lives ) ) PORT_DIPLOCATION("SW1:2,1")
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x02, "5" )
	PORT_DIPSETTING(    0x03, DEF_STR( Free_Play ) )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_DIPNAME( 0x06, 0x00, DEF_STR( Coinage ) ) PORT_DIPLOCATION("SW1:5,4")
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_4C ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SW1:3")
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Cocktail ) )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_SPECIAL )    /* protection bit */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SPECIAL )    /* protection bit */
INPUT_PORTS_END

static INPUT_PORTS_START( 800fath )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )

	PORT_START("IN1")
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Lives ) ) PORT_DIPLOCATION("SW1:5,6")
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x02, "5" )
	PORT_DIPSETTING(    0x03, DEF_STR( Free_Play ) )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_DIPNAME( 0x06, 0x00, DEF_STR( Coinage ) ) PORT_DIPLOCATION("SW1:2,3")
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_4C ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SW1:4")
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Cocktail ) )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_SPECIAL )    /* protection bit */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SPECIAL )    /* protection bit */
INPUT_PORTS_END

static INPUT_PORTS_START( triplep )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_4WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_4WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_4WAY
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )

	PORT_START("IN1")
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x02, "5" )
	PORT_DIPSETTING(    0x03, "256 (Cheat)")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_COCKTAIL
	PORT_DIPNAME( 0x06, 0x00, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x02, "A 1/2 B 1/1 C 1/2" )
	PORT_DIPSETTING(    0x04, "A 1/3 B 3/1 C 1/3" )
	PORT_DIPSETTING(    0x00, "A 1/1 B 2/1 C 1/1" )
	PORT_DIPSETTING(    0x06, "A 1/4 B 4/1 C 1/4" )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Cocktail ) )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_4WAY
	PORT_SERVICE( 0x20, IP_ACTIVE_HIGH )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_4WAY
	PORT_DIPNAME( 0x80, 0x00, "Rack Test (Cheat)" ) PORT_CODE(KEYCODE_F1)
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )
INPUT_PORTS_END

static INPUT_PORTS_START( knockoutb )
	PORT_INCLUDE(triplep)

	PORT_MODIFY("IN1")
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "1" )
	PORT_DIPSETTING(    0x01, "2" )
	PORT_DIPSETTING(    0x02, "3" )
	PORT_DIPSETTING(    0x03, "256 (Cheat)")
INPUT_PORTS_END

/* ckongs coinage DIPs are spread across two input ports */
CUSTOM_INPUT_MEMBER(scramble_state::ckongs_coinage_r)
{
	int bit_mask = (uintptr_t)param;
	return (ioport("FAKE")->read() & bit_mask) ? 0x01 : 0x00;
}

static INPUT_PORTS_START( ckongs )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_4WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )    /* probably unused */
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )    /* probably unused */
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_4WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_4WAY
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )

	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, scramble_state,ckongs_coinage_r, (void *)0x01)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, scramble_state,ckongs_coinage_r, (void *)0x02)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )    /* probably unused */
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_COCKTAIL
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x04, "3" )
	PORT_DIPSETTING(    0x00, "4" )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, scramble_state,ckongs_coinage_r, (void *)0x04)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_4WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )    /* probably unused */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_4WAY
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )    /* probably unused */

	PORT_START("FAKE")
	PORT_DIPNAME( 0x07, 0x07, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_4C ) )
INPUT_PORTS_END

static INPUT_PORTS_START( mars )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_RIGHT ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_LEFT ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_RIGHT ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_LEFT ) PORT_8WAY
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )

	PORT_START("IN1")
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_5C ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_DOWN ) PORT_8WAY PORT_PLAYER(2)  /* this also control cocktail mode */
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x08, "3" )
	PORT_DIPSETTING(    0x00, "255 (Cheat)")
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_UP ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_DOWN ) PORT_8WAY
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_DOWN ) PORT_8WAY
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_UP ) PORT_8WAY

	PORT_START("IN3")
	PORT_BIT( 0x1f, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_UP ) PORT_8WAY PORT_PLAYER(2)
INPUT_PORTS_END

static INPUT_PORTS_START( devilfsh )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )

	PORT_START("IN1")
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x00, "10000" )
	PORT_DIPSETTING(    0x01, "15000" )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Cocktail ) )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "4" )
	PORT_DIPSETTING(    0x02, "5" )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_1C ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_5C ) )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END

static INPUT_PORTS_START( newsin7 )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )

	PORT_START("IN1")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x03, " A 1C/1C  B 2C/1C" )
	PORT_DIPSETTING(    0x01, " A 1C/3C  B 3C/1C" )
	PORT_DIPSETTING(    0x02, " A 1C/2C  B 1C/1C" )
	PORT_DIPSETTING(    0x00, " A 1C/4C  B 4C/1C" )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START("IN2")   /* IN2 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Unknown ) )  /* difficulty? */
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x08, "5" )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END

static INPUT_PORTS_START( mrkougar )
	PORT_START("IN0")
	PORT_BIT( 0x03, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )

	PORT_START("IN1")
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_5C ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x04, "5" )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Easy ) )
	PORT_BIT( 0xf0, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END

static INPUT_PORTS_START( hotshock )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0xc0, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_SERVICE1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN ) /* pressing this disables the coins */
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_START1 )

	PORT_START("IN2")
	PORT_DIPNAME( 0x0f, 0x00, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 2C_2C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 2C_4C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 2C_6C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 2C_7C ) )
	PORT_DIPSETTING(    0x0f, DEF_STR( 2C_8C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_8C ) )
	PORT_DIPNAME( 0xf0, 0x00, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x90, DEF_STR( 2C_2C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0xb0, DEF_STR( 2C_4C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0xd0, DEF_STR( 2C_6C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 2C_7C ) )
	PORT_DIPSETTING(    0xf0, DEF_STR( 2C_8C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x50, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(    0x70, DEF_STR( 1C_8C ) )

	PORT_START("IN3")
	PORT_DIPNAME( 0x03, 0x01, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPSETTING(    0x01, "3" )
	PORT_DIPSETTING(    0x02, "4" )
	PORT_DIPSETTING(    0x03, "5" )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Language ) )
	PORT_DIPSETTING(    0x04, DEF_STR( English ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Italian ) )
	PORT_DIPNAME( 0x18, 0x00, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x00, "75000" )
	PORT_DIPSETTING(    0x08, "150000" )
	PORT_DIPSETTING(    0x10, "200000" )
	PORT_DIPSETTING(    0x18, DEF_STR( None ) )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Cocktail ) )
INPUT_PORTS_END

static INPUT_PORTS_START( hunchbks )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )

	PORT_START("IN1")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x01, "3" )
	PORT_DIPSETTING(    0x00, "5" )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x02, "A 2/1 B 1/3" )
	PORT_DIPSETTING(    0x00, "A 1/1 B 1/5" )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_DIPNAME( 0x06, 0x00, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x00, "10000" )
	PORT_DIPSETTING(    0x02, "20000" )
	PORT_DIPSETTING(    0x04, "40000" )
	PORT_DIPSETTING(    0x06, "80000" )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Cocktail ) )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )    /* protection check? */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )    /* protection check? */
INPUT_PORTS_END

static INPUT_PORTS_START( hncholms )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )

	PORT_START("IN1")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x01, "3" )
	PORT_DIPSETTING(    0x00, "5" )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x00, "A 2/1 B 1/3" )
	PORT_DIPSETTING(    0x02, "A 1/1 B 1/5" )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_DIPNAME( 0x06, 0x00, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x00, "10000" )
	PORT_DIPSETTING(    0x02, "20000" )
	PORT_DIPSETTING(    0x04, "40000" )
	PORT_DIPSETTING(    0x06, "80000" )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Cocktail ) )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )    /* protection check? */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )    /* protection check? */
INPUT_PORTS_END

static INPUT_PORTS_START( cavelon )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN ) /* force UR controls in CK mode? */
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )

	PORT_START("IN1")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x00, "A 1/1 B 1/6" )
	PORT_DIPSETTING(    0x02, "A 2/1 B 1/3" )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_DIPNAME( 0x06, 0x02, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "5" )
	PORT_DIPSETTING(    0x04, "4" )
	PORT_DIPSETTING(    0x02, "3" )
	PORT_DIPSETTING(    0x06, "2" )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Unknown) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )    /* protection check? */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )    /* protection check? */
INPUT_PORTS_END

/* Same as 'mimonkey' (scobra.cpp driver) */
static INPUT_PORTS_START( mimonscr )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )

	PORT_START("IN1")
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x02, "5" )
	PORT_DIPSETTING(    0x03, "6" )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_DIPNAME( 0x06, 0x00, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_2C ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Cocktail ) )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_DIPNAME( 0x20, 0x00, "Infinite Lives (Cheat)")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Unknown ) )   /* used, something to do with the bullets */
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

static INPUT_PORTS_START( ad2083 )
	/* There are no Player 2 controls for this game:
	 * Dipswitch is read upon startup. If Cabinet = Cocktail, a 1 is stored @400F.
	 * 400F in turn is only read just before Player 2 turn. If 400F=1 then flip line
	 * is set. That is all. If there is a dedicated player 2 input,
	 * it must be multiplexed by flip line. */
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_SERVICE1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN ) // if ON it doesn't accept any COIN
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_START1 )

	PORT_START("IN2")
	PORT_DIPNAME( 0x0f, 0x00, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 2C_2C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 2C_4C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 2C_6C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 2C_7C ) )
	PORT_DIPSETTING(    0x0f, DEF_STR( 2C_8C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_8C ) )
	PORT_DIPNAME( 0xf0, 0x00, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x90, DEF_STR( 2C_2C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0xb0, DEF_STR( 2C_4C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0xd0, DEF_STR( 2C_6C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 2C_7C ) )
	PORT_DIPSETTING(    0xf0, DEF_STR( 2C_8C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x50, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(    0x70, DEF_STR( 1C_8C ) )

	PORT_START("IN3")
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x03, "2" )
	PORT_DIPSETTING(    0x02, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x00, "5" )
	PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( None ) )
	PORT_DIPSETTING(    0x04, "150000" )
	PORT_DIPSETTING(    0x08, "100000" )
	PORT_DIPSETTING(    0x00, "200000" )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Allow_Continue ) )
	PORT_DIPSETTING(    0x20, DEF_STR( No ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END


static INPUT_PORTS_START( harem ) /* Switch 8 doesn't appear to mapped (just like Scorpion) */
	PORT_START("IN0")   // $6100 - PPI0 Port A
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )        /* Same hardware as Scorpion: P2 Up in Cocktail - Not Used */
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )

	PORT_START("IN1")   // $6101 - PPI0 Port B
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Lives ) ) PORT_DIPLOCATION("SW1:1,2")
	PORT_DIPSETTING(    0x03, "2" )
	PORT_DIPSETTING(    0x02, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x00, DEF_STR( Infinite ) )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )        /* Same hardware as Scorpion: P2 Button2 in Cocktail - Not Used */
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )        /* Same hardware as Scorpion: P2 Button1 in Cocktail - Not Used */
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )        /* Same hardware as Scorpion: P2 Right in Cocktail - Not Used */
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )        /* Same hardware as Scorpion: P2 Left in Cocktail - Not Used */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START("IN2")   // $6102 - PPI0 Port C
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )        /* Same hardware as Scorpion: P2 Down in Cocktail */
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW1:5")
	PORT_DIPSETTING(    0x02, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hard ) )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Coin_B ) ) PORT_DIPLOCATION("SW1:3,4")
	PORT_DIPSETTING(    0x0c, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_3C ) )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_UP )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN )
	PORT_DIPNAME( 0xa0, 0xa0, "Initial Bonus Points" ) PORT_DIPLOCATION("SW1:6,7")
	PORT_DIPSETTING(    0xa0, "500" )
	PORT_DIPSETTING(    0x80, "850" )
	PORT_DIPSETTING(    0x20, "1150" )
	PORT_DIPSETTING(    0x00, "1350" )
INPUT_PORTS_END



/**************************************************************************/

static const gfx_layout scramble_charlayout =
{
	8,8,
	RGN_FRAC(1,2),
	2,
	{ RGN_FRAC(0,2), RGN_FRAC(1,2) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};
static const gfx_layout scramble_spritelayout =
{
	16,16,
	RGN_FRAC(1,2),
	2,
	{ RGN_FRAC(0,2), RGN_FRAC(1,2) },
	{ 0, 1, 2, 3, 4, 5, 6, 7,
			8*8+0, 8*8+1, 8*8+2, 8*8+3, 8*8+4, 8*8+5, 8*8+6, 8*8+7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			16*8, 17*8, 18*8, 19*8, 20*8, 21*8, 22*8, 23*8 },
	32*8
};

static const gfx_layout devilfsh_charlayout =
{
	8,8,    /* 8*8 characters */
	256,    /* 256 characters */
	2,  /* 2 bits per pixel */
	{ 0, 2*256*8*8 },   /* the bitplanes are separated */
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8 /* every char takes 8 consecutive bytes */
};
static const gfx_layout devilfsh_spritelayout =
{
	16,16,  /* 16*16 sprites */
	64, /* 64 sprites */
	2,  /* 2 bits per pixel */
	{ 0, 2*64*16*16 },  /* the bitplanes are separated */
	{ 0, 1, 2, 3, 4, 5, 6, 7,
			8*8+0, 8*8+1, 8*8+2, 8*8+3, 8*8+4, 8*8+5, 8*8+6, 8*8+7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			16*8, 17*8, 18*8, 19*8, 20*8, 21*8, 22*8, 23*8 },
	32*8    /* every sprite takes 32 consecutive bytes */
};
static const gfx_layout newsin7_charlayout =
{
	8,8,    /* 8*8 characters */
	256,    /* 256 characters */
	3,  /* 3 bits per pixel */
	{ 2*2*256*8*8, 0, 2*256*8*8 },  /* the bitplanes are separated */
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8 /* every char takes 8 consecutive bytes */
};
static const gfx_layout newsin7_spritelayout =
{
	16,16,  /* 16*16 sprites */
	64, /* 64 sprites */
	3,  /* 3 bits per pixel */
	{ 2*2*64*16*16, 0, 2*64*16*16 },    /* the bitplanes are separated */
	{ 0, 1, 2, 3, 4, 5, 6, 7,
			8*8+0, 8*8+1, 8*8+2, 8*8+3, 8*8+4, 8*8+5, 8*8+6, 8*8+7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			16*8, 17*8, 18*8, 19*8, 20*8, 21*8, 22*8, 23*8 },
	32*8    /* every sprite takes 32 consecutive bytes */
};

static const gfx_layout mrkougar_charlayout =
{
	8,8,
	256,
	2,
	{ 0, 4 },
	{ 8*8+0, 8*8+1, 8*8+2, 8*8+3, 0, 1, 2, 3 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	16*8
};
static const gfx_layout mrkougar_spritelayout =
{
	16,16,
	64,
	2,
	{ 0, 4 },
	{ 8*8+0, 8*8+1, 8*8+2, 8*8+3, 0, 1, 2, 3,
		24*8+0, 24*8+1, 24*8+2, 24*8+3, 16*8+0, 16*8+1, 16*8+2, 16*8+3 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
		32*8, 33*8, 34*8, 35*8, 36*8, 37*8, 38*8, 39*8 },
	64*8
};

static const gfx_layout ad2083_charlayout =
{
	8,8,    /* 8*8 characters */
	RGN_FRAC(1,2),
	2,  /* 2 bits per pixel */
	{ RGN_FRAC(0,2), RGN_FRAC(1,2) },   /* the two bitplanes are separated */
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8 /* every char takes 8 consecutive bytes */
};
static const gfx_layout ad2083_spritelayout =
{
	16,16,  /* 16*16 sprites */
	RGN_FRAC(1,2),
	2,  /* 2 bits per pixel */
	{ RGN_FRAC(0,2), RGN_FRAC(1,2) },   /* the two bitplanes are separated */
	{ 0, 1, 2, 3, 4, 5, 6, 7,
			8*8+0, 8*8+1, 8*8+2, 8*8+3, 8*8+4, 8*8+5, 8*8+6, 8*8+7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			16*8, 17*8, 18*8, 19*8, 20*8, 21*8, 22*8, 23*8 },
	32*8    /* every sprite takes 32 consecutive bytes */
};

static GFXDECODE_START( scramble )
	GFXDECODE_ENTRY( "gfx1", 0x0000, scramble_charlayout,   0, 8 )
	GFXDECODE_ENTRY( "gfx1", 0x0000, scramble_spritelayout, 0, 8 )
GFXDECODE_END

static GFXDECODE_START( devilfsh )
	GFXDECODE_ENTRY( "gfx1", 0x0000, devilfsh_charlayout,   0, 8 )
	GFXDECODE_ENTRY( "gfx1", 0x0800, devilfsh_spritelayout, 0, 8 )
GFXDECODE_END

static GFXDECODE_START( newsin7 )
	GFXDECODE_ENTRY( "gfx1", 0x0000, newsin7_charlayout,   0, 4 )
	GFXDECODE_ENTRY( "gfx1", 0x0800, newsin7_spritelayout, 0, 4 )
GFXDECODE_END

static GFXDECODE_START( mrkougar )
	GFXDECODE_ENTRY( "gfx1", 0x0000, mrkougar_charlayout,   0, 8 )
	GFXDECODE_ENTRY( "gfx1", 0x0000, mrkougar_spritelayout, 0, 8 )
GFXDECODE_END

static GFXDECODE_START( ad2083 )
	GFXDECODE_ENTRY( "gfx1", 0x0000, ad2083_charlayout,    0, 8 )
	GFXDECODE_ENTRY( "gfx1", 0x0000, ad2083_spritelayout,  0, 8 )
GFXDECODE_END


/**************************************************************************/

MACHINE_CONFIG_START(scramble_state::scramble)

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", Z80, 18432000/6)    /* 3.072 MHz */
	MCFG_CPU_PROGRAM_MAP(scramble_map)

	MCFG_CPU_ADD("audiocpu", Z80, 14318000/8)   /* 1.78975 MHz */
	MCFG_CPU_PROGRAM_MAP(scramble_sound_map)
	MCFG_CPU_IO_MAP(scramble_sound_io_map)
	MCFG_CPU_IRQ_ACKNOWLEDGE_DRIVER(scramble_state,scramble_sh_irq_callback)

	MCFG_DEVICE_ADD("7474_9m_1", TTL7474, 0)
	MCFG_7474_OUTPUT_CB(WRITELINE(scramble_state,galaxold_7474_9m_1_callback))

	MCFG_DEVICE_ADD("7474_9m_2", TTL7474, 0)
	MCFG_7474_COMP_OUTPUT_CB(WRITELINE(scramble_state,galaxold_7474_9m_2_q_callback))

	MCFG_DEVICE_ADD("konami_7474", TTL7474, 0)
	MCFG_7474_COMP_OUTPUT_CB(WRITELINE(scramble_state,scramble_sh_7474_q_callback))

	MCFG_TIMER_DRIVER_ADD("int_timer", scramble_state, galaxold_interrupt_timer)

	MCFG_WATCHDOG_ADD("watchdog")

	MCFG_MACHINE_RESET_OVERRIDE(scramble_state,scramble)

	MCFG_DEVICE_ADD("ppi8255_0", I8255A, 0)
	MCFG_I8255_IN_PORTA_CB(IOPORT("IN0"))
	MCFG_I8255_IN_PORTB_CB(IOPORT("IN1"))
	MCFG_I8255_IN_PORTC_CB(IOPORT("IN2"))

	MCFG_DEVICE_ADD("ppi8255_1", I8255A, 0)
	MCFG_I8255_OUT_PORTA_CB(DEVWRITE8("soundlatch", generic_latch_8_device, write))
	MCFG_I8255_OUT_PORTB_CB(WRITE8(scramble_state, scramble_sh_irqtrigger_w))

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(16000.0/132/2)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MCFG_SCREEN_SIZE(32*8, 32*8)
	MCFG_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)
	MCFG_SCREEN_UPDATE_DRIVER(scramble_state, screen_update_galaxold)
	MCFG_SCREEN_PALETTE("palette")

	MCFG_GFXDECODE_ADD("gfxdecode", "palette", scramble)
	MCFG_PALETTE_ADD("palette", 32+64+2+1)  /* 32 for characters, 64 for stars, 2 for bullets, 0/1 for background */

	MCFG_PALETTE_INIT_OWNER(scramble_state,scrambold)
	MCFG_VIDEO_START_OVERRIDE(scramble_state,scrambold)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")

	MCFG_GENERIC_LATCH_8_ADD("soundlatch")

	MCFG_SOUND_ADD("8910.1", AY8910, 14318000/8)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.16)

	MCFG_SOUND_ADD("8910.2", AY8910, 14318000/8)
	MCFG_AY8910_PORT_A_READ_CB(DEVREAD8("soundlatch", generic_latch_8_device, read))
	MCFG_AY8910_PORT_B_READ_CB(READ8(scramble_state, scramble_portB_r))
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.16)
MACHINE_CONFIG_END

MACHINE_CONFIG_START(scramble_state::mars)
	scramble(config);

	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(mars_map)

	MCFG_DEVICE_REMOVE("ppi8255_1")
	MCFG_DEVICE_ADD("ppi8255_1", I8255A, 0)
	MCFG_I8255_OUT_PORTA_CB(DEVWRITE8("soundlatch", generic_latch_8_device, write))
	MCFG_I8255_OUT_PORTB_CB(WRITE8(scramble_state, scramble_sh_irqtrigger_w))
	MCFG_I8255_IN_PORTC_CB(IOPORT("IN3"))

	/* video hardware */
	MCFG_PALETTE_MODIFY("palette")
	MCFG_PALETTE_ENTRIES(32+64+2+0)  /* 32 for characters, 64 for stars, 2 for bullets, 0/1 for background */
	MCFG_PALETTE_INIT_OWNER(scramble_state,galaxold)
MACHINE_CONFIG_END

MACHINE_CONFIG_START(scramble_state::devilfsh)
	scramble(config);

	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(mars_map)

	/* video hardware */
	MCFG_GFXDECODE_MODIFY("gfxdecode", devilfsh)
	MCFG_PALETTE_MODIFY("palette")
	MCFG_PALETTE_ENTRIES(32+64+2+0)  /* 32 for characters, 64 for stars, 2 for bullets, 0/1 for background */
	MCFG_PALETTE_INIT_OWNER(scramble_state,galaxold)
MACHINE_CONFIG_END

MACHINE_CONFIG_START(scramble_state::newsin7)
	scramble(config);

	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(newsin7_map)
	MCFG_CPU_VBLANK_INT_DRIVER("screen", scramble_state,  irq0_line_hold) // newsin7a has a corrupt opcode at 0x67, the irq routine instead of NMI avoids it by jumping to 0x68 after doing some other things, probably intentional. newsin7 has this fixed, maybe a bootleg?

	/* video hardware */
	MCFG_GFXDECODE_MODIFY("gfxdecode", newsin7)
	MCFG_PALETTE_MODIFY("palette")
	MCFG_PALETTE_ENTRIES(32+64+2+0)  /* 32 for characters, 64 for stars, 2 for bullets, 0/1 for background */
	MCFG_PALETTE_INIT_OWNER(scramble_state,galaxold)
	MCFG_VIDEO_START_OVERRIDE(scramble_state,newsin7)
MACHINE_CONFIG_END

MACHINE_CONFIG_START(scramble_state::mrkougb)
	scramble(config);

	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(mrkougar_map)

	MCFG_DEVICE_REMOVE("ppi8255_1")
	MCFG_DEVICE_ADD("ppi8255_1", I8255A, 0)
	MCFG_I8255_OUT_PORTA_CB(DEVWRITE8("soundlatch", generic_latch_8_device, write))
	MCFG_I8255_OUT_PORTB_CB(WRITE8(scramble_state, mrkougar_sh_irqtrigger_w))

	/* video hardware */
	MCFG_PALETTE_MODIFY("palette")
	MCFG_PALETTE_ENTRIES(32+64+2+0)  /* 32 for characters, 64 for stars, 2 for bullets, 0/1 for background */
	MCFG_PALETTE_INIT_OWNER(scramble_state,galaxold)
MACHINE_CONFIG_END

MACHINE_CONFIG_START(scramble_state::mrkougar)
	mrkougb(config);

	/* video hardware */
	MCFG_GFXDECODE_MODIFY("gfxdecode", mrkougar)
MACHINE_CONFIG_END

MACHINE_CONFIG_START(scramble_state::ckongs)
	scramble(config);

	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(ckongs_map)

	/* video hardware */
	MCFG_PALETTE_MODIFY("palette")
	MCFG_PALETTE_ENTRIES(32+64+2+0)  /* 32 for characters, 64 for stars, 2 for bullets, 0/1 for background */
	MCFG_PALETTE_INIT_OWNER(scramble_state,galaxold)
	MCFG_VIDEO_START_OVERRIDE(scramble_state,ckongs)
MACHINE_CONFIG_END

MACHINE_CONFIG_START(scramble_state::hotshock)
	scramble(config);

	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(hotshock_map)

	MCFG_DEVICE_REMOVE( "ppi8255_0" )
	MCFG_DEVICE_REMOVE( "ppi8255_1" )

	MCFG_CPU_MODIFY("audiocpu")
	MCFG_CPU_IO_MAP(hotshock_sound_io_map)

	MCFG_MACHINE_RESET_OVERRIDE(scramble_state,galaxold)

	/* video hardware */
	MCFG_PALETTE_MODIFY("palette")
	MCFG_PALETTE_ENTRIES(32+64+2+0)  /* 32 for characters, 64 for stars, 2 for bullets, 0/1 for background */
	MCFG_PALETTE_INIT_OWNER(scramble_state,galaxold)
	MCFG_VIDEO_START_OVERRIDE(scramble_state,pisces)

	MCFG_SOUND_MODIFY("8910.1")
	MCFG_SOUND_ROUTES_RESET()
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.33)

	MCFG_SOUND_MODIFY("8910.2")
	MCFG_AY8910_PORT_A_READ_CB(READ8(scramble_state, hotshock_soundlatch_r))
	MCFG_AY8910_PORT_B_READ_CB(READ8(scramble_state, scramble_portB_r))
	MCFG_SOUND_ROUTES_RESET()
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.33)
MACHINE_CONFIG_END

MACHINE_CONFIG_START(scramble_state::cavelon)
	scramble(config);

	/* basic machine hardware */

	/* video hardware */
	MCFG_PALETTE_MODIFY("palette")
	MCFG_PALETTE_ENTRIES(32+64+2+0)  /* 32 for characters, 64 for stars, 2 for bullets, 0/1 for background */
	MCFG_PALETTE_INIT_OWNER(scramble_state,galaxold)
	MCFG_VIDEO_START_OVERRIDE(scramble_state,ckongs)
MACHINE_CONFIG_END

MACHINE_CONFIG_START(scramble_state::mimonscr)
	scramble(config);

	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(mimonscr_map)

	/* video hardware */
	MCFG_VIDEO_START_OVERRIDE(scramble_state,mimonkey)
MACHINE_CONFIG_END

/* Triple Punch and Mariner are different - only one CPU, one 8910 */
MACHINE_CONFIG_START(scramble_state::triplep)
	scramble(config);

	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(triplep_map)
	MCFG_CPU_IO_MAP(triplep_io_map)

	MCFG_DEVICE_REMOVE("audiocpu")
	MCFG_DEVICE_REMOVE("ppi8255_1")
	MCFG_DEVICE_REMOVE("konami_7474")

	/* video hardware */
	MCFG_PALETTE_MODIFY("palette")
	MCFG_PALETTE_ENTRIES(32+64+2+0)  /* 32 for characters, 64 for stars, 2 for bullets */
	MCFG_PALETTE_INIT_OWNER(scramble_state,galaxold)

	/* sound hardware */
	MCFG_SOUND_MODIFY("8910.1")
	MCFG_SOUND_CLOCK(18432000/12) // triple punch/knock out ay clock is 1.535MHz, derived from main cpu xtal; verified on hardware


	MCFG_SOUND_ROUTES_RESET()
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

	MCFG_DEVICE_REMOVE("8910.2")
MACHINE_CONFIG_END

MACHINE_CONFIG_START(scramble_state::mariner)
	triplep(config);

	/* basic machine hardware */

	/* video hardware */
	MCFG_PALETTE_MODIFY("palette")
	MCFG_PALETTE_ENTRIES(32+64+2+16) /* 32 for characters, 64 for stars, 2 for bullets, 16 for background */

	MCFG_PALETTE_INIT_OWNER(scramble_state,mariner)
	MCFG_VIDEO_START_OVERRIDE(scramble_state,mariner)
MACHINE_CONFIG_END

/* Hunchback replaces the Z80 with a S2650 CPU */
MACHINE_CONFIG_START(scramble_state::hunchbks)
	scramble(config);

	/* basic machine hardware */
	MCFG_CPU_REPLACE("maincpu", S2650, 18432000/6)
	MCFG_CPU_PROGRAM_MAP(hunchbks_map)
	MCFG_CPU_IO_MAP(hunchbks_readport)
	MCFG_S2650_SENSE_INPUT(DEVREADLINE("screen", screen_device, vblank))
	MCFG_CPU_VBLANK_INT_DRIVER("screen", scramble_state,  hunchbks_vh_interrupt)

	MCFG_SCREEN_MODIFY("screen")
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500))

	/* video hardware */
	MCFG_PALETTE_MODIFY("palette")
	MCFG_PALETTE_ENTRIES(32+64+2+0)  /* 32 for characters, 64 for stars, 2 for bullets */

	MCFG_PALETTE_INIT_OWNER(scramble_state,galaxold)
MACHINE_CONFIG_END

MACHINE_CONFIG_START(scramble_state::hncholms)
	hunchbks(config);

	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_CLOCK(18432000/6/2/2)

	MCFG_VIDEO_START_OVERRIDE(scramble_state,scorpion)
MACHINE_CONFIG_END

MACHINE_CONFIG_START(scramble_state::ad2083)
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", Z80, 18432000/6)    /* 3.072 MHz */
	MCFG_CPU_PROGRAM_MAP(ad2083_map)

	MCFG_DEVICE_ADD("konami_7474", TTL7474, 0)
	MCFG_7474_COMP_OUTPUT_CB(WRITELINE(scramble_state,scramble_sh_7474_q_callback))

	MCFG_DEVICE_ADD("7474_9m_1", TTL7474, 0)
	MCFG_7474_OUTPUT_CB(WRITELINE(scramble_state,galaxold_7474_9m_1_callback))

	MCFG_DEVICE_ADD("7474_9m_2", TTL7474, 0)
	MCFG_7474_COMP_OUTPUT_CB(WRITELINE(scramble_state,galaxold_7474_9m_2_q_callback))

	MCFG_TIMER_DRIVER_ADD("int_timer", scramble_state, galaxold_interrupt_timer)

	MCFG_WATCHDOG_ADD("watchdog")

	MCFG_MACHINE_RESET_OVERRIDE(scramble_state,galaxold)

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(16000.0/132/2)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MCFG_SCREEN_SIZE(32*8, 32*8)
	MCFG_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)
	MCFG_SCREEN_UPDATE_DRIVER(scramble_state, screen_update_galaxold)
	MCFG_SCREEN_PALETTE("palette")

	MCFG_GFXDECODE_ADD("gfxdecode", "palette", ad2083)
	MCFG_PALETTE_ADD("palette", 32+64+2+8)  /* 32 for characters, 64 for stars, 2 for bullets, 8 for background */

	MCFG_PALETTE_INIT_OWNER(scramble_state,turtles)
	MCFG_VIDEO_START_OVERRIDE(scramble_state,ad2083)

	/* sound hardware */

	ad2083_audio(config);

MACHINE_CONFIG_END


MACHINE_CONFIG_START(scramble_state::harem)
	scramble(config);

	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(harem_map)
	MCFG_CPU_OPCODES_MAP(decrypted_opcodes_map)

	MCFG_CPU_MODIFY("audiocpu")
	MCFG_CPU_PROGRAM_MAP(harem_sound_map)
	MCFG_CPU_IO_MAP(harem_sound_io_map)

	MCFG_VIDEO_START_OVERRIDE(scramble_state,harem)

	/* extra AY8910 with I/O ports */
	MCFG_SOUND_ADD("8910.3", AY8910, 14318000/8)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.16)
	MCFG_AY8910_PORT_A_WRITE_CB(DEVWRITE8("digitalker", digitalker_device, digitalker_data_w))
	MCFG_AY8910_PORT_B_WRITE_CB(WRITE8(scramble_state, harem_digitalker_control_w))

	MCFG_DIGITALKER_ADD("digitalker", 4000000)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.16)
MACHINE_CONFIG_END

/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( triplep )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "triplep.2g",   0x0000, 0x1000, CRC(c583a93d) SHA1(2bd4a02f945d64ef3ff814d0b8cbf32380d3f790) )
	ROM_LOAD( "triplep.2h",   0x1000, 0x1000, CRC(c03ddc49) SHA1(5a2fba848c4ddf2ef0bb0f00e93dbd88e939441a) )
	ROM_LOAD( "triplep.2k",   0x2000, 0x1000, CRC(e83ca6b5) SHA1(b16690cfe6fb45cf7b9a9cfa22822ba947c0e432) )
	ROM_LOAD( "triplep.2l",   0x3000, 0x1000, CRC(982cc3b9) SHA1(28bb08679126e5aa8bd0b8f387b881fe799fb009) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "triplep.5f",   0x0000, 0x0800, CRC(d51cbd6f) SHA1(c3766a69a4599e54b8d7fb893e45802ec8bf6713) )
	ROM_LOAD( "triplep.5h",   0x0800, 0x0800, CRC(f21c0059) SHA1(b1ba87f13908e3e662de8bf444f59bd5c2009720) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "tripprom.6e",  0x0000, 0x0020, CRC(624f75df) SHA1(0e9a7c48dd976af1dca1d5351236d4d5bf7a9dc8) )
ROM_END

ROM_START( knockout )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "knockout.2h",  0x0000, 0x1000, CRC(eaaa848e) SHA1(661026567db87206200ee610c3d5f5eb725aeec9) )
	ROM_LOAD( "knockout.2k",  0x1000, 0x1000, CRC(bc26d2c0) SHA1(b9934ddb2918f6c4123dafd07cc39ae31d7e28e9) )
	ROM_LOAD( "knockout.2l",  0x2000, 0x1000, CRC(02025c10) SHA1(16ffc7681d949172034b8c85dc72c1a528309abf) )
	ROM_LOAD( "knockout.2m",  0x3000, 0x1000, CRC(e9abc42b) SHA1(93b9c55a76e273b4709ee65870c0848a0d3db7cc) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "triplep.5f",   0x0000, 0x0800, CRC(d51cbd6f) SHA1(c3766a69a4599e54b8d7fb893e45802ec8bf6713) )
	ROM_LOAD( "triplep.5h",   0x0800, 0x0800, CRC(f21c0059) SHA1(b1ba87f13908e3e662de8bf444f59bd5c2009720) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "tripprom.6e",  0x0000, 0x0020, CRC(624f75df) SHA1(0e9a7c48dd976af1dca1d5351236d4d5bf7a9dc8) )
ROM_END

/***************************************************************************
Triple Punch
(C)1982 KKI

board silkscreend       PCO-008-01

Empty 24 pin socket at 2E
Empty 40 pin socket at 0A

.2h 2732    stickered   TD4
.2k 2732    stickered   TC3
.2l 2732    stickered   TE2
.2m 2732    stickered   TD1
.5h 2716    stickered   TA7
.5f 2716    stickered   TA6
.6e 82s123  stickered   TA
***************************************************************************/

ROM_START( triplepa )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "td4.2h", 0x0000, 0x1000, CRC(15a6d46a) SHA1(a76d97876268d303bc18bf6f18f05227a6689b9d) )
	ROM_LOAD( "tc3.2k", 0x1000, 0x1000, CRC(bc26d2c0) SHA1(b9934ddb2918f6c4123dafd07cc39ae31d7e28e9) )
	ROM_LOAD( "te2.2l", 0x2000, 0x1000, CRC(02025c10) SHA1(16ffc7681d949172034b8c85dc72c1a528309abf) )
	ROM_LOAD( "td1.2m", 0x3000, 0x1000, CRC(e9abc42b) SHA1(93b9c55a76e273b4709ee65870c0848a0d3db7cc) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "ta6.5f", 0x0000, 0x0800, CRC(d51cbd6f) SHA1(c3766a69a4599e54b8d7fb893e45802ec8bf6713) )
	ROM_LOAD( "ta7.5h", 0x0800, 0x0800, CRC(f21c0059) SHA1(b1ba87f13908e3e662de8bf444f59bd5c2009720) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "ta.6e", 0x0000, 0x0020, CRC(624f75df) SHA1(0e9a7c48dd976af1dca1d5351236d4d5bf7a9dc8) )
ROM_END

ROM_START( knockoutb )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "k1-2h.bin", 0x0000, 0x1000, CRC(f16bdd40) SHA1(e14aad2f2820f643f32e1ee4dbb78cd469367498) )
	ROM_LOAD( "k2-2k.bin", 0x1000, 0x1000, CRC(fa6214ef) SHA1(5e569b2a4eaf5e4f033cf2375d607c54fb96b1f8) )
	ROM_LOAD( "k3-2l.bin", 0x2000, 0x1000, CRC(9f734d29) SHA1(430c53b9c3fa888fe45f9feeb4ad8f92011adc05) )
	ROM_LOAD( "k4-2m.bin", 0x3000, 0x1000, CRC(1809ec77) SHA1(97d575f47f861a572a483559790e271c21775ac5) )
	ROM_LOAD( "k5-2p.bin", 0x6000, 0x0800, CRC(60c1aa52) SHA1(f0598be70cfca8651edb7bb3c58f01368d6965ce) )
	ROM_CONTINUE(             0x5800, 0x0800             )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "ta6.5f", 0x0000, 0x0800, CRC(d51cbd6f) SHA1(c3766a69a4599e54b8d7fb893e45802ec8bf6713) )
	ROM_LOAD( "ta7.5h", 0x0800, 0x0800, CRC(f21c0059) SHA1(b1ba87f13908e3e662de8bf444f59bd5c2009720) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "ta.6e", 0x0000, 0x0020, CRC(624f75df) SHA1(0e9a7c48dd976af1dca1d5351236d4d5bf7a9dc8) )
ROM_END

ROM_START( knockoutc )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "1.bin", 0x0000, 0x1000, CRC(7c386d1e) SHA1(e846a70bebd41b240758ffb2c7801fa7260b314e) )
	ROM_LOAD( "2.bin", 0x1000, 0x1000, CRC(85dc1d78) SHA1(61e82cfb5b0f0da6413713293c382ea57074e801) )
	ROM_LOAD( "3.bin", 0x2000, 0x1000, CRC(9f734d29) SHA1(430c53b9c3fa888fe45f9feeb4ad8f92011adc05) )
	ROM_LOAD( "4.bin", 0x3000, 0x1000, CRC(48e2d6cd) SHA1(7eea464b8cfef84dbad66e33dffaba9515605da6) )
	ROM_LOAD( "5.bin", 0x6000, 0x0800, CRC(26ac55ba) SHA1(876b8bf2ddb0ffb6256d1447408c8b7f7d048d9f) )
	ROM_CONTINUE(             0x5800, 0x0800             )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "6.bin", 0x0000, 0x0800, CRC(d51cbd6f) SHA1(c3766a69a4599e54b8d7fb893e45802ec8bf6713) )
	ROM_LOAD( "7.bin", 0x0800, 0x0800, CRC(f21c0059) SHA1(b1ba87f13908e3e662de8bf444f59bd5c2009720) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "ta.6e", 0x0000, 0x0020, CRC(624f75df) SHA1(0e9a7c48dd976af1dca1d5351236d4d5bf7a9dc8) )
ROM_END


ROM_START( mariner )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "tp1.2h",       0x0000, 0x1000, CRC(dac1dfd0) SHA1(57b9106bb7452640544ba0ab2d2ba290cccb45f0) )
	ROM_LOAD( "tm2.2k",       0x1000, 0x1000, CRC(efe7ca28) SHA1(496f8eb2ebc9edeed5b19d87f437f23bbeb2a007) )
	ROM_LOAD( "tm3.2l",       0x2000, 0x1000, CRC(027881a6) SHA1(47953aa5140a157ade484341609d477510e8342b) )
	ROM_LOAD( "tm4.2m",       0x3000, 0x1000, CRC(a0fde7dc) SHA1(ea6700520b1bd31e6c6bfac6f067bbf652676eef) )
	ROM_LOAD( "tm5.2p",       0x6000, 0x0800, CRC(d7ebcb8e) SHA1(bddefdc5f04c2f940e08a6968fbd6f930d16b8e4) )
	ROM_CONTINUE(             0x5800, 0x0800             )

	ROM_REGION( 0x2000, "gfx1", 0 )
	ROM_LOAD( "tm8.5f",       0x0000, 0x1000, CRC(70ae611f) SHA1(2686dc6d3910bd58b290d6296f30c552686709f5) )
	ROM_LOAD( "tm9.5h",       0x1000, 0x1000, CRC(8e4e999e) SHA1(195e6896ca2f3175137d8c92777ba32c41e835d3) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "t4.6e",        0x0000, 0x0020, CRC(ca42b6dd) SHA1(d1e224e788e3dcf57249e72f03f9fe3fd71e6c12) )

	ROM_REGION( 0x0100, "user1", 0 )
	ROM_LOAD( "t6.6p",        0x0000, 0x0100, CRC(ad208ccc) SHA1(66a4122e46467344a7f3ddcc953a5f7f451411fa) )    /* background color prom */

	ROM_REGION( 0x0020, "user2", 0 )
	ROM_LOAD( "t5.7p",        0x0000, 0x0020, CRC(1bd88cff) SHA1(8d1620386ef654d99c51e489c822eeb2e8a4fe76) )    /* char banking and star placement */
ROM_END

ROM_START( 800fath )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "tu1.2h",       0x0000, 0x1000, CRC(5dd3d42f) SHA1(887403c385897044e1cb9709ab2b6ff5abdf9eb4) )
	ROM_LOAD( "tm2.2k",       0x1000, 0x1000, CRC(efe7ca28) SHA1(496f8eb2ebc9edeed5b19d87f437f23bbeb2a007) )
	ROM_LOAD( "tm3.2l",       0x2000, 0x1000, CRC(027881a6) SHA1(47953aa5140a157ade484341609d477510e8342b) )
	ROM_LOAD( "tm4.2m",       0x3000, 0x1000, CRC(a0fde7dc) SHA1(ea6700520b1bd31e6c6bfac6f067bbf652676eef) )
	ROM_LOAD( "tu5.2p",       0x6000, 0x0800, CRC(f864a8a6) SHA1(bd0c84284d13d099da4e139db7c9948a074d6774) )
	ROM_CONTINUE(             0x5800, 0x0800             )

	ROM_REGION( 0x2000, "gfx1", 0 )
	ROM_LOAD( "tm8.5f",       0x0000, 0x1000, CRC(70ae611f) SHA1(2686dc6d3910bd58b290d6296f30c552686709f5) )
	ROM_LOAD( "tm9.5h",       0x1000, 0x1000, CRC(8e4e999e) SHA1(195e6896ca2f3175137d8c92777ba32c41e835d3) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "t4.6e",        0x0000, 0x0020, CRC(ca42b6dd) SHA1(d1e224e788e3dcf57249e72f03f9fe3fd71e6c12) )

	ROM_REGION( 0x0100, "user1", 0 )
	ROM_LOAD( "t6.6p",        0x0000, 0x0100, CRC(ad208ccc) SHA1(66a4122e46467344a7f3ddcc953a5f7f451411fa) )    /* background color prom */

	ROM_REGION( 0x0020, "user2", 0 )
	ROM_LOAD( "t5.7p",        0x0000, 0x0020, CRC(1bd88cff) SHA1(8d1620386ef654d99c51e489c822eeb2e8a4fe76) )    /* char banking and star placement */
ROM_END

ROM_START( ckongs )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "vid_2c.bin",   0x0000, 0x1000, CRC(49a8c234) SHA1(91d8da03a76094b6fed4bf1d9a3943dee72bf039) )
	ROM_LOAD( "vid_2e.bin",   0x1000, 0x1000, CRC(f1b667f1) SHA1(c09e0f3b70afd5a4b6ec47ac9237f278dff75783) )
	ROM_LOAD( "vid_2f.bin",   0x2000, 0x1000, CRC(b194b75d) SHA1(514b195dd02a7324e439dd63ae654af117e0c70d) )
	ROM_LOAD( "vid_2h.bin",   0x3000, 0x1000, CRC(2052ba8a) SHA1(e4200219d1a142a4aba8ef21ae1dd806400f4422) )
	ROM_LOAD( "vid_2j.bin",   0x4000, 0x1000, CRC(b377afd0) SHA1(8e42e7623a1749cea1c9861cd7dfa9b97571dc8b) )
	ROM_LOAD( "vid_2l.bin",   0x5000, 0x1000, CRC(fe65e691) SHA1(736fe70c9adc6d2c142fa876f1a1e3c6879eccd8) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "turt_snd.5c",  0x0000, 0x1000, CRC(f0c30f9a) SHA1(5621f336e9be8acf986a34bbb8855ed5d45c28ef) )
	ROM_LOAD( "snd_5d.bin",   0x1000, 0x1000, CRC(892c9547) SHA1(c3ec98049b560eb0ddefdb1e1b2d551b418b9a1c) )

	ROM_REGION( 0x2000, "gfx1", 0 )
	ROM_LOAD( "vid_5f.bin",   0x0000, 0x1000, CRC(7866d2cb) SHA1(62dd8b80bc0459c7337d8a8cb83e53b999e7f4a9) )
	ROM_LOAD( "vid_5h.bin",   0x1000, 0x1000, CRC(7311a101) SHA1(49d54c8b94cae4ba81d7a7684eaa4e87815bb4da) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "vid_6e.bin",   0x0000, 0x0020, CRC(5039af97) SHA1(b1a5b32b8c944bf19d9d97aaf678726df003c194) )
ROM_END

ROM_START( mars )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "u26.3",        0x0000, 0x0800, CRC(2f88892c) SHA1(580c7b502321868f63d9e67286e63b5c5268827c) )
	ROM_LOAD( "u56.4",        0x0800, 0x0800, CRC(9e6bcbf7) SHA1(c3acdba073a1f3703776a7905867d81acf328f37) )
	ROM_LOAD( "u69.5",        0x1000, 0x0800, CRC(df496e6e) SHA1(b597e996ac797a239e4bc8f242f59c98a850723c) )
	ROM_LOAD( "u98.6",        0x1800, 0x0800, CRC(75f274bb) SHA1(2eb83ccc8404c69ab262bf680dce892c23c94f39) )
	ROM_LOAD( "u114.7",       0x2000, 0x0800, CRC(497fd8d0) SHA1(545aaf1d68ff727df356bbcf8ddd23df75b5ce97) )
	ROM_LOAD( "u133.8",       0x2800, 0x0800, CRC(3d4cd59f) SHA1(da96d96a40a896e1272700c50cc34f91ac9f7a23) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "u39.9",        0x0000, 0x0800, CRC(bb5968b9) SHA1(8bc57fd80da7aff294e12e991e9acf60c1ab2893) )
	ROM_LOAD( "u51.10",       0x0800, 0x0800, CRC(75fd7720) SHA1(3ee4ca0d85ffacf0388cada17581da0cdaaf83ef) )
	ROM_LOAD( "u78.11",       0x1000, 0x0800, CRC(72a492da) SHA1(a272f72378850f7ecf52498746c2015f6dad3ab9) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "u72.1",        0x0000, 0x0800, CRC(279789d0) SHA1(3ccf39da252df8b3605efc26299831279d697dd8) )
	ROM_LOAD( "u101.2",       0x0800, 0x0800, CRC(c5dc627f) SHA1(529307238707d0676d1cae508f4eb66bbdd623d7) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "c01s.6e",    0x0000, 0x0020, CRC(4e3caeab) SHA1(a25083c3e36d28afdefe4af6e6d4f3155e303625) )
ROM_END

ROM_START( devilfsh )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "u26.1",        0x0000, 0x0800, CRC(ec047d71) SHA1(c35555010fe239213e92946b65a54612d5a23399) )
	ROM_LOAD( "u56.2",        0x0800, 0x0800, CRC(0138ade9) SHA1(8c75572fa0a5d0665cc46b1c0080192bb0148df9) )
	ROM_LOAD( "u69.3",        0x1000, 0x0800, CRC(5dd0b3fc) SHA1(cf19193986920853d93e4ff38b70dcd46b42276b) )
	ROM_LOAD( "u98.4",        0x1800, 0x0800, CRC(ded0b745) SHA1(2a3c741f11d211b4ec8dfa2dd2b3ae0c0a2d9590) )
	ROM_LOAD( "u114.5",       0x2000, 0x0800, CRC(5fd40176) SHA1(536dd870057c48591f0bee468325c8780afb7026) )
	ROM_LOAD( "u133.6",       0x2800, 0x0800, CRC(03538336) SHA1(66cffcbc53e42c626880151a62721ef3e6bf90bc) )
	ROM_LOAD( "u143.7",       0x3000, 0x0800, CRC(64676081) SHA1(6ae628ad582680d6a825238b60d1195990dbb56f) )
	ROM_LOAD( "u163.8",       0x3800, 0x0800, CRC(bc3d6770) SHA1(ac8803520a668b1759acba23f06ba7a6c5792cbd) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "u39.9",        0x0000, 0x0800, CRC(09987e2e) SHA1(6b0d8413a137726a67ceedeacfc0c48b058698f1) )
	ROM_LOAD( "u51.10",       0x0800, 0x0800, CRC(1e2b1471) SHA1(3ba33b26a5bff21b9ddc34e0a468db7b89361314) )
	ROM_LOAD( "u78.11",       0x1000, 0x0800, CRC(45279aaa) SHA1(20aca9bcfa1010311290cb3d8e4fb548182dcd4b) )

	ROM_REGION( 0x2000, "gfx1", 0 )
	ROM_LOAD( "u72.12",       0x0000, 0x1000, CRC(5406508e) SHA1(1d01a796c3ac22e3fddd1ec2103ef5bd8c409278) )
	ROM_LOAD( "u101.13",      0x1000, 0x1000, CRC(8c4018b6) SHA1(83c4f73a3e40fa6ecece38404fa5f64ab59d7b4e) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "c01s.6e",    0x0000, 0x0020, CRC(4e3caeab) SHA1(a25083c3e36d28afdefe4af6e6d4f3155e303625) )
ROM_END

ROM_START( newsin7 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "newsin.1",     0x0000, 0x1000, CRC(e6c23fe0) SHA1(b15c56ca7868be0f038c246f29ba54f41b5cb755) )
	ROM_LOAD( "newsin.2",     0x1000, 0x1000, CRC(3d477b5f) SHA1(9e22f7262077ce6b00b2efbba0e13dcec143d122) )
	ROM_LOAD( "newsin.3",     0x2000, 0x1000, CRC(7dfa9af0) SHA1(8cbbfff22a3c6429f7ab22d86c8d760b08871bac) )
	ROM_LOAD( "newsin.4",     0x3000, 0x1000, CRC(d1b0ba19) SHA1(66c128bc9b306aa6470de5d413f782ae17e78b14) )
	ROM_LOAD( "newsin.5",     0xa000, 0x1000, CRC(06275d59) SHA1(a5a5436c0b014af06181eeb044d8c4e3188414ea) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "newsin.13",    0x0000, 0x0800, CRC(d88489a2) SHA1(ab10fd4862129824301a0a847de298f1826aa03e) )
	ROM_LOAD( "newsin.12",    0x0800, 0x0800, CRC(b154a7af) SHA1(91ca28b2530f22786ff581e5710b40f0cf99f516) )
	ROM_LOAD( "newsin.11",    0x1000, 0x0800, CRC(7ade709b) SHA1(bda1401172139cd6e3e03424c56e4f59e5afebd5) )

	ROM_REGION( 0x3000, "gfx1", 0 )
	ROM_LOAD( "newsin.7",     0x2000, 0x1000, CRC(6bc5d64f) SHA1(4f52224a4d5294a7487a1fc55eba13cf0b5fb6af) )
	ROM_LOAD( "newsin.8",     0x1000, 0x1000, CRC(0c5b895a) SHA1(994ad7f051b30a3045ffc08ac8d9d7092fbadef3) )
	ROM_LOAD( "newsin.9",     0x0000, 0x1000, CRC(6b87adff) SHA1(c0943832e498ab04978b11b163ba951d4a7e2e60) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "newsin.6",     0x0000, 0x0020, CRC(5cf2cd8d) SHA1(0c85737add75545ab11aaf64fe37c7bd078308c9) )
ROM_END

ROM_START( newsin7a )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "epr2732-1.u26",   0x0000, 0x1000, CRC(67bd4b05) SHA1(a3e397fe0b9baba48fa876c9cf53319f5c7c73f6) )
	ROM_LOAD( "epr2732-2.u56",   0x1000, 0x1000, CRC(93ac59de) SHA1(8eed2bab741ca3d0deeeaede22547ebe90b46a46) )
	ROM_LOAD( "epr2732-3.u69",   0x2000, 0x1000, CRC(6a778d95) SHA1(bf94017f06e7029a8a70e7b7ae741f7eac9d3e07) )
	ROM_LOAD( "epr2732-4.u98",   0x3000, 0x1000, CRC(7f351640) SHA1(5db70661c75f125eb531e84df66d0127c29cb821) )
	ROM_LOAD( "epr2732-5.u114",  0xa000, 0x1000, CRC(06275d59) SHA1(a5a5436c0b014af06181eeb044d8c4e3188414ea) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "epr2716.u39",     0x0000, 0x0800, CRC(d88489a2) SHA1(ab10fd4862129824301a0a847de298f1826aa03e) )
	ROM_LOAD( "epr2716.u51",     0x0800, 0x0800, CRC(b154a7af) SHA1(91ca28b2530f22786ff581e5710b40f0cf99f516) )
	ROM_LOAD( "epr2716.u78",     0x1000, 0x0800, CRC(7ade709b) SHA1(bda1401172139cd6e3e03424c56e4f59e5afebd5) ) /* u78 socket was empty, using the newsin7 one */

	ROM_REGION( 0x3000, "gfx1", 0 )
	ROM_LOAD( "epr2732-7.db",    0x2000, 0x1000, CRC(6bc5d64f) SHA1(4f52224a4d5294a7487a1fc55eba13cf0b5fb6af) )
	ROM_LOAD( "epr2732-6.u133",  0x1000, 0x1000, CRC(0c5b895a) SHA1(994ad7f051b30a3045ffc08ac8d9d7092fbadef3) )
	ROM_LOAD( "epr2732-8.db",    0x0000, 0x1000, CRC(6b87adff) SHA1(c0943832e498ab04978b11b163ba951d4a7e2e60) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "prom.u59",        0x0000, 0x0020, CRC(16cb5f4e) SHA1(36753611808d245d4f3ee55f71394297dd2afc09) ) /* Slightly different color prom */
ROM_END

ROM_START( mrkougar )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "2732-7.bin",   0x0000, 0x1000, CRC(fd060ffb) SHA1(b3bee6fe879f13f3178bef3b2dff3041e698f061) )
	ROM_LOAD( "2732-6.bin",   0x1000, 0x1000, CRC(9e05d868) SHA1(802514f5de347913f0315b42b3689baa37030141) )
	ROM_LOAD( "2732-5.bin",   0x2000, 0x1000, CRC(cbc7c536) SHA1(b959c29bb7ab81ee123ba2f397eef1e32656b441) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "atw-6w-2.bin", 0x0000, 0x1000, CRC(af42a371) SHA1(edacbb29df34fdf400a5c726d851af1479a34c70) )
	ROM_LOAD( "atw-6y-3.bin", 0x1000, 0x1000, CRC(862b8902) SHA1(91dcbc634f7c7ed78dfbd0be5cf1e0631429cfbf) )
	ROM_LOAD( "atw-6z-4.bin", 0x2000, 0x1000, CRC(a0396cc8) SHA1(c8266b58b144a4bc564f3a2503d5b953c0ba6ca7) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "2732-1.bin",   0x0000, 0x1000, CRC(60ef1d43) SHA1(ab42fa98350051526fcc4bfe35ebed3d6daf424f) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "c01s.6e",    0x0000, 0x0020, CRC(4e3caeab) SHA1(a25083c3e36d28afdefe4af6e6d4f3155e303625) )
ROM_END

ROM_START( mrkougar2 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "atw-7l-7.bin", 0x0000, 0x1000, CRC(7b34b198) SHA1(c7793c49c5bd1360ef2d419bc4710b35f0a02760) )
	ROM_LOAD( "atw-7k-6.bin", 0x1000, 0x1000, CRC(fbca23c7) SHA1(da24a01d83174bad36072d4bf6764c5a3e242561) )
	ROM_LOAD( "atw-7h-5.bin", 0x2000, 0x1000, CRC(05b257a2) SHA1(728df1f1cb726d333818db8fedb27bf537be8a36) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "atw-6w-2.bin", 0x0000, 0x1000, CRC(af42a371) SHA1(edacbb29df34fdf400a5c726d851af1479a34c70) )
	ROM_LOAD( "atw-6y-3.bin", 0x1000, 0x1000, CRC(862b8902) SHA1(91dcbc634f7c7ed78dfbd0be5cf1e0631429cfbf) )
	ROM_LOAD( "atw-6z-4.bin", 0x2000, 0x1000, CRC(a0396cc8) SHA1(c8266b58b144a4bc564f3a2503d5b953c0ba6ca7) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "atw-1h-1.bin", 0x0000, 0x1000, CRC(38fdfb63) SHA1(9fc4eafd6d106ffe35c179e59a234c706c489f8c) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "atw-prom.bin", 0x0000, 0x0020, CRC(c65db188) SHA1(90f0a5f22bb761693ab5895da08b20821e79ba65) )
ROM_END

ROM_START( mrkougb )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "p01.bin",      0x0000, 0x0800, CRC(dea0cde1) SHA1(aaf9c622b86d475a90f91628d033989e72dda361) )
	ROM_LOAD( "p02.bin",      0x0800, 0x0800, CRC(c8017751) SHA1(021bd6a6efb90119767162a5847b4bbbc47f321e) )
	ROM_LOAD( "p03.bin",      0x1000, 0x0800, CRC(b8921984) SHA1(1adccd2bad8f748995f844183cac487ad00dd71e) )
	ROM_LOAD( "p04.bin",      0x1800, 0x0800, CRC(b3c9754c) SHA1(16a162a19079125fa01f49d90dbf8cd61b9b4833) )
	ROM_LOAD( "p05.bin",      0x2000, 0x0800, CRC(8d94adbc) SHA1(ac5932c84864e08c6b7937ef20d5bdceb48e2d24) )
	ROM_LOAD( "p06.bin",      0x2800, 0x0800, CRC(acc921ff) SHA1(f75158c62c6b9871ef05a6a97542469698100eb0) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "atw-6w-2.bin", 0x0000, 0x1000, CRC(af42a371) SHA1(edacbb29df34fdf400a5c726d851af1479a34c70) )
	ROM_LOAD( "atw-6y-3.bin", 0x1000, 0x1000, CRC(862b8902) SHA1(91dcbc634f7c7ed78dfbd0be5cf1e0631429cfbf) )
	ROM_LOAD( "atw-6z-4.bin", 0x2000, 0x1000, CRC(a0396cc8) SHA1(c8266b58b144a4bc564f3a2503d5b953c0ba6ca7) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "g07.bin",      0x0000, 0x0800, CRC(0ecfd116) SHA1(0ea173c4f7f2613ef71ee5dcd52c4c6f640020b7) )
	ROM_LOAD( "g08.bin",      0x0800, 0x0800, CRC(00bfa3c6) SHA1(57a7fc48ec740b72baece96d50380dbbc826af77) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "atw-prom.bin", 0x0000, 0x0020, CRC(c65db188) SHA1(90f0a5f22bb761693ab5895da08b20821e79ba65) )
ROM_END

ROM_START( mrkougb2 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "mrk1.bin",     0x0000, 0x0800, CRC(fc93acb9) SHA1(e53373d47959a99f0b6574654d198f43b493c20f) )
	ROM_LOAD( "p02.bin",      0x0800, 0x0800, CRC(c8017751) SHA1(021bd6a6efb90119767162a5847b4bbbc47f321e) )
	ROM_LOAD( "p03.bin",      0x1000, 0x0800, CRC(b8921984) SHA1(1adccd2bad8f748995f844183cac487ad00dd71e) )
	ROM_LOAD( "p04.bin",      0x1800, 0x0800, CRC(b3c9754c) SHA1(16a162a19079125fa01f49d90dbf8cd61b9b4833) )
	ROM_LOAD( "p05.bin",      0x2000, 0x0800, CRC(8d94adbc) SHA1(ac5932c84864e08c6b7937ef20d5bdceb48e2d24) )
	ROM_LOAD( "p06.bin",      0x2800, 0x0800, CRC(acc921ff) SHA1(f75158c62c6b9871ef05a6a97542469698100eb0) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "atw-6w-2.bin", 0x0000, 0x1000, CRC(af42a371) SHA1(edacbb29df34fdf400a5c726d851af1479a34c70) )
	ROM_LOAD( "atw-6y-3.bin", 0x1000, 0x1000, CRC(862b8902) SHA1(91dcbc634f7c7ed78dfbd0be5cf1e0631429cfbf) )
	ROM_LOAD( "atw-6z-4.bin", 0x2000, 0x1000, CRC(a0396cc8) SHA1(c8266b58b144a4bc564f3a2503d5b953c0ba6ca7) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "g07.bin",      0x0000, 0x0800, CRC(0ecfd116) SHA1(0ea173c4f7f2613ef71ee5dcd52c4c6f640020b7) )
	ROM_LOAD( "g08.bin",      0x0800, 0x0800, CRC(00bfa3c6) SHA1(57a7fc48ec740b72baece96d50380dbbc826af77) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "atw-prom.bin", 0x0000, 0x0020, CRC(c65db188) SHA1(90f0a5f22bb761693ab5895da08b20821e79ba65) )
ROM_END

ROM_START( troopy )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "ic2cb.bin",   0x0000, 0x1000, CRC(a1798961) SHA1(45a1f6183016229fced3b459c95c99d83408151a) )
	ROM_LOAD( "ic2eb.bin",   0x1000, 0x1000, CRC(7f3572f9) SHA1(2f89f743a32378ed4ac4184627ed9be007c3334a) )
	ROM_LOAD( "ic2fb.bin",   0x2000, 0x1000, CRC(42e666fd) SHA1(caa6a2b07098ef1d6203309ddd3a591194b4ac70) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "ic5c.bin", 0x0000, 0x1000, CRC(af42a371) SHA1(edacbb29df34fdf400a5c726d851af1479a34c70) )
	ROM_LOAD( "ic5d.bin", 0x1000, 0x1000, CRC(862b8902) SHA1(91dcbc634f7c7ed78dfbd0be5cf1e0631429cfbf) )
	ROM_LOAD( "ic5e.bin", 0x2000, 0x1000, CRC(a0396cc8) SHA1(c8266b58b144a4bc564f3a2503d5b953c0ba6ca7) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "ic5h_neu.bin",      0x0000, 0x0800, CRC(0f4a2394) SHA1(a6c309ca6afa59fbe6549d6fd282902f018a1a48) )
	ROM_LOAD( "ic5f_neu.bin",      0x0800, 0x0800, CRC(cbbfefc2) SHA1(2378949275b8d3fc69551b00d9b2c654b91fd780) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "82s123", 0x0000, 0x0020, CRC(4e3caeab) SHA1(a25083c3e36d28afdefe4af6e6d4f3155e303625) )
ROM_END

ROM_START( hotshock )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "0d.l10", 0x0000, 0x1000, CRC(3e8aeaeb) SHA1(0572623928d36d106c9d8028d92fbd02375291a5) )
	ROM_LOAD( "1d.l9",  0x1000, 0x1000, CRC(0eab3246) SHA1(fac21e341186a7b70893c48cc00b1209dc31bcca) )
	ROM_LOAD( "2d.l8",  0x2000, 0x1000, CRC(e646bde3) SHA1(a0349a096ea00c077d162a945e4797e164a1508f) )
	ROM_LOAD( "3d.l7",  0x3000, 0x1000, CRC(5bde9312) SHA1(d3ba06790c8210f41902bb8ad27a1e5abafccb33) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "6d.b3",  0x0000, 0x1000, CRC(c5e02651) SHA1(105b28d28cad2cddfc0a32b5dec1d21d1ef3f663) )
	ROM_LOAD( "7d.b4",  0x1000, 0x1000, CRC(49dc113d) SHA1(c59642d6cbd6e9c54c9802b8ec550b106e6a3ec3) )

	ROM_REGION( 0x2000, "gfx1", 0 )
	ROM_LOAD( "4d.h3",  0x0000, 0x1000, CRC(751c850e) SHA1(067aaea400bbbcb0819d74a24fba5d7e035c2466) )
	ROM_LOAD( "5d.h5",  0x1000, 0x1000, CRC(fc74282e) SHA1(9a7df0a972cba4ee3c317ef3617b1d69d516bebb) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "18s030.1k",    0x0000, 0x0020, CRC(4e3caeab) SHA1(a25083c3e36d28afdefe4af6e6d4f3155e303625) )
ROM_END

ROM_START( hotshockb )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "hotshock.l10", 0x0000, 0x1000, CRC(401078f7) SHA1(d4415a41eba1d3a2dcbb119f3136c177b02d1fb6) )
	ROM_LOAD( "hotshock.l9",  0x1000, 0x1000, CRC(af76c237) SHA1(bb54e1652a2d2e56731434ed85b40dab4aad91c9) )
	ROM_LOAD( "hotshock.l8",  0x2000, 0x1000, CRC(30486031) SHA1(bed06cb62afee6b000a0e21927559ac5d3538b38) )
	ROM_LOAD( "hotshock.l7",  0x3000, 0x1000, CRC(5bde9312) SHA1(d3ba06790c8210f41902bb8ad27a1e5abafccb33) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "hotshock.b3",  0x0000, 0x1000, CRC(0092f0e2) SHA1(d85b05370fa0ce4ba27fd331bb6a7fae067ce83b) )
	ROM_LOAD( "hotshock.b4",  0x1000, 0x1000, CRC(c2135a44) SHA1(809cf305b1f43f99f2248020c369fb5f1d7c5c44) )

	ROM_REGION( 0x2000, "gfx1", 0 )
	ROM_LOAD( "hotshock.h4",  0x0000, 0x1000, CRC(60bdaea9) SHA1(fd10109803661dc1ce72e1291e3721bdb2bb159f) )
	ROM_LOAD( "hotshock.h5",  0x1000, 0x1000, CRC(4ef17453) SHA1(7dc58456b2f25775c85b3ae92f605d69bb68d590) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "c01s.6e",    0x0000, 0x0020, CRC(4e3caeab) SHA1(a25083c3e36d28afdefe4af6e6d4f3155e303625) )
ROM_END

/*

Conqueror (c) ???? ????
ROM data reveals that it's a space shooter, based on Domino's Hot Shocker, >=1982.

CPU: Z80 (x2)
Sound: AY-3-8910 (x2)
RAM: 74S201 (x5), 2114 (x8), Mostek MN4801AN-3IRL
X1: ??

Notes: Has a very crude, homemade looking potted module which crumbled apart when handled.
Contained two 20-pin DIP chips with no markings. Could be PROMs, PLDs or TTL

*/

ROM_START( conquer )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "conquer3.l10",       0x0000, 0x1000, BAD_DUMP CRC(a33a824f) SHA1(787ac1f1942ba97c64317b9455d6788281c02f60) )
	ROM_LOAD( "conquer2.l9",        0x1000, 0x1000, CRC(3ffa8285) SHA1(a110e52fe5f637606c1be3a9e290fc6625b9aa48) )
	ROM_LOAD( "conquer1.l8",        0x2000, 0x1000, CRC(9ded2dff) SHA1(9364195d3f86e55df5ecf90d53041517c3658388) )

	ROM_REGION( 0x10000, "audiocpu", 0 )    /* 64k for sound code */
	ROM_LOAD( "conquer6.b3",       0x0000, 0x1000, CRC(d363b2ea) SHA1(ca4887d51eee4053cd981b4a97fb8a29eecd14e9) )
	ROM_LOAD( "conquer7.b4",       0x1000, 0x1000, CRC(e6a63d71) SHA1(84e199cd214046829f038bc9f151373e9ced575c) )

	ROM_REGION( 0x2000, "gfx1", 0 )
	ROM_LOAD( "conquer4.h3",       0x0000, 0x1000, CRC(ac533893) SHA1(bb1fee3ec1b856423aa032a905c90a62f405bba8) )
	ROM_LOAD( "conquer5.h5",       0x1000, 0x1000, CRC(d884fd49) SHA1(108ed4a1aebd20b2c44e0bf07c2144b9b58dbda1) ) // data unused? (hotshock gfx still intact)

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "c01s.6e",    0x0000, 0x0020, CRC(4e3caeab) SHA1(a25083c3e36d28afdefe4af6e6d4f3155e303625) )
ROM_END

ROM_START( hunchbks )
	ROM_REGION( 0x8000, "maincpu", 0 )
	ROM_LOAD( "2c_hb01.bin",  0x0000, 0x0800, CRC(8bebd834) SHA1(08f2ce732d2d8754bf559260e1f656a33e2a06a5) )
	ROM_LOAD( "2e_hb02.bin",  0x0800, 0x0800, CRC(07de4229) SHA1(9f333509ae3d6c579f6d96caa172a0abe9eefb30) )
	ROM_LOAD( "2f_hb03.bin",  0x2000, 0x0800, CRC(b75a0dfc) SHA1(c60c833f28c6de027d46f5a2a54ad5646ec58453) )
	ROM_LOAD( "2h_hb04.bin",  0x2800, 0x0800, CRC(f3206264) SHA1(36a614db3fda4f97cc085d84bd13ea44969de95b) )
	ROM_LOAD( "2j_hb05.bin",  0x4000, 0x0800, CRC(1bb78728) SHA1(aebfca355d937825217d069689f9b4d7a113b10a) )
	ROM_LOAD( "2l_hb06.bin",  0x4800, 0x0800, CRC(f25ed680) SHA1(7854e4975a4f75916f60749ac24147c335927394) )
	ROM_LOAD( "2m_hb07.bin",  0x6000, 0x0800, CRC(c72e0e17) SHA1(90da1e375733873bc592e11980bdaf8168bd5aea) )
	ROM_LOAD( "2p_hb08.bin",  0x6800, 0x0800, CRC(412087b0) SHA1(4d6f343577ae73031f32cda8903c74e5a840e71d) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "11d_snd.bin",  0x0000, 0x0800, CRC(88226086) SHA1(fe2da172313063e5b056fc8c8d8b2a5c64db5179) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "5f_hb09.bin",  0x0000, 0x0800, CRC(db489c3d) SHA1(df08607ad07222c1c1c4b3589b50b785bdeefbf2) )
	ROM_LOAD( "5h_hb10.bin",  0x0800, 0x0800, CRC(3977650e) SHA1(1de05d6ceed3f2ed0925caa8235b63a93f03f61e) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "6e_prom.bin",  0x0000, 0x0020, CRC(01004d3f) SHA1(e53cbc54ea96e846481a67bbcccf6b1726e70f9c) )
ROM_END

ROM_START( hunchbks2 )
	ROM_REGION( 0x8000, "maincpu", 0 )
	ROM_LOAD( "hb01.bin",     0x0000, 0x0800, CRC(fec3466a) SHA1(d8ec3b432f7037e99bf1ac1ba7911a34eff6869d) )
	ROM_LOAD( "2e_hb02.bin",  0x0800, 0x0800, CRC(07de4229) SHA1(9f333509ae3d6c579f6d96caa172a0abe9eefb30) )
	ROM_LOAD( "2f_hb03.bin",  0x2000, 0x0800, CRC(b75a0dfc) SHA1(c60c833f28c6de027d46f5a2a54ad5646ec58453) )
	ROM_LOAD( "hb04.bin",     0x2800, 0x0800, CRC(731e349b) SHA1(cfa1ac322cdfe1d4d112b0a4dd85d3552a6e33d0) )
	ROM_LOAD( "2j_hb05.bin",  0x4000, 0x0800, CRC(1bb78728) SHA1(aebfca355d937825217d069689f9b4d7a113b10a) )
	ROM_LOAD( "2l_hb06.bin",  0x4800, 0x0800, CRC(f25ed680) SHA1(7854e4975a4f75916f60749ac24147c335927394) )
	ROM_LOAD( "2m_hb07.bin",  0x6000, 0x0800, CRC(c72e0e17) SHA1(90da1e375733873bc592e11980bdaf8168bd5aea) )
	ROM_LOAD( "2p_hb08.bin",  0x6800, 0x0800, CRC(412087b0) SHA1(4d6f343577ae73031f32cda8903c74e5a840e71d) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "11d_snd.bin",  0x0000, 0x0800, CRC(88226086) SHA1(fe2da172313063e5b056fc8c8d8b2a5c64db5179) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "5f_hb09.bin",  0x0000, 0x0800, CRC(db489c3d) SHA1(df08607ad07222c1c1c4b3589b50b785bdeefbf2) )
	ROM_LOAD( "5h_hb10.bin",  0x0800, 0x0800, CRC(3977650e) SHA1(1de05d6ceed3f2ed0925caa8235b63a93f03f61e) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "6e_prom.bin",  0x0000, 0x0020, CRC(01004d3f) SHA1(e53cbc54ea96e846481a67bbcccf6b1726e70f9c) )
ROM_END


ROM_START( hncholms )
	ROM_REGION( 0x8000, "maincpu", 0 )
	ROM_LOAD( "hncholym.2d",  0x0000, 0x0800, CRC(fb453f9c) SHA1(e4c059b10af1aa8405958c0fd139fb84d08ec9f3) )
	ROM_LOAD( "hncholym.2e",  0x0800, 0x0800, CRC(b1429420) SHA1(9e393750e5651c8b14acc11e3591db0a0a599a4d) )
	ROM_LOAD( "hncholym.2f",  0x2000, 0x0800, CRC(afc98e28) SHA1(efc7918a95d9011cbc0c5fbaee3793d95ecbcf89) )
	ROM_LOAD( "hncholym.2h",  0x2800, 0x0800, CRC(6785bf17) SHA1(e0dadda7d55d2046312a87ed700654952662a6b3) )
	ROM_LOAD( "hncholym.2j",  0x4000, 0x0800, CRC(0e1e4133) SHA1(84c3b8e3e81f6ef3311f1272ee6633cec10b796e) )
	ROM_LOAD( "hncholym.2l",  0x4800, 0x0800, CRC(6e982609) SHA1(2d2aa16ad27f6de486eebfd82b23f7ac706faee5) )
	ROM_LOAD( "hncholym.2m",  0x6000, 0x0800, CRC(b9141914) SHA1(955f7b909b3ec27a07817d031fcbb4029e1cff81) )
	ROM_LOAD( "hncholym.2p",  0x6800, 0x0800, CRC(ca37b55b) SHA1(cb423c1aac91654657e72d0a0cbc311cffc9df0c) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "hncholym.5c",  0x0000, 0x0800, CRC(e7758775) SHA1(3ca843e7519d7f38812e6e2e7b5bb78ac3c02676) )

	ROM_REGION( 0x2000, "gfx1", 0 )
	ROM_LOAD( "hncholym.5f",  0x0000, 0x1000, CRC(75ad3542) SHA1(1094a30861c68c1f4fc85fbfd5606c5feec3843b) )
	ROM_LOAD( "hncholym.5h",  0x1000, 0x1000, CRC(6fec9dd3) SHA1(2366b10e8f9ba58a565ef2e1a6eddf7c4b51fe79) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "prom.6e",      0x0000, 0x0020, CRC(4e3caeab) SHA1(a25083c3e36d28afdefe4af6e6d4f3155e303625) )

	ROM_REGION( 0x0200, "user1", 0 ) /* unknown - from the custom module */
	ROM_LOAD( "82s147.1a",    0x0000, 0x0200, CRC(d461a48b) SHA1(832fc1de4875d5f19e53d72ccf5dcdb5bcbee1af) )
ROM_END

ROM_START( cavelon )
	ROM_REGION( 0x14000, "maincpu", 0 ) /* 64k + 16K banked for code */
	ROM_LOAD( "2.bin",       0x00000, 0x2000, CRC(a3b353ac) SHA1(1d5cc402f83c410f2ccd186dafb8bf16a7778fb0) )
	ROM_LOAD( "1.bin",       0x02000, 0x2000, CRC(3f62efd6) SHA1(b03a46f8478f499812c5d9c11816ee28d67fb77b) )
	ROM_RELOAD(              0x12000, 0x2000)
	ROM_LOAD( "3.bin",       0x10000, 0x2000, CRC(39d74e4e) SHA1(4789eab2741555f59a97ef5a10b0500f6b64a6ce) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "1c_snd.bin",   0x0000, 0x0800, CRC(f58dcf55) SHA1(517dab8684109188d7d78c8a2cf94a4fac17d40c) )

	ROM_REGION( 0x2000, "gfx1", 0 )
	ROM_LOAD( "h.bin",        0x0000, 0x1000, CRC(d44fcd6f) SHA1(c275741bb1d876e7308e131cac2f1fee249613c7) )
	ROM_LOAD( "k.bin",        0x1000, 0x1000, CRC(59bc7f9e) SHA1(4374955d0fdfbba57ba432da22b0d94b66832fb8) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "cavelon.clr",  0x0000, 0x0020, CRC(d133356b) SHA1(58db4013a9ad77107f0d462c96363d7c38d86fa2) )
ROM_END

ROM_START( mimonscr )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "mm1",          0x0000, 0x1000, CRC(0399a0c4) SHA1(8314124f9b535ce531663625d19cd3a76782ed3b) )
	ROM_LOAD( "mm2",          0x1000, 0x1000, CRC(2c5e971e) SHA1(39f979b99566e30a19c63115c936bb11fae4c609) )
	ROM_LOAD( "mm3",          0x2000, 0x1000, CRC(24ce1ce3) SHA1(ae5ba6913cabab2152bf48c0c0d5983ecbe5c700) )
	ROM_LOAD( "mm4",          0x3000, 0x1000, CRC(c83fb639) SHA1(38ddd80b25cc0707b9e53396c322fe731ea8bc3e) )
	ROM_LOAD( "mm5",          0xc000, 0x1000, CRC(a9f12dfc) SHA1(c279e3ac84194cc83642a2c330fd869eaae8f063) )
	ROM_LOAD( "mm6",          0xd000, 0x1000, CRC(e492a40c) SHA1(d01d6f9c18821fd8c7ed11d65d13bd0c9595881f) )
	ROM_LOAD( "mm7",          0xe000, 0x1000, CRC(5339928d) SHA1(7c28516fb7d762e2f77d0ed3dc56a57d0213dbf9) )
	ROM_LOAD( "mm8",          0xf000, 0x1000, CRC(eee7a12e) SHA1(bde6bfe98b15215c48c85a22615b0242ea4f0224) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "mmsound1",     0x0000, 0x1000, CRC(2d14c527) SHA1(062414ce0415b6c471149319ecae22f465df3a4f) )
	ROM_LOAD( "mmsnd2a",      0x1000, 0x1000, CRC(35ed0f96) SHA1(5aaacae5c2acf97540b72491f71ea823f5eeae1a) )

	ROM_REGION( 0x4000, "gfx1", 0 )
	ROM_LOAD( "mmgfx1",       0x0000, 0x2000, CRC(4af47337) SHA1(225f7bcfbb61e3a163ecaed675d4c81b3609562f) )
	ROM_LOAD( "mmgfx2",       0x2000, 0x2000, CRC(def47da8) SHA1(8e62e5dc5c810efaa204d0fcb3d02bc84f61ba35) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "c01s.6e",    0x0000, 0x0020, CRC(4e3caeab) SHA1(a25083c3e36d28afdefe4af6e6d4f3155e303625) )
ROM_END


ROM_START( mimonscra )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "1.c2",          0x0000, 0x1000, CRC(cfff26f3) SHA1(6e611e370ce5a93099bdcdb3b34b86c9c6d49180) )
	ROM_LOAD( "2.e2",          0x1000, 0x1000, CRC(1fca805f) SHA1(eb85d3c08b141d8bd85da38936a5fafcdd2ff709) )
	ROM_LOAD( "3.f2",          0x2000, 0x1000, CRC(24ce1ce3) SHA1(ae5ba6913cabab2152bf48c0c0d5983ecbe5c700) )
	ROM_LOAD( "4.h2",          0x3000, 0x1000, CRC(c83fb639) SHA1(38ddd80b25cc0707b9e53396c322fe731ea8bc3e) )
	ROM_LOAD( "5.j2",          0xc000, 0x1000, CRC(a9f12dfc) SHA1(c279e3ac84194cc83642a2c330fd869eaae8f063) )
	ROM_LOAD( "6.l2",          0xd000, 0x1000, CRC(e492a40c) SHA1(d01d6f9c18821fd8c7ed11d65d13bd0c9595881f) )
	ROM_LOAD( "7.m2",          0xe000, 0x1000, CRC(5339928d) SHA1(7c28516fb7d762e2f77d0ed3dc56a57d0213dbf9) )
	ROM_LOAD( "8.p2",          0xf000, 0x1000, CRC(0b9915b8) SHA1(bb5155eede699d0b612cae458499bd245ab44b4d) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "2732.c5",      0x0000, 0x1000, CRC(5995f24b) SHA1(e3cad71006346e6ca6579c09e65195718687dca1) )
	ROM_LOAD( "2732.d5",      0x1000, 0x1000, CRC(35ed0f96) SHA1(5aaacae5c2acf97540b72491f71ea823f5eeae1a) )

	ROM_REGION( 0x4000, "gfx1", 0 )
	ROM_LOAD( "top.g5",      0x0000, 0x1000, CRC(f73a8412) SHA1(9baf4336cceb9b039372b0a1c733910aeab5ec6d) )
	ROM_LOAD( "bottom.g5",   0x1000, 0x1000, CRC(3828c9db) SHA1(eaf9e81c803ad2be6c2db3104f07f80788378286) )
	ROM_LOAD( "top.f5",      0x2000, 0x1000, CRC(9e0e9289) SHA1(79d412dbceb364bc798feda658b15792feb63338) )
	ROM_LOAD( "bottom.f5",   0x3000, 0x1000, CRC(92085b0c) SHA1(a791703fa9f17e42450c871d902430fc3c6b10ef) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "mb7051.e6",    0x0000, 0x0020, CRC(4e3caeab) SHA1(a25083c3e36d28afdefe4af6e6d4f3155e303625) )
ROM_END

ROM_START( ad2083 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "ad0.10o",      0x0000, 0x2000, CRC(4d34325a) SHA1(4a0eb1cd94382c44ab2642d734d3da9025872eba) )
	ROM_LOAD( "ad1.9o",       0x2000, 0x2000, CRC(0f37134b) SHA1(a935ae013e9fb26b5ef44f7ebd2a043763b146db) )
	ROM_LOAD( "ad2.8o",       0xa000, 0x2000, CRC(bcfa655f) SHA1(6a552c67f48e9ece6c6d38b4151ff6f3dbfd8dcb) )
	ROM_LOAD( "ad3.7o",       0xc000, 0x2000, CRC(60655225) SHA1(628796b23ad66f8f7b2c160d923ecbea10fd7553) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "ad1s.3d",      0x0000, 0x2000, CRC(80f39b0f) SHA1(35671eaf6fc7643ad691414349f1b2772d020e9a) )
	ROM_LOAD( "ad2s.4d",      0x2000, 0x1000, CRC(5177fe2b) SHA1(9aee953ae43131c4db9db71ca69a8ce9ad62ff05) )

	ROM_REGION( 0x4000, "gfx1", 0 )
	ROM_LOAD( "ad4.5k",       0x0000, 0x2000, CRC(388cdd21) SHA1(52f97d8e4f7c7f45a2875f03eadc622b540693e7) )
	ROM_LOAD( "ad5.3k",       0x2000, 0x2000, CRC(f53f3449) SHA1(0711f2e47504f256d46eea1e225e35f9bde8b9fb) )

	ROM_REGION( 0x2000, "tmsprom", 0 ) /* data for the TMS5110 speech chip */
	ROM_LOAD( "ad1v.9a",      0x0000, 0x1000, CRC(4cb93fff) SHA1(2cc686a9a58a85f2bb04fb6ced4626e9952635bb) )
	ROM_LOAD( "ad2v.10a",     0x1000, 0x1000, CRC(4b530ea7) SHA1(8793b3497b598f33b34bf9524e360c6c62e8001d) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "prom-am27s19dc.1m", 0x0000, 0x0020, CRC(2759aebd) SHA1(644fd2c95ca49cbbc0ee1b88ca2563451ddd4fe0) )

	ROM_REGION( 0x0020, "5110ctrl", 0 ) /* data to program TMS5110 speech chip 3x Reset 1x speak */
	ROM_LOAD( "prom-sn74s188.8a",  0x0000, 0x0020, BAD_DUMP CRC(c58a4f6a) SHA1(35ef244b3e94032df2610aa594ea5670b91e1449) )
ROM_END


ROM_START( harem ) /* Main PCB version similar to Scorpion (also developed by I.G.R), sound PCB is identical to Scorpion */
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "harem_prom0.ic85", 0x0000, 0x2000, CRC(4521b753) SHA1(9033f9c3be8fec1e5ff251e9f60faaf3848a1a1e) )
	ROM_LOAD( "harem_prom1.ic87", 0x8000, 0x2000, CRC(3cc5d1e8) SHA1(827e2d20de2a00ec016ead249ed3afdccd0c856c) ) // encrypted

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "harem_sound1.ic12", 0x0000, 0x2000, CRC(b54799dd) SHA1(b6aeb010257cba48a52afd33b4f8031c7d99550c) )
	ROM_LOAD( "harem_sound2.ic13", 0x2000, 0x1000, CRC(2d5573a4) SHA1(1fdcd99d89e078509634742b2116a35bb199fe4b) )

	ROM_REGION( 0x2000, "digitalker", 0 ) // DigiTalker ROM (same exact sound PCB as Scorpion (galaxian.cpp))
	ROM_LOAD( "harem_h1+h2.ic25",  0x0000, 0x2000, CRC(279f923a) SHA1(166b1b625997766f0de7cc18af52c42268022fcb) )

	ROM_REGION( 0x4000, "gfx1", 0 )
	ROM_LOAD( "harem_mask1.ic37", 0x0000, 0x2000, CRC(cb0324fb) SHA1(61612f683810339d5d5f31daa4c475d0338d446f) )
	ROM_LOAD( "harem_mask0.ic36", 0x2000, 0x2000, CRC(64b3c6d6) SHA1(e71092585f7ffdae85b2a4c9add1bc71e5a608a8) )

	ROM_REGION( 0x0020, "proms", 0 )
	ROM_LOAD( "74s288.ic46", 0x0000, 0x0020, CRC(c9a2bf73) SHA1(dad65ebf43a5df147e334afd552e67f5fcd26df7) ) /* BPROM type is 74S288 */
ROM_END


GAME( 1982, triplep,  0,        triplep,  triplep,  scramble_state, scramble_ppi, ROT90, "K.K. International",  "Triple Punch (set 1)",           MACHINE_SUPPORTS_SAVE )
GAME( 1982, triplepa, triplep,  triplep,  triplep,  scramble_state, scramble_ppi, ROT90, "K.K. International",  "Triple Punch (set 2)",           MACHINE_SUPPORTS_SAVE )
GAME( 1982, knockout, triplep,  triplep,  triplep,  scramble_state, scramble_ppi, ROT90, "bootleg? (KKK)",      "Knock Out!! (bootleg, set 1)",         MACHINE_SUPPORTS_SAVE )
GAME( 1982, knockoutb,triplep,  triplep,  knockoutb,scramble_state, scramble_ppi, ROT90, "bootleg",             "Knock Out!! (bootleg, set 2)",         MACHINE_SUPPORTS_SAVE )
GAME( 1982, knockoutc,triplep,  triplep,  knockoutb,scramble_state, scramble_ppi, ROT90, "bootleg (ESG)",       "Knock Out!! (bootleg, set 3)",         MACHINE_SUPPORTS_SAVE ) // ESG = Elektronik Spiel Gerate

GAME( 1981, mariner,  0,        mariner,  scramble, scramble_state, mariner,      ROT90, "Amenip",              "Mariner",                        MACHINE_SUPPORTS_SAVE | MACHINE_IMPERFECT_SOUND )
GAME( 1981, 800fath,  mariner,  mariner,  800fath,  scramble_state, mariner,      ROT90, "Amenip (US Billiards Inc. license)", "800 Fathoms",     MACHINE_SUPPORTS_SAVE | MACHINE_IMPERFECT_SOUND )

GAME( 1981, ckongs,   ckong,    ckongs,   ckongs,   scramble_state, ckongs,       ROT90, "bootleg",             "Crazy Kong (Scramble hardware)", MACHINE_SUPPORTS_SAVE )

GAME( 1981, mars,     0,        mars,     mars,     scramble_state, mars,         ROT90, "Artic",               "Mars",                           MACHINE_SUPPORTS_SAVE )

GAME( 1982, devilfsh, 0,        devilfsh, devilfsh, scramble_state, devilfsh,     ROT90, "Artic",               "Devil Fish",                     MACHINE_SUPPORTS_SAVE )

GAME( 1983, newsin7,  0,        newsin7,  newsin7,  scramble_state, mars,         ROT90, "ATW USA, Inc.",       "New Sinbad 7 (set 1)",           MACHINE_SUPPORTS_SAVE )
GAME( 1982, newsin7a, newsin7,  newsin7,  newsin7,  scramble_state, newsin7a,     ROT90, "ATW USA, Inc",        "New Sinbad 7 (set 2)",           MACHINE_SUPPORTS_SAVE )

GAME( 1984, mrkougar, 0,        mrkougar, mrkougar, scramble_state, mrkougar,     ROT90, "ATW",                 "Mr. Kougar",                     MACHINE_SUPPORTS_SAVE )
GAME( 1983, mrkougar2,mrkougar, mrkougar, mrkougar, scramble_state, mrkougar,     ROT90, "ATW",                 "Mr. Kougar (earlier)",           MACHINE_SUPPORTS_SAVE )
GAME( 1984, mrkougb,  mrkougar, mrkougb,  mrkougar, scramble_state, 0,            ROT90, "bootleg (Gross)",     "Mr. Kougar (German bootleg)",    MACHINE_SUPPORTS_SAVE )
GAME( 1983, mrkougb2, mrkougar, mrkougb,  mrkougar, scramble_state, 0,            ROT90, "bootleg",             "Mr. Kougar (bootleg)",           MACHINE_SUPPORTS_SAVE )
GAME( 1984, troopy,   mrkougar, mrkougb,  mrkougar, scramble_state, mrkougar,     ROT90, "bootleg",             "Troopy (bootleg of Mr. Kougar)", MACHINE_IMPERFECT_GRAPHICS | MACHINE_SUPPORTS_SAVE ) // wrong loading / decoding or bad GFX ROMs?

GAME( 1982, hotshock, 0,        hotshock, hotshock, scramble_state, hotshock,     ROT90, "E.G. Felaco (Domino license)", "Hot Shocker",           MACHINE_SUPPORTS_SAVE )
GAME( 1982, hotshockb,hotshock, hotshock, hotshock, scramble_state, hotshock,     ROT90, "E.G. Felaco",         "Hot Shocker (early revision?)",  MACHINE_SUPPORTS_SAVE ) // has "Dudley presents" (protagonist of the game), instead of Domino

GAME( 198?, conquer,  0,        hotshock, hotshock, scramble_state, 0,            ROT90, "<unknown>",           "Conqueror",                      MACHINE_NOT_WORKING   )

GAME( 1983, hunchbks, hunchbak, hunchbks, hunchbks, scramble_state, scramble_ppi, ROT90, "Century Electronics", "Hunchback (Scramble hardware)",  MACHINE_SUPPORTS_SAVE )
GAME( 1983, hunchbks2,hunchbak, hunchbks, hunchbks, scramble_state, scramble_ppi, ROT90, "bootleg (Sig)",       "Hunchback (Scramble hardware, bootleg)",  MACHINE_SUPPORTS_SAVE )

GAME( 1984, hncholms, huncholy, hncholms, hncholms, scramble_state, scramble_ppi, ROT90, "Century Electronics / Seatongrove Ltd", "Hunchback Olympic (Scramble hardware)", MACHINE_SUPPORTS_SAVE )

GAME( 1983, cavelon,  0,        cavelon,  cavelon,  scramble_state, cavelon,      ROT90, "Jetsoft",             "Cavelon",                        MACHINE_SUPPORTS_SAVE )

GAME( 1982, mimonscr, mimonkey, mimonscr, mimonscr, scramble_state, mimonscr,     ROT90, "bootleg",              "Mighty Monkey (bootleg on Scramble hardware)", MACHINE_SUPPORTS_SAVE )
GAME( 1982, mimonscra,mimonkey, mimonscr, mimonscr, scramble_state, mimonscr,     ROT90, "bootleg (Kaina Games)","Mighty Monkey (Kaina Games, bootleg on Scramble hardware)", MACHINE_SUPPORTS_SAVE )

GAME( 1983, ad2083,   0,        ad2083,   ad2083,   scramble_state, ad2083,       ROT90, "Midcoin",             "A. D. 2083",                     MACHINE_SUPPORTS_SAVE | MACHINE_IMPERFECT_SOUND )

GAME( 1983, harem,    0,        harem,    harem,    scramble_state, harem,        ROT90, "I.G.R.",              "Harem",                          MACHINE_SUPPORTS_SAVE )
