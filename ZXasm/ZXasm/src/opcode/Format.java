package opcode;

@FunctionalInterface
interface Assembler {
    byte[] assemble(Opcode op, int org);
}
public enum Format {

    NUL((opcode, org) -> {
        //TODO
        return null;
    }),
    R_R((opcode, org) -> { return null; }),
    RR_RR((opcode, org) -> { return null; }),
    R_N((opcode, org) -> { return null; }),
    R((opcode, org) -> { return null; }),
    RR((opcode, org) -> { return null; }),
    N((opcode, org) -> { return null; }),
    NN((opcode, org) -> { return null; }),
    D((opcode, org) -> { return null; }),
    IM((opcode, org) -> { return null; }),
    RST((opcode, org) -> { return null; }),
    F_D((opcode, org) -> { return null; }),//jr f, d
    F_NN((opcode, org) -> { return null; }),
    F((opcode, org) -> { return null; }),
    N_R((opcode, org) -> { return null; }),// bit and register
    INDIRECT_RR_RR((opcode, org) -> { return null; }),
    R_INDIRECT_NN((opcode, org) -> { return null; }),
    INDIRECT_NN_R((opcode, org) -> { return null; }),
    RR_INDIRECT_NN((opcode, org) -> { return null; }),
    INDIRECT_NN_RR((opcode, org) -> { return null; }),
    R_INDIRECT_R((opcode, org) -> { return null; }),
    R_INDIRECT_RR((opcode, org) -> { return null; }),
    INDIRECT_RR_R((opcode, org) -> { return null; }),
    INDIRECT_R((opcode, org) -> { return null; }),
    INDIRECT_R_R((opcode, org) -> { return null; });

    Assembler ass;

    Format(Assembler ass) {
        this.ass = ass;
    }
}
