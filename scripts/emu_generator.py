
class Operation:
    def __init__(self, name, pform, val):
        self.name = name
        self.pformat = pform
        self.val = val

    
    def print_op(self, optype):
        print('cpu->' + optype + '[' + self.val + '].pformat = ' + self.pformat + ';')
        print('cpu->' + optype + '[' + self.val + '].func = ' + self.name + self.val + ';')
        print('strcpy(cpu->' + optype + '[' + self.val + '].name, "' + self.name.upper() + '");')

    
    def print_func(self):
        print('void ' + self.name + self.val + '(struct mipscpu* cpu, struct instruction* ins);')


def main():
    filedata = []
    rops = []
    otherops = []

    currops = 0

    with open('opcodes.csv', 'r') as file:
        filedata = file.readlines()

    for i in filedata:
        if i.replace('\n', '') == '':
            continue
        if i.replace('\n', '') == 'OTHERTYPE':
            currops = 1
            continue

        string = i.replace('\n', '').split(',')
        op = Operation(string[1], ''.join(string[2:]).replace('(', '').replace(')', '').upper(), string[0])

        if currops == 0:
            rops.append(op)
        else:
            otherops.append(op)

    
    for i in rops:
        i.print_func()
    

    for i in otherops:
        i.print_func()


if __name__ == '__main__':
    main()
