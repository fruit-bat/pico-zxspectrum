package register;

import literal.Address;

import java.util.Arrays;

public enum Register8 {

    // opcode ordering
    B("b"),
    C("c"),
    D("d"),
    E("e"),
    H("h", "ixh", "iyh"),
    L("l", "ixl", "iyl"),
    IND_HL("(hl)", "(ix*)", "(iy*)"),// do jp (hl) => remove inconstancy
    A("a"),
    I("i"),
    R("r"),
    BAD_REG(null);//allows continuation to other parse options without null pointer exception

    String name;
    String ix, iy;

    Register8(String name) {
        this.name = name;
        this.ix = null;
        this.iy = null;
    }

    Register8(String name, String ix, String iy) {
        this(name);
        this.ix = ix;
        this.iy = iy;
    }

    static byte parse(String num, Register8 reg, boolean isIX) throws Exception {
        //remove space
        num = num.trim();
        Arrays.stream(num.split(" ")).reduce((a, b) -> a + b);
        String fuzz = isIX ? reg.ix : reg.iy;
        if(fuzz.indexOf("*") < 0) return 0;// no *
        num = num.substring(fuzz.indexOf("*") + 1);
        int tail = fuzz.length() - fuzz.indexOf("*") - 1;
        num = num.substring(0, fuzz.length() - tail);
        return (byte)(new Address(num, 0, false)).address;
    }

    static boolean fuzzEquals(String reg, String fuzz) {
        int head = fuzz.indexOf("*");
        if(head < 0) {
            return reg.equals(fuzz);// no *
        }
        int tail = fuzz.length() - fuzz.indexOf("*") - 1;
        if(reg.substring(head).equals(fuzz.substring(head))) {
            String end = reg.substring(reg.length() - tail);
            if(end.equals(fuzz.substring(fuzz.length() - tail))) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    public static Register getRegister(String reg) {
        try {
            for (Register8 r : Register8.values()) {
                if (reg.trim().toLowerCase().equals(r.name))
                    return new Register(r);
                if (r.ix != null) {
                    if (fuzzEquals(reg.trim().toLowerCase(), r.ix))
                        return new Register(r, true, false,
                                parse(reg, r, false));
                }
                if (r.iy != null) {
                    if (fuzzEquals(reg.trim().toLowerCase(), r.iy))
                        return new Register(r, false, true,
                                parse(reg, r, true));
                }
            }
        } catch (Exception e) {
            return null;
        }
        return null;
    }
}
