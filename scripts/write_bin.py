import sys

def main():
    with open(sys.argv[1], 'wb') as file:
        data = []

        for i in sys.argv:
            if i != sys.argv[1] and i != sys.argv[0]:
                data.append(int(i, 16))

        file.write(bytes(data))
        

if __name__ == '__main__':
    main()
