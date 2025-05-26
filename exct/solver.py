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


def tokenise(expr: str) -> list[str]:
	tokens = []
	i = 0
	while i < len(expr):
		ch = expr[i]

		if ch in "+*/^%":
			tokens.append(ch)
			i += 1

		elif ch == '-':
			if i == 0 or expr[i - 1] in "+-*/^%":
				j = i + 1
				while j < len(expr) and (expr[j].isdigit() or expr[j] == '.' or expr[j] == 'e'):
					j += 1
				tokens.append(expr[i:j])
				i = j
			else:
				tokens.append('-')
				i += 1

		elif ch.isdigit() or ch == '.':
			j = i
			while j < len(expr) and (expr[j].isdigit() or expr[j] == '.' or expr[j] == 'e' or expr[j] == '-'):
				j += 1
			tokens.append(expr[i:j])
			i = j

		elif ch.isalpha():
			j = i
			while j < len(expr) and expr[j].isalpha():
				j += 1
			tokens.append(expr[i:j])
			i = j

		else:
			i += 1
	return tokens



def solveEqu(messageData: str) -> tuple[str, float]:
	equ = messageData.replace("/solve ", "").strip().replace(" ", "").lower()

	#Horrific regex.
	if regex.search(r"-?(?:\d+(?:\.\d*)?|\.\d+)(?:e-?(?:\d+(?:\.\d*)?|\.\d+))?(?:[\+\-\*\/\^%]-?(?:\d+(?:\.\d*)?|\.\d+)(?:e-?(?:\d+(?:\.\d*)?|\.\d+))?)*", equ) is None:
		return ("Equation is not solvable", maths.nan)

	tokens = tokenise(equ)
	thisOperands = []
	thisOperators = []
	for token in tokens:
		if token in operatorsStr:
			thisOperators.append(token)
		else:
			msg, val = toFloat(token)
			if maths.isnan(val):
				return (f"Invalid operand: {msg}", maths.nan)
			thisOperands.append(val)

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
					return ("Equation is not solvable: [error] " + e, maths.nan)
					result = maths.inf

				operands[i] = result
				del operands[i + 1]
				del operators[i]
				break
	

	result = operands[0]
	if result == maths.floor(result): result = int(result)
	return ("Solved successfully:", result)