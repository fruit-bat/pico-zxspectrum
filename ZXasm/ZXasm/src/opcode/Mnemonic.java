package opcode;

import literal.Address;
import literal.Label;

import static opcode.Format.*;

public enum Mnemonic {

    LD("ld", new Format[]{ R_R, INDIRECT_NN_RR, RR_INDIRECT_NN,
        R_INDIRECT_RR, INDIRECT_RR_R, R_INDIRECT_NN, INDIRECT_NN_R }),
    PUSH("push", new Format[]{ Format.RR }),
    POP("", new Format[]{ Format.RR }),
    EX("ex", new Format[]{ RR_RR }),
    LDI("ldi", new Format[]{ NUL }),
    LDIR("ldir", new Format[]{ NUL }),
    CPI("cpi", new Format[]{ NUL }),
    CPIR("cpir", new Format[]{ NUL }),
    ADD("add", new Format[]{ R_R, RR_RR }),
    SUB("sub", new Format[]{ R_R, RR_RR }),
    AND("and", new Format[]{ R_R, R_N }),
    XOR("xor", new Format[]{ R_R, R_N }),
    OR("or", new Format[]{ R_R, R_N }),
    CP("cp", new Format[]{ R_R, R_N }),
    INC("inc", new Format[]{ R, Format.RR }),
    DEC("dec", new Format[]{ R, Format.RR }),
    ADC("adc", new Format[]{ R, RR_RR, R_N }),
    SBC("sbc", new Format[]{ R, RR_RR, R_N }),
    DAA("daa", new Format[]{ NUL }),
    CPL("cpl", new Format[]{ NUL }),
    NEG("neg", new Format[]{ NUL }),
    CCF("ccf", new Format[]{ NUL }),
    SCF("scf", new Format[]{ NUL }),
    NOP("nop", new Format[]{ NUL }),
    HALT("halt", new Format[]{ NUL }),
    DI("di", new Format[]{ NUL }),
    EI("ei", new Format[]{ NUL }),
    IM("im", new Format[]{ Format.IM }),
    RLCA("rlca", new Format[]{ NUL }),
    RLA("rla", new Format[]{ NUL }),
    RRCA("rrca", new Format[]{ NUL }),
    RRA("rra", new Format[]{ NUL }),
    RLC("rlc", new Format[]{ NUL }),
    RL("rl", new Format[]{ R }),
    RRC("rrc", new Format[]{ R }),
    RR("rr", new Format[]{ R }),
    SLA("sla", new Format[]{ R }),
    SLL("sll", new Format[]{ R }),
    SRA("sra", new Format[]{ R }),
    SRL("srl", new Format[]{ R }),
    RLD("rld", new Format[]{ R }),
    RRD("rrd", new Format[]{ R }),
    BIT("bit", new Format[]{ N_R }),
    SET("set", new Format[]{ N_R }),
    RES("res", new Format[]{ N_R }),
    JR("jr", new Format[]{ D, F_D }),
    JP("jp", new Format[]{ NN, F_NN }),
    DJNZ("djnz", new Format[]{ D }),
    CALL("call", new Format[]{ NN, F_NN }),
    RET("ret", new Format[]{ NUL, F }),
    RETI("reti", new Format[]{ NUL }),
    RETN("retn", new Format[]{ NUL }),
    RST("rst", new Format[]{ Format.RST }),
    IN("in", new Format[]{ R_INDIRECT_R, INDIRECT_R }),
    INI("ini", new Format[]{ NUL }),
    IND("ind", new Format[]{ NUL }),
    INIR("inir", new Format[]{ NUL }),
    INDR("indr", new Format[]{ NUL }),
    OUT("out", new Format[]{ INDIRECT_R_R, INDIRECT_R }),
    OUTI("outi", new Format[]{ NUL }),
    OUTD("outd", new Format[]{ NUL }),
    OTIR("otir", new Format[]{ NUL }),
    OTDR("otdr", new Format[]{ NUL }),
    JPJ("jpj", new Format[]{ Format.RR }),
    JPC("jpc", new Format[]{ Format.RR }),
    XIT("xit", new Format[]{ NUL }),
    RTC("rtc", new Format[]{ NUL }),
    CPC("cpc", new Format[]{ NUL }),
    LDC("ldc", new Format[]{ NUL }),
    CPCR("cpcr", new Format[]{ NUL }),
    LDCR("ldcr", new Format[]{ NUL }),
    MUL("mul", new Format[]{ Format.RR }),
    NOPN("nopn", new Format[]{ NUL }),
    BRK("brk", new Format[]{ NUL }),

    DB("db", new Format[]{ MACRO }),
    DW("dw", new Format[]{ MACRO }),
    DS("ds", new Format[]{ MACRO }),
    UPTO("upto", new Format[]{ MACRO }),
    ORG("org", new Format[]{ MACRO }),
    EQU("equ", new Format[]{ MACRO }),
    MACRO_DEF("macro", new Format[]{ MACRO }),//whole file as macro with #n args??
    MACRO_END("end", new Format[]{ MACRO }),
    FILL("fill", new Format[]{ MACRO }),
    LABEL(null, new Format[]{ MACRO });// for threaded automatics

    String name;
    Format[] allows;

    Mnemonic(String name, Format[] allows) {
        this.name = name;
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

    public boolean hasFormat(Format f) {
        for(int i = 0; i < allows.length; i++) {
            if(allows[i] == f) return true;
        }
        return false;
    }

    public boolean hasFlagFormat() {
        return hasFormat(F) || hasFormat(F_D) || hasFormat(F_NN);
    }
}
