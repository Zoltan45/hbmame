// license:CC0
// copyright-holders:Colin Douglas Howell

#include "netlist/devices/net_lib.h"


#ifndef __PLIB_PREPROCESSOR__

	#define NOISE(_name)                                                     \
			NET_REGISTER_DEV(NOISE, _name)

#endif

#define USE_FRONTIERS	1
#define FAST_SWITCHES	1

// I got this circuit's layout from Midway's schematic, "Gun Fight Sound
// Generator Detail P.C. 597-907E".

// All of the amplifying transistors in this circuit are NPN transistors of a
// particular type, marked as "A-138" on the Gun Fight schematics. This seems
// to be the Amperex A-138, a low-power silicon NPN transistor made by Amperex
// Electronics, which was a U.S. division of Philips. This transistor is long
// obsolete and has fallen into obscurity. The only source I could find with
// any data on it (given in greatly abbreviated form) is _Transistor
// Discontinued Devices D.A.T.A.Book, Edition 16_ (1980), by Derivation and
// Tabulation Associates; the A-138 is listed on p. 76, line 86. From that
// source, and the way the transistor is used in Gun Fight, it appears to be
// a fairly ordinary NPN transistor not very different from the 2N3904.

// Gun Fight's noise generator circuit is based on a reverse-biased 9.1-volt
// Zener diode (D304, a 1N5239). The Zener's noise current is amplified by a
// transistor, resulting in white noise output whose peak-to-peak voltage
// should be around a volt, according to various sources (e.g., _Diode,
// Transistor & FET Circuits Manual_, R. M. Marston (2013), p. 107). Since the
// netlist library does not yet support Zener diodes or their noise behavior,
// I use a very simple 1-volt peak-to-peak white noise generator instead.

static NETLIST_START(NOISE)
	VS(FV, 0)
	PARAM(FV.FUNC, "rand() - 0.5")

	ALIAS(P, FV.P)
	ALIAS(N, FV.N)
NETLIST_END()

// Most of the Gun Fight sound circuits have a common structure which is
// repeated four times, one for each type of sound effect. (For the common
// left-master and right-master amplifiers and outputs, the structure is
// repeated only twice.) To make the netlist easier to follow and verify, I've
// grouped its components and connections by function, ordering the groups
// approximately according to the flow of signals from the activating switches
// (and the common noise source) to the initial sound-effect filters and
// amplifiers, to their coupling capacitors and potentiometers, and then to
// the mixing circuits and the second-stage amplifiers, coupling capacitors
// and master-volume potentiometers. Within each group, I've grouped the sets
// of four similar components or connections together and listed them in the
// order: left-shot, right-shot, left-hit, right-hit. (For master-related
// components, the order is simply left and right.)

// The sound effects are simple "bang" sounds that are generated by
// single-transistor amplifiers which are wired in a common-emitter
// configuration. These amplifiers draw their current from storage capacitors
// which are charged from the 16.5-volt power supply via dual-transistor
// switches; the switches, in turn, are activated by digital signals from TTL
// logic. The charging of the capacitors by the switches, and their
// discharging by the amplifiers, provide the attack and decay of the "bang"
// sounds.

static NETLIST_START(gunfight_schematics)

#if FAST_SWITCHES

	// Use fast, abstracted activation switches instead of a detailed
	// circuit model of the actual dual-transistor activation switches.
	// Although the latter would simulate the total circuit more
	// accurately, it does not make any audible difference in the
	// sound output produced.

	RES_SWITCH(SW_LEFT_SHOT,  IN_LS, I_V16_5.Q, R130.1)
	RES_SWITCH(SW_RIGHT_SHOT, IN_RS, I_V16_5.Q, R230.1)
	RES_SWITCH(SW_LEFT_HIT,   IN_LH, I_V16_5.Q, R117.1)
	RES_SWITCH(SW_RIGHT_HIT,  IN_RH, I_V16_5.Q, R217.1)

	// The default on-resistance of 1 ohm is a bit high for my tastes,
	// considering the charging resistor which follows is only 15 ohms.
	PARAM(SW_LEFT_SHOT.RON, 0.1)
	PARAM(SW_RIGHT_SHOT.RON, 0.1)
	PARAM(SW_LEFT_HIT.RON, 0.1)
	PARAM(SW_RIGHT_HIT.RON, 0.1)

