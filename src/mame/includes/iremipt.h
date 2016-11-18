// license:BSD-3-Clause
// copyright-holders:Bryan McPhail, Nicola Salmoria
/*******************************************************************************

    Input port macros used by many games in mutiple IREM drivers

*******************************************************************************/


/**************************** Inputs ****************************/


#define IREM_INPUT_JOY_LOW(PL, STATE, WAY, B01, B02, B04, B08) \
	PORT_BIT( 0x0001, STATE, IPT_JOYSTICK_##B01 ) WAY PORT_PLAYER(PL) \
	PORT_BIT( 0x0002, STATE, IPT_JOYSTICK_##B02 ) WAY PORT_PLAYER(PL) \
	PORT_BIT( 0x0004, STATE, IPT_JOYSTICK_##B04 ) WAY PORT_PLAYER(PL) \
	PORT_BIT( 0x0008, STATE, IPT_JOYSTICK_##B08 ) WAY PORT_PLAYER(PL)

#define IREM_INPUT_JOY_HIGH(PL, STATE, WAY, B100, B200, B400, B800) \
	PORT_BIT( 0x0100, STATE, IPT_JOYSTICK_##B100 ) WAY PORT_PLAYER(PL) \
	PORT_BIT( 0x0200, STATE, IPT_JOYSTICK_##B200 ) WAY PORT_PLAYER(PL) \
	PORT_BIT( 0x0400, STATE, IPT_JOYSTICK_##B400 ) WAY PORT_PLAYER(PL) \
	PORT_BIT( 0x0800, STATE, IPT_JOYSTICK_##B800 ) WAY PORT_PLAYER(PL)

#define IREM_INPUT_BUTTONS_LOW(PL, STATE, BUTTON_A, BUTTON_B, BUTTON_C, BUTTON_D) \
	PORT_BIT( 0x0010, STATE, IPT_##BUTTON_A ) PORT_PLAYER(PL) \
	PORT_BIT( 0x0020, STATE, IPT_##BUTTON_B ) PORT_PLAYER(PL) \
	PORT_BIT( 0x0040, STATE, IPT_##BUTTON_C ) PORT_PLAYER(PL) \
	PORT_BIT( 0x0080, STATE, IPT_##BUTTON_D ) PORT_PLAYER(PL)

#define IREM_INPUT_BUTTONS_HIGH(PL, STATE, BUTTON_A, BUTTON_B, BUTTON_C, BUTTON_D) \
	PORT_BIT( 0x1000, STATE, IPT_##BUTTON_A ) PORT_PLAYER(PL) \
	PORT_BIT( 0x2000, STATE, IPT_##BUTTON_B ) PORT_PLAYER(PL) \
	PORT_BIT( 0x4000, STATE, IPT_##BUTTON_C ) PORT_PLAYER(PL) \
	PORT_BIT( 0x8000, STATE, IPT_##BUTTON_D ) PORT_PLAYER(PL)


#define IREM_GENERIC_JOYSTICKS_2_BUTTONS(PL_L, PL_H) \
	IREM_INPUT_JOY_LOW(PL_L, IP_ACTIVE_LOW, PORT_8WAY, RIGHT, LEFT, DOWN, UP) \
	IREM_INPUT_BUTTONS_LOW(PL_L, IP_ACTIVE_LOW, UNUSED, UNUSED, BUTTON2, BUTTON1) \
	IREM_INPUT_JOY_HIGH(PL_H, IP_ACTIVE_LOW, PORT_8WAY, RIGHT, LEFT, DOWN, UP) \
	IREM_INPUT_BUTTONS_HIGH(PL_H, IP_ACTIVE_LOW, UNUSED, UNUSED, BUTTON2, BUTTON1)

#define IREM_GENERIC_JOYSTICKS_3_BUTTONS(PL_L, PL_H) \
	IREM_INPUT_JOY_LOW(PL_L, IP_ACTIVE_LOW, PORT_8WAY, RIGHT, LEFT, DOWN, UP) \
	IREM_INPUT_BUTTONS_LOW(PL_L, IP_ACTIVE_LOW, UNUSED, BUTTON3, BUTTON2, BUTTON1) \
	IREM_INPUT_JOY_HIGH(PL_H, IP_ACTIVE_LOW, PORT_8WAY, RIGHT, LEFT, DOWN, UP) \
	IREM_INPUT_BUTTONS_HIGH(PL_H, IP_ACTIVE_LOW, UNUSED, BUTTON3, BUTTON2, BUTTON1)

#define IREM_GENERIC_JOYSTICKS_4_BUTTONS(PL_L, PL_H) \
	IREM_INPUT_JOY_LOW(PL_L, IP_ACTIVE_LOW, PORT_8WAY, RIGHT, LEFT, DOWN, UP) \
	IREM_INPUT_BUTTONS_LOW(PL_L, IP_ACTIVE_LOW, BUTTON4, BUTTON3, BUTTON2, BUTTON1) \
	IREM_INPUT_JOY_HIGH(PL_H, IP_ACTIVE_LOW, PORT_8WAY, RIGHT, LEFT, DOWN, UP) \
	IREM_INPUT_BUTTONS_HIGH(PL_H, IP_ACTIVE_LOW, BUTTON4, BUTTON3, BUTTON2, BUTTON1)

#define IREM_GENERIC_JOYSTICKS_2_BUTTONS_START_COIN(PL_L, PL_H, LOW_START, HIGH_START, LOW_COIN, HIGH_COIN) \
	IREM_INPUT_JOY_LOW(PL_L, IP_ACTIVE_LOW, PORT_8WAY, RIGHT, LEFT, DOWN, UP) \
	IREM_INPUT_BUTTONS_LOW(PL_L, IP_ACTIVE_LOW, LOW_START, LOW_COIN, BUTTON2, BUTTON1) \
	IREM_INPUT_JOY_HIGH(PL_H, IP_ACTIVE_LOW, PORT_8WAY, RIGHT, LEFT, DOWN, UP) \
	IREM_INPUT_BUTTONS_HIGH(PL_H, IP_ACTIVE_LOW, HIGH_START, HIGH_COIN, BUTTON2, BUTTON1)

#define IREM_INPUT_PLAYER_3 \
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(3) \
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(3) \
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(3) \
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(3) \
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_START3 ) /* If common slots, Coin3 if separate */ \
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_COIN3 ) \
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(3) \
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(3)
#define IREM_INPUT_PLAYER_4 \
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(4) \
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(4) \
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(4) \
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(4) \
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_START4 ) /* If common slots, Coin3 if separate */ \
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_COIN4 ) \
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(4) \
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(4)


#define IREM_COINS \
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START1 ) \
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START2 ) \
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN1 ) \
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN2 ) \
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE1 ) \
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_SERVICE )  \
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )  \
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* Actually vblank, handled above */

/**************************** Coinage Dip Switches ****************************/

/* Mode 2 Coinage */

#define IREM_COIN_MODE_2_OLD_LOC(DIPBANK) \
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Coin_A ) ) PORT_CONDITION("DSW2", 0x04, EQUALS, 0x00) PORT_DIPLOCATION(#DIPBANK":5,6") \
	PORT_DIPSETTING(    0x10, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(    0x20, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_1C ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) ) \
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Coin_B ) ) PORT_CONDITION("DSW2", 0x04, EQUALS, 0x00) PORT_DIPLOCATION(#DIPBANK":7,8") \
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_2C ) ) \
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_3C ) ) \
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_5C ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_6C ) )

