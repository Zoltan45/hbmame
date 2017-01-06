// license:GPL-2.0+
// copyright-holders:Couriersud
/* € */ // ABC

#include "netlist/devices/net_lib.h"

/* ----------------------------------------------------------------------------
 *  Define
 * ---------------------------------------------------------------------------*/

/* set to 1 to use optimizations increasing performance significantly */

#define USE_OPTMIZATIONS	1

/* ----------------------------------------------------------------------------
 *  Library section header START
 * ---------------------------------------------------------------------------*/

#ifndef __PLIB_PREPROCESSOR__

#define LM358_DIP(_name)            			                               \
		NET_REGISTER_DEV_X(LM358_DIP, _name)

#define G501534_DIP(_name)                                                     \
		NET_REGISTER_DEV_X(G501534_DIP, _name)


NETLIST_EXTERNAL(congob_lib)

#endif

/* ----------------------------------------------------------------------------
 *  Library section header END
 * ---------------------------------------------------------------------------*/


NETLIST_START(dummy)
//  EESCHEMA NETLIST VERSION 1.1 (SPICE FORMAT) CREATION DATE: WED 01 JUL 2015 11:09:25 PM CEST
//  TO EXCLUDE A COMPONENT FROM THE SPICE NETLIST ADD [SPICE_NETLIST_ENABLED] USER FIELD SET TO: N
//  TO REORDER THE COMPONENT SPICE NODE SEQUENCE ADD [SPICE_NODE_SEQUENCE] USER FIELD AND DEFINE SEQUENCE: 2,1,0
// SHEET NAME:/
// IGNORED O_AUDIO0: O_AUDIO0  64 0
// .END

	PARAM(Solver.ACCURACY, 1e-8)
	PARAM(Solver.NR_LOOPS, 9000)
	PARAM(Solver.SOR_FACTOR, 0.001)
	PARAM(Solver.GS_LOOPS, 1)
	//PARAM(Solver.GS_THRESHOLD, 99)
	PARAM(Solver.ITERATIVE, "MAT_CR")

#if USE_OPTMIZATIONS
	SOLVER(Solver, 24000)
	PARAM(Solver.DYNAMIC_TS, 0)
#else
	SOLVER(Solver, 24000)
	PARAM(Solver.DYNAMIC_TS, 1)
	PARAM(Solver.DYNAMIC_LTE, 1e-4)
	PARAM(Solver.MIN_TIMESTEP, 5e-7)
	PARAM(Solver.PARALLEL, 0)
	PARAM(Solver.PIVOT, 0)
#endif

	LOCAL_SOURCE(congob_lib)
	INCLUDE(congob_lib)

	TTL_INPUT(I_BASS_DRUM0, 0)
	TTL_INPUT(I_CONGA_H0, 0)
	TTL_INPUT(I_CONGA_L0, 0)
	TTL_INPUT(I_GORILLA0, 0)
	TTL_INPUT(I_RIM0, 0)

	ALIAS(I_V0.Q, GND.Q)

	ANALOG_INPUT(I_V12, 12)
	ANALOG_INPUT(I_V5, 5)
	ANALOG_INPUT(I_V6, 6)

	/* temporary output stage */
	RES(RO, RES_K(50))
	CAP(CO, CAP_U(10))

	NET_C(R94.1, CO.1)
	NET_C(CO.2, RO.1)
	NET_C(RO.2, GND)

	// FIXME: Same as 1N4148
	NET_MODEL("1S2075 D(Is=2.52n Rs=.568 N=1.752 Cjo=4p M=.4 tt=20n Iave=200m Vpk=75)")
	NET_MODEL("2SC1941 NPN(IS=46.416f BF=210 NF=1.0022 VAF=600 IKF=500m ISE=60f NE=1.5 BR=2.0122 NR=1.0022 VAR=10G IKR=10G ISC=300p NC=2 RB=13.22 IRB=10G RBM=13.22 RE=100m RC=790m CJE=26.52p VJE=900m MJE=518m TF=1.25n XTF=10 VTF=10 ITF=500m PTF=0 CJC=4.89p VJC=750m MJC=237m XCJC=500m TR=100n CJS=0 VJS=750m MJS=500m XTB=1.5 EG=1.11 XTI=3 KF=0 AF=1 FC=500m)")

	INCLUDE(CongoBongo_schematics)