#else

	// Each dual-transistor switch has two parts: a "low-side" NPN
	// transistor which is switched by a TTL logic input, and a
	// "high-side" PNP transistor which is switched by the low-side
	// NPN transistor. It is the PNP transistor which actually switches
	// the 16.5-volt power.

	// "Low-side" NPN transistor switches, driven by TTL logic inputs.

	RES(R134, RES_R(470))
	RES(R234, RES_R(470))
	RES(R121, RES_R(470))
	RES(R221, RES_R(470))

	NET_C(IN_LS, R134.1)
	NET_C(IN_RS, R234.1)
	NET_C(IN_LH, R121.1)
	NET_C(IN_RH, R221.1)

	RES(R133, RES_K(5.1))
	RES(R233, RES_K(5.1))
	RES(R120, RES_K(5.1))
	RES(R220, RES_K(5.1))

	QBJT_SW(Q108, "NPN")
	QBJT_SW(Q208, "NPN")
	QBJT_SW(Q105, "NPN")
	QBJT_SW(Q205, "NPN")

	// These actually all go to TTL ground at pin 7 of 7404 IC H6,
	// rather than the usual circuit ground.
	NET_C(R133.2, R233.2, R120.2, R220.2, Q108.E, Q208.E, Q105.E, Q205.E, GND)

	NET_C(R134.2, R133.1, Q108.B)
	NET_C(R234.2, R233.1, Q208.B)
	NET_C(R121.2, R120.1, Q105.B)
	NET_C(R221.2, R220.1, Q205.B)

	RES(R131, RES_K(1))
	RES(R231, RES_K(1))
	RES(R118, RES_K(1))
	RES(R218, RES_K(1))

	NET_C(Q108.C, R131.1)
	NET_C(Q208.C, R231.1)
	NET_C(Q105.C, R118.1)
	NET_C(Q205.C, R218.1)

	// "High-side" PNP transistor switches, driven by "low-side"
	// NPN switch outputs. The output of these goes to the storage
	// capacitors and the current inputs of the sound-effect amplifiers.

	RES(R132, RES_K(5.1))
	RES(R232, RES_K(5.1))
	RES(R119, RES_K(5.1))
	RES(R219, RES_K(5.1))

	QBJT_SW(Q107, "PNP")
	QBJT_SW(Q207, "PNP")
	QBJT_SW(Q104, "PNP")
	QBJT_SW(Q204, "PNP")

	// All connected to 16.5-volt power.
	NET_C(R132.1, R232.1, R119.1, R219.1, Q107.E, Q207.E, Q104.E, Q204.E, I_V16_5.Q)

	NET_C(R131.2, R132.2, Q107.B)
	NET_C(R231.2, R232.2, Q207.B)
	NET_C(R118.2, R119.2, Q104.B)
	NET_C(R218.2, R219.2, Q204.B)

	NET_C(Q107.C, R130.1)
	NET_C(Q207.C, R230.1)
	NET_C(Q104.C, R117.1)
	NET_C(Q204.C, R217.1)

	// Description of switches ends here.

