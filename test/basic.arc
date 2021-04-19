import std;

namespace lib;

struct some_data {
    value: u32;
    pointer: *u32;

    func compute() : u32 {
        return value + *pointer;
    }
}

alias u32_pointer = *u32;

func main(argc: u32, argv: **u8) : u32 {
    let data: some_data;
    data.value = 100;
    data.pointer = &data.value;

    return data.compute();
}