#if USE_OPTMIZATIONS
	/* provide resistance feedback loop. This helps convergence for
	 * Newton-Raphson a lot. This puts a resistor of 1e100 Ohms in parallel
	 * to the feedback loop consisting of D9 and Q2.
	 */

	RES(RX1, 1e100)
	NET_C(RX1.1, Q2.C)
	NET_C(RX1.2, XU16.7)

	/* The opamp actually has an FPF of about 500k. This doesn't work here and causes oscillations.
	 * FPF here therefore about half the Solver clock.
	 */
	PARAM(XU16.B.MODEL, "MB3614(UGF=11k)")
	PARAM(XU17.C.MODEL, "MB3614(UGF=11k)")

	PARAM(XU17.A.MODEL, "MB3614(TYPE=1)")
	PARAM(XU17.B.MODEL, "MB3614(TYPE=1)")
	PARAM(XU17.D.MODEL, "MB3614(TYPE=1)")

	//PARAM(XU16.A.MODEL, "MB3614(TYPE=1)")
	PARAM(XU16.C.MODEL, "MB3614(TYPE=1)")
	PARAM(XU16.D.MODEL, "MB3614(TYPE=1)")

#if 0
	PARAM(XU13.A.MODEL, "MB3614(TYPE=1)")
	PARAM(XU13.B.MODEL, "MB3614(TYPE=1)")
	PARAM(XU13.C.MODEL, "MB3614(TYPE=1)")
	PARAM(XU13.D.MODEL, "MB3614(TYPE=1)")
#endif

#if 1
	OPTIMIZE_FRONTIER(C51.1, RES_K(20), 50)
	OPTIMIZE_FRONTIER(R77.2, RES_K(20), 50)

	OPTIMIZE_FRONTIER(C25.2, RES_K(240), 50)
	OPTIMIZE_FRONTIER(C29.2, RES_K(390), 50)
	OPTIMIZE_FRONTIER(C37.2, RES_K(390), 50)
	OPTIMIZE_FRONTIER(C44.2, RES_K(200), 50)

	OPTIMIZE_FRONTIER(R90.2, RES_K(100), 50)
	OPTIMIZE_FRONTIER(R92.2, RES_K(15), 50)
#endif
#endif
NETLIST_END()

