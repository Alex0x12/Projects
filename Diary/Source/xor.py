def xor(string:str, key:str)->str:
    if ord(key) < 0: return
    return ''.join([(chr(ord(ch)^~ord(key))) for ch in string])

crypted = xor('ab c dess',32)

print(crypted)

print(xor(crypted, 32))  