package opcode;

import literal.Address;
import literal.Label;

import static opcode.Format.*;

public enum Mnemonic {

    LD("ld", (byte)0x22, (byte)0x2a, new Format[]{ LD_R_R, LD_RR_NN,//done
            INDIRECT_NN_R, R_INDIRECT_NN, //done
            INDIRECT_RR_R, R_INDIRECT_RR,//done
            SP_HL,//done
            INDIRECT_NN_RR, RR_INDIRECT_NN, LD_IR }),//done
    PUSH("push", (byte)0xc5, new Format[]{ PP_RR }),//done
    POP("pop", (byte)0xc1, new Format[]{ PP_RR }),//done
    EX("ex", (byte)0x08, (byte)0xe3, new Format[]{ EX_RR, EX_RR_RR, EX_IXY_HL }),//done
    EXX("exx", (byte)0xd9, new Format[]{ NUL_1 }),//done
    LDI("ldi", new byte[] { (byte)0xed, (byte) 0xa0 }, new Format[]{ NUL_1 }),//done
    LDIR("ldir", new byte[] { (byte)0xed, (byte) 0xb0 }, new Format[]{ NUL_1 }),//done
    CPI("cpi", new byte[] { (byte)0xed, (byte) 0xa1 }, new Format[]{ NUL_1 }),//done
    CPIR("cpir", new byte[] { (byte)0xed, (byte) 0xb1 }, new Format[]{ NUL_1 }),//done
    ADD("add", (byte)0x09, new Format[]{ ALU_R, ALU_N, ADD_RR_RR }),
    SUB("sub", new Format[]{ ALU_R, ALU_N }),
    ADC("adc", new byte[] { (byte)0xed, (byte) 0x4a }, new Format[]{ ALU_R, ALU_N, RR_RR, R_N }),
    SBC("sbc",  new byte[] { (byte)0xed, (byte) 0x42 }, new Format[]{ ALU_R, ALU_N, RR_RR, R_N }),
    AND("and", new Format[]{ ALU_R, ALU_N, R_N }),
    XOR("xor", new Format[]{ ALU_R, ALU_N, R_N }),
    OR("or", new Format[]{ ALU_R, ALU_N, R_N }),
    CP("cp", new Format[]{ ALU_R, ALU_N, R_N }),
    INC("inc", (byte)0x04, (byte)0x03, new Format[]{ ID_R, ID_RR }),//done
    DEC("dec", (byte)0x05, (byte)0x0b, new Format[]{ ID_R, ID_RR }),//done
    DAA("daa", (byte)0x27, new Format[]{ NUL_1 }),//done
    CPL("cpl", (byte)0x2f, new Format[]{ NUL_1 }),//done
    NEG("neg", new byte[] { (byte)0xed, (byte) 0x44 }, new Format[]{ NUL_1 }),//done
    CCF("ccf", (byte)0x3f, new Format[]{ NUL_1 }),//done
    SCF("scf", (byte)0x37, new Format[]{ NUL_1 }),//done
    NOP("nop", (byte)0x00, new Format[]{ NUL_1 }),//done
    HALT("halt", (byte)0x76, new Format[]{ NUL_1 }),//done
    DI("di", (byte)0xf3, new Format[]{ NUL_1 }),//done
    EI("ei", (byte)0xfb, new Format[]{ NUL_1 }),//done
    IM("im", new byte[] { (byte)0xed, (byte) 0x46 }, new Format[]{ Format.IM }),//done
    RLCA("rlca", (byte)0x07, new Format[]{ NUL_1 }),//done
    RLA("rla", (byte)0x17, new Format[]{ NUL_1 }),//done
    RRCA("rrca", (byte)0x0f, new Format[]{ NUL_1 }),//done
    RRA("rra", (byte)0x1f, new Format[]{ NUL_1 }),//done
    RLC("rlc", new byte[] { (byte)0xcb, (byte) 0x00 }, new Format[]{ R }),//done
    RL("rl", new byte[] { (byte)0xcb, (byte) 0x10 }, new Format[]{ R }),//done
    RRC("rrc", new byte[] { (byte)0xcb, (byte) 0x08 }, new Format[]{ R }),//done
    RR("rr", new byte[] { (byte)0xcb, (byte) 0x18 }, new Format[]{ R }),//done
    SLA("sla", new byte[] { (byte)0xcb, (byte) 0x20 }, new Format[]{ R }),//done
    SLL("sll", new byte[] { (byte)0xcb, (byte) 0x30 }, new Format[]{ R }),//done
    SRA("sra", new byte[] { (byte)0xcb, (byte) 0x28 }, new Format[]{ R }),//done
    SRL("srl", new byte[] { (byte)0xcb, (byte) 0x38 }, new Format[]{ R }),//done
    RLD("rld", new byte[] { (byte)0xed, (byte) 0x6f }, new Format[]{ NUL_1 }),//done
    RRD("rrd", new byte[] { (byte)0xed, (byte) 0x67 }, new Format[]{ NUL_1 }),//done
    BIT("bit", new byte[] { (byte)0xcb, (byte) 0x40 }, new Format[]{ N_R }),//done
    SET("set", new byte[] { (byte)0xcb, (byte) 0xc0 }, new Format[]{ N_R }),//done
    RES("res", new byte[] { (byte)0xcb, (byte) 0x80 }, new Format[]{ N_R }),//done
    JR("jr", (byte)0x18, (byte)0x20, new Format[]{ D, F_D }),//done
    JP("jp", (byte)0xc3, (byte)0xc2, new Format[]{ JP_NN, JP_F_NN, JP_HL }),//done
    DJNZ("djnz", (byte)0x10, new Format[]{ D }),//done
    CALL("call", (byte)0xcd, (byte)0xcc, new Format[]{ JP_NN, JP_F_NN }),
    RET("ret", (byte)0xc9, new Format[]{ NUL_1, RET_F }),//done
    RETI("reti", new byte[] { (byte)0xed, (byte) 0x4d }, new Format[]{ NUL_1 }),//done
    RETN("retn", new byte[] { (byte)0xed, (byte) 0x45 }, new Format[]{ NUL_1 }),//done
    RST("rst", (byte)0xc7, new Format[]{ Format.RST }),
    IN("in", (byte)0xdb, new Format[]{ IN_AN, R_INDIRECT_R, INDIRECT_R }),//done
    INI("ini", new byte[] { (byte)0xed, (byte) 0xa2 }, new Format[]{ NUL_1 }),//done
    IND("ind", new byte[] { (byte)0xed, (byte) 0xaa }, new Format[]{ NUL_1 }),//done
    INIR("inir", new byte[] { (byte)0xed, (byte) 0xb2 }, new Format[]{ NUL_1 }),//done
    INDR("indr", new byte[] { (byte)0xed, (byte) 0xba }, new Format[]{ NUL_1 }),//done
    OUT("out", (byte)0xd3, new Format[]{ OUT_NA, INDIRECT_R_R, INDIRECT_R }),//done
    OUTI("outi", new byte[] { (byte)0xed, (byte) 0xa3 }, new Format[]{ NUL_1 }),//done
    OUTD("outd", new byte[] { (byte)0xed, (byte) 0xab }, new Format[]{ NUL_1 }),//done
    OTIR("otir", new byte[] { (byte)0xed, (byte) 0xb3 }, new Format[]{ NUL_1 }),//done
    OTDR("otdr", new byte[] { (byte)0xed, (byte) 0xbb }, new Format[]{ NUL_1 }),//done
    JPJ("jpj", new Format[]{ Format.RR }),
    JPC("jpc", new Format[]{ Format.RR }),
    XIT("xit", new byte[] { (byte)0xed, (byte) 0x9e }, new Format[]{ NUL_1 }),//done thread tail exit
    RTC("rtc", new byte[] { (byte)0xed, (byte) 0x9f }, new Format[]{ NUL_1 }),//done thread continue
    CPC("cpc", new byte[] { (byte)0xed, (byte) 0xa7 }, new Format[]{ NUL_1 }),//done
    LDC("ldc", new byte[] { (byte)0xed, (byte) 0xaf }, new Format[]{ NUL_1 }),//done
    CPCR("cpcr", new byte[] { (byte)0xed, (byte) 0xb7 }, new Format[]{ NUL_1 }),//done
    LDCR("ldcr", new byte[] { (byte)0xed, (byte) 0xbf }, new Format[]{ NUL_1 }),//done
    MUL("mul",  new byte[] { (byte)0xed, (byte) 0x7f }, new Format[]{ MUL_DE }),//done
    NOPN("nopn", new byte[] { (byte)0xed, (byte) 0x00 }, new Format[]{ NUL_1 }),//done
    BRK("brk", new byte[] { (byte)0xed, (byte) 0x02 }, new Format[]{ NUL_1 }),//done