NETLIST_START(CongoBongo_schematics)
	CAP(C20, CAP_N(68))
	CAP(C21, CAP_U(1))
	CAP(C22, CAP_U(47))
	CAP(C23, CAP_N(100))
	CAP(C24, CAP_N(100))
	CAP(C25, CAP_U(1))
	CAP(C26, CAP_N(68))
	CAP(C27, CAP_N(33))
	CAP(C28, CAP_U(47))
	CAP(C29, CAP_U(1))
	CAP(C30, CAP_N(33))
	CAP(C31, CAP_N(33))
	CAP(C32, CAP_N(68))
	CAP(C33, CAP_N(33))
	CAP(C34, CAP_U(47))
	CAP(C35, CAP_N(33))
	CAP(C36, CAP_N(33))
	CAP(C37, CAP_U(1))
	CAP(C38, CAP_N(10))
	CAP(C39, CAP_N(3.3))
	CAP(C40, CAP_U(2.2))
	CAP(C41, CAP_N(6.8))
	CAP(C42, CAP_N(6.8))
	CAP(C43, CAP_N(47))
	CAP(C44, CAP_U(1))
	CAP(C45, CAP_U(33))
	CAP(C46, CAP_N(100))
	CAP(C47, CAP_P(470))
	CAP(C48, CAP_N(1.5))
	CAP(C49, CAP_P(220))
	CAP(C50, CAP_N(3.9))
	CAP(C51, CAP_U(1))
	CAP(C52, CAP_U(1))
	CAP(C53, CAP_U(1))
	CAP(C54, CAP_U(1))
	CAP(C55, CAP_U(1))
	CAP(C56, CAP_U(10))
	CAP(C57, CAP_N(47))
	CAP(C58, CAP_N(22))
	CAP(C59, CAP_U(10))
	CAP(C60, CAP_N(22))
	CAP(C62, CAP_N(22))
	CAP(C61, CAP_U(1))
	DIODE(D1, "1S2075")
	DIODE(D2, "1S2075")
	DIODE(D3, "1S2075")
	DIODE(D4, "1S2075")
	DIODE(D5, "1S2075")
	DIODE(D6, "1S2075")
	DIODE(D7, "1S2075")
	DIODE(D8, "1S2075")
	QBJT_EB(Q2, "2SC1941")
	RES(R21, RES_K(10))
	RES(R22, RES_K(47))
	RES(R23, RES_K(47))
	RES(R24, RES_K(10))
	RES(R25, RES_K(47))
	RES(R26, RES_K(22))
	RES(R27, RES_K(10))
	RES(R28, RES_K(470))
	RES(R29, RES_K(1))
	RES(R30, RES_K(240))
	RES(R31, RES_K(10))
	RES(R32, RES_K(47))
	RES(R33, RES_K(47))
	RES(R34, RES_K(47))
	RES(R35, RES_K(47))
	RES(R36, RES_K(22))
	RES(R37, RES_K(10))
	RES(R38, RES_M(1))
	RES(R39, 330)
	RES(R40, RES_K(390))
	RES(R41, RES_K(10))
	RES(R42, RES_K(47))
	RES(R43, RES_K(47))
	RES(R44, RES_K(47))
	RES(R45, RES_K(47))
	RES(R46, RES_K(22))
	RES(R47, RES_K(10))
	RES(R48, RES_M(1))
	RES(R49, 220)
	RES(R50, RES_K(390))
	RES(R51, RES_K(10))
	RES(R52, RES_K(22))
	RES(R53, RES_K(22))
	RES(R54, RES_K(22))
	RES(R55, RES_K(22))
	RES(R56, RES_K(10))
	RES(R57, RES_K(4.7))
	RES(R58, RES_M(1))
	RES(R59, 470)
	RES(R60, RES_M(2.2))
	RES(R61, RES_M(2.2))
	RES(R62, RES_K(200))
	RES(R63, RES_K(22))
	RES(R64, RES_K(22))
	RES(R65, RES_K(20))
	RES(R66, RES_K(20))
	RES(R67, RES_K(20))
	RES(R68, RES_K(20))
	RES(R69, RES_K(20))
	RES(R70, RES_K(100))
	RES(R71, RES_K(150))
	RES(R72, RES_K(330))
	RES(R73, RES_K(1))
	RES(R74, RES_K(1))
	RES(R75, RES_K(470))
	RES(R76, RES_K(10))
	RES(R77, RES_K(20))
	RES(R78, RES_K(47))
	RES(R79, RES_K(22))
	RES(R80, RES_K(20))
	RES(R81, RES_K(10))
	RES(R82, RES_K(100))
	RES(R83, RES_K(51))
	RES(R84, RES_K(51))
	RES(R85, RES_K(51))
	RES(R86, RES_K(51))
	RES(R87, RES_K(100))
	RES(R88, RES_K(2.2))
	RES(R89, RES_K(10))
	RES(R90, RES_K(100))
	RES(R91, RES_K(10))
	RES(R92, RES_K(15))
	RES(R93, RES_K(15))
	RES(R94, RES_K(51))
	MB3614_DIP(XU13)
	G501534_DIP(XU15)
	MB3614_DIP(XU16)
	MB3614_DIP(XU17)
	CD4001_DIP(XU18)
	CD4538_DIP(XU19)
	MM5837_DIP(XU20)
	TTL_7416_DIP(XU6)

	NET_C(D1.A, C21.2, R23.1)
	NET_C(D1.K, C20.1, R22.1)
	NET_C(XU13.1, C37.2, C36.1, R48.1)
	NET_C(XU13.2, C35.2, R48.2)
	NET_C(XU13.3, R44.1, R46.2, R45.1)
	NET_C(XU13.4, R27.1, R21.1, R37.1, R31.1, R47.1, R41.1, R57.1, R51.1, C46.2, C45.2, XU17.4, R80.2, XU16.4, XU20.4, XU15.12, I_V12.Q)
	NET_C(XU13.5, R54.1, R56.2, R55.1)
	NET_C(XU13.6, C41.2, R58.2, R60.2)
	NET_C(XU13.7, C44.2, C42.1, R58.1, R61.1)
	NET_C(XU13.8, C29.2, C31.1, R38.1)
	NET_C(XU13.9, C30.2, R38.2)
	NET_C(XU13.10, R34.1, R36.2, R35.1)
	NET_C(XU13.11, C22.2, R29.2, R25.2, R23.2, R22.2, XU6.1, XU6.3, XU6.7, C28.2, R39.2, R35.2, R33.2, R32.2, C34.2, R49.2, R45.2, R43.2, R42.2, C40.2, R59.2, R55.2, R53.2, R52.2, C43.2, R69.1, R64.1, C49.2, C48.2, C47.2, C46.1, C45.1, XU17.11, XU19.1, XU19.4, XU19.8, XU19.12, XU19.15, R81.1, C56.2, C55.2, C53.2, C52.2, XU18.1, XU18.2, XU18.7, XU18.12, XU18.13, C54.2, XU16.11, R84.1, R88.1, Q2.E, C58.2, C60.2, XU20.1, XU20.2, XU15.4, I_V0.Q)
	NET_C(XU13.12, R24.1, R26.2, R25.1)
	NET_C(XU13.13, C23.2, R28.2)
	NET_C(XU13.14, C25.2, C24.1, R28.1)
	NET_C(C25.1, R30.2)
	NET_C(C24.2, C23.1, R29.1)
	NET_C(C21.1, R24.2)
	NET_C(C20.2, R21.2, XU6.8)
	NET_C(C22.1, R27.2, R26.1)
	NET_C(R30.1, R40.1, R50.1, R62.1, R94.1)
	//NET_C(XU6.2, XU6.4, XU19.7, XU18.3, XU18.11, XU15.5, XU15.6, XU15.7, XU15.8, XU15.9, XU15.10, XU15.11, XU15.14)
	NET_C(XU6.5, I_CONGA_L0.Q)
	NET_C(XU6.6, C26.2, R31.2)
	NET_C(XU6.9, I_BASS_DRUM0.Q)
	NET_C(XU6.10, C38.2, R51.2)
	NET_C(XU6.11, I_RIM0.Q)
	NET_C(XU6.12, C32.2, R41.2)
	NET_C(XU6.13, I_CONGA_H0.Q)
	NET_C(XU6.14, D5.K, XU19.16, R70.2, R76.2, R71.2, XU18.14, I_V5.Q)
	NET_C(D2.A, C27.2, R33.1)
	NET_C(D2.K, C26.1, R32.1)
	NET_C(C29.1, R40.2)
	NET_C(C31.2, C30.1, R39.1)
	NET_C(C27.1, R34.2)
	NET_C(C28.1, R37.2, R36.1)
	NET_C(D3.A, C33.2, R43.1)
	NET_C(D3.K, C32.1, R42.1)
	NET_C(C37.1, R50.2)
	NET_C(C36.2, C35.1, R49.1)
	NET_C(C33.1, R44.2)
	NET_C(C34.1, R47.2, R46.1)
	NET_C(D4.A, C39.2, R53.1)
	NET_C(D4.K, C38.1, R52.1)
	NET_C(C44.1, R62.2)
	NET_C(C42.2, C41.1, R59.1)
	NET_C(C39.1, R54.2)
	NET_C(C40.1, R57.2, R56.1)
	NET_C(R60.1, R61.2, C43.1)
	NET_C(R63.1, R64.2, C47.1, D5.A, XU18.5, XU18.6)
	NET_C(R63.2, XU20.3)
	NET_C(R65.1, R66.2, C48.1)
	NET_C(R65.2, XU18.4)
	NET_C(R66.1, R67.2, C50.2)
	NET_C(R67.1, C49.1, XU17.10)
	NET_C(R68.1, R69.2, XU17.9)
	NET_C(R68.2, C50.1, XU17.8, C51.1)
	NET_C(XU17.1, XU16.6, C62.1)
	NET_C(XU17.2, R82.1, C62.2, R85.2)
	NET_C(XU17.3, R83.1, R84.2)
	NET_C(XU17.5, C55.1, R72.1, R73.1)
	NET_C(XU17.6, XU17.7, R77.2)
	NET_C(XU17.12, R80.1, R81.2, C56.1)
	NET_C(XU17.13, R78.1, R79.2, R77.1)
	NET_C(XU17.14, R79.1, R82.2, R83.2)
	NET_C(C51.2, R78.2)
	NET_C(XU19.2, R70.1, C52.1)
	NET_C(XU19.3, XU19.13, R76.1)
	NET_C(XU19.5, XU19.11, I_GORILLA0.Q)
	NET_C(XU19.6, XU18.9)
	NET_C(XU19.9, XU18.8)
	NET_C(XU19.10, D7.A, R75.2)
	NET_C(XU19.14, R71.1, C53.1)
	NET_C(R72.2, D6.A, XU18.10)
	NET_C(R73.2, D6.K)
	NET_C(D7.K, R74.2)
	NET_C(R74.1, R75.1, C54.1, XU16.10)
	NET_C(XU16.1, R91.1, R92.2)
	NET_C(XU16.2, R90.1, R91.2)
	NET_C(XU16.3, R86.2, I_V6.Q)
	NET_C(XU16.5, R86.1, R87.2)
	NET_C(XU16.7, R87.1, D8.A, R90.2)
	NET_C(XU16.8, XU16.9, XU15.13)
	NET_C(XU16.12, R93.1, C58.1)
	NET_C(XU16.13, XU16.14, C57.1, C59.2)
	NET_C(R85.1, Q2.C)
	NET_C(R89.1, D8.K)
	NET_C(R89.2, R88.2, Q2.B)
	NET_C(R92.1, C57.2, R93.2)
	NET_C(C59.1, XU15.1)
	NET_C(C60.1, XU15.2)
	NET_C(XU15.3, C61.2)
	NET_C(C61.1, R94.2)