#define IREM_COIN_MODE_2_DSW1_LOC(DIPBANK) \
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Coin_A ) ) PORT_CONDITION("DSW2", 0x04, EQUALS, 0x00) PORT_DIPLOCATION(#DIPBANK":5,6") \
	PORT_DIPSETTING(    0x00, DEF_STR( 5C_1C ) ) \
	PORT_DIPSETTING(    0x10, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(    0x20, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_1C ) ) \
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Coin_B ) ) PORT_CONDITION("DSW2", 0x04, EQUALS, 0x00) PORT_DIPLOCATION(#DIPBANK":7,8") \
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_2C ) ) \
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_3C ) ) \
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_5C ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_6C ) )

/* on newer boards, coinage settings moved to DSW2 and coin mode dip was put on   */
/* DSW1. We may add the tag in the condition and the location as parameters here, */
/* to make the definitions a bit more general. However, these are enough to cover */
/* all the currently emulated games */
#define IREM_COIN_MODE_2 \
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Coin_A ) ) PORT_CONDITION("DSW1", 0x04, EQUALS, 0x00) PORT_DIPLOCATION("SW2:5,6") \
	PORT_DIPSETTING(    0x00, DEF_STR( 5C_1C ) ) \
	PORT_DIPSETTING(    0x10, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(    0x20, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_1C ) ) \
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Coin_B ) ) PORT_CONDITION("DSW1", 0x04, EQUALS, 0x00) PORT_DIPLOCATION("SW2:7,8") \
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_2C ) ) \
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_3C ) ) \
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_5C ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_6C ) )

