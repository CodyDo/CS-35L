import argparse
import random
import sys

class shuf:
    def __init__(self, input, LO_HI, LIMIT, REPEAT):
        self.lines = []
        self.useSTDin = False
        self.limit = LIMIT
        self.repeat = REPEAT
        
        # If no LO_HI then read from input
        if LO_HI:
            # Split range and add appropriate integers to lines
            splitWords = LO_HI.split('-')
            lower = int(splitWords[0])
            upper = int(splitWords[1])
            i = lower
            while (i <= upper):
                self.lines.append(str(i)+"\n")
                i = i + 1
            # If no repeating and no limit, then limit based on range
            if not self.limit and not self.repeat:
                self.limit = upper - lower + 1
        else:
            self.useSTDin = True

        if self.useSTDin:
            # If this is true, then read from stdin. If not,
            # then input[0] has the filename.
            if input == '-':
                for line in sys.stdin:
                     self.lines.append(str(line))
                self.limit = len(self.lines)
                
                # If no provided value for the limit, then it's 
                # the length of lines
                if LIMIT < self.limit and self.repeat:
                    self.limit = LIMIT
            else:
                # Open file and read from it
                f = open(input, 'r')
                self.lines = f.readlines()
                f.close()
                self.limit = len(self.lines)
                # If the user input limit is less than the new limit
                # aka size of self.lines, reset it
                if LIMIT < self.limit and self.repeat:
                    self.limit = LIMIT
        
        # Final check. If the input limit exists (it's not false)
        # and the new limit set by any STDin is larger than the input
        # limit, reset the limit to the input limit
        if LIMIT:
            if LIMIT < self.limit:
                self.limit = LIMIT

    def printLine(self):
        # Pick a random line from the array of lines
        randomLine = random.choice(self.lines)
        # If we're repeating, return randomLine. If not, remove it
        # then return
        if not self.repeat:
            self.lines.remove(randomLine)
        return randomLine

    def shufflePrint(self):
        # If repeating is true and there's no specified limit, go forever
        if self.repeat and not self.limit:
            while True:
                #print(self.printLine())
                line = self.printLine()
                sys.stdout.write(line)

        # If not, print out up to LIMIT
        else:
            for i in range(self.limit):
                #print(self.printLine())
                line = self.printLine()
                sys.stdout.write(line)

def main():
    file_info = """ 
    The file to be read. If nothing is provided read from stdin. Press
    Enter to submit line. Press "Control-D" when finished submitting.
    """

    # Create parser
    parser = argparse.ArgumentParser(
        description="Generate random permutations")

    ###### Add the possible arguments ######
    # -i, --input-range
    parser.add_mutually_exclusive_group().add_argument("-i", "--input-range", 
            action="store",
            dest="LO_HI", 
            default=False,
            help="Treat each number from LO through HI as an input line")

    # -n, --head-count
    parser.add_argument("-n", "--head-count", type=int, action="store",
            dest="LIMIT", default=False, help="Output at most LIMIT lines")
            # Potential issue if LIMIT = 0

    # -r, --repeat
    parser.add_argument("-r", "--repeat", action="store_true",
            dest="REPEAT", default=False, help="Output lines can be repeated")

    # FILENAME; HELD IN A LIST/ARRAY SO ACCESS FILE[0]
    parser.add_mutually_exclusive_group().add_argument("FILE", nargs='*', 
            action="store", 
            default="-", 
            help=file_info)

    args = parser.parse_args()

    ##### Validate the arguments #####
    if args.LO_HI:
        # Makes sure input is a range between two numbers separated by '-'
        try:
            splitWords = args.LO_HI.split('-')
            lower = int(splitWords[0])
            upper = int(splitWords[1])
        except:
            parser.error("Invalid input range: {}".format(args.LO_HI))

        # Makes sure that the lower bound is actually lower than the upper
        if (lower > upper):
            parser.error("Invalid input range: {}".format(args.LO_HI))

    if args.LIMIT:
        # Makes sure that LIMIT is actually an integer
        try:
            LIMIT = int(args.LIMIT)
        except:
            parser.error("Invalid line limit: {}".format(LIMIT))
        
        # Makes sure that the limit is non-negative
        if (LIMIT < 0):
            parser.error("Invalid line limit: {}".format(LIMIT))
    
    # If both a filename was input AND LO_HI was used, exit. They can't
    # both be used.
    if (args.FILE[0] != '-') and args.LO_HI:
        parser.error("Cannot use -i and a file at the same time.")

    # Checks to make sure the file can be read in the first place
    if args.FILE[0] != '-':
        try:
            with open(args.FILE[0]) as f:
                test = f.read()
        except IOError as err:
            parser.error("I/O Error {0}: {1}".format(err.errno,err.strerror))

    ##### Create the shuf class and print out appropriate lines #####
    try: 
        shuffler = shuf(args.FILE[0], args.LO_HI, args.LIMIT, args.REPEAT)
        shuffler.shufflePrint()
    except IOError as err:
        parser.error("I/O Error {0}: {1}".format(err.errno,err.strerror))

if __name__ == '__main__':
    main()
