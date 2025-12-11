#!/usr/bin/python3

from sys import argv
from typing import Callable, Optional, Tuple, List, Any
from math import inf
from collections import defaultdict

Result    = Optional[Tuple[List[Any], str]]
Predicate = Callable[[str], bool]
ParserMap = Callable[[Any], Any]

PC = 0

class   Parser:

    def __init__(self, fn: Callable[[str], Result]) -> None:
        self.fn = fn

    def __call__(self, arg):
        if isinstance(arg, str):
            return self.fn(arg)
        elif isinstance(arg, ParserMap):
            return self.map(arg)

    def __add__(self, other):
        def adder(s: str):
            result = self.fn(s)
            if result is None:
                return None
            r1, remain = result
            result = other.fn(remain)
            if result is None:
                return None
            r2, remain = result
            return (r1 + r2, remain)
        return Parser(adder)

    def __lshift__(self, other):
        def ignore(s: str):
            result = self.fn(s)
            if result is None:
                return None
            r, remain = result
            result = other.fn(remain)
            if result is None:
                return (r, remain)
            _, remain = result
            return (r, remain)
        return Parser(ignore)

    def __rshift__(self, other):
        def ignore(s: str):
            result = self.fn(s)
            if result is None:
                return None
            _, remain = result
            return other.fn(remain)
        return Parser(ignore)

    def __or__(self, other):
        def either(s: str):
            result = self.fn(s)
            if result is not None:
                return result
            return other.fn(s)
        return Parser(either)

    def __and__(self, other):
        def both(s: str):
            result = self.fn(s)
            if result is None:
                return None
            r1, remain = result
            result = other.fn(remain)
            if result is None:
                return (r1, remain)
            return result
        return Parser(both)

    def range(self, lb=0, ub=0):
        def many(s: str) -> Result:
            found = 0
            remain = s
            results = []
            while found < ub:
                result = self.fn(s)
                if result is None:
                    break
                found += 1
                r, remain = result
                if s == remain:
                    break
                s = remain
                results.extend(r)
            if lb <= found <= ub:
                return (results, remain)
            return None
        return Parser(many)

    def __matmul__(self, n: int):
        if n != inf and not isinstance(n, int):
            raise RuntimeError
        return self.range(0, n)

    def __rmatmul__(self, other):
        return self.__matmul__(other)

    def __mul__(self, n: int):
        if not isinstance(n, int):
            raise RuntimeError
        return self.range(n, n)
    
    def __rmul__(self, n: int):
        return self.__mul__(n)

    def many(self):
        return self.range(1, inf)

    def __invert__(self):
        def option(s: str):
            result = self.fn(s)
            if result is None:
                return ([], s)
            return result
        return Parser(option)

    def __pos__(self):
        def look(s: str):
            result = self.fn(s)
            if result is None:
                return None
            r, _ = result
            return (r, s)
        return Parser(look)

    def __neg__(self):
        def nlook(s: str):
            result = self.fn(s)
            r, _ = result
            if result is not None:
                return None
            return ([], s)
        return Parser(nlook)

    def map(self, map_fn: ParserMap):
        def mapper(s: str):
            result = self.fn(s)
            if result is None:
                return None
            r, remain = result
            return (list(map(map_fn, r)), remain)
        return Parser(mapper)

    def join(self):
        def joiner(s: str):
            result = self.fn(s)
            if result is None:
                return None
            r, remain = result
            return (["".join(r)], remain)
        return Parser(joiner)

# Classics

def CHAR(c: str) -> Parser:
    def parser(s: str) -> Result:
        if len(s) == 0 or s[0] != c:
            return None
        return ([c], s[1:])
    return Parser(parser)

def CHAR_CLASS(pred: Predicate) -> Parser:
    def parser(s: str) -> Result:
        if len(s) == 0 or not pred(s[0]):
            return None
        return ([s[0]], s[1:])
    return Parser(parser)

def STRING(target: str) -> Parser:
    def parser(s: str) -> Result:
        if s[:len(target)] != target:
            return None
        return ([target], s[len(target):])
    return Parser(parser)

# Customs

ops = [
    ("raw", 0,  0),
    ("add", 1,  3),
    ("mul", 2,  3),
    ("in",  3,  1),
    ("out", 4,  1),
    ("jnz", 5,  2),
    ("jz",  6,  2),
    ("clt", 7,  3),
    ("ceq", 8,  3),
    ("rel", 9,  1),
    ("hlt", 99, 0),
]
opcodes = {o[0]:o[1] for o in ops}
mnemos  = {o[1]:o[0] for o in ops}

SPACE = CHAR_CLASS(lambda c: c.isspace())
DIGIT = CHAR_CLASS(lambda c: c.isdigit())
ALPHA = CHAR_CLASS(lambda c: c.isalpha())
ALNUM = DIGIT | ALPHA

