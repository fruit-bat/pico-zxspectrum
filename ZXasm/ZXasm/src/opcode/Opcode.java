package opcode;

import register.Register;

public class Opcode {

    Mnemonic op;
    CCode flag;
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

    public void setRegisters(Register reg, CCode flag) {
        src = reg;
        this.flag = flag;
    }

    public boolean hasOne() {
        return (src != null) ^ (dst != null);
    }

    public boolean hasBoth() {
        return (src != null) & (dst != null);
    }

    public byte[] compile() {
        for (int i = 0; i < op.allows.length; i++) {
            byte[] b = op.allows[i].ass.assemble(this);
            if(b != null) return b;
        }
        return null;
    }
}
