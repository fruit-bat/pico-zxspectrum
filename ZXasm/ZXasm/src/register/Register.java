package register;

public class Register {

    boolean is8;
    boolean isIX, isIY;
    Register8 reg8;
    Register16 reg16;
    byte offset;

    public static Register getRegister(String reg) {
        Register r = Register8.getRegister(reg);
        if(r == null) {
            r = Register16.getRegister(reg);
        }
        return r;
    }

    public Register(Register8 reg) {
        is8 = true;
        reg8 = reg;
    }

    public Register(Register16 reg) {
        reg16 = reg;
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
}