#endif

	// The following are the current inputs to the storage capacitors and
	// sound-effect amplifiers.

	RES(R130, RES_R(15))
	RES(R230, RES_R(15))
	RES(R117, RES_R(15))
	RES(R217, RES_R(15))

	CAP(C122, CAP_U(10))
	CAP(C222, CAP_U(10))
	CAP(C116, CAP_U(20))
	CAP(C216, CAP_U(20))
	NET_C(C122.2, C222.2, C116.2, C216.2, GND)

	NET_C(R130.2, C122.1, R126.1, R124.1)
	NET_C(R230.2, C222.1, R226.1, R224.1)
	NET_C(R117.2, C116.1, R113.1, R111.1)
	NET_C(R217.2, C216.1, R213.1, R211.1)

	// Use a simple noise generating macro device, based on a voltage
	// source, to simulate the AC noise generated by amplified Zener.
	// This is the basic noise source which is filtered and amplified by
	// the sound-effect circuits.

	NOISE(N1)
	NET_C(N1.N, GND)

	// Coupling capacitors from noise generator to sound effect
	// frequency filters.

	CAP(C303, CAP_U(0.1))
	CAP(C306, CAP_U(0.1))
	CAP(C304, CAP_U(0.1))
	CAP(C305, CAP_U(0.1))

	NET_C(C303.1, C306.1, C304.1, C305.1, N1.P)

	// Sound effect frequency filters. Each of these is a second-order
	// low-pass filter with cutoff frequency determined by its component
	// values. The different capacitor values produce each sound effect's
	// distinct pitch.

	RES(R129, RES_K(20))
	RES(R229, RES_K(20))
	RES(R116, RES_K(20))
	RES(R216, RES_K(20))

	NET_C(C303.2, R129.1)
	NET_C(C306.2, R229.1)
	NET_C(C304.2, R116.1)
	NET_C(C305.2, R216.1)

	CAP(C121, CAP_U(0.015))
	CAP(C221, CAP_U(0.015))
	CAP(C115, CAP_U(0.033))
	CAP(C215, CAP_U(0.033))
	NET_C(C121.2, C221.2, C115.2, C215.2, GND)

	RES(R128, RES_K(30))
	RES(R228, RES_K(30))
	RES(R115, RES_K(30))
	RES(R215, RES_K(30))

	NET_C(R129.2, C121.1, R128.1)
	NET_C(R229.2, C221.1, R228.1)
	NET_C(R116.2, C115.1, R115.1)
	NET_C(R216.2, C215.1, R215.1)

	CAP(C120, CAP_U(0.01))
	CAP(C220, CAP_U(0.015))
	CAP(C114, CAP_U(0.1))
	CAP(C214, CAP_U(0.047))
	NET_C(C120.2, C220.2, C114.2, C214.2, GND)

	// Sound effect amplifier circuits. Each of these is a single NPN
	// transistor wired as a common-emitter amplifier. The "hit"
	// sound effect amplifiers also have a bypass capacitor at the
	// emitter. The attack and decay of the sound effects is handled
	// by controlling the current supply to each amplifier, which is
	// done by the switching circuits described above.

	RES(R126, RES_K(330))
	RES(R226, RES_K(330))
	RES(R113, RES_K(330))
	RES(R213, RES_K(330))

	RES(R127, RES_K(30))
	RES(R227, RES_K(30))
	RES(R114, RES_K(30))
	RES(R214, RES_K(30))
	NET_C(R127.2, R227.2, R114.2, R214.2, GND)

	RES(R124, RES_K(5.1))
	RES(R224, RES_K(5.1))
	RES(R111, RES_K(5.1))
	RES(R211, RES_K(5.1))

	RES(R125, RES_R(100))
	RES(R225, RES_R(100))
	RES(R112, RES_K(1))
	RES(R212, RES_K(1))
	NET_C(R125.2, R225.2, R112.2, R212.2, GND)

	CAP(C113, CAP_U(50))
	CAP(C213, CAP_U(50))
	NET_C(C113.2, C213.2, GND)

	QBJT_EB(Q106, "NPN")
	QBJT_EB(Q206, "NPN")
	QBJT_EB(Q103, "NPN")
	QBJT_EB(Q203, "NPN")

	NET_C(R128.2, C120.1, R126.2, R127.1, Q106.B)
	NET_C(R228.2, C220.1, R226.2, R227.1, Q206.B)
	NET_C(R115.2, C114.1, R113.2, R114.1, Q103.B)
	NET_C(R215.2, C214.1, R213.2, R214.1, Q203.B)

	NET_C(R125.1, Q106.E)
	NET_C(R225.1, Q206.E)
	NET_C(R112.1, C113.1, Q103.E)
	NET_C(R212.1, C213.1, Q203.E)

	// Coupling capacitors and volume potentiometers for sound effect
	// amplifier outputs. There are four volume pots, for shot and hit
	// sounds on left and right. Presently these pots are fixed at the
	// midpoint of their range.

	CAP(C119, CAP_U(0.047))
	CAP(C219, CAP_U(0.047))
	CAP(C112, CAP_U(0.1))
	CAP(C212, CAP_U(0.1))

	NET_C(R124.2, Q106.C, C119.1)
	NET_C(R224.2, Q206.C, C219.1)
	NET_C(R111.2, Q103.C, C112.1)
	NET_C(R211.2, Q203.C, C212.1)

	CAP(C118, CAP_U(0.022))
	CAP(C218, CAP_U(0.022))
	CAP(C111, CAP_U(0.033))
	CAP(C211, CAP_U(0.033))
	NET_C(C118.2, C218.2, C111.2, C211.2, GND)

	// FIXME - sound effect volume potentiometers should be adjustable.
	POT(R123, RES_K(50))
	POT(R223, RES_K(50))
	POT(R110, RES_K(50))
	POT(R210, RES_K(50))
	NET_C(R123.3, R223.3, R110.3, R210.3, GND)

	// Reverse the sense of pot adjustments so that larger values result
	// in greater volume, which we will want if, as I hope, these are
	// made user-adjustable.
	PARAM(R123.REVERSE, 1)
	PARAM(R223.REVERSE, 1)
	PARAM(R110.REVERSE, 1)
	PARAM(R210.REVERSE, 1)
	PARAM(R123.DIAL, 0.5)
	PARAM(R223.DIAL, 0.5)
	PARAM(R110.DIAL, 0.5)
	PARAM(R210.DIAL, 0.5)

	NET_C(C119.2, C118.1, R123.1)
	NET_C(C219.2, C218.1, R223.1)
	NET_C(C112.2, C111.1, R110.1)
	NET_C(C212.2, C211.1, R210.1)

	// Mixing of shot and hit sounds for each side.

	RES(R122, RES_K(30))
	RES(R222, RES_K(30))
	RES(R109, RES_K(30))
	RES(R209, RES_K(30))

	NET_C(R123.2, R122.2)
	NET_C(R223.2, R222.2)
	NET_C(R110.2, R109.2)
	NET_C(R210.2, R209.2)

	CAP(C117, CAP_U(0.047))
	CAP(C217, CAP_U(0.047))
	CAP(C110, CAP_U(0.1))
	CAP(C210, CAP_U(0.1))

	NET_C(R122.1, C117.2)
	NET_C(R222.1, C217.2)
	NET_C(R109.1, C110.2)
	NET_C(R209.1, C210.2)

	// Second-stage amplifier circuits, which amplify each side's
	// mixed shot and hit sounds. These are similar to the sound-effect
	// amplifiers (particularly the "hit" ones), each being a single
	// NPN transistor wired in common-emitter configuration, with a
	// bypass capacitor at the emitter. They have no need for an
	// attack-decay envelope, however, and so get their current
	// directly from the 16.5-volt power supply.

	RES(R107, RES_K(150))
	RES(R207, RES_K(150))

	RES(R105, RES_K(30))
	RES(R205, RES_K(30))
	NET_C(R105.2, R205.2, GND)

	RES(R108, RES_K(5.1))
	RES(R208, RES_K(5.1))

	RES(R106, RES_K(1))
	RES(R206, RES_K(1))
	NET_C(R106.2, R206.2, GND)

	CAP(C109, CAP_U(50))
	CAP(C209, CAP_U(50))
	NET_C(C109.2, C209.2, GND)

	NET_C(R107.1, R207.1, R108.1, R208.1, I_V16_5.Q)

	QBJT_EB(Q102, "NPN")
	QBJT_EB(Q202, "NPN")

	NET_C(C110.1, C117.1, R107.2, R105.1, Q102.B)
	NET_C(C210.1, C217.1, R207.2, R205.1, Q202.B)

	NET_C(R106.1, C109.1, Q102.E)
	NET_C(R206.1, C209.1, Q202.E)

	// Coupling capacitors and volume potentiometers for second-stage
	// amplifier outputs. There are two volume pots, for left and right
	// master volume. Presently these pots are fixed at the midpoint of
	// their range.

	CAP(C108, CAP_U(0.047))
	CAP(C208, CAP_U(0.047))

	NET_C(R108.2, Q102.C, C108.1)
	NET_C(R208.2, Q202.C, C208.1)

	RES(R104, RES_K(30))
	RES(R204, RES_K(30))

	NET_C(C108.2, R104.1)
	NET_C(C208.2, R204.1)

	CAP(C107, CAP_U(0.001))
	CAP(C207, CAP_U(0.001))
	NET_C(C107.2, C207.2, GND)

	// FIXME - master volume potentiometers should be adjustable.
	POT(R103, RES_K(47))
	POT(R203, RES_K(47))
	NET_C(R103.3, R203.3, GND)

	// Reverse the sense of pot adjustments so that larger values result
	// in greater volume, which we will want if, as I hope, these are
	// made user-adjustable.
	PARAM(R103.REVERSE, 1)
	PARAM(R203.REVERSE, 1)
	PARAM(R103.DIAL, 0.5)
	PARAM(R203.DIAL, 0.5)

	NET_C(R104.2, C107.1, R103.1)
	NET_C(R204.2, C207.1, R203.1)

	// The following are the left and right speaker outputs. In the real
	// circuit they don't drive the speakers directly, being far too weak,
	// but rather go to pin 7, the signal input, of the respective
	// TAA621A11 (or LM354) audio power amplifier ICs for the left and
	// right speakers.

	ALIAS(OUT_L, R103.2)
	ALIAS(OUT_R, R203.2)