#define IREM_COIN_MODE_2_HIGH \
	PORT_DIPNAME( 0x3000, 0x3000, DEF_STR( Coin_A ) ) PORT_CONDITION("DSW", 0x0800, EQUALS, 0x0000) PORT_DIPLOCATION("SW2:5,6") \
	PORT_DIPSETTING(      0x0000, DEF_STR( 5C_1C ) ) \
	PORT_DIPSETTING(      0x1000, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(      0x2000, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(      0x3000, DEF_STR( 1C_1C ) ) \
	PORT_DIPNAME( 0xc000, 0xc000, DEF_STR( Coin_B ) ) PORT_CONDITION("DSW", 0x0800, EQUALS, 0x0000) PORT_DIPLOCATION("SW2:7,8") \
	PORT_DIPSETTING(      0xc000, DEF_STR( 1C_2C ) ) \
	PORT_DIPSETTING(      0x8000, DEF_STR( 1C_3C ) ) \
	PORT_DIPSETTING(      0x4000, DEF_STR( 1C_5C ) ) \
	PORT_DIPSETTING(      0x0000, DEF_STR( 1C_6C ) )


/* Mode 1 Coinage */

#define IREM_Z80_COINAGE_TYPE_1_LOC(DIPBANK) \
	/* Coin Mode 1 */ \
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coinage ) ) PORT_CONDITION("DSW2", 0x04, NOTEQUALS, 0x00) PORT_DIPLOCATION(#DIPBANK":5,6,7,8") \
/*  PORT_DIPSETTING(    0x80, DEF_STR( 1C_1C ) ) */ \
	PORT_DIPSETTING(    0x90, DEF_STR( 7C_1C ) ) \
	PORT_DIPSETTING(    0xa0, DEF_STR( 6C_1C ) ) \
	PORT_DIPSETTING(    0xb0, DEF_STR( 5C_1C ) ) \
	PORT_DIPSETTING(    0xc0, DEF_STR( 4C_1C ) ) \
	PORT_DIPSETTING(    0xd0, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(    0xe0, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) ) \
	PORT_DIPSETTING(    0x70, DEF_STR( 1C_2C ) ) \
	PORT_DIPSETTING(    0x60, DEF_STR( 1C_3C ) ) \
	PORT_DIPSETTING(    0x50, DEF_STR( 1C_4C ) ) \
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_5C ) ) \
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_6C ) ) \
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_7C ) ) \
	PORT_DIPSETTING(    0x10, DEF_STR( 1C_8C ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) ) \
	/* Coin Mode 2 */ \
	IREM_COIN_MODE_2_OLD_LOC(DIPBANK)

#define IREM_Z80_COINAGE_TYPE_2_LOC(DIPBANK) \
	/* Coin Mode 1 */ \
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coinage ) ) PORT_CONDITION("DSW2", 0x04, NOTEQUALS, 0x00) PORT_DIPLOCATION(#DIPBANK":5,6,7,8") \
/*  PORT_DIPSETTING(    0x80, DEF_STR( Free_Play ) ) */ \
/*  PORT_DIPSETTING(    0x90, DEF_STR( Free_Play ) ) */ \
	PORT_DIPSETTING(    0xa0, DEF_STR( 6C_1C ) ) \
	PORT_DIPSETTING(    0xb0, DEF_STR( 5C_1C ) ) \
	PORT_DIPSETTING(    0xc0, DEF_STR( 4C_1C ) ) \
	PORT_DIPSETTING(    0xd0, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(    0xe0, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) ) \
	PORT_DIPSETTING(    0x70, DEF_STR( 1C_2C ) ) \
	PORT_DIPSETTING(    0x60, DEF_STR( 1C_3C ) ) \
	PORT_DIPSETTING(    0x50, DEF_STR( 1C_4C ) ) \
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_5C ) ) \
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_6C ) ) \
/*  PORT_DIPSETTING(    0x20, DEF_STR( Free_Play ) ) */ \
/*  PORT_DIPSETTING(    0x10, DEF_STR( Free_Play ) ) */ \
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) ) \
	/* Coin Mode 2 */ \
	IREM_COIN_MODE_2_OLD_LOC(DIPBANK)

