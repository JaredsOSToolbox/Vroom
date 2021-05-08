#!/usr/bin/env python3.9
import queue
import re


def int_to_bytes(i: int, *, signed: bool = False) -> bytes:
    """
    Mad credit to this lovely human:
    https://stackoverflow.com/a/54141411
    Thanks Python
    """

    length = ((i + ((i * signed) < 0)).bit_length() + 7 + signed) // 8
    return i.to_bytes(length, byteorder='big', signed=signed)


class Address():
    def __init__(self, position: int):
        self.logical = int(position)
        self.offset = self.offset_(self.logical)
        self.page = self.page_(self.logical)
        self.access_count = 0
        self.frame = None

    def page_(self, x: int):
        return ((x & 0xFF00) >> 8)

    def offset_(self, x: int):
        return (x & 0xFF)

    def __eq__(self, other):
        if(other is None):
            return False
        return (self.page == other.page)

        # return ((self.logical == other.logical) and
        # (self.offset == other.offset) and
        # (self.page == other.page) and
        # (self.frame == other.frame))

    def __repr__(self):
        _frame = None if self.frame is None else (
            self.frame << 8 | self.offset)
        return f'logical: {self.logical} (page: {self.page}, offset: {self.offset}) ---> physical: {_frame}, frame: {self.frame}'


class AddressReader():
    def __init__(self, path: str):
        with open(path, "r") as fp:
            self.contents = [x.strip() for x in fp.readlines()]


class CorrectReader():
    def __init__(self, path: str):
        _re = re.compile(
            "Virtual address\:\s*([0-9]+)\s*Physical\s?address\:\s+?([0-9]+)\s*Value\:\s*(\-?[0-9]+)")
        self.contents = []
        with open(path, "r") as fp:
            for line in fp.readlines():
                match = _re.match(line)
                if(match):
                    # _m = (int(a), int(b), bytes([int(c)]))
                    _m = (int(element) for element in match.groups())
                    self.contents.append(_m)


class BinaryReader():
    def __init__(self, path: str, chunk=256):
        self.file = open(path, 'r+b')
        self.chunk = chunk

    def seek(self, page: int):
        self.file.seek(self.chunk * page)

    def read_page(self):
        return self.file.read(self.chunk)

    def __del__(self):
        self.file.close()


class CacheContainer():
    def __init__(self, current_size: int, max_size: int):
        self.container = [None] * (max_size)
        self.max_size = max_size
        self.capacity = 0
        self.current_size = current_size
        self.free_list = queue.Queue()
        for x in range(0, max_size+1):
            self.free_list.put(x)

    def full(self):
        return (self.capacity >= self.current_size)

    def contains(self, element) -> tuple:
        for i, e in enumerate(self.container):
            if(e == element):
                return (True, e)
        return (False, None)

    def free_frame(self):
        if(self.free_list.empty()):
            raise Exception('queue is empty')
        return self.free_list.get()


class TLB(CacheContainer):
    def __init__(self, current_size=16, max_size=16):
        super().__init__(current_size, max_size)
        self.stack_pointer = 0

    def insert(self, element):
        override = False
        if(self.full()):
            # we just remove the last element from the array
            # self.stack_pointer += 1
            override = True
        else:
            self.capacity += 1
        index = (self.max_size - 1) if override else element.frame
        # index = (self.stack_pointer %
        # self.max_size) if override else element.frame
        self.container[index] = element


class PageTable(CacheContainer):
    def __init__(self, current_size=256, max_size=256):
        super().__init__(current_size, max_size)

    def insert(self, element):
        override = False
        if(self.full()):
            # we just remove the last element from the array
            self.prune()
            print("override")
            override = True
        else:
            self.capacity += 1

        element.frame = self.free_frame()

        print(element.frame)

        self.container[element.frame] = element

    def prune(self):
        _t, i = [], 0

        for x, element in enumerate(self.container):
            if(element and element.access_count < 2 and i < 5):
                _t.append(element)
                self.free_list.put(x)
                i += 1

        for element in _t:
            print(f'[INFO] removing {element}')
            del element


blob = "/tmp/backing.bin"

input_file = "/tmp/address.txt"
correct_output = "/tmp/correct.txt"

address_file = AddressReader(input_file)
correct_file = CorrectReader(correct_output)

binary_read = BinaryReader(blob)

page_table = PageTable(current_size=128, max_size=256)
tlb = TLB(current_size=16, max_size=16)
tlb_hit, page_fault = 0, 0
iterations = 1000

for x, element in enumerate(address_file.contents[:iterations]):
    print(f'[{x}]')
    address = Address(element)
    _tlb, _page_table = False, False

    virtual, offset, page = address.logical, address.offset, address.page
    _t_contains, _t_element = tlb.contains(address)
    if(_t_contains):
        address.frame = _t_element.frame
        tlb_hit += 1
        _tlb = True

    if(not _tlb):
        _page_contains, _p_element = page_table.contains(address)
        if(not _page_contains):
            print("page fault")
            page_fault += 1
            # update the tlb and page table without any regard for saftey
            page_table.insert(address)
            tlb.insert(address)
        else:
            print("page hit")
            _page_table = True
            address.frame = _p_element.frame

    assert(address.frame is not None)

    binary_read.seek(page)
    contents = binary_read.read_page()
    _read = contents[offset]
    value = int_to_bytes(_read, signed=True if(int(_read) < 0) else False)

    _virtual, _physical, _value = correct_file.contents[x]
    _value = int_to_bytes(_value, signed=True if(int(_value) < 0) else False)
    physical = (address.frame << 8) | offset
    # if(address.frame == 24):
    # print(f'current address: {address}')
    # p = (23 << 8) | offset
    # print(p)

    # for tlb_entry in tlb.container:
    # print(tlb_entry)

    print(f'Current Frame: {address.frame}')
    print(f'Intended Frame: {(_physical | offset) >> 8}')

    print(f'Virtual: {_virtual} == {virtual}')
    print(f'Physical: {_physical} == {physical}')
    print(f'Value: {_value} == {value}')
    print(f'Offset: ({offset}), Page: ({page})')
    print('-'*50)

    assert((_virtual == virtual))
    assert((_physical == physical))
    assert((_value == value))
    assert(((physical | offset) >> 8) == address.frame)

# print(
    # f'Access count: {iterations}\nPage Fault Rate: {page_fault/iterations}\nTLB Hit Ratio: {tlb_hit/iterations}')
print(
    "Access count   Tlb hit count   Page fault count   Tlb hit rate   Page fault rate\n"
    f'{iterations}  {tlb_hit}       {page_fault}       {tlb_hit/iterations}   {page_fault/iterations}\n'
)