NETLIST_END()

NETLIST_START(opamp_mod)

	/* Opamp model from
	 *
	 * http://www.ecircuitcenter.com/Circuits/opmodel1/opmodel1.htm
	 *
	 * MB3614 Unit Gain frequency is about 500 kHz and the first pole frequency
	 * about 5 Hz. We have to keep the Unity Gain Frequency below our sampling
	 * frequency of 24 Khz.
	 *
	 * Simple Opamp Model Calculation
	 *
	 * First Pole Frequency		      5	Hz
	 * Unity Gain Frequency		 11,000	Hz
	 * RP						100,000	Ohm
	 * DC Gain / Aol			   2200
	 * CP		                  0.318	uF
	 * KG		                 0.022
	 *
	 */

	/* Terminal definitions for calling netlists */

	ALIAS(PLUS, G1.IP) // Positive input
	ALIAS(MINUS, G1.IN) // Negative input
	ALIAS(OUT, EBUF.OP) // Opamp output ...

	AFUNC(fUH, 1, "A0 1.2 -")
	AFUNC(fUL, 1, "A0 1.2 +")

	ALIAS(VCC, fUH.A0) // VCC terminal
	ALIAS(GND, fUL.A0) // VGND terminal

	AFUNC(fVREF, 2, "A0 A1 + 0.5 *")
	NET_C(fUH.A0, fVREF.A0)
	NET_C(fUL.A0, fVREF.A1)

	NET_C(EBUF.ON, fVREF)
	/* The opamp model */

	LVCCS(G1)
	PARAM(G1.RI, RES_K(1000))