#define IREM_Z80_COINAGE_TYPE_3_LOC(DIPBANK) \
	/* Coin Mode 1 */ \
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coinage ) ) PORT_CONDITION("DSW2", 0x04, NOTEQUALS, 0x00) PORT_DIPLOCATION(#DIPBANK":5,6,7,8") \
	PORT_DIPSETTING(    0x90, DEF_STR( 7C_1C ) ) \
	PORT_DIPSETTING(    0xa0, DEF_STR( 6C_1C ) ) \
	PORT_DIPSETTING(    0xb0, DEF_STR( 5C_1C ) ) \
	PORT_DIPSETTING(    0xc0, DEF_STR( 4C_1C ) ) \
	PORT_DIPSETTING(    0xd0, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(    0xe0, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) ) \
/*  PORT_DIPSETTING(    0x80, DEF_STR( 1C_1C ) ) */ \
	PORT_DIPSETTING(    0x70, DEF_STR( 1C_2C ) ) \
	PORT_DIPSETTING(    0x60, DEF_STR( 1C_3C ) ) \
	PORT_DIPSETTING(    0x50, DEF_STR( 1C_4C ) ) \
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_5C ) ) \
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_6C ) ) \
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_7C ) ) \
	PORT_DIPSETTING(    0x10, DEF_STR( 1C_8C ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) ) \
	/* Coin Mode 2 */ \
	IREM_COIN_MODE_2_OLD_LOC(DIPBANK)

#define IREM_Z80_COINAGE_TYPE_4_LOC(DIPBANK) \
	/* Coin Mode 1 */ \
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coinage ) ) PORT_CONDITION("DSW2", 0x04, NOTEQUALS, 0x00) PORT_DIPLOCATION(#DIPBANK":5,6,7,8") \
	PORT_DIPSETTING(    0xa0, DEF_STR( 6C_1C ) ) \
	PORT_DIPSETTING(    0xb0, DEF_STR( 5C_1C ) ) \
	PORT_DIPSETTING(    0xc0, DEF_STR( 4C_1C ) ) \
	PORT_DIPSETTING(    0xd0, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(    0x10, DEF_STR( 8C_3C ) ) \
	PORT_DIPSETTING(    0xe0, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0x20, DEF_STR( 5C_3C ) ) \
	PORT_DIPSETTING(    0x30, DEF_STR( 3C_2C ) ) \
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) ) \
	PORT_DIPSETTING(    0x40, DEF_STR( 2C_3C ) ) \
	PORT_DIPSETTING(    0x90, DEF_STR( 1C_2C ) ) \
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_3C ) ) \
	PORT_DIPSETTING(    0x70, DEF_STR( 1C_4C ) ) \
	PORT_DIPSETTING(    0x60, DEF_STR( 1C_5C ) ) \
	PORT_DIPSETTING(    0x50, DEF_STR( 1C_6C ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) ) \
	/* Coin Mode 2 */ \
	IREM_COIN_MODE_2_DSW1_LOC(DIPBANK)

