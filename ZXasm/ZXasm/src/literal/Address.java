package literal;

public class Address {

    int address;

    public Address(int address) {
        this.address = address;
    }

    public Address(Address base, int relative) {
        this.address = base.address + relative;
    }

    public Address(Address base, Address relative) {
        this.address = base.address + relative.address;
    }
}
