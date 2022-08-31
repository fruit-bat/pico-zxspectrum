package opcode;

public enum Format {

    NUL(),
    R_R(),
    RR_RR(),
    R_N(),
    R(),
    RR(),
    N(),
    NN(),
    D(),
    IM(),
    RST(),
    F_D(),//jr f, d
    F_NN(),
    F(),
    N_R(),// bit and register
    INDIRECT_RR_RR(),
    R_INDIRECT_NN(),
    INDIRECT_NN_R(),
    RR_INDIRECT_NN(),
    INDIRECT_NN_RR(),
    R_INDIRECT_R(),
    R_INDIRECT_RR(),
    INDIRECT_RR_R(),
    INDIRECT_R(),
    INDIRECT_R_R();
}