#define IREM_Z80_COINAGE_TYPE_5_LOC(DIPBANK) \
	/* Coin Mode 1 */ \
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coinage ) ) PORT_CONDITION("DSW2", 0x04, NOTEQUALS, 0x00) PORT_DIPLOCATION(#DIPBANK":5,6,7,8") \
	PORT_DIPSETTING(    0x00, DEF_STR( 8C_1C ) ) \
	PORT_DIPSETTING(    0xa0, DEF_STR( 6C_1C ) ) \
	PORT_DIPSETTING(    0xb0, DEF_STR( 5C_1C ) ) \
	PORT_DIPSETTING(    0xc0, DEF_STR( 4C_1C ) ) \
	PORT_DIPSETTING(    0xd0, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(    0xe0, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0x10, DEF_STR( 5C_3C ) ) \
	PORT_DIPSETTING(    0x20, "3 Coins/5 Credits" ) \
	PORT_DIPSETTING(    0x30, DEF_STR( 3C_2C ) ) \
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) ) \
	PORT_DIPSETTING(    0x40, DEF_STR( 2C_3C ) ) \
	PORT_DIPSETTING(    0x90, DEF_STR( 1C_2C ) ) \
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_3C ) ) \
	PORT_DIPSETTING(    0x70, DEF_STR( 1C_4C ) ) \
	PORT_DIPSETTING(    0x60, DEF_STR( 1C_5C ) ) \
	PORT_DIPSETTING(    0x50, DEF_STR( 1C_6C ) ) \
	/* Coin Mode 2 */ \
	IREM_COIN_MODE_2_DSW1_LOC(DIPBANK)


/* New Coin Mode 1, introduced in later m72 games (m72 early games have different Coinage) */

#define IREM_COIN_MODE_1_NEW \
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coinage ) ) PORT_DIPLOCATION("SW2:5,6,7,8") \
	PORT_DIPSETTING(    0xa0, DEF_STR( 6C_1C ) ) \
	PORT_DIPSETTING(    0xb0, DEF_STR( 5C_1C ) ) \
	PORT_DIPSETTING(    0xc0, DEF_STR( 4C_1C ) ) \
	PORT_DIPSETTING(    0xd0, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(    0xe0, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0x10, "2 Coins to Start/1 to Continue") \
	PORT_DIPSETTING(    0x30, DEF_STR( 3C_2C ) ) \
	PORT_DIPSETTING(    0x20, DEF_STR( 4C_3C ) ) \
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) ) \
	PORT_DIPSETTING(    0x40, DEF_STR( 2C_3C ) ) \
	PORT_DIPSETTING(    0x90, DEF_STR( 1C_2C ) ) \
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_3C ) ) \
	PORT_DIPSETTING(    0x70, DEF_STR( 1C_4C ) ) \
	PORT_DIPSETTING(    0x60, DEF_STR( 1C_5C ) ) \
	PORT_DIPSETTING(    0x50, DEF_STR( 1C_6C ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )

#define IREM_COIN_MODE_1_NEW_HIGH \
	PORT_DIPNAME( 0xf000, 0xf000, DEF_STR( Coinage ) ) PORT_CONDITION("DSW", 0x0800, NOTEQUALS, 0x0000)  PORT_DIPLOCATION("SW2:5,6,7,8") \
	PORT_DIPSETTING(      0xa000, DEF_STR( 6C_1C ) ) \
	PORT_DIPSETTING(      0xb000, DEF_STR( 5C_1C ) ) \
	PORT_DIPSETTING(      0xc000, DEF_STR( 4C_1C ) ) \
	PORT_DIPSETTING(      0xd000, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(      0xe000, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(      0x1000, "2 Coins to Start/1 to Continue") \
	PORT_DIPSETTING(      0x3000, DEF_STR( 3C_2C ) ) \
	PORT_DIPSETTING(      0x2000, DEF_STR( 4C_3C ) ) \
	PORT_DIPSETTING(      0xf000, DEF_STR( 1C_1C ) ) \
	PORT_DIPSETTING(      0x4000, DEF_STR( 2C_3C ) ) \
	PORT_DIPSETTING(      0x9000, DEF_STR( 1C_2C ) ) \
	PORT_DIPSETTING(      0x8000, DEF_STR( 1C_3C ) ) \
	PORT_DIPSETTING(      0x7000, DEF_STR( 1C_4C ) ) \
	PORT_DIPSETTING(      0x6000, DEF_STR( 1C_5C ) ) \
	PORT_DIPSETTING(      0x5000, DEF_STR( 1C_6C ) ) \
	PORT_DIPSETTING(      0x0000, DEF_STR( Free_Play ) )
