package opcode;

public enum Mnemonic {

    LD("ld"),
    PUSH("push"),
    POP(""),
    EX("ex"),
    LDI("ldi"),
    LDIR("ldir"),
    CPI("cpi"),
    CPIR("cpir"),
    ADD("add"),
    SUB("sub"),
    AND("and"),
    XOR("xor"),
    OR("or"),
    CP("cp"),
    INC("inc"),
    DEC("dec"),
    ADC("adc"),
    SBC("sbc"),
    DAA("daa"),
    CPL("cpl"),
    NEG("neg"),
    CCF("ccf"),
    SCF("scf"),
    NOP("nop"),
    HALT("halt"),
    DI("di"),
    EI("ei"),
    IM("im"),
    RLCA("rlca"),
    RLA("rla"),
    RRCA("rrca"),
    RRA("rra"),
    RLC("rlc"),
    RL("rl"),
    RRC("rrc"),
    RR("rr"),
    SLA("sla"),
    SLL("sll"),
    SRA("sra"),
    SRL("srl"),
    RLD("rld"),
    RRD("rrd"),
    BIT("bit"),
    SET("set"),
    RES("res"),
    JP("jp"),
    DJNZ("djnz"),
    CALL("call"),
    RET("ret"),
    RETI("reti"),
    RETN("retn"),
    RST("rst"),
    IN("in"),
    INI("ini"),
    IND("ind"),
    INIR("inir"),
    INDR("indr"),
    OUT("out"),
    OUTI("outi"),
    OUTD("outd"),
    OTIR("otir"),
    OTDR("otdr"),
    JPJ("jpj"),
    JPC("jpc"),
    RTJ("rtj"),
    RTC("rtc"),
    CPC("cpc"),
    LDC("ldc"),
    CPCR("cpcr"),
    LDCR("ldcr"),
    MUL("mul");

    String name;

    Mnemonic(String name) {
        this.name = name;
    }

    public static Mnemonic getMnemonic(String op) {
        for (Mnemonic m: Mnemonic.values()) {
            if(op.trim().toLowerCase().equals(m.name)) {
                return m;
            }
        }
        return null;
    }
}
