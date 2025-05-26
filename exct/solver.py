import re as regex
import math as maths
import operator


constants = {
	"pi": maths.pi, "π": maths.pi,
	"e": maths.e, "exp": maths.e,
	"tau": maths.tau, "τ": maths.tau,
	"inf": maths.inf
}
operatorsStr = "+-*/^%"
operatorPrecedence = {
	"+": 1, "-": 1,
	"*": 2, "/": 2, "%": 2,
	"^": 3
}
opFuncs = {
    "+": operator.add,
    "-": operator.sub,
    "*": operator.mul,
    "/": operator.truediv,
    "^": operator.pow,
    "%": operator.mod
}


def toFloat(value: str) -> tuple[str, float]:
	if value in constants:
		return constants[value]
	elif regex.search(r"-?[0-9]+.?[0.9]*e-?[0-9]+.?[0-9]*", value) is not None:
		#In format 3.4e-7 or similar.
		splitV = value.split("e")

		mantissa = float(splitV[0])
		exponent = float(splitV[1])

		return ("Successfully converted value", mantissa * (10 ** exponent))

	elif regex.search(r"-?[0-9]+.?[0-9]*", value) is not None:
		#Single value
		return ("Successfully converted value", float(value))

	else:
		return ("Unknown numerical value: " + value, maths.nan)



#messageData = "/solve 3.7+2%2"
def solveEqu(messageData: str) -> tuple[str, float]:
	equ = messageData.replace("/solve ", "").strip().replace(" ", "").lower()

	#Horrific regex.
	if regex.search(r"-?(?:\d+(?:\.\d*)?|\.\d+)(?:e-?(?:\d+(?:\.\d*)?|\.\d+))?(?:[\+\-\*\/\^%]-?(?:\d+(?:\.\d*)?|\.\d+)(?:e-?(?:\d+(?:\.\d*)?|\.\d+))?)*", equ) is None:
		return ("Equation is not solvable", maths.nan)

	opSep = equ
	for op in operatorsStr:
		opSep = opSep.replace(op, "|")
	opSep = opSep.split("|")

	operands = []
	operandsVerbose = [toFloat(value) for value in opSep]
	for value in operandsVerbose:
		if maths.isnan(value[1]):
			return ("Equation is not solvable: " + value[0], maths.nan)
	operands.append(value[1])


	operators = []
	for op in equ:
		if op in operators: operators.append(op)

	precList = {}
	for idx, op in enumerate(operators):
		precList[idx] = operatorPrecedence[op]

	precListSorted = {k: v for k, v in sorted(precList.items(), key=lambda item: item[1], reverse=True)}


	for precOpIdx in sorted(precListSorted, key=lambda k: precListSorted[k], reverse=True):
	    maxPrec = max(operatorPrecedence[op] for op in operators)
	    for i, op in enumerate(operators):
	        if operatorPrecedence[op] == maxPrec:
	            func = opFuncs[op]
	            left = operands[i]
	            right = operands[i + 1]
	            try:
	                result = func(left, right)
	            except Exception as e:
	                print(f"Error performing {left} {op} {right}: {e}")
	                result = maths.inf

	            operands[i] = result
	            del operands[i + 1]
	            del operators[i]
	            break
	

	return ("Solved successfully:", operands[0])