WS_SKIP = SPACE @ inf
TOKEN   = lambda p: WS_SKIP >> p << WS_SKIP

CHAR_FC = ALPHA | CHAR("_")
CHAR_ID = CHAR_FC | DIGIT
ID      = (CHAR_ID.many()).join()

NUMBER  = (~(CHAR("+") | CHAR("-")) + DIGIT.many()).join()
COMMA   = CHAR(",")

SCOPE  = lambda s, p, e: (CHAR(s) + p + CHAR(e)).join()

LABEL    = SCOPE("(", ID, ")")
LABEL_ID = CHAR("(") >> ID << CHAR(")")
VALUE    = NUMBER | ID
REL      = (CHAR("~") + VALUE).join()
PC_REL   = (CHAR("@") + VALUE).join()
ADDR     = VALUE | PC_REL
DEREF    = (CHAR("[") + ADDR + CHAR("]")).join()

mnemonics = [STRING(m) for m, *_ in ops]
MNEMONIC = mnemonics[0]

for m in mnemonics[1:]:
    MNEMONIC |= m

OPERAND_KIND  = ADDR | DEREF | REL
OPERAND       = TOKEN(OPERAND_KIND) << ~COMMA
OPERANDS      = OPERAND @ inf
INSTRUCTION   = TOKEN(MNEMONIC) + OPERANDS
LABEL_DECL    = TOKEN(LABEL) << CHAR(":")

LINE = OPERANDS | INSTRUCTION | LABEL_DECL

labels = defaultdict(int)

def label_set(s: str, pc: int) -> None:
    labels[s] = pc

def label_get(s) -> int:
    return labels[s]

def label_parse(r: [str], pc: int) -> bool:
    if len(r) > 1:
        return False
    result = LABEL_ID(r[0])
    if result is None:
        return False
    r, _ = result
    label_set(r[0], pc)
    return True

class   Operand:
    global PC

    def __init__(self, v: int, m: int):
        self.value = v
        self.mode = m

    def __str__(self):
        return f"OP[{self.mode}:{self.value}]"

    def __repr__(self):
        return self.__str__()

class   Instruction:
    global PC

    def __init__(self, mnemo: str, operands: [Operand]):
        self.op = opcodes[mnemo]
        self.operands = operands
        
    def as_code(self) -> [int]:
        code = []
        op = self.op
        mode = 0
        coef = 100
        for o in self.operands:
            op += o.mode * coef
            coef *= 10
            if type(o.value) == str is not None:
                o.value = label_get(o.value)
            code.append(o.value)
        return ([op] if self.op != 0 else []) + code

class   IntStream:

    def __init__(self):
        self.stream = []

    def push(self, n: Instruction):
        global PC
        self.stream.append(n)

    def raw(self):
        print(",".join(str(i) for ints in self.stream for i in ints.as_code()))

    def dump(self, source: [str]):
        for line, ints in zip(source, self.stream):
            print(f"{line:32s}| {ints.as_code()}")

def pOperand(s: str) -> Optional[Operand]:
    deref = CHAR("[") >> ADDR << CHAR("]")
    result = deref(s)
    if result is not None:
        r, _ = result
        term = r[0]
        return pTerm(term, 0)
    return pTerm(s, 1)

def pTerm(s: str, mode) -> Optional[Operand]:
    mode_char = ~(CHAR("~") | CHAR("@"))
    r, remain = mode_char(s)
    if r == []:
        return pValue(s, mode)
    sign = r[0]
    mode += {"~": 2, "@": 4}[sign]
    return pValue(remain, mode)

def pValue(s: str, mode: int) -> Optional[Operand]:
    global PC
    result = VALUE(s)
    if result is None:
        return None
    r, remain = result
    if len(remain):
        return None
    v = r[0]
    result = NUMBER(v)
    if result is not None:
        r, _ = result
        value = int(r[0])
    else:
        value = s
    if mode & 4:
        value += PC
        mode = 0
    return Operand(value, mode)

def assemble(source: [str]) -> bool:
    global PC
    STREAM = IntStream()
    for i in range(len(source)):
        line = source[i]
        result = (INSTRUCTION | LABEL_DECL)(line)
        if result is None:
            continue
        r, _ = result
        if label_parse(r, PC):
            i += 1
            continue
        opcode, *remain = r
        if opcode != "raw":
            PC += 1
        PC += len(remain)
        operands = [pOperand(o) for o in remain]
        STREAM.push(Instruction(opcode, operands))
    new_source = []
    for s in source:
        if LABEL_DECL(s) is None:
            new_source.append(s)
    #STREAM.dump(new_source)
    STREAM.raw()

def main():
    if len(argv) < 2:
        file = open(0)
    else:
        file = open(argv[1])
    source = file.read().split('\n')
    assemble(source)

if __name__ == "__main__":
    main()