    DB("db", new Format[]{ MACRO }),
    DW("dw", new Format[]{ MACRO }),
    DS("ds", new Format[]{ MACRO }),
    UPTO("upto", new Format[]{ MACRO }),
    ORG("org", new Format[]{ MACRO }),
    EQU("equ", new Format[]{ MACRO }),
    MACRO_DEF("macro", new Format[]{ MACRO }),//whole file as macro with #n args??
    MACRO_END("end", new Format[]{ MACRO }),
    FILL("fill", new Format[]{ MACRO }),

    DF("df", new Format[]{ MACRO }),//forth header like macro
    LABEL(null, new Format[]{ MACRO });// for threaded automatics

    String name;
    Format[] allows;
    byte[] baseOpcode;
    byte baseDouble;

    Mnemonic(String name, Format[] allows) {
        this.name = name;
    }
    Mnemonic(String name, byte baseOpcode, Format[] allows) {
        this(name, allows);
        this.baseOpcode = new byte[1];
        this.baseOpcode[0] = baseOpcode;
    }

    Mnemonic(String name, byte[] baseOpcode, Format[] allows) {
        this(name, allows);
        this.baseOpcode = baseOpcode;
    }

    Mnemonic(String name, byte baseOpcode, byte baseDouble, Format[] allows) {
        this(name, baseOpcode, allows);;
        this.baseDouble = baseDouble;
    }

    public static Mnemonic getMnemonic(String op, int org, boolean allowDupe) {
        for (Mnemonic m: Mnemonic.values()) {
            if(op.trim().toLowerCase().equals(m.name)) {
                return m;
            }
        }
        Address l = Label.findLabel(op.trim().toLowerCase(), org, allowDupe);
        // found as label
        if(l != null) return LABEL;
        return null;
    }

    public byte[] cloneOpcode() {
        byte[] b = new byte[baseOpcode.length];
        for(int i = 0; i < baseOpcode.length; i++) {
            b[i] = baseOpcode[i];
        }
        return b;
    }
}