NETLIST_END()


NETLIST_START(gunfight)

	SOLVER(Solver, 48000)
	PARAM(Solver.SORT_TYPE, "ASCENDING")
	// For this netlist, ASCENDING turns out to be the fastest of the
	// available sort types, roughly 40% faster than the default
	// PREFER_IDENTITY_TOP_LEFT.

	LOCAL_LIB_ENTRY(NOISE)

	LOCAL_SOURCE(gunfight_schematics)

	INCLUDE(gunfight_schematics)

	ANALOG_INPUT(I_V16_5, 16.5)  // 16.5-volt power for sound amplifiers
	ANALOG_INPUT(I_V5, 5)  // 5-volt power for logic input devices

	LOGIC_INPUT(I_LEFT_SHOT,  0, "74XX")
	LOGIC_INPUT(I_RIGHT_SHOT, 0, "74XX")
	LOGIC_INPUT(I_LEFT_HIT,   0, "74XX")
	LOGIC_INPUT(I_RIGHT_HIT,  0, "74XX")

	// Power and ground connections for logic input devices:
	NET_C(I_V5.Q, I_LEFT_SHOT.VCC, I_RIGHT_SHOT.VCC,
		I_LEFT_HIT.VCC, I_RIGHT_HIT.VCC)
	NET_C(GND, I_LEFT_SHOT.GND, I_RIGHT_SHOT.GND,
		I_LEFT_HIT.GND, I_RIGHT_HIT.GND)

	ALIAS(IN_LS, I_LEFT_SHOT.Q)
	ALIAS(IN_RS, I_RIGHT_SHOT.Q)
	ALIAS(IN_LH, I_LEFT_HIT.Q)
	ALIAS(IN_RH, I_RIGHT_HIT.Q)

#if USE_FRONTIERS
	// These frontiers keep the mostly independant halves of the circuit
	// (left channel and right channel) from affecting each other and the
	// noise generator, which speeds up processing by about 10% while
	// making no audible change in the output. Perhaps better ones could
	// be found, but I don't know how to do so or if it's even possible.
	OPTIMIZE_FRONTIER(C303.1, RES_M(1), 50)
	OPTIMIZE_FRONTIER(C306.1, RES_M(1), 50)
	OPTIMIZE_FRONTIER(C304.1, RES_M(1), 50)
	OPTIMIZE_FRONTIER(C305.1, RES_M(1), 50)
#endif

NETLIST_END()
