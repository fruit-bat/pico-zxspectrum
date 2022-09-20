package literal;

public class Address {

    public int address;

    public Address(int address) {
        this.address = address;
    }

    public Address(Address base, Address relative) {
        this.address = base.address + relative.address;
    }
}
