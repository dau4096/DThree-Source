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

	operandsVerbose = [toFloat(value) for value in opSep]
	thisOperands = []
	for op in operandsVerbose:
		if maths.isnan(op[1]):
			return ("Equation is not solvable: " + op[0], maths.nan)
		thisOperands.append(op[1])


	thisOperators = []
	for op in equ:
		if op in operatorsStr: thisOperators.append(op)

	precList = {}
	for idx, op in enumerate(thisOperators):
		precList[idx] = operatorPrecedence[op]

	precListSorted = {k: v for k, v in sorted(precList.items(), key=lambda item: item[1], reverse=True)}

	operands = thisOperands[:]
	operators = thisOperators[:]
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
	

	result = operands[0]
	if result == maths.floor(result): result = int(result)
	return ("Solved successfully:", result)