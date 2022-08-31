package opcode;

@FunctionalInterface
interface Assembler {
    byte[] assemble(Opcode op);
}
public enum Format {

    NUL((opcode) -> { return null; }),
    R_R((opcode) -> { return null; }),
    RR_RR((opcode) -> { return null; }),
    R_N((opcode) -> { return null; }),
    R((opcode) -> { return null; }),
    RR((opcode) -> { return null; }),
    N((opcode) -> { return null; }),
    NN((opcode) -> { return null; }),
    D((opcode) -> { return null; }),
    IM((opcode) -> { return null; }),
    RST((opcode) -> { return null; }),
    F_D((opcode) -> { return null; }),//jr f, d
    F_NN((opcode) -> { return null; }),
    F((opcode) -> { return null; }),
    N_R((opcode) -> { return null; }),// bit and register
    INDIRECT_RR_RR((opcode) -> { return null; }),
    R_INDIRECT_NN((opcode) -> { return null; }),
    INDIRECT_NN_R((opcode) -> { return null; }),
    RR_INDIRECT_NN((opcode) -> { return null; }),
    INDIRECT_NN_RR((opcode) -> { return null; }),
    R_INDIRECT_R((opcode) -> { return null; }),
    R_INDIRECT_RR((opcode) -> { return null; }),
    INDIRECT_RR_R((opcode) -> { return null; }),
    INDIRECT_R((opcode) -> { return null; }),
    INDIRECT_R_R((opcode) -> { return null; });

    Assembler ass;

    Format(Assembler ass) {
        this.ass = ass;
    }
}