#if 0
	PARAM(G1.G, 0.0022)
	RES(RP1, 1e6)
	CAP(CP1, 0.0318e-6)
#else
	PARAM(G1.G, 0.002)
	PARAM(G1.CURLIM, 0.002)
	RES(RP1, 9.5e6)
	CAP(CP1, 0.0033e-6)
#endif
	VCVS(EBUF)
	PARAM(EBUF.RO, 50)
	PARAM(EBUF.G, 1)

	NET_C(G1.ON, fVREF)
	NET_C(RP1.2, fVREF)
	NET_C(CP1.2, fVREF)
	NET_C(EBUF.IN, fVREF)

	NET_C(RP1.1, G1.OP)
	NET_C(CP1.1, RP1.1)

	DIODE(DP,"D(IS=1e-15 N=1)")
	DIODE(DN,"D(IS=1e-15 N=1)")
#if 1
	NET_C(DP.K, fUH.Q)
	NET_C(fUL.Q, DN.A)
	NET_C(DP.A, DN.K, RP1.1)
#else
	/*
	 * This doesn't add any performance by decreasing iteration loops.
	 * To the contrary, it significantly decreases iterations
	 */
	RES(RH1, 0.1)
	RES(RL1, 0.1)
	NET_C(DP.K, RH1.1)
	NET_C(RH1.2, fUH.Q)
	NET_C(fUL.Q, RL1.1)
	NET_C(RL1.2, DN.A)
	NET_C(DP.A, DN.K, RP1.1)

#endif
	NET_C(EBUF.IP, RP1.1)

NETLIST_END()


NETLIST_START(G501534_DIP)
	AFUNC(f, 2, "A0 A1 0.2 * *")

	/*
	 * 12:   VCC
	 *  4:   GND
	 *  1:   IN
	 *  3:	 OUT
	 * 13:	 CV
	 *  2:   RDL - connected via Capacitor to ground
	 */

	DUMMY_INPUT(DU1)
	DUMMY_INPUT(DU2)
	DUMMY_INPUT(DU3)

	RES(RO, 1000)

	ALIAS(12, DU1.I)
	ALIAS(4,  DU2.I)
	ALIAS(2,  DU3.I)
	ALIAS(1,  f.A0)
	ALIAS(13, f.A1)
	NET_C(f.Q, RO.1)
	ALIAS(3,  RO.2)

NETLIST_END()

NETLIST_START(congob_lib)

	LOCAL_LIB_ENTRY(G501534_DIP)

NETLIST_END()
