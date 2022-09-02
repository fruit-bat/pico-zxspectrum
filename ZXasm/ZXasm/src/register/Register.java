package register;

import literal.Address;
import literal.Label;
import opcode.CCode;

public class Register {

    boolean is8;
    boolean isIX, isIY;
    Register8 reg8;
    Register16 reg16;
    byte offset;
    CCode flags;
    String literal;
    Address data;

    public static Register getRegister(String reg, int org) {
        Register r = null;
        if(reg.charAt(0) == '\"' && reg.charAt(reg.length() - 1) == '\"') {
            r = new Register(reg);
        }
        if(r == null) {
            r = Register8.getRegister(reg);
        }
        if(r == null) {
            r = Register16.getRegister(reg);
        }
        if(r == null) {
            r = CCode.getCCode(reg);
        }
        if(r == null) {
            Address a = null;
            String based = reg.substring(1);
            try {
                if(reg.charAt(0) == '$') {
                    a = new Address((char) Integer.parseInt(based, 16));
                } else if(reg.charAt(0) == '%') {
                    a = new Address((char) Integer.parseInt(based, 2));
                } else {
                    a = new Address((char) Integer.parseInt(reg));
                }
            } catch(Exception e) {
                // leave as null
            }
            if(a != null) {
                r = new Register(a);
            }
        }
        if(r == null) {
            r = new Register(Label.findLabel(reg, org));//could be un-found 0
        }
        return r;
    }

    public Register(Register8 reg) {
        is8 = true;
        reg8 = reg;
    }

    public Register(String reg) {
        literal = reg.substring(1, reg.length() - 1);
    }

    public Register(Register16 reg) {
        reg16 = reg;
    }

    public Register(CCode flags) {
        this.flags = flags;
    }

    public Register(Register8 reg, boolean ix, boolean iy, byte offset) {
        is8 = true;
        reg8 = reg;
        isIX = ix;
        isIY = iy;
        this.offset = offset;
    }

    public Register(Register16 reg, boolean ix, boolean iy) {
        reg16 = reg;
        isIX = ix;
        isIY = iy;
    }

    public Register(Address address) {
        this.data = address;
    }
}
