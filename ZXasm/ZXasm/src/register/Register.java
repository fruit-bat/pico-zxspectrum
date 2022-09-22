package register;

import asm.Main;
import literal.Address;
import literal.Label;
import opcode.CCode;

public class Register {

    boolean is8;
    boolean isIX, isIY;
    public Register8 reg8;
    public Register16 reg16;
    byte offset;
    public CCode flags;
    String literal;
    public Address data;
    public boolean indirect = false;

    public static Register getRegister(String reg, int org, boolean allowDupe) {
        Register r = null;
        if(reg.length() < 1) {
            Main.error(Main.Errors.NULL_REGISTER);
            return new Register(new Address(0));
        }
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
        if(reg.charAt(0) == '(' && reg.charAt(reg.length() - 1) == ')') {//indirect
            r = getRegister(reg.substring(1, reg.length() - 1), org, allowDupe);
            r.indirect = true;
            return r;// recurse
        }
        if(r == null) {
            Address a = null;
            String based = reg.substring(1);
            try {
                if(reg.charAt(0) == '$') {
                    if(reg.length() == 1) {
                        a = new Address(org);//special address of 1st byte of compile
                    } else {
                        a = new Address((char) Integer.parseInt(based, 16));
                    }
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
            r = new Register(Label.findLabel(reg, org, allowDupe));//could be un-found 0
        }
        return r;
    }

    public boolean hasIXIY() {
        return isIX || isIY;
    }

    public byte[] withIXIY(byte[] in) {
        if(!isIX && ! isIY) return in;
        byte[] ok = new byte[in.length + 2];
        for(int i = 0; i < in.length; i++) {
            ok[i + 1] = in[i];
        }
        if(isIX) {
            ok[0] = (byte)0xdd;
        }
        if(isIY) {
            ok[0] = (byte)0xfd;
        }
        ok[in.length] = offset;
        return ok;
    }

    public byte[] withIXIYBit(byte[] in) {
        if(!isIX && ! isIY) return in;
        byte[] ok = withIXIYBit(in);
        byte tmp = ok[2];
        ok[2] = ok[3];
        ok[3] = tmp;//some wierd selection of displacement location
        return ok;
    }

    public byte[] withIXIYNotIndirect(byte[] in) {
        if(!isIX && ! isIY) return in;
        byte[] ok = new byte[in.length + 1];
        for(int i = 0; i < in.length; i++) {
            ok[i + 1] = in[i];
        }
        if(isIX) {
            ok[0] = (byte)0xdd;
        }
        if(isIY) {
            ok[0] = (byte)0xfd;
        }
        return ok;
    }

    public Register(Register8 reg) {
        is8 = true;
        reg8 = reg;
    }

    public Register(String reg) {
        literal = reg.substring(1, reg.length() - 1)
                .replace("\"\"", "\"");
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
