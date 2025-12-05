#!/usr/bin/python3

from collections import defaultdict
import sys
import re

labels = defaultdict()
variables = defaultdict()

def add_label(s: str, pc: int) -> None:
    m = re.match(r"^\(([a-zA-Z0-9_]*)\):$", s)
    if m is None:
        print(f"ERROR: invalid label declaration {s}.")
        raise ValueError
    name = m.group(1)
    labels[name] = pc

def get_label(s: str) -> int:
    m = re.match(r"^\(([a-zA-Z0-9_]*)\)$", s)
    if m is None:
        print(f"ERROR: unknown label '{s}'.")
        raise ValueError
    name = m.group(1)
    return labels[name]

class Operation:

    def __init__(self, opcode: int, mode: list [int], op_list: list [int]):
        self.opcode = opcode
        self.mode = mode
        self.op_list = op_list

    def modestring(self) -> str:
        if len(self.mode) == 0:
            return ""
        return "".join([str(m) for m in self.mode])

    def operandstring(self) -> str:
        if len(self.op_list) == 0:
            return ""
        return "," + ",".join([str(m) if type(m) == int else str(get_label(m)) for m in self.op_list])
    
    def dump(self) -> None:
        opcode = f"{ops[self.opcode][0]:02d}"
        mode = self.modestring()
        operands = self.operandstring()
        op = f"{mode}{opcode}"
        print(f"{int(op)}{operands}", end="")

def get_mode_oplist(values: list [str]):
    op_list = []
    mode = []
    for v in values:
        if v[-1] == ",":
            v = v[:-1]
        if v[0] in "0123456789-":
            op_list.append(int(v))
            mode.append(1)
            continue
        if v[0] == "[":
            if v[1:].startswith("rel+"):
                mode.append(2)
                op_list.append(int(v[5:-1]))
            else:
                mode.append(0)
                op_list.append(int(v[1:-1]))
            continue
        if v[0] == "(":
            op_list.append(v)
            mode.append(0)
            continue
        print(f"ERROR: invalid operand '{v}'")
        raise ValueError
    return mode, op_list

ops = {
    "res": (0,  0),
    "add": (1,  3),
    "mul": (2,  3),
    "in":  (3,  1),
    "out": (4,  1),
    "jz":  (5,  2),
    "jnz": (6,  2),
    "clt": (7,  3),
    "ceq": (8,  3),
    "rel": (9,  1),
    "hlt": (99, 0),
}

def assemble(source: str):
    PC = 0
    STREAM = []
    for line in source:
        tokens = [s.strip() for s in line.split()]
        if len(tokens) == 0:
            continue

        op, *values = tokens
        
        if op[0] == '(':
            add_label(op, PC)
            continue

        if op in ops:
            if len(values) != ops[op][1]:
                print(f"ERROR: invalid operand count for '{op}'.")
                raise ValueError
            mode, op_list = get_mode_oplist(values)
        else:
            print(f"ERROR: unknown instruction '{op}'.")
            raise ValueError
        
        STREAM.append(Operation(op, mode, op_list))
        PC += ops[op][1] + 1

    for i, o in enumerate(STREAM):
        o.dump()
        if i != len(STREAM) - 1:
            print(",", end="")

def main():
    if len(sys.argv) < 2:
        file = open(0)
    else:
        file = open(sys.argv[1])
    source = file.read().split('\n')
    assemble(source)

if __name__ == "__main__":
    main()
