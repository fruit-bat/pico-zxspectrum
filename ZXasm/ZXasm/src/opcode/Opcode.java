package opcode;

import register.Register;

public class Opcode {

    Mnemonic op;
    Register src, dst;
    public Mnemonic mnemonic() {
        return op;
    }

    public void setMnemonic(Mnemonic op) {
        this.op = op;
    }

    public void setRegisters(Register reg, boolean dest) {
        if(dest) {
            src = reg;
        } else {
            dst = reg;
        }
    }

    public boolean hasOne() {
        return (src != null) ^ (dst != null);
    }

    public boolean hasBoth() {
        return (src != null) & (dst != null);
    }

    public boolean compile() {
        //TODO
        return false;
    }
}
