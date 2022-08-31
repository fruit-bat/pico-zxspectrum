package opcode;

public enum Format {

    R_R(),
    RR_RR(),
    R_N(),
    R(),
    RR(),
    N(),
    NN(),
    D(),
    F_D(),//jr f, d
    F_NN(),
    B_R(),// bit and register
    INDIRECT_RR_RR(),
    R_INDIRECT_NN(),
    INDIRECT_NN_R(),
    R_INDIRECT_R(),
    INDIRECT_R_R();


}
